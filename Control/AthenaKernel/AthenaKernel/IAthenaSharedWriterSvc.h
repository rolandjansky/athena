/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IATHENASHAREDWRITERSVC_H
#define ATHENAKERNEL_IATHENASHAREDWRITERSVC_H

#include "GaudiKernel/IService.h"

static const InterfaceID IID_IAthenaSharedWriterSvc( "IAthenaSharedWriterSvc", 1, 0 );

class IAthenaSharedWriterSvc : virtual public ::IService {
public:
   virtual StatusCode share(int numClients = 0) = 0;

   /// Gaudi boilerplate
   static const InterfaceID& interfaceID() { return IID_IAthenaSharedWriterSvc; }
};

#endif
