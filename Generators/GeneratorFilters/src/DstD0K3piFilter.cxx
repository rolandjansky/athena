/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/DstD0K3piFilter.h"


DstD0K3piFilter::DstD0K3piFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("DstPtcut", m_Ptmin    = 4500.);
  declareProperty("DstEtacut",m_EtaRange = 2.5);
}


// If mcpart first child lundId equals chLundId, return true
/// @todo No need for this to be a member function... useful in a HepMC utils library?
bool DstD0K3piFilter::CheckChildLundId(HepMC::ConstGenParticlePtr mcpart, unsigned int nth, int chLundId) {
#ifdef HEPMC3
  auto  DecayVtx = mcpart->end_vertex();
  if (!DecayVtx) return false;
  auto children=DecayVtx->particles_out();
  if (children.size() < 2) return false;
  if (children.size() < nth) return false;
  if (std::abs(children.at(nth)->pdg_id()) == chLundId) return true;
  return false;
#else
  int nChild = 0;
  const HepMC::GenVertex* DecayVtx = mcpart->end_vertex();
  if (DecayVtx != 0) nChild = DecayVtx->particles_out_size();
  if (nChild < 2) return false;
  if (nth ==0) {
    HepMC::GenVertex::particles_in_const_iterator child_mcpartItr  = DecayVtx->particles_out_const_begin();
    HepMC::GenParticle* child_mcpart = *child_mcpartItr;
    if (std::abs(child_mcpart->pdg_id()) == chLundId) return true;
  } else if (nth == 1) {
    HepMC::GenVertex::particles_in_const_iterator child_mcpartItr  = DecayVtx->particles_out_const_begin();
    HepMC::GenVertex::particles_in_const_iterator child_mcpartItrE = DecayVtx->particles_out_const_end();
    ++child_mcpartItr;
    if (child_mcpartItr == child_mcpartItrE) return false;
    HepMC::GenParticle* child_mcpart = *child_mcpartItr;
    if (std::abs(child_mcpart->pdg_id()) == chLundId) return true;
  }
  return false;
#endif  
}


