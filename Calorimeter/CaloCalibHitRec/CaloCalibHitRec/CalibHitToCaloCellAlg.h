/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALIBHITTOCALOCELLALG_H
#define CALIBHITTOCALOCELLALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <string>
#include <vector>

class CalibHitToCaloCellTool;
class CaloCellContainer;

class CalibHitToCaloCellAlg : public AthAlgorithm
{
 public:
  
  CalibHitToCaloCellAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CalibHitToCaloCellAlg();                         
    
  StatusCode initialize();    
  StatusCode execute();
  StatusCode finalize();
    
 private:
  ToolHandle<CalibHitToCaloCellTool> m_calibhitToCaloCellTool; 

};

#endif
