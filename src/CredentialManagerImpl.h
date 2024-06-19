#include <napi.h>

#include <windows.h>

Napi::Value GetCredentialImpl(Napi::Env &env, LPWSTR targetName, DWORD type);
bool StoreCredentialImpl(LPWSTR targetName, LPWSTR username, const char* password, DWORD type, DWORD persist);
bool DeleteCredentialImpl(LPWSTR targetName, DWORD type);
Napi::Value ListCredentialsImpl(Napi::Env &env);