/// @todo No need for this to be a member function
bool DstD0K3piFilter::IsCandidate(std::vector<float>& lundIds, std::vector<HepMC::ConstGenParticlePtr>& genParticles) {
#ifdef HEPMC3
  unsigned int nDecay = lundIds.size();
  if (nDecay == 2) {
    unsigned int id0 = std::abs( static_cast<int>(lundIds[0]) );
    unsigned int id1 = std::abs( static_cast<int>(lundIds[1]) );
    // 10323  211   k_1+ CLHEP::pi-
    if ( id0 == 10323 && id1 == 211 ) { // // K_1+ CLHEP::pi-
      if ( CheckChildLundId(genParticles[0], 0, 313) ) { // K*0 pi+
        auto DecayVtx = genParticles[0]->end_vertex();
        if ( !DecayVtx) return false;
        auto children=DecayVtx->particles_out();
        if ( children.size() != 2 ) return false;
        if ( CheckChildLundId(children[0], 0, 321) ) return true;
      } else if ( CheckChildLundId(genParticles[0], 0, 321) ) {  // K+ rho0 or K+ CLHEP::pi CLHEP::pi
        if ( CheckChildLundId(genParticles[0], 1, 113) || CheckChildLundId(genParticles[0], 1, 211) ) return true;
      }
    } else if (id0 == 313 && id1 == 113) { // // K^*0 rho0
      if ( CheckChildLundId(genParticles[0], 0, 321) )  // rho0 decays to CLHEP::pi+CLHEP::pi- 100% in decay.dec
        return true;
    } else if (id0 == 321 && id1 == 20213) { // // K+ a_1+
      if ( CheckChildLundId(genParticles[1], 0, 113) )  // a_1+ -> rho0 (113) CLHEP::pi+
        return true;
    }

  } else if ( nDecay == 3 ) {
    unsigned int id0 = std::abs( static_cast<int>(lundIds[0]) );
    unsigned int id1 = std::abs( static_cast<int>(lundIds[1]) );
    unsigned int id2 = std::abs( static_cast<int>(lundIds[2]) );
    if (id0 == 321 && id1 == 211 && id2 == 113) { // K+ CLHEP::pi- rho0
      return true;  // rho0 decays to CLHEP::pi+CLHEP::pi- 100% in decay.dec
    }
    if (id0 == 313 && id1 == 211 && id2 == 211 ) { // K^*+ CLHEP::pi- CLHEP::pi+
      if ( CheckChildLundId(genParticles[0], 0, 321) ) return true;
    }

  } else if (nDecay ==4) { // K CLHEP::pi CLHEP::pi CLHEP::pi non-resonant
    unsigned int id0 = std::abs( static_cast<int>(lundIds[0]) );
    unsigned int id1 = std::abs( static_cast<int>(lundIds[1]) );
    unsigned int id2 = std::abs( static_cast<int>(lundIds[2]) );
    unsigned int id3 = std::abs( static_cast<int>(lundIds[3]) );
    if (id0 == 321 && id1 == 211 && id2 == 211 && id3 == 211) {
      return true;
    }
  }
  return false;
#else  
  unsigned int nDecay = lundIds.size();
  if (nDecay == 2) {
    unsigned int id0 = std::abs( static_cast<int>(lundIds[0]) );
    unsigned int id1 = std::abs( static_cast<int>(lundIds[1]) );
    // 10323  211   k_1+ CLHEP::pi-
    if ( id0 == 10323 && id1 == 211 ) { // // K_1+ CLHEP::pi-
      if ( CheckChildLundId(genParticles[0], 0, 313) ) { // K*0 pi+
        int nChild = 0;
        const HepMC::GenVertex * DecayVtx = genParticles[0]->end_vertex();
        if ( DecayVtx != 0 ) nChild = DecayVtx->particles_out_size();
        if ( nChild != 2 ) return false;

        HepMC::GenVertex::particles_in_const_iterator child_mcpartItr  = DecayVtx->particles_out_const_begin();
        HepMC::GenParticle * child_mcpart = (*child_mcpartItr); // K*0
        if ( CheckChildLundId(child_mcpart, 0, 321) ) return true;
      } else if ( CheckChildLundId(genParticles[0], 0, 321) ) {  // K+ rho0 or K+ CLHEP::pi CLHEP::pi
        if ( CheckChildLundId(genParticles[0], 1, 113) || CheckChildLundId(genParticles[0], 1, 211) ) return true;
      }
    } else if (id0 == 313 && id1 == 113) { // // K^*0 rho0
      if ( CheckChildLundId(genParticles[0], 0, 321) )  // rho0 decays to CLHEP::pi+CLHEP::pi- 100% in decay.dec
        return true;
    } else if (id0 == 321 && id1 == 20213) { // // K+ a_1+
      if ( CheckChildLundId(genParticles[1], 0, 113) )  // a_1+ -> rho0 (113) CLHEP::pi+
        return true;
    }

  } else if ( nDecay == 3 ) {
    unsigned int id0 = std::abs( static_cast<int>(lundIds[0]) );
    unsigned int id1 = std::abs( static_cast<int>(lundIds[1]) );
    unsigned int id2 = std::abs( static_cast<int>(lundIds[2]) );
    if (id0 == 321 && id1 == 211 && id2 == 113) { // K+ CLHEP::pi- rho0
      return true;  // rho0 decays to CLHEP::pi+CLHEP::pi- 100% in decay.dec
    }
    if (id0 == 313 && id1 == 211 && id2 == 211 ) { // K^*+ CLHEP::pi- CLHEP::pi+
      if ( CheckChildLundId(genParticles[0], 0, 321) ) return true;
    }

  } else if (nDecay ==4) { // K CLHEP::pi CLHEP::pi CLHEP::pi non-resonant
    unsigned int id0 = std::abs( static_cast<int>(lundIds[0]) );
    unsigned int id1 = std::abs( static_cast<int>(lundIds[1]) );
    unsigned int id2 = std::abs( static_cast<int>(lundIds[2]) );
    unsigned int id3 = std::abs( static_cast<int>(lundIds[3]) );
    if (id0 == 321 && id1 == 211 && id2 == 211 && id3 == 211) {
      return true;
    }
  }
  return false;
#endif  
}


