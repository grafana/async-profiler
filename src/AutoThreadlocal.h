//
// Created by korniltsev on 3/27/25.
//

#ifndef AUTO_THREAD_LOCAL_H
#define AUTO_THREAD_LOCAL_H

#include <pthread.h>

namespace pyroscope
{
    template <typename T>
    class AutoThreadLocal
    {
    public:
        AutoThreadLocal()
        {
            pthread_key_t k;
            pthread_key_create(&k, destructor);
            pthread_key_create(&k, destructor);
            _k = k;
        }

        ~AutoThreadLocal() = delete;

        // may return null if pthread_setspecific fails
        [[nodiscard]] T* get() const
        {
            void* p = pthread_getspecific(_k);
            if (p != nullptr)
            {
                return static_cast<T*>(p);
            }
            return getSlow();
        };

    private:
        T* getSlow() const
        {
            T* p = new T();
            __asm__ __volatile__("" : : : "memory");
            const int res = pthread_setspecific(_k, p);
            if (res != 0)
            {
                delete p;
                return nullptr;
            }
            return p;
        };

        static void destructor(void* p)
        {
            const T* t = static_cast<const T*>(p);
            delete t;
        }

        pthread_key_t _k;
    };
}

#endif // AUTO_THREAD_LOCAL_H
