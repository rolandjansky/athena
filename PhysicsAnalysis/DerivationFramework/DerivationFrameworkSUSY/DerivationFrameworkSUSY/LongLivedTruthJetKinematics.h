#ifndef DerivationFramework_LongLivedTruthJetKinematics_H
#define DerivationFramework_LongLivedTruthJetKinematics_H

// Jennifer Roloff, Harvard University
// November 2017

#include <string>
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandleArray.h"
#include "xAODCore/ShallowCopy.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include <iostream>

#include "xAODBase/IParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h" 
#include "xAODTruth/TruthParticle.h" 
#include "xAODTruth/TruthVertexContainer.h" 
#include "xAODTruth/TruthVertex.h" 
#include "xAODJet/JetContainer.h" 
#include "xAODJet/Jet.h" 

namespace DerivationFramework {

class LongLivedTruthJetKinematics: public AthAlgTool, public IAugmentationTool{
  //ASG_TOOL_CLASS(LongLivedTruthJetKinematics, IAsgTool)
  public:
  LongLivedTruthJetKinematics(const std::string &name); // MEN: constructor 
    LongLivedTruthJetKinematics(const std::string& t, const std::string& n, const IInterface* p);
    ~LongLivedTruthJetKinematics(){};
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;
  
protected:
  std::string m_inputTruthJetContainer = ""; //!
  std::string m_inputParticleContainer = ""; //!
  std::string m_outputTruthJetContainer = ""; //!
	int m_llp_pdgid; //!
	double m_caloRad; //!
	double m_dR_matching; //!

	double getEtaFromTheta(double theta) const;
	double getThetaFromEta(double eta) const;
	TLorentzVector getDVKine( TLorentzVector longLivedParticle, TLorentzVector decayProduct, double r_dv, double R_cal) const;
	bool isDecayParticle(xAOD::TruthParticle* particle) const;
	TLorentzVector matchJets(xAOD::Jet* mytruthJet) const;

};
}


#endif

