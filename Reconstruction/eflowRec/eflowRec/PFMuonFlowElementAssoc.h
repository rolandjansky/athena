/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFMuonFlowElementAssoc.h
 * Header file for class PFEGammaPFOAssoc
 *                                                                                                                                                             
 * Created by M.T. Anthony
 */
  

#ifndef PFMUONFLOWELEMENTASSOC_H
#define PFMUONFLOWELEMENTASSOC_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "StoreGate/WriteDecorHandle.h"

/*
  AthAlgorithm to associate Flow Elements (PFOs) to Muons.
  Associates tracks from charged flow elements to Muons only (otherwise links to NULL)
 */


class PFMuonFlowElementAssoc : public AthAlgorithm {

public:

  PFMuonFlowElementAssoc(const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~PFMuonFlowElementAssoc();

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;
  
private:
  
  /* Write key for adding charged Flow Element link decorations to muons */
  SG::WriteDecorHandleKey<xAOD::MuonContainer> m_muonChargedFEWriteDecorKey;
  /* Write key for adding Muon link decorations to charged Flow Elements */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_ChargedFEmuonWriteDecorKey;

  /* Write key for adding neutral Flow Element link decorations to muons */
  SG::WriteDecorHandleKey<xAOD::MuonContainer> m_muonNeutralFEWriteDecorKey;
  /* Write key for adding Muon link decorations to neutral Flow Elements */
  SG::WriteDecorHandleKey<xAOD::FlowElementContainer> m_NeutralFEmuonWriteDecorKey;




};

#endif // PFMUONFLOWELEMENTASSOC.H
