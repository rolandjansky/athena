/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EGAMMATOPOCLUSTERCOPIER_H
#define EGAMMAALGS_EGAMMATOPOCLUSTERCOPIER_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODCaloEvent/CaloClusterFwd.h" 
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class egammaTopoClusterCopier : public AthReentrantAlgorithm {

 public:

  /** @bried constructor */
  egammaTopoClusterCopier (const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief initialize method */
  virtual StatusCode initialize() override final;
  /** @brief execute on container */
  virtual StatusCode execute_r(const EventContext& ctx) const override;
  /** @brief finalize method */
  virtual StatusCode finalize() override final;
  
  private:

  StatusCode  checkEMFraction (const xAOD::CaloCluster *clus, float &emFrac) const;
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_inputTopoCollection;
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outputTopoCollectionShallow;
  SG::WriteHandleKey<ConstDataVector <xAOD::CaloClusterContainer> > m_outputTopoCollection;
  float m_etaCut;
  float m_ECut;
  float m_EMFracCut;
  float m_EMCrackEtCut;
  
};

#endif // EGAMMATOOLS_EMCLUSTERTOOL_H
