//
// Created by korniltsev on 3/27/25.
//

#ifndef CONTEXT_H
#define CONTEXT_H

#include <array>
#include <jni.h>
#include <spinLock.h>

#include "AsyncRef.h"
#include "AutoThreadlocal.h"


#if !defined(PYROSCOPE_LABELS_TESTING)
#include "vmEntry.h"
#define pyroscope_jni() VM::jni()
#endif // PYROSCOPE_LABELS_TESTING


namespace pyroscope
{
#if defined(PYROSCOPE_LABELS_TESTING)
    extern JNIEnv* (*_testing_jni_hook)();
#define pyroscope_jni() _testing_jni_hook()
#endif // PYROSCOPE_LABELS_TESTING

    class Labels final : public CntObject
    {
    public:
        explicit Labels(jobject global_ref)
            : _global_ref(global_ref)
        {
            assert(global_ref != nullptr);
        }

        ~Labels() override
        {
            JNIEnv* jni = pyroscope_jni();
            if (jni)
            {
                jni->DeleteGlobalRef(_global_ref);
            }
        }

        const jobject &GetJobjectRef() const
        {
            return _global_ref;
        }

    private:
        jobject _global_ref;
    };

    struct Context
    {
        Context() = default;

        Ref<Labels> labels;

        void setLabels(jobject global_ref)
        {
            // todo do we need any barriers here
            // todo do we need a single exchange instead of 2?
            this->labels.Clear();
            if (global_ref != nullptr)
            {
                const Ref<Labels> ref{global_ref};
                this->labels.AsyncSafeCopy(ref); // this does not have to be asyncsafe, just reusing same api
            }
        }
    };


    extern const AutoThreadLocal<Context>* _auto_thread_local;

    static u64 calcHash(jobject o);

    template<std::size_t SIZE >
    class LabelsStore
    {
    public:
        uint64_t Lookup(const Ref<Labels>& ref)
        {
            auto ptr = ref.Get();
            if (ptr == nullptr)
            {
                return 0xffffffff; // todo enum/const
            }
            auto h = calcHash(ptr->GetJobjectRef());
            if (!_lock.tryLock())
            {
                return 0xffffffff;
            };
            auto idx = h % SIZE;
            for (int i = 0; i < 2; ++i)
            {
                auto it = _refs.at(idx).Get();
                if (it == nullptr)
                {
                    _refs.at(idx).AsyncSafeCopy(ref);
                    _lock.unlock();
                    return idx;
                }
                if (it->GetJobjectRef() == ptr->GetJobjectRef())
                {
                    _lock.unlock();
                    return idx;
                }
                idx = (idx + 1) % SIZE;
            }
            _lock.unlock();
            return 0;
        }

        void clear()
        {
            _lock.lock();
            for (auto & _ref : _refs)
            {
                _ref.Clear();
            }
            _lock.unlock();
        }

    private:
        SpinLock _lock;
        std::array<Ref<Labels>, SIZE> _refs;
    };

    // Adaptation of MurmurHash64A by Austin Appleby
    // u64 CallTraceStorage::calcHash(int num_frames, ASGCT_CallFrame* frames) {
    static u64 calcHash(void *d, int d_len) {
        const u64 M = 0xc6a4a7935bd1e995ULL;
        const int R = 47;

        int len = d_len;
        u64 h = len * M;

        const u64* data = (const u64*)d;
        const u64* end = data + len / 8;

        while (data != end) {
            u64 k = *data++;
            k *= M;
            k ^= k >> R;
            k *= M;
            h ^= k;
            h *= M;
        }

        if (len & 4) {
            h ^= *(u32*)data;
            h *= M;
        }

        h ^= h >> R;
        h *= M;
        h ^= h >> R;

        return h;
    }

    static u64 calcHash(jobject o)
    {
        static_assert(sizeof(jobject) == 8, "jobject size is not 8 bytes");
        return calcHash(&o, sizeof(jobject));
    }
}
#endif //CONTEXT_H