StatusCode DstD0K3piFilter::filterEvent() {
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) { // Loop to select D*
    const HepMC::GenEvent* genEvt = *itr;
#ifdef HEPMC3
    for (auto  pitr: *genEvt) {
      if (std::abs(pitr->pdg_id()) != 413) continue; // D*+
      if (pitr->momentum().perp() < m_Ptmin) continue;
      if (std::abs(pitr->momentum().pseudoRapidity()) > m_EtaRange) continue;
      auto DstDecayVtx = pitr->end_vertex();
      if (!DstDecayVtx) continue;       // Check that we got a valid pointer and retrieve the number of daughters
      auto DstChild = DstDecayVtx->particles_out();
      ATH_MSG_DEBUG("D*+- meson found with Nchild = " << DstChild.size() << " and PDG ID = " << pitr->pdg_id());
      if (DstChild.size() != 2) continue;
      for ( auto  mcpartD0: DstChild ) {
        if (std::abs(mcpartD0->pdg_id()) != 421) continue; // D0
        auto D0DecayVtx = mcpartD0->end_vertex();        
        if (!D0DecayVtx)  continue; // Check that we got a valid pointer and retrieve the number of daughters
        auto D0Child = D0DecayVtx->particles_out();
        if (D0Child.size() > 4) continue; //  For this analysis we are only interested in D*->D0 pi+
        ATH_MSG_DEBUG("D0 meson found with Nchild = " << D0Child.size() << " and PDF ID = " << mcpartD0->pdg_id());

        std::vector<float> lundIds;
        std::vector<HepMC::ConstGenParticlePtr> genParticles;
        for (auto  grandchild: *D0DecayVtx){
            genParticles.push_back(grandchild);
            lundIds.push_back(grandchild->pdg_id());
        }
        if (IsCandidate(lundIds, genParticles)) {
          return StatusCode::SUCCESS;
        }
      }
    }
#else
    for (HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr!=genEvt->particles_end(); ++pitr) {
      // Work only with D*
      if (std::abs((*pitr)->pdg_id()) != 413) continue; // D*+
      if ((*pitr)->momentum().perp() < m_Ptmin) continue;
      if (std::abs((*pitr)->momentum().pseudoRapidity()) > m_EtaRange) continue;

      int nDstChild = 0;
      HepMC::GenParticle* mcpartDst = *pitr;
      const HepMC::GenVertex* DstDecayVtx = mcpartDst->end_vertex();
      // Check that we got a valid pointer and retrieve the number of daughters
      if (DstDecayVtx != 0) nDstChild = DstDecayVtx->particles_out_size();
      //  For this analysis we are only interested in D*->D0 pi+
      /// @todo Which analysis? Still valid?
      ATH_MSG_DEBUG("Dst meson found with Nchild = " << nDstChild << " and PDG ID = " << (*pitr)->pdg_id());
      if (nDstChild != 2) continue;

      HepMC::GenVertex::particles_in_const_iterator child_mcpartItr  = DstDecayVtx->particles_out_const_begin();
      HepMC::GenVertex::particles_in_const_iterator child_mcpartItrE = DstDecayVtx->particles_out_const_end();
      for (; child_mcpartItr != child_mcpartItrE; ++child_mcpartItr) {
        HepMC::GenParticle* child_mcpart = (*child_mcpartItr);
        if (std::abs(child_mcpart->pdg_id()) != 421) continue; // D0
        int nD0Child = 0;
        HepMC::GenParticle* mcpartD0 = *child_mcpartItr;
        const HepMC::GenVertex* D0DecayVtx = mcpartD0->end_vertex();
        // Check that we got a valid pointer and retrieve the number of daughters
        if (D0DecayVtx != 0) nD0Child = D0DecayVtx->particles_out_size();
        //  For this analysis we are only interested in D*->D0 pi+
        if (nD0Child > 4) continue;
        ATH_MSG_DEBUG("D0 meson found with Nchild = " << nD0Child << " and PDF ID = " << child_mcpart->pdg_id());

        std::vector<float> lundIds;
        std::vector<const HepMC::GenParticle*> genParticles;

        if (D0DecayVtx) {
          HepMC::GenVertex::particles_in_const_iterator grandchild_mcpartItr  = D0DecayVtx->particles_out_const_begin();
          HepMC::GenVertex::particles_in_const_iterator grandchild_mcpartItrE = D0DecayVtx->particles_out_const_end();
          for (; grandchild_mcpartItr != grandchild_mcpartItrE; ++grandchild_mcpartItr) {
            HepMC::GenParticle * grandchild_mcpart = (*grandchild_mcpartItr);
            genParticles.push_back(grandchild_mcpart);
            lundIds.push_back(grandchild_mcpart->pdg_id());
          }
        }

        if (IsCandidate(lundIds, genParticles)) {
          std::stringstream ss;
          ss << "lundIds : ";
          for (unsigned int i = 0; i < lundIds.size(); ++i) {
            ss << "  " << lundIds[i];
          }
          ATH_MSG_DEBUG(ss.str());
          return StatusCode::SUCCESS;
        }
      }
    }
#endif    
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
