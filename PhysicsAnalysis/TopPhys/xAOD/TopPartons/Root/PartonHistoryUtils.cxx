#include "TopPartons/PartonHistoryUtils.h"

namespace top {
namespace PartonHistoryUtils {
  
  const xAOD::TruthParticle* findAfterFSR(const xAOD::TruthParticle* particle) {
    bool isAfterFSR(false);
    const int particle_ID = particle->pdgId();
    int forLoop = 0;

    while (!isAfterFSR) {
      forLoop = 0;
      for (size_t j = 0; j < particle->nChildren(); j++) {
        const xAOD::TruthParticle* tmp_children = particle->child(j);
        if (tmp_children && tmp_children->pdgId() == particle_ID) {
          particle = particle->child(j);
          forLoop++;
          break;
        }//if
      }//for
      if (forLoop == 0) isAfterFSR = true;
    }//while
    return particle;
  }

  bool hasParticleIdenticalParent(const xAOD::TruthParticle* particle) {
    bool skipit(false);

    for (size_t i = 0; i < particle->nParents(); i++) {
      const xAOD::TruthParticle* parent = particle->parent(i);
      if (parent && parent->pdgId() == particle->pdgId()) {
        skipit = true;
        break;
      }//if
    }//for
    return skipit;
  }

  HiggsDecay AnalyzeHiggsDecay(const xAOD::TruthParticle* higgs) {

    HiggsDecay result;

   

    if (higgs->pdgId() != 25) return result;

    /// identify after FSR higgs (just to be sure)
    const xAOD::TruthParticle* higgs_fsr = findAfterFSR(higgs);
    if (higgs_fsr->nChildren() != 2) return result;

    /// identify direct higgs decays (MC_Higgs_decay1 and MC_Higgs_decay2)
    result.decay1_vector = higgs_fsr->child(0)->p4();
    result.decay2_vector = higgs_fsr->child(1)->p4();
    result.decay1_pdgId = higgs_fsr->child(0)->pdgId();
    result.decay2_pdgId = higgs_fsr->child(1)->pdgId();

    /// check if higgs decays into tau, W or Z  
    const bool isZ = (result.decay1_pdgId == 23 || result.decay2_pdgId == 23) ? true : false;
    const bool isW = (std::abs(result.decay1_pdgId) == 24 || std::abs(result.decay2_pdgId) == 24) ? true : false;


    
    if (std::abs(result.decay1_pdgId) == 15) { // Tautau channel -> Check if the Tau is hadronic or leptonic
      result.tau_decay1_isHadronic = TauIsHadronic(higgs_fsr->child(0),result.tau_decay1_vector);
    }
    if (std::abs(result.decay2_pdgId) == 15) { // Tautau channel -> Check if the Tau is hadronic or leptonic
      result.tau_decay2_isHadronic = TauIsHadronic(higgs_fsr->child(1),result.tau_decay2_vector);
    }

    if (!isZ && !isW) return result;
    
    // Check that for the WW and ZZ channels, the W or Z from Higgs have 2 children
    const xAOD::TruthParticle* decay1 = findAfterFSR(higgs->child(0));
    if (decay1->nChildren() != 2) return result; 
    const xAOD::TruthParticle* decay2 = findAfterFSR(higgs->child(1));
    if (decay2->nChildren() != 2) return result;

    /// decays of W/Z 
    result.decay1_from_decay1_vector = decay1->child(0)->p4();
    result.decay2_from_decay1_vector = decay1->child(1)->p4();
    result.decay1_from_decay1_pdgId  = decay1->child(0)->pdgId();
    result.decay2_from_decay1_pdgId  = decay1->child(1)->pdgId();
    
    result.decay1_from_decay2_vector = decay2->child(0)->p4();
    result.decay2_from_decay2_vector = decay2->child(1)->p4();
    result.decay1_from_decay2_pdgId  = decay2->child(0)->pdgId();
    result.decay2_from_decay2_pdgId  = decay2->child(1)->pdgId();
    
    // If the W or Z decays into a Tau, we need to know if the Tau is leptonic or hadronic
    
    result.tau_decay1_from_decay1_isHadronic = TauIsHadronic(decay1->child(0),result.tau_decay1_from_decay1_vector);
    result.tau_decay2_from_decay1_isHadronic = TauIsHadronic(decay1->child(1),result.tau_decay2_from_decay1_vector);
    result.tau_decay1_from_decay2_isHadronic = TauIsHadronic(decay2->child(0),result.tau_decay1_from_decay2_vector);
    result.tau_decay2_from_decay2_isHadronic = TauIsHadronic(decay2->child(1),result.tau_decay2_from_decay2_vector);

    
    return result;
    
  }

  

  int TauIsHadronic(const xAOD::TruthParticle* tau, TLorentzVector& tau_visible_decay_p4) {

    int tau_decay_pdgId = -99; 

    if (!tau) return tau_decay_pdgId;

    if (std::abs(tau->pdgId()) != 15) return tau_decay_pdgId;
    

    const xAOD::TruthParticle* afterFsr = findAfterFSR(tau);
    
    bool there_are_leptons = false;


    for (size_t k = 0; k < afterFsr->nChildren(); k++) {      
      if (std::abs(afterFsr->child(k)->pdgId()) == 16){
	if (there_are_leptons == false){
	  tau_visible_decay_p4 = afterFsr->p4() - afterFsr->child(k)->p4(); // p4 of HadTau visible decay = p(tau) - p(neutrino-tau)
	  if (afterFsr->pdgId() == 15){
	    tau_decay_pdgId = 24;
	  }else{
	    tau_decay_pdgId = -24;
	  } 	  
	}
      }
      
      if (std::abs(afterFsr->child(k)->pdgId()) == 11 or std::abs(afterFsr->child(k)->pdgId())  == 13){ // Leptonic Tau
	tau_visible_decay_p4 = afterFsr->child(k)->p4();       // p4 of LepTau visible decay is the p4 of Lepton
	tau_decay_pdgId = afterFsr->child(k)->pdgId();               // pdgId of LepTau is the pdgId of Lepton 
    	there_are_leptons = true;
      }
      
    }
    
    return tau_decay_pdgId;


  }

    
}
}
