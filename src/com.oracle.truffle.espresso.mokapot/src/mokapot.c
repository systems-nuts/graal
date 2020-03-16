/*
 * Copyright (c) 2017, 2019, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */
#include "mokapot.h"
#include "os.h"

#include <trufflenfi.h>
#include <jni.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Global
MokapotEnv *mokaEnv = NULL;

JavaVM* getJavaVM() {
  return (*mokaEnv)->vm;
}

#define JNI_INVOKE_INTERFACE_METHODS(V) \
  V(DestroyJavaVM) \
  V(AttachCurrentThread) \
  V(DetachCurrentThread) \
  V(GetEnv) \
  V(AttachCurrentThreadAsDaemon)


MokapotEnv* initializeMokapotContext(TruffleEnv *truffle_env, JNIEnv* env, void* (*fetch_by_name)(const char *)) {

  MokapotEnv *moka_env = (MokapotEnv *) malloc(sizeof(*moka_env));
 
  struct MokapotNativeInterface_ *functions = (struct MokapotNativeInterface_*) malloc(sizeof(*functions));
  struct JNIInvokeInterface_ *java_vm_functions = (struct JNIInvokeInterface_*) malloc(sizeof(*java_vm_functions));

  JavaVM * java_vm = (JavaVM*) malloc(sizeof(*java_vm));

  *java_vm = java_vm_functions;
  functions->vm = java_vm;
  *moka_env = functions;

  #define INIT__(name) \
      functions->name = fetch_by_name(#name);
  VM_METHOD_LIST(INIT__)
  #undef INIT_

  // Persist env globally.
  mokaEnv = moka_env;

  #define INIT_VM__(name) \
      java_vm_functions->name = fetch_by_name(#name);

  JNI_INVOKE_INTERFACE_METHODS(INIT_VM__)
  #undef INIT_VM__

  return moka_env;
}

MokapotEnv* getEnv() {
  return mokaEnv;
}

void disposeMokapotContext(TruffleEnv *truffle_env, MokapotEnv* moka_env) {
  struct MokapotNativeInterface_ *functions = (struct MokapotNativeInterface_*) *moka_env;
  struct JNIInvokeInterface_ *java_vm_functions = (struct JNIInvokeInterface_ *)(*(functions->vm));

  #define DISPOSE__(name) \
       (*truffle_env)->releaseClosureRef(truffle_env, functions->name); \
       functions->name = NULL;

  VM_METHOD_LIST(DISPOSE__)
  #undef DISPOSE__

  #define DISPOSE_INVOCATION_API__(name) \
      (*truffle_env)->releaseClosureRef(truffle_env, java_vm_functions->name); \
      java_vm_functions->name = NULL;

  JNI_INVOKE_INTERFACE_METHODS(DISPOSE_INVOCATION_API__)
  #undef DISPOSE_INVOCATION_API__

  free((*moka_env)->vm);
  functions->vm = NULL;
  free(functions);
  *moka_env = NULL;
  free(moka_env);
}

jint JVM_GetInterfaceVersion(void) {
  IMPLEMENTED(JVM_GetInterfaceVersion);
  return (*getEnv())->JVM_GetInterfaceVersion();
}

jint JVM_IHashCode(JNIEnv *env, jobject obj) {
  IMPLEMENTED(JVM_IHashCode);
  return (*getEnv())->JVM_IHashCode(env, obj);
}

void JVM_MonitorWait(JNIEnv *env, jobject obj, jlong ms) {
  IMPLEMENTED(JVM_MonitorWait);
  (*getEnv())->JVM_MonitorWait(env, obj, ms);
}

void JVM_MonitorNotify(JNIEnv *env, jobject obj) {
  IMPLEMENTED(JVM_MonitorNotify);
  (*getEnv())->JVM_MonitorNotify(env, obj);
}

void JVM_MonitorNotifyAll(JNIEnv *env, jobject obj) {
  IMPLEMENTED(JVM_MonitorNotifyAll);
  (*getEnv())->JVM_MonitorNotifyAll(env, obj);
}

jobject JVM_Clone(JNIEnv *env, jobject obj) {
  IMPLEMENTED(JVM_Clone);
  return (*getEnv())->JVM_Clone(env, obj);
}

jstring JVM_InternString(JNIEnv *env, jstring str) {
  IMPLEMENTED(JVM_InternString);
  return (*getEnv())->JVM_InternString(env, str);
}

jlong JVM_CurrentTimeMillis(JNIEnv *env, jclass ignored) {
  IMPLEMENTED(JVM_CurrentTimeMillis);
  return (*getEnv())->JVM_CurrentTimeMillis(env, ignored);
}

jlong JVM_NanoTime(JNIEnv *env, jclass ignored) {
  IMPLEMENTED(JVM_NanoTime);
  return (*getEnv())->JVM_NanoTime(env, ignored);
}

void JVM_ArrayCopy(JNIEnv *env, jclass ignored, jobject src, jint src_pos, jobject dst, jint dst_pos, jint length) {
  IMPLEMENTED(JVM_ArrayCopy);
  (*getEnv())->JVM_ArrayCopy(env, ignored, src, src_pos, dst, dst_pos, length);
}

jobject JVM_InitProperties(JNIEnv *env, jobject p) {
  IMPLEMENTED(JVM_InitProperties);
  return (*getEnv())->JVM_InitProperties(env, p);
}

void JVM_OnExit(void (*func)(void)) {
  UNIMPLEMENTED(JVM_OnExit);

}

void JVM_Exit(jint code) {
  IMPLEMENTED(JVM_Exit);
  (*getEnv())->JVM_Exit(code);
}

void JVM_Halt(jint code) {
  IMPLEMENTED(JVM_Halt);
  (*getEnv())->JVM_Halt(code);
}

void JVM_GC(void) {
  IMPLEMENTED(JVM_GC);
  (*getEnv())->JVM_GC();
}

jlong JVM_MaxObjectInspectionAge(void) {
  UNIMPLEMENTED(JVM_MaxObjectInspectionAge);
  return 0;
}

void JVM_TraceInstructions(jboolean on) {
  UNIMPLEMENTED(JVM_TraceInstructions);

}

void JVM_TraceMethodCalls(jboolean on) {
  UNIMPLEMENTED(JVM_TraceMethodCalls);

}

jlong JVM_TotalMemory(void) {
  IMPLEMENTED(JVM_TotalMemory);
  return (*getEnv())->JVM_TotalMemory();
}

jlong JVM_FreeMemory(void) {
  IMPLEMENTED(JVM_FreeMemory);
  return (*getEnv())->JVM_FreeMemory();
}

jlong JVM_MaxMemory(void) {
  IMPLEMENTED(JVM_MaxMemory);
  return (*getEnv())->JVM_MaxMemory();
}

jint JVM_ActiveProcessorCount(void) {
  IMPLEMENTED(JVM_ActiveProcessorCount);
  return (*getEnv())->JVM_ActiveProcessorCount();
}

void *JVM_LoadLibrary(const char *name) {
  IMPLEMENTED(JVM_LoadLibrary);
  return (*getEnv())->JVM_LoadLibrary(name);
}

void JVM_UnloadLibrary(void *handle) {
  IMPLEMENTED(JVM_UnloadLibrary);
  (*getEnv())->JVM_UnloadLibrary(handle);
}

void *JVM_FindLibraryEntry(void *handle, const char *name) {
  IMPLEMENTED(JVM_FindLibraryEntry);
  return (*getEnv())->JVM_FindLibraryEntry(handle, name);
}

jboolean JVM_IsSupportedJNIVersion(jint version) {
  IMPLEMENTED(JVM_IsSupportedJNIVersion);
  return (*getEnv())->JVM_IsSupportedJNIVersion(version);
}

jboolean JVM_IsNaN(jdouble d) {
  IMPLEMENTED(JVM_IsNaN);
  return (*getEnv())->JVM_IsNaN(d);
}

void JVM_FillInStackTrace(JNIEnv *env, jobject throwable) {
  IMPLEMENTED(JVM_FillInStackTrace);
  (*getEnv())->JVM_FillInStackTrace(env, throwable);
}

jint JVM_GetStackTraceDepth(JNIEnv *env, jobject throwable) {
  IMPLEMENTED(JVM_GetStackTraceDepth);
  return (*getEnv())->JVM_GetStackTraceDepth(env, throwable);
}

jobject JVM_GetStackTraceElement(JNIEnv *env, jobject throwable, jint index) {
  IMPLEMENTED(JVM_GetStackTraceElement);
  return (*getEnv())->JVM_GetStackTraceElement(env, throwable, index);
}

void JVM_InitializeCompiler(JNIEnv *env, jclass compCls) {
  UNIMPLEMENTED(JVM_InitializeCompiler);

}

jboolean JVM_IsSilentCompiler(JNIEnv *env, jclass compCls) {
  UNIMPLEMENTED(JVM_IsSilentCompiler);
  return 0;
}

jboolean JVM_CompileClass(JNIEnv *env, jclass compCls, jclass cls) {
  IMPLEMENTED(JVM_CompileClass);
  // According to hotspot:

	// java.lang.Compiler //
	/** The initial cuts of the HotSpot VM will not support JITs, and all existing
	 * JITs would need extensive changes to work with HotSpot.  The JIT-related JVM
	 * functions are all silently ignored unless JVM warnings are printed.
	 */
  return 0;
}

jboolean JVM_CompileClasses(JNIEnv *env, jclass cls, jstring jname) {
  UNIMPLEMENTED(JVM_CompileClasses);
  return 0;
}

jobject JVM_CompilerCommand(JNIEnv *env, jclass compCls, jobject arg) {
  UNIMPLEMENTED(JVM_CompilerCommand);
  return NULL;
}

void JVM_EnableCompiler(JNIEnv *env, jclass compCls) {
  UNIMPLEMENTED(JVM_EnableCompiler);

}

void JVM_DisableCompiler(JNIEnv *env, jclass compCls) {
  UNIMPLEMENTED(JVM_DisableCompiler);

}

void JVM_StartThread(JNIEnv *env, jobject thread) {
  UNIMPLEMENTED(JVM_StartThread);

}

void JVM_StopThread(JNIEnv *env, jobject thread, jobject exception) {
  UNIMPLEMENTED(JVM_StopThread);

}

jboolean JVM_IsThreadAlive(JNIEnv *env, jobject thread) {
  UNIMPLEMENTED(JVM_IsThreadAlive);
  return 0;
}

void JVM_SuspendThread(JNIEnv *env, jobject thread) {
  UNIMPLEMENTED(JVM_SuspendThread);

}

void JVM_ResumeThread(JNIEnv *env, jobject thread) {
  UNIMPLEMENTED(JVM_ResumeThread);

}

void JVM_SetThreadPriority(JNIEnv *env, jobject thread, jint prio) {
  UNIMPLEMENTED(JVM_SetThreadPriority);

}

void JVM_Yield(JNIEnv *env, jclass threadClass) {
  UNIMPLEMENTED(JVM_Yield);

}

void JVM_Sleep(JNIEnv *env, jclass threadClass, jlong millis) {
  UNIMPLEMENTED(JVM_Sleep);

}

jobject JVM_CurrentThread(JNIEnv *env, jclass threadClass) {
  UNIMPLEMENTED(JVM_CurrentThread);
  return NULL;
}

jint JVM_CountStackFrames(JNIEnv *env, jobject thread) {
  UNIMPLEMENTED(JVM_CountStackFrames);
  return 0;
}

void JVM_Interrupt(JNIEnv *env, jobject thread) {
  UNIMPLEMENTED(JVM_Interrupt);

}

jboolean JVM_IsInterrupted(JNIEnv *env, jobject thread, jboolean clearInterrupted) {
  UNIMPLEMENTED(JVM_IsInterrupted);
  return 0;
}

jboolean JVM_HoldsLock(JNIEnv *env, jclass threadClass, jobject obj) {
  UNIMPLEMENTED(JVM_HoldsLock);
  return 0;
}

void JVM_DumpAllStacks(JNIEnv *env, jclass unused) {
  UNIMPLEMENTED(JVM_DumpAllStacks);

}

jobjectArray JVM_GetAllThreads(JNIEnv *env, jclass dummy) {
  IMPLEMENTED(JVM_GetAllThreads);
  return (*getEnv())->JVM_GetAllThreads(env, dummy);
}

void JVM_SetNativeThreadName(JNIEnv *env, jobject jthread, jstring name) {
  UNIMPLEMENTED(JVM_SetNativeThreadName);

}

jobjectArray JVM_DumpThreads(JNIEnv *env, jclass threadClass, jobjectArray threads) {
  UNIMPLEMENTED(JVM_DumpThreads);
  return NULL;
}

jclass JVM_CurrentLoadedClass(JNIEnv *env) {
  IMPLEMENTED(JVM_CurrentLoadedClass);
  return (*getEnv())->JVM_CurrentLoadedClass(env);
}

jobject JVM_CurrentClassLoader(JNIEnv *env) {
  IMPLEMENTED(JVM_CurrentClassLoader);
  return (*getEnv())->JVM_CurrentClassLoader(env);
}

jobjectArray JVM_GetClassContext(JNIEnv *env) {
  IMPLEMENTED(JVM_GetClassContext);
  return (*getEnv())->JVM_GetClassContext(env);
}

jint JVM_ClassDepth(JNIEnv *env, jstring name) {
  IMPLEMENTED(JVM_ClassDepth);
  return (*getEnv())->JVM_ClassDepth(env, name);
}

jint JVM_ClassLoaderDepth(JNIEnv *env) {
  IMPLEMENTED(JVM_ClassLoaderDepth);
  return (*getEnv())->JVM_ClassLoaderDepth(env);
}

jstring JVM_GetSystemPackage(JNIEnv *env, jstring name) {
  IMPLEMENTED(JVM_GetSystemPackage);
  return (*getEnv())->JVM_GetSystemPackage(env, name);
}

jobjectArray JVM_GetSystemPackages(JNIEnv *env) {
  IMPLEMENTED(JVM_GetSystemPackages);
  return (*getEnv())->JVM_GetSystemPackages(env);
}

jobject JVM_AllocateNewObject(JNIEnv *env, jobject obj, jclass currClass, jclass initClass) {
  UNIMPLEMENTED(JVM_AllocateNewObject);
  return NULL;
}

jobject JVM_AllocateNewArray(JNIEnv *env, jobject obj, jclass currClass, jint length) {
  UNIMPLEMENTED(JVM_AllocateNewArray);
  return NULL;
}

jobject JVM_LatestUserDefinedLoader(JNIEnv *env) {
  IMPLEMENTED(JVM_LatestUserDefinedLoader);
  return (*getEnv())->JVM_LatestUserDefinedLoader(env);
}

jclass JVM_LoadClass0(JNIEnv *env, jobject obj, jclass currClass, jstring currClassName) {
  UNIMPLEMENTED(JVM_LoadClass0);
  return NULL;
}

jint JVM_GetArrayLength(JNIEnv *env, jobject arr) {
  IMPLEMENTED(JVM_GetArrayLength);
  return (*getEnv())->JVM_GetArrayLength(env, arr);
}

jobject JVM_GetArrayElement(JNIEnv *env, jobject arr, jint index) {
  IMPLEMENTED(JVM_GetArrayElement);
  return (*getEnv())->JVM_GetArrayElement(env, arr, index);
}

jvalue JVM_GetPrimitiveArrayElement(JNIEnv *env, jobject arr, jint index, jint wCode) {
  jvalue result = {0};
  UNIMPLEMENTED(JVM_GetPrimitiveArrayElement);  
  return result;
}

void JVM_SetArrayElement(JNIEnv *env, jobject arr, jint index, jobject val) {
  UNIMPLEMENTED(JVM_SetArrayElement);

}

void JVM_SetPrimitiveArrayElement(JNIEnv *env, jobject arr, jint index, jvalue v, unsigned char vCode) {
  UNIMPLEMENTED(JVM_SetPrimitiveArrayElement);

}

jobject JVM_NewArray(JNIEnv *env, jclass eltClass, jint length) {
  UNIMPLEMENTED(JVM_NewArray);
  return NULL;
}

jobject JVM_NewMultiArray(JNIEnv *env, jclass eltClass, jintArray dim) {
  UNIMPLEMENTED(JVM_NewMultiArray);
  return NULL;
}

jclass JVM_GetCallerClass(JNIEnv *env, int depth) {
  IMPLEMENTED(JVM_GetCallerClass);
  return (*getEnv())->JVM_GetCallerClass(env, depth);
}

jclass JVM_FindPrimitiveClass(JNIEnv *env, const char *utf) {
  UNIMPLEMENTED(JVM_FindPrimitiveClass);
  return NULL;
}

void JVM_ResolveClass(JNIEnv *env, jclass cls) {
  IMPLEMENTED(JVM_ResolveClass);
  /* nop */
  // Java classes are already resoled. Legacy method, even hotspot emits a warning on use.
}

jclass JVM_FindClassFromBootLoader(JNIEnv *env, const char *name) {
  IMPLEMENTED(JVM_FindClassFromBootLoader);
  return (*getEnv())->JVM_FindClassFromBootLoader(env, name);
}

jclass JVM_FindClassFromCaller(JNIEnv *env, const char *name, jboolean init, jobject loader, jclass caller) {
  UNIMPLEMENTED(JVM_FindClassFromCaller);
  return NULL;
}

jclass JVM_FindClassFromClassLoader(JNIEnv *env, const char *name, jboolean init, jobject loader, jboolean throwError) {
  UNIMPLEMENTED(JVM_FindClassFromClassLoader);
  return NULL;
}

jclass JVM_FindClassFromClass(JNIEnv *env, const char *name, jboolean init, jclass from) {
  UNIMPLEMENTED(JVM_FindClassFromClass);
  return NULL;
}

jclass JVM_FindLoadedClass(JNIEnv *env, jobject loader, jstring name) {
  IMPLEMENTED(JVM_FindLoadedClass);
  return (*getEnv())->JVM_FindLoadedClass(env, loader, name);
}

jclass JVM_DefineClass(JNIEnv *env, const char *name, jobject loader, const jbyte *buf, jsize len, jobject pd) {
  IMPLEMENTED(JVM_DefineClass);
  return (*getEnv())->JVM_DefineClass(env, name, loader, buf, len, pd);
}

jclass JVM_DefineClassWithSource(JNIEnv *env, const char *name, jobject loader, const jbyte *buf, jsize len, jobject pd,
                                 const char *source) {
  IMPLEMENTED(JVM_DefineClassWithSource);
  return (*getEnv())->JVM_DefineClassWithSource(env, name, loader, buf, len, pd, source);
}

jstring JVM_GetClassName(JNIEnv *env, jclass cls) {
  UNIMPLEMENTED(JVM_GetClassName);
  return NULL;
}

jobjectArray JVM_GetClassInterfaces(JNIEnv *env, jclass cls) {
  UNIMPLEMENTED(JVM_GetClassInterfaces);
  return NULL;
}

jboolean JVM_IsInterface(JNIEnv *env, jclass cls) {
  UNIMPLEMENTED(JVM_IsInterface);
  return 0;
}

jobjectArray JVM_GetClassSigners(JNIEnv *env, jclass cls) {
  UNIMPLEMENTED(JVM_GetClassSigners);
  return NULL;
}

void JVM_SetClassSigners(JNIEnv *env, jclass cls, jobjectArray signers) {
  UNIMPLEMENTED(JVM_SetClassSigners);

}

jobject JVM_GetProtectionDomain(JNIEnv *env, jclass cls) {
  UNIMPLEMENTED(JVM_GetProtectionDomain);
  return NULL;
}

jboolean JVM_IsArrayClass(JNIEnv *env, jclass cls) {
  UNIMPLEMENTED(JVM_IsArrayClass);
  return 0;
}

jboolean JVM_IsPrimitiveClass(JNIEnv *env, jclass cls) {
  UNIMPLEMENTED(JVM_IsPrimitiveClass);
  return 0;
}

jclass JVM_GetComponentType(JNIEnv *env, jclass cls) {
  UNIMPLEMENTED(JVM_GetComponentType);
  return NULL;
}

jint JVM_GetClassModifiers(JNIEnv *env, jclass cls) {
  IMPLEMENTED(JVM_GetClassModifiers);
  return (*getEnv())->JVM_GetClassModifiers(env, cls);
}

jobjectArray JVM_GetDeclaredClasses(JNIEnv *env, jclass ofClass) {
  UNIMPLEMENTED(JVM_GetDeclaredClasses);
  return NULL;
}

jclass JVM_GetDeclaringClass(JNIEnv *env, jclass ofClass) {
  UNIMPLEMENTED(JVM_GetDeclaringClass);
  return NULL;
}

jstring JVM_GetClassSignature(JNIEnv *env, jclass cls) {
  UNIMPLEMENTED(JVM_GetClassSignature);
  return NULL;
}

jbyteArray JVM_GetClassAnnotations(JNIEnv *env, jclass cls) {
  UNIMPLEMENTED(JVM_GetClassAnnotations);
  return NULL;
}

jbyteArray JVM_GetClassTypeAnnotations(JNIEnv *env, jclass cls) {
  UNIMPLEMENTED(JVM_GetClassTypeAnnotations);
  return NULL;
}

jbyteArray JVM_GetFieldTypeAnnotations(JNIEnv *env, jobject field) {
  IMPLEMENTED(JVM_GetFieldTypeAnnotations);
  return (*getEnv())->JVM_GetFieldTypeAnnotations(env, field);
}

jbyteArray JVM_GetMethodTypeAnnotations(JNIEnv *env, jobject method) {
  IMPLEMENTED(JVM_GetMethodTypeAnnotations);
  return (*getEnv())->JVM_GetMethodTypeAnnotations(env, method);
}

jobjectArray JVM_GetClassDeclaredMethods(JNIEnv *env, jclass ofClass, jboolean publicOnly) {
  UNIMPLEMENTED(JVM_GetClassDeclaredMethods);
  return NULL;
}

jobjectArray JVM_GetClassDeclaredFields(JNIEnv *env, jclass ofClass, jboolean publicOnly) {
  UNIMPLEMENTED(JVM_GetClassDeclaredFields);
  return NULL;
}

jobjectArray JVM_GetClassDeclaredConstructors(JNIEnv *env, jclass ofClass, jboolean publicOnly) {
  UNIMPLEMENTED(JVM_GetClassDeclaredConstructors);
  return NULL;
}

jint JVM_GetClassAccessFlags(JNIEnv *env, jclass cls) {
  IMPLEMENTED(JVM_GetClassAccessFlags);
  return (*getEnv())->JVM_GetClassAccessFlags(env, cls);
}

jobject JVM_InvokeMethod(JNIEnv *env, jobject method, jobject obj, jobjectArray args0) {
  UNIMPLEMENTED(JVM_InvokeMethod);
  return NULL;
}

jobject JVM_NewInstanceFromConstructor(JNIEnv *env, jobject c, jobjectArray args0) {
  IMPLEMENTED(JVM_NewInstanceFromConstructor);
  return (*getEnv())->JVM_NewInstanceFromConstructor(env, c, args0);
}

jobject JVM_GetClassConstantPool(JNIEnv *env, jclass cls) {
  UNIMPLEMENTED(JVM_GetClassConstantPool);
  return NULL;
}

jint JVM_ConstantPoolGetSize(JNIEnv *env, jobject unused, jobject jcpool) {
  IMPLEMENTED(JVM_ConstantPoolGetSize);
  return (*getEnv())->JVM_ConstantPoolGetSize(env, unused, jcpool);
}

jclass JVM_ConstantPoolGetClassAt(JNIEnv *env, jobject unused, jobject jcpool, jint index) {
  IMPLEMENTED(JVM_ConstantPoolGetClassAt);
  return (*getEnv())->JVM_ConstantPoolGetClassAt(env, unused, jcpool, index);
}

jclass JVM_ConstantPoolGetClassAtIfLoaded(JNIEnv *env, jobject unused, jobject jcpool, jint index) {
  UNIMPLEMENTED(JVM_ConstantPoolGetClassAtIfLoaded);
  return NULL;
}

jobject JVM_ConstantPoolGetMethodAt(JNIEnv *env, jobject unused, jobject jcpool, jint index) {
  UNIMPLEMENTED(JVM_ConstantPoolGetMethodAt);
  return NULL;
}

jobject JVM_ConstantPoolGetMethodAtIfLoaded(JNIEnv *env, jobject unused, jobject jcpool, jint index) {
  UNIMPLEMENTED(JVM_ConstantPoolGetMethodAtIfLoaded);
  return NULL;
}

jobject JVM_ConstantPoolGetFieldAt(JNIEnv *env, jobject unused, jobject jcpool, jint index) {
  UNIMPLEMENTED(JVM_ConstantPoolGetFieldAt);
  return NULL;
}

jobject JVM_ConstantPoolGetFieldAtIfLoaded(JNIEnv *env, jobject unused, jobject jcpool, jint index) {
  UNIMPLEMENTED(JVM_ConstantPoolGetFieldAtIfLoaded);
  return NULL;
}

jobjectArray JVM_ConstantPoolGetMemberRefInfoAt(JNIEnv *env, jobject unused, jobject jcpool, jint index) {
  UNIMPLEMENTED(JVM_ConstantPoolGetMemberRefInfoAt);
  return NULL;
}

jint JVM_ConstantPoolGetIntAt(JNIEnv *env, jobject unused, jobject jcpool, jint index) {
  IMPLEMENTED(JVM_ConstantPoolGetIntAt);
  return (*getEnv())->JVM_ConstantPoolGetIntAt(env, unused, jcpool, index);
}

jlong JVM_ConstantPoolGetLongAt(JNIEnv *env, jobject unused, jobject jcpool, jint index) {
  IMPLEMENTED(JVM_ConstantPoolGetLongAt);
  return (*getEnv())->JVM_ConstantPoolGetLongAt(env, unused, jcpool, index);
}

jfloat JVM_ConstantPoolGetFloatAt(JNIEnv *env, jobject unused, jobject jcpool, jint index) {
  IMPLEMENTED(JVM_ConstantPoolGetFloatAt);
  return (*getEnv())->JVM_ConstantPoolGetFloatAt(env, unused, jcpool, index);
}

jdouble JVM_ConstantPoolGetDoubleAt(JNIEnv *env, jobject unused, jobject jcpool, jint index) {
  IMPLEMENTED(JVM_ConstantPoolGetDoubleAt);
  return (*getEnv())->JVM_ConstantPoolGetDoubleAt(env, unused, jcpool, index);
}

jstring JVM_ConstantPoolGetStringAt(JNIEnv *env, jobject unused, jobject jcpool, jint index) {
  IMPLEMENTED(JVM_ConstantPoolGetStringAt);
  return (*getEnv())->JVM_ConstantPoolGetStringAt(env, unused, jcpool, index);
}

jstring JVM_ConstantPoolGetUTF8At(JNIEnv *env, jobject unused, jobject jcpool, jint index) {
  IMPLEMENTED(JVM_ConstantPoolGetUTF8At);
  return (*getEnv())->JVM_ConstantPoolGetUTF8At(env, unused, jcpool, index);
}

jobjectArray JVM_GetMethodParameters(JNIEnv *env, jobject method) {
  IMPLEMENTED(JVM_GetMethodParameters);
  return (*getEnv())->JVM_GetMethodParameters(env, method);
}

jobject JVM_DoPrivileged(JNIEnv *env, jclass cls, jobject action, jobject context, jboolean wrapException) {
  IMPLEMENTED(JVM_DoPrivileged);
  return (*getEnv())->JVM_DoPrivileged(env, cls, action, context, wrapException);
}

jobject JVM_GetInheritedAccessControlContext(JNIEnv *env, jclass cls) {
  IMPLEMENTED(JVM_GetInheritedAccessControlContext);
  return (*getEnv())->JVM_GetInheritedAccessControlContext(env, cls);
}

jobject JVM_GetStackAccessControlContext(JNIEnv *env, jclass cls) {
  IMPLEMENTED(JVM_GetStackAccessControlContext);
  return (*getEnv())->JVM_GetStackAccessControlContext(env, cls);
}

void *JVM_RegisterSignal(jint sig, void *handler) {
  UNIMPLEMENTED(JVM_RegisterSignal);
  return NULL;
}

jboolean JVM_RaiseSignal(jint sig) {
  UNIMPLEMENTED(JVM_RaiseSignal);
  return 0;
}

jint JVM_FindSignal(const char *name) {
  UNIMPLEMENTED(JVM_FindSignal);
  return 0;
}

jboolean JVM_DesiredAssertionStatus(JNIEnv *env, jclass unused, jclass cls) {
  IMPLEMENTED(JVM_DesiredAssertionStatus);
  return (*getEnv())->JVM_DesiredAssertionStatus(env, unused, cls);
}

jobject JVM_AssertionStatusDirectives(JNIEnv *env, jclass unused) {
  IMPLEMENTED(JVM_AssertionStatusDirectives);
  return (*getEnv())->JVM_AssertionStatusDirectives(env, unused);
}

jboolean JVM_SupportsCX8(void) {
  IMPLEMENTED(JVM_SupportsCX8);
  return (*getEnv())->JVM_SupportsCX8();
}

jint JVM_DTraceGetVersion(JNIEnv *env) {
  UNIMPLEMENTED(JVM_DTraceGetVersion);
  return 0;
}

jlong JVM_DTraceActivate(JNIEnv *env, jint version, jstring module_name, jint providers_count,
                         JVM_DTraceProvider *providers) {
  UNIMPLEMENTED(JVM_DTraceActivate);
  return 0;
}

jboolean JVM_DTraceIsProbeEnabled(JNIEnv *env, jmethodID method) {
  UNIMPLEMENTED(JVM_DTraceIsProbeEnabled);
  return 0;
}

void JVM_DTraceDispose(JNIEnv *env, jlong activation_handle) {
  UNIMPLEMENTED(JVM_DTraceDispose);

}

jboolean JVM_DTraceIsSupported(JNIEnv *env) {
  UNIMPLEMENTED(JVM_DTraceIsSupported);
  return 0;
}

const char *JVM_GetClassNameUTF(JNIEnv *env, jclass cb) {
  UNIMPLEMENTED(JVM_GetClassNameUTF);
  return NULL;
}

void JVM_GetClassCPTypes(JNIEnv *env, jclass cb, unsigned char *types) {
  UNIMPLEMENTED(JVM_GetClassCPTypes);

}

jint JVM_GetClassCPEntriesCount(JNIEnv *env, jclass cb) {
  UNIMPLEMENTED(JVM_GetClassCPEntriesCount);
  return 0;
}

jint JVM_GetClassFieldsCount(JNIEnv *env, jclass cb) {
  UNIMPLEMENTED(JVM_GetClassFieldsCount);
  return 0;
}

jint JVM_GetClassMethodsCount(JNIEnv *env, jclass cb) {
  UNIMPLEMENTED(JVM_GetClassMethodsCount);
  return 0;
}

void JVM_GetMethodIxExceptionIndexes(JNIEnv *env, jclass cb, jint method_index, unsigned short *exceptions) {
  UNIMPLEMENTED(JVM_GetMethodIxExceptionIndexes);

}

jint JVM_GetMethodIxExceptionsCount(JNIEnv *env, jclass cb, jint method_index) {
  UNIMPLEMENTED(JVM_GetMethodIxExceptionsCount);
  return 0;
}

void JVM_GetMethodIxByteCode(JNIEnv *env, jclass cb, jint method_index, unsigned char *code) {
  UNIMPLEMENTED(JVM_GetMethodIxByteCode);

}

jint JVM_GetMethodIxByteCodeLength(JNIEnv *env, jclass cb, jint method_index) {
  UNIMPLEMENTED(JVM_GetMethodIxByteCodeLength);
  return 0;
}

void JVM_GetMethodIxExceptionTableEntry(JNIEnv *env, jclass cb, jint method_index, jint entry_index,
                                        JVM_ExceptionTableEntryType *entry) {
  UNIMPLEMENTED(JVM_GetMethodIxExceptionTableEntry);

}

jint JVM_GetMethodIxExceptionTableLength(JNIEnv *env, jclass cb, int index) {
  UNIMPLEMENTED(JVM_GetMethodIxExceptionTableLength);
  return 0;
}

jint JVM_GetFieldIxModifiers(JNIEnv *env, jclass cb, int index) {
  UNIMPLEMENTED(JVM_GetFieldIxModifiers);
  return 0;
}

jint JVM_GetMethodIxModifiers(JNIEnv *env, jclass cb, int index) {
  UNIMPLEMENTED(JVM_GetMethodIxModifiers);
  return 0;
}

jint JVM_GetMethodIxLocalsCount(JNIEnv *env, jclass cb, int index) {
  UNIMPLEMENTED(JVM_GetMethodIxLocalsCount);
  return 0;
}

jint JVM_GetMethodIxArgsSize(JNIEnv *env, jclass cb, int index) {
  UNIMPLEMENTED(JVM_GetMethodIxArgsSize);
  return 0;
}

jint JVM_GetMethodIxMaxStack(JNIEnv *env, jclass cb, int index) {
  UNIMPLEMENTED(JVM_GetMethodIxMaxStack);
  return 0;
}

jboolean JVM_IsConstructorIx(JNIEnv *env, jclass cb, int index) {
  UNIMPLEMENTED(JVM_IsConstructorIx);
  return 0;
}

jboolean JVM_IsVMGeneratedMethodIx(JNIEnv *env, jclass cb, int index) {
  UNIMPLEMENTED(JVM_IsVMGeneratedMethodIx);
  return 0;
}

const char *JVM_GetMethodIxNameUTF(JNIEnv *env, jclass cb, jint index) {
  UNIMPLEMENTED(JVM_GetMethodIxNameUTF);
  return NULL;
}

const char *JVM_GetMethodIxSignatureUTF(JNIEnv *env, jclass cb, jint index) {
  UNIMPLEMENTED(JVM_GetMethodIxSignatureUTF);
  return NULL;
}

const char *JVM_GetCPFieldNameUTF(JNIEnv *env, jclass cb, jint index) {
  UNIMPLEMENTED(JVM_GetCPFieldNameUTF);
  return NULL;
}

const char *JVM_GetCPMethodNameUTF(JNIEnv *env, jclass cb, jint index) {
  UNIMPLEMENTED(JVM_GetCPMethodNameUTF);
  return NULL;
}

const char *JVM_GetCPMethodSignatureUTF(JNIEnv *env, jclass cb, jint index) {
  UNIMPLEMENTED(JVM_GetCPMethodSignatureUTF);
  return NULL;
}

const char *JVM_GetCPFieldSignatureUTF(JNIEnv *env, jclass cb, jint index) {
  UNIMPLEMENTED(JVM_GetCPFieldSignatureUTF);
  return NULL;
}

const char *JVM_GetCPClassNameUTF(JNIEnv *env, jclass cb, jint index) {
  UNIMPLEMENTED(JVM_GetCPClassNameUTF);
  return NULL;
}

const char *JVM_GetCPFieldClassNameUTF(JNIEnv *env, jclass cb, jint index) {
  UNIMPLEMENTED(JVM_GetCPFieldClassNameUTF);
  return NULL;
}

const char *JVM_GetCPMethodClassNameUTF(JNIEnv *env, jclass cb, jint index) {
  UNIMPLEMENTED(JVM_GetCPMethodClassNameUTF);
  return NULL;
}

jint JVM_GetCPFieldModifiers(JNIEnv *env, jclass cb, int index, jclass calledClass) {
  UNIMPLEMENTED(JVM_GetCPFieldModifiers);
  return 0;
}

jint JVM_GetCPMethodModifiers(JNIEnv *env, jclass cb, int index, jclass calledClass) {
  UNIMPLEMENTED(JVM_GetCPMethodModifiers);
  return 0;
}

void JVM_ReleaseUTF(const char *utf) {
  UNIMPLEMENTED(JVM_ReleaseUTF);

}

jboolean JVM_IsSameClassPackage(JNIEnv *env, jclass class1, jclass class2) {
  UNIMPLEMENTED(JVM_IsSameClassPackage);
  return 0;
}

jint JVM_GetLastErrorString(char *buf, int len) {
  NATIVE(JVM_GetLastErrorString);
  return os_lasterror(buf, len);
}

char *JVM_NativePath(char *pathname) {
  NATIVE(JVM_NativePath);  
  return os_native_path(pathname);
}

jint JVM_Open(const char *fname, jint flags, jint mode) {
  int result = 0;
  NATIVE(JVM_Open);
  result = os_open(fname, flags, mode);
  if (result >= 0) {
    return result;
  } else {
    switch(errno) {
      case EEXIST:
        return JVM_EEXIST;
      default:
        return -1;
    }
  }
}

jint JVM_Close(jint fd) {
  NATIVE(JVM_Close);
  return os_close(fd);
}

jint JVM_Read(jint fd, char *buf, jint nbytes) {
  UNIMPLEMENTED(JVM_Read);
  return 0;
}

jint JVM_Write(jint fd, char *buf, jint nbytes) {
  UNIMPLEMENTED(JVM_Write);
  return 0;
}

jint JVM_Available(jint fd, jlong *pbytes) {
  UNIMPLEMENTED(JVM_Available);
  return 0;
}

jlong JVM_Lseek(jint fd, jlong offset, jint whence) {
  UNIMPLEMENTED(JVM_Lseek);
  return 0;
}

jint JVM_SetLength(jint fd, jlong length) {
  UNIMPLEMENTED(JVM_SetLength);
  return 0;
}

jint JVM_Sync(jint fd) {
  UNIMPLEMENTED(JVM_Sync);
  return 0;
}

// Networking library support

jint JVM_InitializeSocketLibrary(void) {
  NATIVE(JVM_InitializeSocketLibrary);
  // Mimics HotSpot.
  return 0;
}

jint JVM_Socket(jint domain, jint type, jint protocol) {
  NATIVE(JVM_Socket);
  return os_socket(domain, type, protocol);
}

jint JVM_SocketClose(jint fd) {
  NATIVE(JVM_SocketClose);
  return os_close(fd);
}

jint JVM_SocketShutdown(jint fd, jint howto) {
  NATIVE(JVM_SocketShutdown);
  return os_socket_shutdown(fd, howto);
}

jint JVM_Recv(jint fd, char *buf, jint nBytes, jint flags) {
  NATIVE(JVM_Recv);
  return os_recv(fd, buf, (size_t)nBytes, (uint)flags);
}

jint JVM_Send(jint fd, char *buf, jint nBytes, jint flags) {
  NATIVE(JVM_Send);
  return os_send(fd, buf, (size_t)nBytes, (uint)flags);
}

jint JVM_Timeout(int fd, long timeout) {
  NATIVE(JVM_Timeout);
  return os_timeout(fd, timeout);
}

jint JVM_Listen(jint fd, jint count) {
  NATIVE(JVM_Listen);
  return os_listen(fd, count);
}

jint JVM_Connect(jint fd, struct sockaddr *him, jint len) {
  NATIVE(JVM_Connect);
  return os_connect(fd, him, len);
}

jint JVM_Bind(jint fd, struct sockaddr *him, jint len) {
  NATIVE(JVM_Bind);
  return os_bind(fd, him, (socklen_t)len);
}

jint JVM_Accept(jint fd, struct sockaddr *him, jint *len) {
  socklen_t socklen;
  jint result;
  NATIVE(JVM_Accept);
  socklen = (socklen_t)(*len);
  // Linux doc says this can't return EINTR, unlike accept() on Solaris.
  // But see attachListener_linux.cpp, LinuxAttachListener::dequeue().
  result = (int)os_accept(fd, him, &socklen);
  *len = (jint)socklen;
  return result;
}

jint JVM_RecvFrom(jint fd, char *buf, int nBytes, int flags, struct sockaddr *from, int *fromlen) {
  socklen_t socklen;
  jint result;
  NATIVE(JVM_RecvFrom);
  socklen = (socklen_t)(*fromlen);
  result = os_recvfrom(fd, buf, (size_t)nBytes, (uint)flags, from, &socklen);
  *fromlen = (int)socklen;
  return result;
}

jint JVM_SendTo(jint fd, char *buf, int len, int flags, struct sockaddr *to, int tolen) {
  NATIVE(JVM_SendTo);
  return os_sendto(fd, buf, len, flags, to, tolen);
}

jint JVM_SocketAvailable(jint fd, jint *result) {
  NATIVE(JVM_SocketAvailable);
  return os_socket_available(fd, result);  
}

jint JVM_GetSockName(jint fd, struct sockaddr *him, int *len) {
  socklen_t socklen;
  jint result;
  NATIVE(JVM_GetSockName);
  socklen = (socklen_t)(*len);
  result = os_get_sock_name(fd, him, &socklen);
  *len = (int)socklen;
  return result;
}

jint JVM_GetSockOpt(jint fd, int level, int optname, char *optval, int *optlen) {
  socklen_t socklen;
  jint result;
  NATIVE(JVM_GetSockOpt);
  socklen = (socklen_t)(*optlen);
  result = os_get_sock_opt(fd, level, optname, optval, &socklen);
  *optlen = (int)socklen;
  return result;
}

jint JVM_SetSockOpt(jint fd, int level, int optname, const char *optval, int optlen) {
  NATIVE(JVM_SetSockOpt);
  return os_set_sock_opt(fd, level, optname, optval, optlen);
}

int JVM_GetHostName(char *name, int namelen) {
  NATIVE(JVM_GetHostName);
  return os_get_host_name(name, namelen);
}

static JNIEnv* getGuestJNI() {
  JNIEnv *jniEnv = NULL;
  JavaVM *vm = (*getEnv())->vm;
  (*vm)->GetEnv(vm, (void **) &jniEnv, JNI_VERSION_1_6);
  return jniEnv;
}

void *JVM_RawMonitorCreate(void) {
  JNIEnv* jniEnv;
  jclass java_lang_Object;
  jmethodID constructor;
  jobject lock;
  NATIVE(JVM_RawMonitorCreate);
  // TODO(peterssen): Cache class and method.
  jniEnv = getGuestJNI();
  java_lang_Object = (*jniEnv)->FindClass(jniEnv, "java/lang/Object");
  constructor = (*jniEnv)->GetMethodID(jniEnv, java_lang_Object, "<init>", "()V");
  lock = (*jniEnv)->NewObject(jniEnv, java_lang_Object, constructor);
  return (void*) (*jniEnv)->NewGlobalRef(jniEnv, lock);
}

void JVM_RawMonitorDestroy(void *mon) {
  jobject lock;
  JNIEnv* jniEnv;
  NATIVE(JVM_RawMonitorDestroy);
  lock = (jobject) mon;
  jniEnv = getGuestJNI();
  (*jniEnv)->DeleteGlobalRef(jniEnv, lock);
}

jint JVM_RawMonitorEnter(void *mon) {
  JNIEnv* jniEnv;
  NATIVE(JVM_RawMonitorEnter);
  jniEnv = getGuestJNI();
  return (*jniEnv)->MonitorEnter(jniEnv, (jobject) mon);
}

void JVM_RawMonitorExit(void *mon) {
  JNIEnv* jniEnv;
  NATIVE(JVM_RawMonitorExit);
  jniEnv = getGuestJNI();
  (*jniEnv)->MonitorExit(jniEnv, (jobject) mon);
}

void *JVM_GetManagement(jint version) {
  IMPLEMENTED(JVM_GetManagement);
  return (*getEnv())->JVM_GetManagement(version);
}

jobject JVM_InitAgentProperties(JNIEnv *env, jobject agent_props) {
  UNIMPLEMENTED(JVM_InitAgentProperties);
  return NULL;
}

jstring JVM_GetTemporaryDirectory(JNIEnv *env) {
  UNIMPLEMENTED(JVM_GetTemporaryDirectory);
  return NULL;
}

jobjectArray JVM_GetEnclosingMethodInfo(JNIEnv *env, jclass ofClass) {
  UNIMPLEMENTED(JVM_GetEnclosingMethodInfo);
  return NULL;
}

jintArray JVM_GetThreadStateValues(JNIEnv *env, jint javaThreadState) {
  UNIMPLEMENTED(JVM_GetThreadStateValues);
  return NULL;
}

jobjectArray JVM_GetThreadStateNames(JNIEnv *env, jint javaThreadState, jintArray values) {
  UNIMPLEMENTED(JVM_GetThreadStateNames);
  return NULL;
}

jboolean JVM_KnownToNotExist(JNIEnv *env, jobject loader, const char *classname) {
  UNIMPLEMENTED(JVM_KnownToNotExist);
  return 0;
}

jobjectArray JVM_GetResourceLookupCacheURLs(JNIEnv *env, jobject loader) {
  UNIMPLEMENTED(JVM_GetResourceLookupCacheURLs);
  return NULL;
}

jintArray JVM_GetResourceLookupCache(JNIEnv *env, jobject loader, const char *resource_name) {
  UNIMPLEMENTED(JVM_GetResourceLookupCache);
  return NULL;
}

void JVM_GetVersionInfo(JNIEnv *env, jvm_version_info *info, size_t info_size) {
  UNIMPLEMENTED(JVM_GetVersionInfo);
}

void JVM_CopySwapMemory(JNIEnv *env, jobject srcObj, jlong srcOffset,
									 jobject dstObj, jlong dstOffset,
									 jlong size,     jlong elemSize) {
  UNIMPLEMENTED(JVM_CopySwapMemory);
}

jboolean JVM_CX8Field(JNIEnv *env, jobject obj, jfieldID fid, jlong oldVal, jlong newVal) {
  UNIMPLEMENTED(JVM_CX8Field);
  return 0;
}

jclass JVM_DefineClassWithSourceCond(JNIEnv *env, const char *name,
                                                  jobject loader, const jbyte *buf,
                                                  jsize len, jobject pd,
                                                  const char *source, jboolean verify) {
  UNIMPLEMENTED(JVM_DefineClassWithSourceCond);
  return NULL;
}

jobject JVM_GetClassLoader(JNIEnv *env, jclass cls) {
  UNIMPLEMENTED(JVM_GetClassLoader);
  return NULL;
}

jbyteArray JVM_GetFieldAnnotations(JNIEnv *env, jobject field) {
  UNIMPLEMENTED(JVM_GetFieldAnnotations);
  return NULL;
}

jbyteArray JVM_GetMethodAnnotations(JNIEnv *env, jobject method) {
  UNIMPLEMENTED(JVM_GetMethodAnnotations);
  return NULL;
}

jbyteArray JVM_GetMethodDefaultAnnotationValue(JNIEnv *env, jobject method) {
  UNIMPLEMENTED(JVM_GetMethodDefaultAnnotationValue);
  return NULL;
}

jbyteArray JVM_GetMethodParameterAnnotations(JNIEnv *env, jobject method) {
  UNIMPLEMENTED(JVM_GetMethodParameterAnnotations);
  return NULL;
}

// region Invocation API

jint JNI_GetCreatedJavaVMs(JavaVM **vm_buf, jsize buf_len, jsize *numVMs) {
    IMPLEMENTED(JNI_GetCreatedJavaVMs);
    return (*getEnv())->JNI_GetCreatedJavaVMs(vm_buf, buf_len, numVMs);
}

// endregion Invocation API

int jio_vsnprintf(char *str, size_t count, const char *fmt, va_list args) {
    NATIVE(jio_vsnprintf);
    return os_vsnprintf(str, count, fmt, args);
}

int jio_snprintf(char *str, size_t count, const char *fmt, ...) {
  int len;
  va_list args;
  NATIVE(jio_snprintf);  
  va_start(args, fmt);
  len = jio_vsnprintf(str, count, fmt, args);
  va_end(args);
  return len;
}

int jio_fprintf(FILE *file, const char *fmt, ...) {
  int len;
  va_list args;
  NATIVE(jio_fprintf);  
  va_start(args, fmt);
  len = jio_vfprintf(file, fmt, args);
  va_end(args);
  return len;
}

int jio_vfprintf(FILE *file, const char *fmt, va_list args) {
    NATIVE(jio_vfprintf);
    return vfprintf(file, fmt, args);
}
