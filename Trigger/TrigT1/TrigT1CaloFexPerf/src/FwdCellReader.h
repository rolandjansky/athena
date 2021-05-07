/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_FWDCELLREADER_H
#define TRIGT1CALOFEXPERF_FWDCELLREADER_H

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloIdentifier/TTOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "FFexEMFinder.h"

#include "xAODTrigger/EmTauRoIAuxContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/EmTauRoI.h"

namespace LVL1{
class FwdCellReader: public ::AthAlgorithm {
 public:
  FwdCellReader( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~FwdCellReader();
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private:

  SG::ReadHandleKey<CaloCellContainer> m_inputCellContainerKey;
  SG::WriteHandleKey<xAOD::EmTauRoIContainer> m_outputClusterContainerKey; 

  float m_range;
  float m_cellEtaCut;
  float m_cellEtCut;

};
}//namespace LVL1
#endif //> !TRIGT1CALOFEXPERF_FWDCELLREADER_H
