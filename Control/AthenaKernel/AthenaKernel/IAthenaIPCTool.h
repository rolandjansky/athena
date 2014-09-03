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
 
   virtual StatusCode makeServer() = 0;
   virtual bool isServer() const = 0;
   virtual StatusCode makeClient() = 0;
   virtual bool isClient() const = 0;

   virtual StatusCode putEvent(long eventNumber, const void* source, size_t num, unsigned int status) = 0;
   virtual StatusCode getLockedEvent(void** source, unsigned int& status) = 0;
   virtual StatusCode lockEvent(long eventNumber) = 0;
   virtual StatusCode unlockEvent() = 0;
};

#endif
