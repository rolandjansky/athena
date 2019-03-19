/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFEGAMMAPFOASSOC_H
#define PFEGAMMAPFOASSOC_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "StoreGate/WriteDecorHandle.h"

class PFEGammaPFOAssoc : public AthAlgorithm {

public:

  PFEGammaPFOAssoc(const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~PFEGammaPFOAssoc();

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;
  
private:
 
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_electronNeutralPFOWriteDecorKey;
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_electronChargedPFOWriteDecorKey;
  SG::WriteDecorHandleKey<xAOD::PFOContainer> m_neutralpfoWriteDecorKey;
  SG::WriteDecorHandleKey<xAOD::PFOContainer> m_chargedpfoWriteDecorKey;
 
};

#endif // PFEGAMMAPFOASSOC_H
