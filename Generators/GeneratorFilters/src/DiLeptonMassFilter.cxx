/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/DiLeptonMassFilter.h"
#include "GaudiKernel/PhysicalConstants.h"


DiLeptonMassFilter::DiLeptonMassFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
  , m_AthenaCalls(0)
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
  m_AthenaCalls = 0;
  ATH_MSG_DEBUG("MinPt           " << m_minPt);
  ATH_MSG_DEBUG("MaxEta          " << m_maxEta);
  ATH_MSG_DEBUG("MinMass         " << m_minMass);
  ATH_MSG_DEBUG("MaxMass         " << m_maxMass);
  ATH_MSG_DEBUG("MinDileptPt     " << m_minDilepPt);
  ATH_MSG_DEBUG("AllowElecMu     " << m_allowElecMu);
  ATH_MSG_DEBUG("AllowSameCharge " << m_allowSameCharge);
  return StatusCode::SUCCESS;
}


StatusCode DiLeptonMassFilter::filterFinalize() {
  ATH_MSG_INFO(
    "\n########## DiLeptonMassFilter Statistics ##########\n"          <<
    "Filter has the following parameters:\n"                         <<
    "    Minimum pT (GeV) : " << m_minPt / Gaudi::Units::GeV << "\n" <<
    "    Maximum eta : " << m_maxEta << "\n"                         <<
    "    Mass range : (" << m_minMass << ", " << m_maxMass << ")\n"  <<
    "    Dilepton pT (GeV) : " << m_minDilepPt << "\n"               <<
    "    Allow el+mu : " << m_allowElecMu << "\n"                    <<
    "    Allow same-sign : " << m_allowSameCharge << "\n"
  );

  if (m_AthenaCalls == 0) {
    ATH_MSG_DEBUG("DiLeptonMassFilter filter is not interfaced/called.");
  } else {
    ATH_MSG_INFO(
      "\nAfter the filtering you have " << m_nPass << " events passing.\n"    <<
      "DiLeptonMassFilter efficiency = " << m_nPass << " / " << m_AthenaCalls <<
      " (Accepted/Generated) = " << m_nPass / (double)(m_AthenaCalls) << "\n" <<
      // "By event weight, that is " << m_wPass << " net weight passed.\n"            <<
      // "Weighted DiLeptonMassFilter efficiency = " << m_wPass << "/" << m_tot_wghts <<
      // " (Accepted/Generated) = " << m_wPass / m_tot_wghts << "\n"
      "########## DiLeptonMassFilter finished ##########\n"
    );
  }

  return StatusCode::SUCCESS;
}


StatusCode DiLeptonMassFilter::filterEvent() {
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    m_AthenaCalls++;

    const HepMC::GenEvent* genEvt = (*itr);
    // Loop over all particles in the event
    for (HepMC::GenEvent::particle_const_iterator pitr1 = genEvt->particles_begin(); pitr1 != genEvt->particles_end(); ++pitr1 ){
      int pdgId1((*pitr1)->pdg_id());
      if((*pitr1)->status()!=1) continue;

      // Pick electrons or muons with Pt > m_inPt and |eta| < m_maxEta
      if (abs(pdgId1) == 11 || abs(pdgId1) == 13) {
        if ((*pitr1)->momentum().perp() >= m_minPt && fabs((*pitr1)->momentum().pseudoRapidity()) <= m_maxEta){

          // Loop over all remaining particles in the event
          HepMC::GenEvent::particle_const_iterator pitr2 = pitr1;
          pitr2++;

          for(; pitr2 != genEvt->particles_end(); ++pitr2){
            int pdgId2((*pitr2)->pdg_id());
            //if((*pitr2)->status()!=1 && pitr1 != pitr2) continue;
            if((*pitr2)->status()!=1 || pitr1 == pitr2) continue;

            // Pick electrons or muons with Pt > m_inPt and |eta| < m_maxEta
            // If m_allowSameChagrge is not true only pick those with opposite charge to the first particle
            // If m_allowElecMu is true allow also Z -> emu compinations (with charge requirements as above)
            if ((m_allowSameCharge  && (abs(pdgId2) == abs(pdgId1) || (m_allowElecMu && (abs(pdgId2) == 11 || abs(pdgId2) == 13) ) ) ) ||
                (!m_allowSameCharge && (pdgId2 == -1*pdgId1 || (m_allowElecMu && (pdgId2 == (pdgId1 < 0 ? 1 : -1) * 11 || (pdgId1 < 0 ? 1 : -1) * pdgId2 == 13) ) ) ) ) {
              if ((*pitr2)->momentum().perp() >= m_minPt && fabs((*pitr2)->momentum().pseudoRapidity()) <= m_maxEta){

                // Calculate invariant mass and apply cut
                HepMC::FourVector vec((*pitr1)->momentum().px() + (*pitr2)->momentum().px(),
                                      (*pitr1)->momentum().py() + (*pitr2)->momentum().py(),
                                      (*pitr1)->momentum().pz() + (*pitr2)->momentum().pz(),
                                      (*pitr1)->momentum().e() + (*pitr2)->momentum().e());

                double invMass = vec.m();
                double dilepPt = vec.perp();

                ATH_MSG_DEBUG(" Lepton1 : Pt = " << (*pitr1)->momentum().perp() << ", Eta = " << (*pitr1)->momentum().pseudoRapidity() <<
                              ", pdgid = " << pdgId1 << ", status = " << (*pitr1)->status() << ", barcode = " << (*pitr1)->barcode() <<
							  " Lepton2 : Pt = " << (*pitr2)->momentum().perp() << ", Eta = " << (*pitr2)->momentum().pseudoRapidity() <<
							  ", pdgid = " << pdgId2 << ", status = " << (*pitr2)->status() << ", barcode = " << (*pitr2)->barcode() <<
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
