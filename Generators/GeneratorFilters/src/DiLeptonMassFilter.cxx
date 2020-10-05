/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/DiLeptonMassFilter.h"


DiLeptonMassFilter::DiLeptonMassFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("MinPt",            m_minPt           = 5000.);
  declareProperty("MaxEta",           m_maxEta          = 5.0);
  declareProperty("MinMass",          m_minMass         = 1000);      // To avoid fsr etc
  declareProperty("MaxMass",          m_maxMass         = 14000000);
  declareProperty("MinDilepPt",       m_minDilepPt      = -1.);
  declareProperty("AllowElecMu",      m_allowElecMu     = false);
  declareProperty("AllowSameCharge",  m_allowSameCharge = true);
}


StatusCode DiLeptonMassFilter::filterInitialize() {
  ATH_MSG_DEBUG("MinPt           " << m_minPt);
  ATH_MSG_DEBUG("MaxEta          " << m_maxEta);
  ATH_MSG_DEBUG("MinMass         " << m_minMass);
  ATH_MSG_DEBUG("MaxMass         " << m_maxMass);
  ATH_MSG_DEBUG("MinDileptPt     " << m_minDilepPt);
  ATH_MSG_DEBUG("AllowElecMu     " << m_allowElecMu);
  ATH_MSG_DEBUG("AllowSameCharge " << m_allowSameCharge);
  return StatusCode::SUCCESS;
}


StatusCode DiLeptonMassFilter::filterEvent() {
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    // Loop over all particles in the event
    for (auto pitr1 = HepMC::begin(*genEvt);  pitr1!=HepMC::end(*genEvt); ++pitr1 ){
      int pdgId1((*pitr1)->pdg_id());
      if((*pitr1)->status()!=1) continue;

      // Pick electrons or muons with Pt > m_inPt and |eta| < m_maxEta
      if (std::abs(pdgId1) == 11 || std::abs(pdgId1) == 13) {
        if ((*pitr1)->momentum().perp() >= m_minPt && std::abs((*pitr1)->momentum().pseudoRapidity()) <= m_maxEta){

          // Loop over all remaining particles in the event
          auto pitr2 = pitr1;
          pitr2++;

          for(; pitr2 != HepMC::end(*genEvt); ++pitr2){
            int pdgId2((*pitr2)->pdg_id());
            //if((*pitr2)->status()!=1 && pitr1 != pitr2) continue;
            if((*pitr2)->status()!=1 || pitr1 == pitr2) continue;

            // Pick electrons or muons with Pt > m_inPt and |eta| < m_maxEta
            // If m_allowSameChagrge is not true only pick those with opposite charge to the first particle
            // If m_allowElecMu is true allow also Z -> emu compinations (with charge requirements as above)
            if ((m_allowSameCharge  && (std::abs(pdgId2) == std::abs(pdgId1) || (m_allowElecMu && (std::abs(pdgId2) == 11 || std::abs(pdgId2) == 13) ) ) ) ||
                (!m_allowSameCharge && (pdgId2 == -1*pdgId1 || (m_allowElecMu && (pdgId2 == (pdgId1 < 0 ? 1 : -1) * 11 || (pdgId1 < 0 ? 1 : -1) * pdgId2 == 13) ) ) ) ) {
              if ((*pitr2)->momentum().perp() >= m_minPt && std::abs((*pitr2)->momentum().pseudoRapidity()) <= m_maxEta){

                // Calculate invariant mass and apply cut
                HepMC::FourVector vec((*pitr1)->momentum().px() + (*pitr2)->momentum().px(),
                                      (*pitr1)->momentum().py() + (*pitr2)->momentum().py(),
                                      (*pitr1)->momentum().pz() + (*pitr2)->momentum().pz(),
                                      (*pitr1)->momentum().e() + (*pitr2)->momentum().e());

                double invMass = vec.m();
                double dilepPt = vec.perp();

                ATH_MSG_DEBUG(" Lepton1 : Pt = " << (*pitr1)->momentum().perp() << ", Eta = " << (*pitr1)->momentum().pseudoRapidity() <<
                              ", pdgid = " << pdgId1 << ", status = " << (*pitr1)->status() << ", barcode = " << HepMC::barcode(*pitr1) <<
							  " Lepton2 : Pt = " << (*pitr2)->momentum().perp() << ", Eta = " << (*pitr2)->momentum().pseudoRapidity() <<
							  ", pdgid = " << pdgId2 << ", status = " << (*pitr2)->status() << ", barcode = " << HepMC::barcode(*pitr2) <<
							  " Mass = " << invMass <<
							  " DilepPt = " << dilepPt);

                if (m_minMass < invMass && invMass < m_maxMass && dilepPt > m_minDilepPt) {
                  ATH_MSG_DEBUG("PASSED FILTER");
                  return StatusCode::SUCCESS;
                }
              }
            }
          }
        }
      }
    }
  }

  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
