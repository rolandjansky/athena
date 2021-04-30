/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IATHENASHAREDWRITERSVC_H
#define ATHENAKERNEL_IATHENASHAREDWRITERSVC_H

#include <string>

#include "GaudiKernel/IService.h"

static const InterfaceID IID_IAthenaSharedWriterSvc( "IAthenaSharedWriterSvc", 1, 0 );

class IAthenaSharedWriterSvc : virtual public ::IService {
public:
   virtual StatusCode share(int numClients = 0) = 0;

   /// Gaudi boilerplate
   static const InterfaceID& interfaceID() { return IID_IAthenaSharedWriterSvc; }

   /// Set/Get Stream Port Suffix
   void setStreamPortSuffix( const std::string& val ) { m_streamPortSuffix = val; }
   std::string getStreamPortSuffix() { return m_streamPortSuffix; }

private:
   std::string m_streamPortSuffix;
};

#endif
