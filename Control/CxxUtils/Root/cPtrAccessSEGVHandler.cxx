/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/cPtrAccessSEGVHandler.h"
#include "CxxUtils/PtrAccessSEGVHandler.h"
namespace {
  PtrAccessSEGVHandler* s_pHandler(0);
}

void setPtrAccessSEGVHandler ATLAS_NOT_THREAD_SAFE (PtrAccessSEGVHandler* h) { s_pHandler=h; }
void cPtrAccessSEGVHandler ATLAS_NOT_THREAD_SAFE (int signal, siginfo_t* si, void* old) {
  s_pHandler->handle(signal, si, old);
}
