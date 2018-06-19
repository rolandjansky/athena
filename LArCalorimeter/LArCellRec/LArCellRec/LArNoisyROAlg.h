/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARNOISYROALG_H
#define LARNOISYROALG_H



/** 
@class LArNoisyROAlg
@brief Find list of suspicious preamplifiers and Front End Boards from cell collection

 Created September 28, 2009  L. Duflot
 Modified May, 2014 B.Trocme 
 - Remove saturated medium cut
 - Create a new weighted Std algorithm

*/



#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ILArNoisyROTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class CaloCellContainer;
class LArNoisyROSummary; 

class LArNoisyROAlg : public AthAlgorithm
{
 public:

  LArNoisyROAlg(const std::string &name,ISvcLocator *pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();   
  virtual StatusCode finalize();

 
 private: 
  ToolHandle<ILArNoisyROTool> m_noisyROTool;
 
  SG::ReadHandleKey<CaloCellContainer> m_CaloCellContainerName;
  SG::WriteHandleKey<LArNoisyROSummary> m_outputKey;

};


#endif
