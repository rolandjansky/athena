/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "TopPartons/CalcTTZPartonHistory.h"
#include "TopConfiguration/TopConfig.h"
#include "TopPartons/PartonHistoryUtils.h"

namespace top {
  CalcTTZPartonHistory::CalcTTZPartonHistory(const std::string& name) : CalcTtbarPartonHistory(name) {}

  void CalcTTZPartonHistory::zHistorySaver(const xAOD::TruthParticleContainer* truthParticles,
                                           xAOD::PartonHistory* ttbarPartonHistory) {
    ttbarPartonHistory->IniVarZ();

    TLorentzVector Z;
    TLorentzVector ZDecay1;
    TLorentzVector ZDecay2;
    int ZDecay1_pdgId;
    int ZDecay2_pdgId;
    bool is_on_shell;
    bool event_has_Z = CalcTTZPartonHistory::getZ(truthParticles,
                                                  &Z,
                                                  &ZDecay1,
                                                  &ZDecay1_pdgId,
                                                  &ZDecay2,
                                                  &ZDecay2_pdgId,
                                                  &is_on_shell);

    if (event_has_Z && !m_ancestry_corrupted) {
      ttbarPartonHistory->auxdecor< float >("MC_Z_m") = Z.M();
      ttbarPartonHistory->auxdecor< float >("MC_Z_pt") = Z.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_Z_eta") = Z.Eta();
      ttbarPartonHistory->auxdecor< float >("MC_Z_phi") = Z.Phi();

      ttbarPartonHistory->auxdecor< float >("MC_Zdecay1_m") = ZDecay1.M();
      ttbarPartonHistory->auxdecor< float >("MC_Zdecay1_pt") = ZDecay1.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_Zdecay1_eta") = ZDecay1.Eta();
      ttbarPartonHistory->auxdecor< float >("MC_Zdecay1_phi") = ZDecay1.Phi();
      ttbarPartonHistory->auxdecor< int >("MC_Zdecay1_pdgId") = ZDecay1_pdgId;

      ttbarPartonHistory->auxdecor< float >("MC_Zdecay2_m") = ZDecay2.M();
      ttbarPartonHistory->auxdecor< float >("MC_Zdecay2_pt") = ZDecay2.Pt();
      ttbarPartonHistory->auxdecor< float >("MC_Zdecay2_eta") = ZDecay2.Eta();
      ttbarPartonHistory->auxdecor< float >("MC_Zdecay2_phi") = ZDecay2.Phi();
      ttbarPartonHistory->auxdecor< int >("MC_Zdecay2_pdgId") = ZDecay2_pdgId;

      ttbarPartonHistory->auxdecor< int >("MC_Z_IsOnShell") = static_cast<int>(is_on_shell);
    }  // if

    ttbarPartonHistory->auxdecor< int >("MC_Z_AncestryCorrupted") = static_cast<int>(m_ancestry_corrupted);
  }

