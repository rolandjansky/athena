/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_TRIGHLTJETRECFROMCLUSTER_H
#define TRIGHLTJETREC_TRIGHLTJETRECFROMCLUSTER_H

#include "TrigHLTJetRec/TrigHLTJetRecBase.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class TrigHLTJetRecFromCluster: 
public TrigHLTJetRecBase<xAOD::CaloClusterContainer>{

 public:
  TrigHLTJetRecFromCluster(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetRecFromCluster();
 protected:

  HLT::ErrorCode getPseudoJets(const xAOD::CaloClusterContainer*,
                               LabelIndex* indexMap,
                               PseudoJetVector& pjv) override;


  const xAOD::JetContainer* build() const override; 

 private:
  bool m_doTrimming{false};
  //ToolHandle<IJetBuildTool> m_jetTrimmerTool;
  ToolHandle<JetRecTool> m_jetTrimmerTool; 
  
  bool m_doxAODoutPut{false};

}; 
#endif
