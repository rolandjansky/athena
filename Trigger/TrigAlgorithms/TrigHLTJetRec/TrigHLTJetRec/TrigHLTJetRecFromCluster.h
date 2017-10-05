/* 
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
*/ 

#ifndef TRIGHLTJETREC_TRIGHLTJETRECFROMCLUSTER_H
#define TRIGHLTJETREC_TRIGHLTJETRECFROMCLUSTER_H

#include "TrigHLTJetRec/TrigHLTJetRecBase.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

class TrigHLTJetRecFromCluster: 
public TrigHLTJetRecBase<xAOD::CaloClusterContainer>{

 public:
  TrigHLTJetRecFromCluster(const std::string & name, ISvcLocator* pSvcLocator);
  virtual ~TrigHLTJetRecFromCluster();



 protected:

  HLT::ErrorCode getPseudoJets(const xAOD::CaloClusterContainer*,
                               jet::LabelIndex* indexMap,
                               jet::PseudoJetVector& pjv) override;

  HLT::ErrorCode checkforSecondaryPseudoJets(
                                      const HLT::TriggerElement* inputTE,
                                      jet::LabelIndex* indexMap,
                                      jet::PseudoJetVector& pjv_secondary) override;

  HLT::ErrorCode retrieveSecondaryPseudoJetGetter();
  
  HLT::ErrorCode getSecondaryPseudoJets(const HLT::TriggerElement* inputTE,
                               jet::LabelIndex* indexMap,
                               jet::PseudoJetVector& pjv);
  HLT::ErrorCode loadSecondaryPseudoJets(
                               jet::PseudoJetVector& pjv);

  const xAOD::JetContainer* build() const override; 

 
 private:
 /* A secondary PseudojetGetter shared by this algorithm and the IJetBuildTool for tracks
 *     The algorithm loads the psg, the ijbt processes them. */
 std::string m_scantype;
}; 
#endif
