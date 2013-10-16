/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZVERTEXFROMKINE_H
#define ZVERTEXFROMKINE_H

#include <string>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"                               


class StoreGateSvc;

using namespace std;

class ZVertexFromKine : public Algorithm 
{
 public:
  
  ZVertexFromKine(const std::string& name, ISvcLocator* pSvcLocator);
  ~ZVertexFromKine();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  StoreGateSvc* m_StoreGate;

  std::string m_mcEventCollectionName;
};


#endif
