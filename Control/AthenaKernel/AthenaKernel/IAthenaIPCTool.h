/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IATHENAIPCTOOL_H
#define ATHENAKERNEL_IATHENAIPCTOOL_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IAthenaIPCTool( "IAthenaIPCTool", 1, 0 );

class IAthenaIPCTool : virtual public ::IAlgTool {
public:
   static const InterfaceID& interfaceID() { return IID_IAthenaIPCTool; }
 
   virtual StatusCode makeServer(int num) = 0;
   virtual bool isServer() const = 0;
   virtual StatusCode makeClient(int num) = 0;
   virtual bool isClient() const = 0;

   virtual StatusCode putEvent(long eventNumber, const void* source, size_t nbytes, unsigned int status) const = 0;
   virtual StatusCode getLockedEvent(void** target, unsigned int& status) const = 0;
   virtual StatusCode lockEvent(long eventNumber) const = 0;

   virtual StatusCode putObject(const void* source, size_t nbytes, int num = 0) const = 0;
   virtual StatusCode getObject(void** target, size_t& nbytes, int num = 0) const = 0;
   virtual StatusCode clearObject(const char** tokenString, int& num) const = 0;
   virtual StatusCode lockObject(const char* tokenString, int num = 0) const = 0;
};

#endif
