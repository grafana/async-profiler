//
// Created by korniltsev on 3/27/25.
//

#ifndef ASYNC_REF_H
#define ASYNC_REF_H

#include <atomic>
#include <cassert>

namespace pyroscope
{
    class CntObject
    {
        mutable std::atomic<int> _cnt;
        
        template <class T>
        friend class Ref;

        void inc() const
        {
            _cnt.fetch_add(1, std::memory_order_relaxed);
        }

        bool dec() const
        {
            return _cnt.fetch_sub(1, std::memory_order_acq_rel) == 1;
        }

    public:
        CntObject() : _cnt(1)
        {
        }

        CntObject(const CntObject& other) = delete;
        CntObject(CntObject&& other) = delete;
        CntObject& operator=(const CntObject& other) = delete;
        CntObject& operator=(CntObject&& other) = delete;

        virtual ~CntObject()
        {
            assert(_cnt.load(std::memory_order_relaxed) == 0); // todo remove
        }
    };

    template <typename T>
    class Ref
    {
    public:

        Ref(const Ref&) = delete;
        const Ref& operator=(const Ref&) = delete;
        Ref(Ref&& other) = delete;
        Ref& operator=(Ref&& other) = delete;


        Ref(): _ptr(nullptr)
        {
        }

        template <typename... Args>
        explicit Ref(Args&&... args) : _ptr(new T(std::forward<Args>(args)...))
        {
        }

        ~Ref()
        {
            Clear();
        }

        void Clear()
        {
            auto prev = _ptr.exchange(nullptr);
            if (prev)
            {
                if (prev->dec())
                {
                    delete prev;
                }
            }
        }

        // AsyncSafeCopy increases the reference count for the other and stores ptr to this
        // precondition: this.ptr should be nullptr and other.ptr should be not nullptr
        void AsyncSafeCopy(const Ref& other)
        {
            auto t = other._ptr.load();
            if (_ptr.load() != nullptr || t == nullptr)
            {
                return;
            }
            t->inc();
            _ptr.store(t);
        }


        [[nodiscard]] T* Get() const
        {
            return _ptr.load();
        }

    private:
        std::atomic<T*> _ptr;
    };
}
#endif // ASYNC_REF_H
