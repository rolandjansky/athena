/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_TRIGHLTJETRECFROMCLUSTER_H
#define TRIGHLTJETREC_TRIGHLTJETRECFROMCLUSTER_H

#include "TrigHLTJetRec/TrigHLTJetRecBase.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

using InputContainer = xAOD::CaloClusterContainer;
class TrigHLTJetRecFromCluster: public TrigHLTJetRecBase<InputContainer>{

 public:
  TrigHLTJetRecFromCluster(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetRecFromCluster();
 protected:

  HLT::ErrorCode getPseudoJets(const xAOD::CaloClusterContainer*,
                               LabelIndex* indexMap,
                               PseudoJetVector& pjv) override;

}; 
#endif
