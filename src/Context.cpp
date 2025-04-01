//
// Created by korniltsev on 3/27/25.
//
#include "Context.h"

namespace pyroscope
{
    const AutoThreadLocal<Context> *_auto_thread_local = new AutoThreadLocal<Context>();
#if defined(PYROSCOPE_LABELS_TESTING)
    JNIEnv* (*_testing_jni_hook)() = nullptr;
#endif
}
