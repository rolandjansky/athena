/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IATHENASHAREDWRITERSVC_H
#define ATHENAKERNEL_IATHENASHAREDWRITERSVC_H

#include "GaudiKernel/IService.h"

static const InterfaceID IID_IAthenaSharedWriterSvc( "IAthenaSharedWriterSvc", 1, 0 );

class IAthenaSharedWriterSvc : virtual public ::IService {
public:
   static const InterfaceID& interfaceID() { return IID_IAthenaSharedWriterSvc; }
};

#endif
