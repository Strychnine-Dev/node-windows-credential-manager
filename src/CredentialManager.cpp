#pragma comment(lib, "node")

#include <napi.h>

#include <string>
#include <windows.h>

#include "CredentialManager.h"
#include "CredentialManagerImpl.h"
#include "Conversions.h"

CredentialManager::CredentialManager(const Napi::CallbackInfo &info) : Napi::ObjectWrap<CredentialManager>(info) {}

CredentialManager::~CredentialManager() {}

Napi::Value CredentialManager::GetCredential(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (!(info[0].IsString()) &&
        !(info[0].IsString() && info[1].IsString()))
    {
        Napi::TypeError::New(env, "A target name must be provided.").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    bool credentialTypeProvided = info[1].IsString();
    std::string strCredType = credentialTypeProvided ? info[1].As<Napi::String>().Utf8Value() : "GENERIC";

    WORD credentialType;
    if (!ParseCredentialType(strCredType, &credentialType))
    {
        Napi::TypeError::New(env, "Unsupported Credential Type: " + strCredType).ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    LPWSTR target = ConvertStringToLPWSTR(info[0].As<Napi::String>().Utf8Value());
    return GetCredentialImpl(env, target, credentialType);
}

Napi::Value CredentialManager::StoreCredential(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (!(info[0].IsString() && info[1].IsString() && info[2].IsString()) &&
        !(info[0].IsString() && info[1].IsString() && info[2].IsString() && info[3].IsString()) &&
        !(info[0].IsString() && info[1].IsString() && info[2].IsString() && info[3].IsString() && info[4].IsString()))
    {
        Napi::TypeError::New(env, "A target name, username, and password must be provided.").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    LPWSTR target = ConvertStringToLPWSTR(info[0].As<Napi::String>().Utf8Value());
    LPWSTR username = ConvertStringToLPWSTR(info[1].As<Napi::String>().Utf8Value());
    char* password = ConvertStringToChar(info[2].As<Napi::String>().Utf8Value());

    bool credentialTypeProvided = info[3].IsString();
    std::string strCredType = credentialTypeProvided ? info[3].As<Napi::String>().Utf8Value() : "GENERIC";

    WORD credentialType;
    if (!ParseCredentialType(strCredType, &credentialType))
    {
        Napi::TypeError::New(env, "Unsupported Credential Type: " + strCredType).ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    bool persistenceProvided = info[4].IsString();
    std::string strPersistence = persistenceProvided ? info[4].As<Napi::String>().Utf8Value() : "PERSIST_LOCAL_MACHINE";

    WORD persistence;
    if (!ParseCredentialPersistence(strPersistence, &persistence))
    {
        Napi::TypeError::New(env, "Unsupported Persistence Type: " + strPersistence).ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    bool result = StoreCredentialImpl(target, username, password, credentialType, persistence);
    if (!result)
    {
        Napi::Error::New(env, "Could not save credential").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Boolean::New(env, result);
}

Napi::Value CredentialManager::DeleteCredential(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (!(info[0].IsString()) &&
        !(info[0].IsString() && info[1].IsString()))
    {
        Napi::TypeError::New(env, "A target name must be provided.").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    bool credentialTypeProvided = info[1].IsString();
    std::string strCredType = credentialTypeProvided ? info[1].As<Napi::String>().Utf8Value() : "GENERIC";

    WORD credentialType;
    if (!ParseCredentialType(strCredType, &credentialType))
    {
        Napi::TypeError::New(env, "Unsupported Credential Type: " + strCredType).ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    try
    {
        LPWSTR target = ConvertStringToLPWSTR(info[0].As<Napi::String>().Utf8Value());
        bool result = DeleteCredentialImpl(target, credentialType);

        return Napi::Boolean::New(env, result);
    }
    catch (...)
    {
        return env.Null();
    }
}

Napi::Value CredentialManager::ListCredentials(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    return ListCredentialsImpl(env);
}

Napi::Function CredentialManager::GetClass(Napi::Env env)
{
    return DefineClass(env, "CredentialManager", {
                                            CredentialManager::InstanceMethod("getCredential", &CredentialManager::GetCredential),
                                            CredentialManager::InstanceMethod("storeCredential", &CredentialManager::StoreCredential),
                                            CredentialManager::InstanceMethod("deleteCredential", &CredentialManager::DeleteCredential),
                                            CredentialManager::InstanceMethod("listCredentials", &CredentialManager::ListCredentials),
                                        });
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::String name = Napi::String::New(env, "CredentialManager");
    exports.Set(name, CredentialManager::GetClass(env));
    return exports;
}

bool CredentialManager::ParseCredentialType(const std::string &credentialType, WORD *type)
{
    if (credentialType == "GENERIC")
    {
        *type = CRED_TYPE_GENERIC;
        return true;
    }
    else if (credentialType == "DOMAIN_PASSWORD")
    {
        *type = CRED_TYPE_DOMAIN_PASSWORD;
        return true;
    }
    else if (credentialType == "DOMAIN_CERTIFICATE")
    {
        *type = CRED_TYPE_DOMAIN_CERTIFICATE;
        return true;
    }
    else if (credentialType == "DOMAIN_VISIBLE_PASSWORD")
    {
        *type = CRED_TYPE_DOMAIN_VISIBLE_PASSWORD;
        return true;
    }
    else if (credentialType == "GENERIC_CERTIFICATE")
    {
        *type = CRED_TYPE_GENERIC_CERTIFICATE;
        return true;
    }
    else if (credentialType == "DOMAIN_EXTENDED")
    {
        *type = CRED_TYPE_DOMAIN_EXTENDED;
        return true;
    }
    else
    {
        return false;
    }
}

bool CredentialManager::ParseCredentialPersistence(const std::string &credentialPersistence, WORD *persistence)
{
    if (credentialPersistence == "PERSIST_SESSION")
    {
        *persistence = CRED_PERSIST_SESSION;
        return true;
    }
    else if (credentialPersistence == "PERSIST_LOCAL_MACHINE")
    {
        *persistence = CRED_PERSIST_LOCAL_MACHINE;
        return true;
    }
    else if (credentialPersistence == "PERSIST_ENTERPRISE")
    {
        *persistence = CRED_PERSIST_ENTERPRISE;
        return true;
    }
    else
    {
        return false;
    }
}

// Register and initialize native add-on
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)