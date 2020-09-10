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

    /// identify direct higgs decays
    result.decay1_vector = higgs_fsr->child(0)->p4();
    result.decay2_vector = higgs_fsr->child(1)->p4();
    result.decay1_pdgId = higgs_fsr->child(0)->pdgId();
    result.decay2_pdgId = higgs_fsr->child(1)->pdgId();

    /// check if higgs decays into tau, W or Z
    const bool isZ = (result.decay1_pdgId == 23 || result.decay2_pdgId == 23) ? true : false;
    const bool isW = (std::abs(result.decay1_pdgId) == 24 || std::abs(result.decay2_pdgId) == 24) ? true : false;

    /// Function to translate the result of the investigation into an integer status
    /// status -1 = problem with the truth record
    /// status  1 = tau decays hadronically
    /// status  0 = tau decays leptonically
    auto tauHadronicIndex = [&result](const xAOD::TruthParticle* particle) {
      bool isOk(false);
      const bool isHadronic = TauIsHadronic(particle, isOk);
      if (!isOk) {
        return -1;
      }

      if (isHadronic) return 1;

      return 0;
    };

    if (std::fabs(result.decay1_pdgId) == 15) {
      result.tau_decay1_isHadronic = tauHadronicIndex(higgs_fsr->child(0));
    }
    if (std::fabs(result.decay2_pdgId) == 15) {
      result.tau_decay2_isHadronic = tauHadronicIndex(higgs_fsr->child(1));
    }

    if (!isZ && !isW) return result;

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

    if (std::fabs(result.decay1_from_decay1_pdgId) == 15) {
      result.tau_decay1_from_decay1_isHadronic = tauHadronicIndex(decay1->child(0));
    }
    if (std::fabs(result.decay2_from_decay1_pdgId) == 15) {
      result.tau_decay2_from_decay1_isHadronic = tauHadronicIndex(decay1->child(1));
    }
    if (std::fabs(result.decay1_from_decay2_pdgId) == 15) {
      result.tau_decay1_from_decay2_isHadronic = tauHadronicIndex(decay2->child(0));
    }
    if (std::fabs(result.decay2_from_decay2_pdgId) == 15) {
      result.tau_decay2_from_decay2_isHadronic = tauHadronicIndex(decay2->child(1));
    }
    
    return result;
  }
  
  bool TauIsHadronic(const xAOD::TruthParticle* tau, bool &isOk) {

    if (!tau) {
      isOk = false;
      return false;
    }

    if (std::fabs(tau->pdgId()) != 15) {
      isOk = false;
      return false;
    }

    const xAOD::TruthParticle* afterFsr = findAfterFSR(tau);

    if (afterFsr->nChildren() != 2) {
      isOk = false;
      return false;
    }

    const xAOD::TruthParticle* child1 = findAfterFSR(afterFsr->child(0));
    const xAOD::TruthParticle* child2 = findAfterFSR(afterFsr->child(1));

    if (std::fabs(child1->pdgId()) == 16) {
      // it means the other particle ahs to be W
      if (std::fabs(child2->pdgId()) != 24) {
        isOk = false;
        return false;
      }
      
      // child2 is W
      if (child2->nChildren() != 2) {
        isOk = false;
        return false;
      }

      isOk = true;

      // everything is fine, check if the W decays hadronically or leptonically
      if (std::fabs(child2->child(0)->pdgId()) < 16) {
        return true;
      } else {
        return false;
      }
    } else {
      // it means the other particle has to be nu
      if (std::fabs(child2->pdgId()) != 16) {
        isOk = false;
        return false;
      }
      
      // child1 is W
      if (child1->nChildren() != 2) {
        isOk = false;
        return false;
      }

      isOk = true;

      // everything is fine, check if the W decays hadronically or leptonically
      if (std::fabs(child1->child(0)->pdgId()) < 16) {
        return true;
      } else {
        return false;
      }
    }
  }
}
}