  bool CalcTTZPartonHistory::getZ(const xAOD::TruthParticleContainer* truthParticles,
                                  TLorentzVector* Z_p4,
                                  TLorentzVector* ZDecay1,
                                  int* ZDecay1_pdgId,
                                  TLorentzVector* ZDecay2,
                                  int* ZDecay2_pdgId,
                                  bool* is_on_shell) {
    bool hasZ {
      false
    };
    bool hasZDecay1 {
      false
    };
    bool hasZDecay2 {
      false
    };

    *is_on_shell = false;
    m_ancestry_corrupted = false;

    // First check for real (on-shell) Z bosons in the truth record
    // (PDG ID 23). Only consider Z bosons at the end of the chain
    // of radiative corrections (i.e. a Z boson decaying into
    // something else than a Z).
    for (const auto& p : *truthParticles) {
      if (!p->isZ()) continue;
      if (PartonHistoryUtils::hasParticleIdenticalParent(p)) continue;

      const auto& z = PartonHistoryUtils::findAfterFSR(p);
      *is_on_shell = true;
      if (z->nChildren() != 2) continue;

      *Z_p4 = z->p4();
      hasZ = true;

      for (size_t i = 0; i < z->nChildren(); ++i) {
        const auto& child = z->child(i);
        if (!child) continue;
        if (child->pdgId() > 0) {
          *ZDecay1 = PartonHistoryUtils::findAfterFSR(child)->p4();
          *ZDecay1_pdgId = child->pdgId();
          hasZDecay1 = true;
        } else {
          *ZDecay2 = PartonHistoryUtils::findAfterFSR(child)->p4();
          *ZDecay2_pdgId = child->pdgId();
          hasZDecay2 = true;
        }
      }  // for
    }  // for

    // Our job is done if the event has a real Z boson.
    if (hasZ && (!hasZDecay1 || !hasZDecay2)) m_ancestry_corrupted = true;
    if (hasZ) return hasZ && hasZDecay1 && hasZDecay2;

    // For off-shell Z bosons it's a bit more tricky. We now look
    // for two same-flavour particles that are attached to the
    // ttbar vertex.
    for (const auto& p : *truthParticles) {
      if (abs(p->pdgId()) > 19) continue;                                            // Only elementary fermions
      if (p->pdgId() < 0) continue;                                            // No anti-particles (to avoid
                                                                               // double-counting)
      const auto& sibling = getFlavourSibling(p);
      if (!sibling) continue;

      // Check whether the particle is attached to the ttbar vertex.
      bool has_top_sibling {
        false
      };
      bool has_antitop_sibling {
        false
      };
      const auto& parent = p->parent(0);
      if (!parent) continue;
      for (size_t i = 0; i < parent->nChildren(); ++i) {
        const auto* child = parent->child(i);
        if (!child) continue;
        if (child == p) continue;                                    // Make sure we don't look at our candidate.
        if (child->pdgId() == 6) has_top_sibling = true;
        if (child->pdgId() == -6) has_antitop_sibling = true;
        if (has_top_sibling && has_antitop_sibling) break;
      }
      if (!(has_top_sibling && has_antitop_sibling)) continue;

      *ZDecay1 = p->p4();
      *ZDecay1_pdgId = p->pdgId();
      hasZDecay1 = true;
      *ZDecay2 = sibling->p4();
      *ZDecay2_pdgId = sibling->pdgId();
      hasZDecay2 = true;

      *Z_p4 = *ZDecay1 + *ZDecay2;
      hasZ = true;
    }  // for

    if (hasZ && (!hasZDecay1 || !hasZDecay2)) m_ancestry_corrupted = true;
    return hasZ && hasZDecay1 && hasZDecay2;
  }

  const xAOD::TruthParticle* CalcTTZPartonHistory::getFlavourSibling(const xAOD::TruthParticle* particle) {
    const auto& parent = particle->parent(0);

    if (!parent) return nullptr;

    for (size_t i = 0; i < parent->nChildren(); ++i) {
      const auto& sibling_candidate = parent->child(i);
      if (!sibling_candidate) continue;
      if (sibling_candidate->pdgId() == -particle->pdgId()) {
        return sibling_candidate;
      }
    }
    return nullptr;
  }

  StatusCode CalcTTZPartonHistory::execute() {
    // Get the Truth Particles
    const xAOD::TruthParticleContainer* truthParticles(nullptr);

    ATH_CHECK(evtStore()->retrieve(truthParticles, m_config->sgKeyMCParticle()));

    // Create the partonHistory xAOD object
    xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer {};
    xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer {};
    partonCont->setStore(partonAuxCont);

    xAOD::PartonHistory* ttbarPartonHistory = new xAOD::PartonHistory {};
    partonCont->push_back(ttbarPartonHistory);

    // Recover the parton history for ttbar events
    ttbarHistorySaver(truthParticles, ttbarPartonHistory);
    zHistorySaver(truthParticles, ttbarPartonHistory);

    // Save to StoreGate / TStore
    std::string outputSGKey = m_config->sgKeyTopPartonHistory();
    std::string outputSGKeyAux = outputSGKey + "Aux.";

    xAOD::TReturnCode save = evtStore()->tds()->record(partonCont, outputSGKey);
    xAOD::TReturnCode saveAux = evtStore()->tds()->record(partonAuxCont, outputSGKeyAux);
    if (!save || !saveAux) {
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }
}
