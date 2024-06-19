#include <napi.h>

#include <string>
#include <Windows.h>

class CredentialManager : public Napi::ObjectWrap<CredentialManager>
{
public:
    CredentialManager(const Napi::CallbackInfo &info);
    ~CredentialManager();
    static Napi::Function GetClass(Napi::Env env);

private:
    Napi::Value GetCredential(const Napi::CallbackInfo &info);
    Napi::Value StoreCredential(const Napi::CallbackInfo &info);
    Napi::Value DeleteCredential(const Napi::CallbackInfo &info);
    Napi::Value ListCredentials(const Napi::CallbackInfo &info);
    
    bool ParseCredentialType(const std::string &credentialType, WORD *type);
    bool ParseCredentialPersistence(const std::string &credentialPersistence, WORD *persistence);
};