﻿#include "stdafx.h"
#include "ResHack.h"
using namespace std;


// --------------------------------------------------------------------------------------
// ResHackFastcall
// --------------------------------------------------------------------------------------

ResHackFastcall::ResHackFastcall(uintptr_t thReadRes, uintptr_t thMalloc) :
	ResHackBase((void*)thReadRes, MyReadRes, (MallocType)thMalloc)
{
}

void* ResHackFastcall::MyReadRes(const char* fileName, DWORD* bytesRead, BOOL isFile)
{
	auto& instance = (ResHackFastcall&)ResHackFactory::GetInstance().GetResHackInstance();
	return instance.ResHackBase::MyReadRes(fileName, bytesRead, isFile);
}

void* ResHackFastcall::CallOriginalReadRes(const char* fileName, DWORD* bytesRead, BOOL isFile)
{
	return m_readResHook.GetOriginalFunction<ReadResType>()(fileName, bytesRead, isFile);
}


// --------------------------------------------------------------------------------------
// ResHackStdcall
// --------------------------------------------------------------------------------------

ResHackStdcall::ResHackStdcall(uintptr_t thReadRes, uintptr_t thMalloc) :
	ResHackBase((void*)thReadRes, MyReadRes, (MallocType)thMalloc)
{
}

void* ResHackStdcall::MyReadRes(const char* fileName, DWORD* bytesRead, BOOL isFile)
{
	auto& instance = (ResHackStdcall&)ResHackFactory::GetInstance().GetResHackInstance();
	return instance.ResHackBase::MyReadRes(fileName, bytesRead, isFile);
}

void* ResHackStdcall::CallOriginalReadRes(const char* fileName, DWORD* bytesRead, BOOL isFile)
{
	return m_readResHook.GetOriginalFunction<ReadResType>()(fileName, bytesRead, isFile);
}


// --------------------------------------------------------------------------------------
// ResHackFirstArgAtEax
// --------------------------------------------------------------------------------------

ResHackFirstArgAtEax::ResHackFirstArgAtEax(uintptr_t thReadRes, uintptr_t thMalloc) :
	ResHackBase((void*)thReadRes, MyReadResWrapper, (MallocType)thMalloc)
{
}

__declspec(naked) void* ResHackFirstArgAtEax::MyReadResWrapper(DWORD* bytesRead, BOOL isFile)
{
	__asm
	{
		pop ecx
		push eax
		push ecx
		jmp MyReadRes
	}
}

void* ResHackFirstArgAtEax::MyReadRes(const char* fileName, DWORD* bytesRead, BOOL isFile)
{
	auto& instance = (ResHackFirstArgAtEax&)ResHackFactory::GetInstance().GetResHackInstance();
	return instance.ResHackBase::MyReadRes(fileName, bytesRead, isFile);
}

void* ResHackFirstArgAtEax::CallOriginalReadRes(const char* fileName, DWORD* bytesRead, BOOL isFile)
{
	void* readRes = m_readResHook.GetOriginalFunction();
	void* result;
	__asm
	{
		push isFile
		push bytesRead
		mov eax, fileName
		call readRes
		mov result, eax
	}
	return result;
}


// --------------------------------------------------------------------------------------
// ResHackTH07
// --------------------------------------------------------------------------------------

ResHackTH07::ResHackTH07(uintptr_t thReadRes, uintptr_t thMalloc, uintptr_t bytesRead) :
	ResHackBase((void*)thReadRes, MyReadRes, (MallocType)thMalloc),
	m_bytesRead((DWORD*)bytesRead)
{
}

void* ResHackTH07::MyReadRes(const char *fileName, BOOL isFile)
{
	auto& instance = (ResHackTH07&)ResHackFactory::GetInstance().GetResHackInstance();
	return instance.ResHackBase::MyReadRes(fileName, instance.m_bytesRead, isFile);
}

void* ResHackTH07::CallOriginalReadRes(const char* fileName, DWORD* bytesRead, BOOL isFile)
{
	return m_readResHook.GetOriginalFunction<ReadResType>()(fileName, isFile);
}


// --------------------------------------------------------------------------------------
// ResHackTH06
// --------------------------------------------------------------------------------------

ResHackTH06::ResHackTH06(uintptr_t thReadRes, uintptr_t thMalloc, uintptr_t bytesRead) :
	ResHackBase((void*)thReadRes, MyReadRes, (MallocType)thMalloc),
	m_bytesRead((DWORD*)bytesRead)
{
}

void* ResHackTH06::MyReadRes(const char *fileName, BOOL isFile)
{
	auto& instance = (ResHackTH06&)ResHackFactory::GetInstance().GetResHackInstance();
	return instance.ResHackBase::MyReadRes(fileName, instance.m_bytesRead, isFile);
}

void* ResHackTH06::CallOriginalReadRes(const char* fileName, DWORD* bytesRead, BOOL isFile)
{
	return m_readResHook.GetOriginalFunction<ReadResType>()(fileName, isFile);
}

// --------------------------------------------------------------------------------------
// ResHackFastcallW
// --------------------------------------------------------------------------------------

ResHackFastcallTH19::ResHackFastcallTH19(uintptr_t thReadRes, uintptr_t thMalloc) :
	ResHackBase((void*)thReadRes, MyReadRes, (MallocType)thMalloc)
{

}

__declspec(naked) void* __fastcall ResHackFastcallTH19::MyReadRes(const char* fileName, DWORD* bytesRead, BOOL isFile)
{
	__asm
	{ 
		push ebp
		mov ebp, esp
		push[ebp+0x8]//isFile
		call MyReadRes_W
		pop ebp
		ret 0
	}
}

void* __fastcall ResHackFastcallTH19::MyReadRes_W(const char* fileName, DWORD* bytesRead, BOOL isFile)
{
	auto& instance = (ResHackFastcallTH19&)ResHackFactory::GetInstance().GetResHackInstance();
	auto res = instance.ResHackBase::MyReadRes(fileName, bytesRead, isFile);
	return res;
}

void* ResHackFastcallTH19::CallOriginalReadRes(const char* fileName, DWORD* bytesRead, BOOL isFile)
{
	auto func=m_readResHook.GetOriginalFunction<ReadResType>();
	void* r = nullptr;
	__asm
	{
		mov ecx,fileName
		mov edx,bytesRead
		mov eax,func
		push isFile
		call eax
		add esp,4
		mov r,eax
	}
	return r;
}
