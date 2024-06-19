#include <napi.h>

#include <string>
#include <sstream>
#include <windows.h>
#include <wincred.h>
#include <winrt/Windows.Foundation.Collections.h>

#include "Conversions.h"

Napi::Value ConvertLPCWSTRToValue(Napi::Env &env, LPCWSTR lpstr) 
{
    if (lpstr == NULL || lpstr[0] == 0) return env.Null();
    int strLength = WideCharToMultiByte(CP_UTF8, 0, &lpstr[0], wcslen(lpstr), NULL, 0, NULL, NULL); 
    std::string str(strLength, 0); 
    WideCharToMultiByte(CP_UTF8, 0, &lpstr[0], wcslen(lpstr), &str[0], strLength, NULL, NULL);

    return Napi::String::New(env, str);
}

LPWSTR ConvertStringToLPWSTR(std::string str)
{
    int strLength = MultiByteToWideChar(CP_UTF8, 0, &str[0], str.length(), NULL, 0); 
    std::wstring wstr(strLength, 0); 
    MultiByteToWideChar(CP_UTF8, 0, &str[0], str.length(), &wstr[0], strLength);

    return &wstr[0];
}

char* ConvertStringToChar(std::string str)
{
    char * ret = new char [str.length()+1];
    strcpy (ret, str.c_str());

    return ret;
}

Napi::Value ConvertLPBYTEToString(Napi::Env &env, LPBYTE blob, DWORD size)
{
    if (!((int)size > 0)) return env.Null();
    std::wstring ws((wchar_t*)blob);
    std::string str = std::string(ws.begin(), ws.end());
    return Napi::String::New(env, str);
}

Napi::Date ConvertFileTimeToDate(Napi::Env &env, FILETIME time)
{
    double seconds = winrt::clock::to_time_t(winrt::clock::from_file_time(time));
    return Napi::Date::New(env, seconds * 1000);
}

Napi::ObjectReference ConvertCredentialToObject(Napi::Env &env, PCREDENTIALW cred)
{
    Napi::ObjectReference result = Napi::Reference<Napi::Object>::New(Napi::Object::New(env));

    Napi::Number flags = Napi::Number::New(env, cred->Flags);
    Napi::Number type = Napi::Number::New(env, cred->Type);
    Napi::Number persist = Napi::Number::New(env, cred->Persist);
    Napi::Value targetName = ConvertLPCWSTRToValue(env, cred->TargetName);
    Napi::Value targetAlias = ConvertLPCWSTRToValue(env, cred->TargetAlias);
    Napi::Value username = ConvertLPCWSTRToValue(env, cred->UserName);
    Napi::Value password = ConvertLPBYTEToString(env, cred->CredentialBlob, cred->CredentialBlobSize);
    Napi::Value comment = ConvertLPCWSTRToValue(env, cred->Comment);
    Napi::Value lastUpdated = ConvertFileTimeToDate(env, cred->LastWritten);

    result.Set("flags", flags);
    result.Set("type", type);
    result.Set("persist", persist);
    result.Set("targetName", targetName);
    result.Set("targetAlias", targetAlias);
    result.Set("username", username);
    result.Set("password", password);
    result.Set("comment", comment);
    result.Set("lastUpdated", lastUpdated);

    return result;
}