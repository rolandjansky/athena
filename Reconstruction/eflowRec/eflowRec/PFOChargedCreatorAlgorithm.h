/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef PFOCHARGEDCREATORALGORITHM_H
#define PFOCHARGEDCREATORALGORITHM_H

#include "eflowRec/eflowCaloObject.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "xAODPFlow/PFOContainer.h"

class PFOChargedCreatorAlgorithm : public AthReentrantAlgorithm {
  
public:
  
  PFOChargedCreatorAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);

  ~PFOChargedCreatorAlgorithm() {}

  StatusCode initialize();
  StatusCode execute(const EventContext&) const;
  StatusCode finalize();

private:
  /** Create the charged PFO */ 
  void createChargedPFO(const eflowCaloObject& energyFlowCaloObject, bool addClusters, SG::WriteHandle<xAOD::PFOContainer>& chargedPFOContainerWriteHandle) const;

  /** Toggle EOverP algorithm mode, whereby no charged shower subtraction is performed */
  Gaudi::Property<bool> m_eOverPMode{this,"EOverPMode",false,"Toggle EOverP algorithm mode, whereby no charged shower subtraction is performed"};

  /** ReadHandleKey for eflowCaloObjectContainer */
  SG::ReadHandleKey<eflowCaloObjectContainer> m_eflowCaloObjectContainerReadHandleKey{this,"eflowCaloObjectContainerName","eflowCaloObjects","ReadHandleKey for eflowCaloObjectContainer"};
  
  /** WriteHandleKey for charged PFO */
  SG::WriteHandleKey<xAOD::PFOContainer> m_chargedPFOContainerWriteHandleKey{this,"PFOOutputName","JetETMissChargedParticleFlowObjects","WriteHandleKey for charged PFO"};
    
};
#endif
