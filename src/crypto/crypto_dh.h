#ifndef SRC_CRYPTO_CRYPTO_DH_H_
#define SRC_CRYPTO_CRYPTO_DH_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "crypto/crypto_keys.h"
#include "crypto/crypto_keygen.h"
#include "crypto/crypto_util.h"
#include "env.h"
#include "memory_tracker.h"
#include "v8.h"

#include <variant>

namespace node {
namespace crypto {
class DiffieHellman final : public BaseObject {
 public:
  static void Initialize(Environment* env, v8::Local<v8::Object> target);
  static void RegisterExternalReferences(ExternalReferenceRegistry* registry);

  DiffieHellman(Environment* env, v8::Local<v8::Object> wrap, DHPointer dh);
  operator DHPointer&() { return dh_; }

  void MemoryInfo(MemoryTracker* tracker) const override;
  SET_MEMORY_INFO_NAME(DiffieHellman)
  SET_SELF_SIZE(DiffieHellman)

 private:
  DHPointer dh_;
};

struct DhKeyPairParams final : public MemoryRetainer {
  // Diffie-Hellman can either generate keys using a fixed prime, or by first
  // generating a random prime of a given size (in bits). Only one of both
  // options may be specified.
  std::variant<BignumPointer, int> prime;
  unsigned int generator;
  SET_NO_MEMORY_INFO()
  SET_MEMORY_INFO_NAME(DhKeyPairParams)
  SET_SELF_SIZE(DhKeyPairParams)
};

using DhKeyPairGenConfig = KeyPairGenConfig<DhKeyPairParams>;

struct DhKeyGenTraits final {
  using AdditionalParameters = DhKeyPairGenConfig;
  static constexpr const char* JobName = "DhKeyPairGenJob";

  static EVPKeyCtxPointer Setup(DhKeyPairGenConfig* params);

  static v8::Maybe<bool> AdditionalConfig(
      CryptoJobMode mode,
      const v8::FunctionCallbackInfo<v8::Value>& args,
      unsigned int* offset,
      DhKeyPairGenConfig* params);
};

using DHKeyPairGenJob = KeyGenJob<KeyPairGenTraits<DhKeyGenTraits>>;

struct DHKeyExportConfig final : public MemoryRetainer {
  SET_NO_MEMORY_INFO()
  SET_MEMORY_INFO_NAME(DHKeyExportConfig)
  SET_SELF_SIZE(DHKeyExportConfig)
};

struct DHKeyExportTraits final {
  static constexpr const char* JobName = "DHKeyExportJob";
  using AdditionalParameters = DHKeyExportConfig;

  static v8::Maybe<bool> AdditionalConfig(
      const v8::FunctionCallbackInfo<v8::Value>& args,
      unsigned int offset,
      DHKeyExportConfig* config);

  static WebCryptoKeyExportStatus DoExport(
      std::shared_ptr<KeyObjectData> key_data,
      WebCryptoKeyFormat format,
      const DHKeyExportConfig& params,
      ByteSource* out);
};

using DHKeyExportJob = KeyExportJob<DHKeyExportTraits>;

struct DHBitsConfig final : public MemoryRetainer {
  std::shared_ptr<KeyObjectData> private_key;
  std::shared_ptr<KeyObjectData> public_key;
  SET_NO_MEMORY_INFO()
  SET_MEMORY_INFO_NAME(DHBitsConfig)
  SET_SELF_SIZE(DHBitsConfig)
};

struct DHBitsTraits final {
  using AdditionalParameters = DHBitsConfig;
  static constexpr const char* JobName = "DHBitsJob";
  static constexpr AsyncWrap::ProviderType Provider =
      AsyncWrap::PROVIDER_DERIVEBITSREQUEST;

  static v8::Maybe<bool> AdditionalConfig(
      CryptoJobMode mode,
      const v8::FunctionCallbackInfo<v8::Value>& args,
      unsigned int offset,
      DHBitsConfig* params);

  static bool DeriveBits(
      Environment* env,
      const DHBitsConfig& params,
      ByteSource* out_);

  static v8::Maybe<bool> EncodeOutput(
      Environment* env,
      const DHBitsConfig& params,
      ByteSource* out,
      v8::Local<v8::Value>* result);
};

using DHBitsJob = DeriveBitsJob<DHBitsTraits>;

v8::Maybe<bool> GetDhKeyDetail(
    Environment* env,
    std::shared_ptr<KeyObjectData> key,
    v8::Local<v8::Object> target);

}  // namespace crypto
}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS
#endif  // SRC_CRYPTO_CRYPTO_DH_H_
