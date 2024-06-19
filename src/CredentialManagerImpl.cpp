#include <napi.h>

#include <windows.h>
#include <wincred.h>
#include <wchar.h>
#include <stdexcept>

#include "conversions.h"

Napi::Value GetCredentialImpl(Napi::Env &env, LPWSTR targetName, DWORD type)
{
    PCREDENTIALW cred;
    bool ok = CredReadW(targetName, type, 0, &cred);
    if (!ok) return env.Null();

    Napi::ObjectReference ret = ConvertCredentialToObject(env, cred);
    CredFree(cred);

    return ret.Value();
}

bool StoreCredentialImpl(LPWSTR targetName, LPWSTR username, const char* password, DWORD type, DWORD persist)
{
    DWORD cbCreds = strlen(password);

    CREDENTIALW cred = {0};
    cred.Type = type;
    cred.TargetName = targetName;
    cred.CredentialBlobSize = cbCreds;
    cred.CredentialBlob = (LPBYTE) password;
    cred.Persist = persist;
    cred.UserName = username;

    return CredWriteW(&cred, 0);
}

bool DeleteCredentialImpl(LPWSTR targetName, DWORD type)
{
    bool ok = CredDeleteW(targetName, type, 0);
    if (!ok) {
        throw std::runtime_error("Credential not found");
    }

    return ok;
}

Napi::Value ListCredentialsImpl(Napi::Env &env)
{
    DWORD count;
    PCREDENTIALW* creds;
    CredEnumerateW(NULL, 1, &count, &creds);

    Napi::Array result = Napi::Array::New(env, count);

    for (int i = 0; i < (int)count; i++)
    {
        PCREDENTIALW cred = creds[i];
        result[i] = ConvertCredentialToObject(env, cred).Value();
    }

    CredFree(creds);

    return result;
}