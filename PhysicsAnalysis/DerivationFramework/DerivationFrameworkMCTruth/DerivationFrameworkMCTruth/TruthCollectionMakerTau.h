/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////OB////////////////////////
// TruthCollectionMakerTau.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRUTHCOLLECTIONMAKERTAU_H
#define DERIVATIONFRAMEWORK_TRUTHCOLLECTIONMAKERTAU_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

class IMCTruthClassifier;

namespace DerivationFramework {

  class TruthCollectionMakerTau : public AthAlgTool, public IAugmentationTool {
  public: 
    TruthCollectionMakerTau(const std::string& t, const std::string& n, const IInterface* p);
    ~TruthCollectionMakerTau();
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;

    void printDecay(const xAOD::TruthParticle& xTruthParticle, int depth = 0) const;
    
    
  private:
    
    StatusCode examineTruthTau(const xAOD::TruthParticle& xTruthParticle) const;
    StatusCode examineTruthTauDecay(const xAOD::TruthParticle& xTruthParticle) const;
    StatusCode getPromptTLV(const xAOD::TruthParticle& xTruthParticle) const;
    
    std::string m_particlesKey;
    std::string m_collectionName; 
    ToolHandle<IMCTruthClassifier> m_classifier;

    bool m_runClassifier;
    bool m_writeVisibleChargedFourMomentum;
    bool m_writeVisibleNeutralFourMomentum;
    bool m_writeDecayModeVector;

    mutable size_t m_iNChargedPions;
    mutable size_t m_iNNeutralPions;
    mutable size_t m_iNChargedOthers;
    mutable size_t m_iNNeutralOthers;
    mutable size_t m_iNChargedDaughters;
    mutable std::vector<int> m_vDecayMode;
  
    mutable TLorentzVector m_vTruthVisTLV;
    mutable TLorentzVector m_vTruthPromptTLV;
    mutable TLorentzVector m_vTruthVisTLVCharged;
    mutable TLorentzVector m_vTruthVisTLVNeutral;

    mutable bool m_bIsHadronicTau;
    
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHCOLLECTIONMAKERTAU_H
