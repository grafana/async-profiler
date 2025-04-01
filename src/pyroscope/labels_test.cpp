//
// Created by korniltsev on 3/31/25.
//
#include <Context.h>
#include <signal.h>
#include <bits/types/siginfo_t.h>


pthread_key_t key_in_handler;

void set_in_handler(const bool b)
{
    const uintptr_t v = b ? 1 : 0;
    pthread_setspecific(key_in_handler, reinterpret_cast<void*>(v));
}

bool get_in_handler()
{
    const auto v = reinterpret_cast<uintptr_t>(pthread_getspecific(key_in_handler));
    return v != 0;
}

void JNICALL DeleteGlobalRef(JNIEnv* env, jobject gref)
{
    assert(!get_in_handler());
    printf("DeleteGlobalRef  %p\n", gref);
}

JNINativeInterface_ f{
    .DeleteGlobalRef = DeleteGlobalRef,
};
JNIEnv env{.functions = &f};

JNIEnv* jni()
{
    return &env;
}


typedef void (*SigAction)(int, siginfo_t*, void*);

static SigAction installSignalHandler(int signo, SigAction action)
{
    struct sigaction sa{};
    struct sigaction oldsa{};
    sigemptyset(&sa.sa_mask);
    assert(action != nullptr);
    sa.sa_sigaction = action;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sigaction(signo, &sa, &oldsa);
    return oldsa.sa_sigaction;
}
pyroscope::LabelsStore<10000>* store = new pyroscope::LabelsStore<10000>();
void handler(int signo, siginfo_t* info, void* context)
{
    set_in_handler(true);

    write(1, "handler\n", 8);
    auto ctx = pyroscope::_auto_thread_local->get();

    if (ctx->labels.Get() != nullptr)
    {
        pyroscope::Ref<pyroscope::Labels> ls{};
        ls.AsyncSafeCopy(ctx->labels);
        u64 idx = store->Lookup(ls);

        printf("idx %llu\n", idx);
    }

    set_in_handler(false);
}

int main(int argc, char** argv)
{
    pthread_key_create(&key_in_handler, nullptr);
    pyroscope::_testing_jni_hook = jni;
    installSignalHandler(SIGPROF, handler);

    auto ctx = pyroscope::_auto_thread_local->get();
    ctx->setLabels(nullptr);
    ctx->setLabels(reinterpret_cast<jobject>(0xcafebabe));
    kill(getpid(), SIGPROF);
    kill(getpid(), SIGPROF);

    ctx->setLabels(nullptr);
    return 0;
}
