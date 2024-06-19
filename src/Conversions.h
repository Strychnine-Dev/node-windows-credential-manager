#include <napi.h>

#include <string>
#include <Windows.h>
#include <wincred.h>

Napi::Value ConvertLPCWSTRToValue(Napi::Env &env, LPCWSTR lpstr);
LPWSTR ConvertStringToLPWSTR(std::string str);
char* ConvertStringToChar(std::string str);
Napi::Value ConvertLPBYTEToString(Napi::Env &env, LPBYTE blob, DWORD size);
Napi::Date ConvertFileTimeToDate(Napi::Env &env, FILETIME time);
Napi::ObjectReference ConvertCredentialToObject(Napi::Env &env, PCREDENTIALW cred);