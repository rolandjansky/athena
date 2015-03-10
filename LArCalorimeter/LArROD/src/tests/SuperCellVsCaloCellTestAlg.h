/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARROD_SUPERCELLVSCALOCELLTESTALG_H
#define LARROD_SUPERCELLVSCALOCELLTESTALG_H 1

//Author: Will Buttinger <will@cern.ch>
//Purpose: Quick comparison of supercell to sum-of-calocell quantities

#include "AthenaBaseComps/AthAlgorithm.h"


#include "CaloTriggerTool/ICaloSuperCellIDTool.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "GaudiKernel/ToolHandle.h"

class TH1D;

class SuperCellVsCaloCellTestAlg: public ::AthAlgorithm { 
 public: 
  SuperCellVsCaloCellTestAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~SuperCellVsCaloCellTestAlg(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 

   ToolHandle<ICaloSuperCellIDTool> m_sc2ccMappingTool;
   const CaloCell_ID* m_ccIdHelper;

   std::string m_scKey, m_ccKey;

   TH1D* m_eReso[CaloCell_ID::NSUBCALO];

}; 

#endif //> !LARROD_SUPERCELLVSCALOCELLTESTALG_H
