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
    const bool isTau = (std::abs(result.decay1_pdgId) == 15 || std::abs(result.decay2_pdgId) == 15) ? true : false;

    if (!isZ && !isW && !isTau) return result;

    const xAOD::TruthParticle* decay1 = findAfterFSR(higgs->child(0));
    if (decay1->nChildren() != 2) return result;
    const xAOD::TruthParticle* decay2 = findAfterFSR(higgs->child(1));
    if (decay2->nChildren() != 2) return result;

    /// decays if W/Z/tau
    result.decay1_from_decay1_vector = decay1->child(0)->p4();
    result.decay2_from_decay1_vector = decay1->child(1)->p4();
    result.decay1_from_decay1_pdgId  = decay1->child(0)->pdgId();
    result.decay2_from_decay1_pdgId  = decay1->child(1)->pdgId();
    
    result.decay1_from_decay2_vector = decay2->child(0)->p4();
    result.decay2_from_decay2_vector = decay2->child(1)->p4();
    result.decay1_from_decay2_pdgId  = decay2->child(0)->pdgId();
    result.decay2_from_decay2_pdgId  = decay2->child(1)->pdgId();

    if (!isTau) return result;

    // identify which decay is W
    bool firsIsW1 = std::abs(result.decay1_from_decay1_pdgId == 24) ? true : false;
    bool firsIsW2 = std::abs(result.decay1_from_decay2_pdgId == 24) ? true : false;
    const xAOD::TruthParticle *W1(nullptr);
    const xAOD::TruthParticle *W2(nullptr);
    if (firsIsW1) {
      W1 = findAfterFSR(decay1->child(0));
    } else {
      W1 = findAfterFSR(decay1->child(1));
    }

    if (firsIsW2) {
      W2 = findAfterFSR(decay2->child(0));
    } else {
      W2 = findAfterFSR(decay2->child(1));
    }

    if (W1->nChildren() != 2) return result;
    result.decay1_from_W_from_tau1_vector = W1->child(0)->p4();
    result.decay2_from_W_from_tau1_vector = W1->child(1)->p4();
    result.decay1_from_W_from_tau1_pdgId = W1->child(0)->pdgId();
    result.decay2_from_W_from_tau1_pdgId = W1->child(1)->pdgId();
    
    if (W2->nChildren() != 2) return result;
    result.decay1_from_W_from_tau2_vector = W2->child(0)->p4();
    result.decay2_from_W_from_tau2_vector = W2->child(1)->p4();
    result.decay1_from_W_from_tau2_pdgId = W2->child(0)->pdgId();
    result.decay2_from_W_from_tau2_pdgId = W2->child(1)->pdgId();

    return result;
  }
}
}
