// Copyright 2014, Yahoo! Inc.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "yahoo/cnet/android/com_yahoo_cnet_Pool.h"

#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "yahoo/cnet/android/cnet_jni.h"
#include "yahoo/cnet/cnet.h"
#include "yahoo/cnet/cnet_pool.h"

// Generated headers
#include "jni/Pool_jni.h"

namespace cnet {
namespace android {

bool PoolAdapterRegisterJni(JNIEnv* j_env) {
  // Register the generated JNI methods.
  return RegisterNativesImpl(j_env);
}

/* static */
PoolAdapter* PoolAdapter::PoolAdapterFromObject(JNIEnv* j_env, jobject j_object) {
  return reinterpret_cast<PoolAdapter*>(Java_Pool_getNativePoolAdapter(j_env,
      j_object));
}

static jlong CreatePoolAdapter(JNIEnv* j_env, jobject j_caller,
    jstring j_user_agent,
    jboolean j_enable_spdy, jboolean j_enable_quic,
    jboolean j_enable_ssl_false_start,
    jstring j_cache_path, jint j_cache_max_bytes,
    jboolean j_trust_all_cert_authorities, jboolean j_disable_system_proxy,
    jint j_log_level) {
  scoped_refptr<base::SingleThreadTaskRunner> ui_runner;
  if (CnetMessageLoopForUiGet() != NULL) {
    ui_runner = reinterpret_cast<base::MessageLoopForUI*>(
        CnetMessageLoopForUiGet())->task_runner();
  }

  std::string user_agent = (j_user_agent == NULL) ? "" :
    base::android::ConvertJavaStringToUTF8(j_env, j_user_agent);
  std::string cache_path = (j_cache_path == NULL) ? "" :
    base::android::ConvertJavaStringToUTF8(j_env, j_cache_path);

  cnet::Pool::Config pool_config;
  pool_config.user_agent = user_agent;
  pool_config.enable_spdy = j_enable_spdy;
  pool_config.enable_quic = j_enable_quic;
  pool_config.enable_ssl_false_start = j_enable_ssl_false_start;
  pool_config.disable_system_proxy = j_disable_system_proxy;
  pool_config.cache_path = base::FilePath(cache_path);
  pool_config.cache_max_bytes = j_cache_max_bytes;
  pool_config.trust_all_cert_authorities = j_trust_all_cert_authorities;
  pool_config.log_level = j_log_level;
  scoped_refptr<cnet::Pool> pool(new cnet::Pool(ui_runner, pool_config));

  PoolAdapter* pool_adapter = new PoolAdapter(pool);
  return reinterpret_cast<jlong>(pool_adapter);
}

PoolAdapter::PoolAdapter(scoped_refptr<cnet::Pool> pool)
    : pool_(pool) {
}

PoolAdapter::~PoolAdapter() {
}

void PoolAdapter::ReleasePoolAdapter(JNIEnv* j_env, jobject j_caller) {
  delete this;
}

void PoolAdapter::SetProxyRules(JNIEnv* j_env, jobject j_caller,
    jstring j_rules) {
  std::string rules = base::android::ConvertJavaStringToUTF8(j_env, j_rules);
  pool_->SetProxyConfig(rules);
}

void PoolAdapter::SetTrustAllCertAuthorities(JNIEnv* j_env, jobject j_caller,
    jboolean j_value) {
  pool_->SetTrustAllCertAuthorities(j_value);
}

jboolean PoolAdapter::GetTrustAllCertAuthorities(JNIEnv* j_env,
    jobject j_caller) {
  return pool_->trust_all_cert_authorities();
}

void PoolAdapter::SetEnableSslFalseStart(JNIEnv* j_env, jobject j_caller,
    jboolean j_value) {
  pool_->SetEnableSslFalseStart(j_value);
}

jboolean PoolAdapter::GetEnableSslFalseStart(JNIEnv* j_env, jobject j_caller) {
  return pool_->get_ssl_false_start();
}

} // namespace android
} // namespace cnet

