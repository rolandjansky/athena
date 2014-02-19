/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_DIGI_ALG_H
#define ALFA_DIGI_ALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

class ALFA_DigiAlg: public AthAlgorithm
{
 
 public:
   
  ALFA_DigiAlg(const std::string& name, ISvcLocator* pSvcLocator);
 
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
   
 private:
  
  ToolHandle<IPileUpTool> m_digiTool;
};
  
#endif