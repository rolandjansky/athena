/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */

#ifndef DerivationFramework_LongLivedTruthJetKinematics_H
#define DerivationFramework_LongLivedTruthJetKinematics_H

// Jennifer Roloff, Harvard University
// November 2017

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandleArray.h"
#include "xAODCore/ShallowCopy.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "xAODBase/IParticleContainer.h"
#include "xAODTruth/TruthParticle.h" 
#include "xAODTruth/TruthParticleContainer.h" 
#include "xAODTruth/TruthVertex.h" 
#include "xAODJet/JetContainer.h" 

#include <string>
#include <vector>

namespace DerivationFramework {


class LongLivedTruthJetKinematics: public AthAlgTool, public IAugmentationTool{

  public:
    LongLivedTruthJetKinematics(const std::string &name); 
    LongLivedTruthJetKinematics(const std::string& t, const std::string& n, const IInterface* p);
    ~LongLivedTruthJetKinematics(){};
    virtual StatusCode initialize();
    virtual StatusCode addBranches() const;

    enum llpTypes{None, RHadron}; 
  
protected:
  std::string m_inputTruthJetContainer = ""; //!
  std::string m_inputParticleContainer = ""; //!
  std::string m_outputTruthJetContainer = ""; //!
  double m_caloRad; //!
  double m_dR_matching; //!
  double m_minRadius; //!
  std::vector<int> m_llp_pdgids; //!
  int m_llpType; //!


  double getEtaFromTheta(double theta) const;
  double getThetaFromEta(double eta) const;
  TLorentzVector getDVKine(const TLorentzVector longLivedParticle, const TLorentzVector decayProduct, double r_dv, double R_cal) const;
  bool isDecayParticle(const xAOD::TruthParticle* particle) const;
  bool matchJets(TLorentzVector& mytruthJet) const;



};
}


#endif
