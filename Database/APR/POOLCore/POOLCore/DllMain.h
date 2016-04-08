/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	DllMain.h
//--------------------------------------------------------------------
//
//	Package    : pool/POOLCore
//
//  Description: The DLL initialisation must be done seperately for 
//               each DLL. This small header allows to customize
//               The DLL initialization in a common way for Linux
//               and windows.
//
//	Author     : M.Frank
//
//====================================================================
#ifndef COMMON_DLLMAIN_H
#define COMMON_DLLMAIN_H 1

static void initializeDLL(void* hinstDLL);
static void finalizeDLL(void* hinstDLL);

#if defined(WIN32) && defined(_DLL)
namespace win {
  #include <windows.h>
};

extern "C" win::BOOL APIENTRY DllMain(
        win::HINSTANCE hinstDLL,  // handle to DLL module
        win::DWORD fdwReason,     // reason for calling function
        win::LPVOID lpvReserved   // reserved
)   
{
  switch( fdwReason ) {
  case DLL_PROCESS_ATTACH:
    initializeDLL(hinstDLL);
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    finalizeDLL(hinstDLL);
    break;
  }
  return TRUE;
}
#else //  linux
static void _init() __attribute__((constructor));
static void _fini() __attribute__((destructor));

static void _init()   {
  initializeDLL(0);
}
static void _fini()   {
  finalizeDLL(0);
}
#endif // WIN32

#endif // COMMON_DLLMAIN_H
