/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ZDC_DIGITIZATION_H
#define ZDC_DIGITIZATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

class ZDC_DigiTop: public AthAlgorithm
{
 
 public:
   
  ZDC_DigiTop(const std::string& name, ISvcLocator* pSvcLocator);
 
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
   
 private:
  
  ToolHandle<IPileUpTool> m_digTool;
};
  
#endif
