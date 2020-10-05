/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/FourLeptonMassFilter.h"


FourLeptonMassFilter::FourLeptonMassFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("MinPt",             m_minPt           = 5000.);
  declareProperty("MaxEta",            m_maxEta          = 3.0);
  declareProperty("MinMass1",          m_minMass1        = 60000);      // To avoid fsr etc
  declareProperty("MaxMass1",          m_maxMass1        = 14000000);
  declareProperty("MinMass2",          m_minMass2        = 12000);      // To avoid fsr etc
  declareProperty("MaxMass2",          m_maxMass2        = 14000000);
  declareProperty("AllowElecMu",       m_allowElecMu     = true);
  declareProperty("AllowSameCharge",   m_allowSameCharge = true);
}


StatusCode FourLeptonMassFilter::filterInitialize() {
  ATH_MSG_DEBUG("MinPt           "  << m_minPt);
  ATH_MSG_DEBUG("MaxEta          "  << m_maxEta);
  ATH_MSG_DEBUG("MinMass1         " << m_minMass1);
  ATH_MSG_DEBUG("MaxMass1         " << m_maxMass1);
  ATH_MSG_DEBUG("MinMass2         " << m_minMass2);
  ATH_MSG_DEBUG("MaxMass2         " << m_maxMass2);
  ATH_MSG_DEBUG("AllowElecMu     "  << m_allowElecMu);
  ATH_MSG_DEBUG("AllowSameCharge "  << m_allowSameCharge);
  return StatusCode::SUCCESS;
}


StatusCode FourLeptonMassFilter::filterEvent() {
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = *itr;
    auto genEvt_particles_begin = HepMC::begin(*genEvt);
    auto genEvt_particles_end = HepMC::begin(*genEvt);
    for (auto  pitr1 = genEvt_particles_begin; pitr1 != genEvt_particles_end; ++pitr1) {
	  if ((*pitr1)->status() != 1) continue;

	  // Pick electrons or muons with Pt > m_inPt and |eta| < m_maxEta
	  int pdgId1((*pitr1)->pdg_id());
	  if (!(std::abs(pdgId1) == 11 || std::abs(pdgId1) == 13)) continue;
	  if (!((*pitr1)->momentum().perp() >= m_minPt && std::abs((*pitr1)->momentum().pseudoRapidity()) <= m_maxEta)) continue;

	  // Loop over all remaining particles in the event
	  auto pitr2 = pitr1;
	  pitr2++;

	  for (; pitr2 != genEvt_particles_end; ++pitr2) {
        if ((*pitr2)->status()!=1 || pitr1 == pitr2) continue;
        // Pick electrons or muons with Pt > m_inPt and |eta| < m_maxEta
        int pdgId2((*pitr2)->pdg_id());
        if (!(std::abs(pdgId2) == 11 || abs(pdgId2) == 13)) continue;
        if (!((*pitr2)->momentum().perp() >= m_minPt && std::abs((*pitr2)->momentum().pseudoRapidity()) <= m_maxEta)) continue;

        // Loop over all remaining particles in the event
        auto pitr3 = pitr2;
        pitr3++;

        for (; pitr3 != genEvt_particles_end; ++pitr3) {
		  if ((*pitr3)->status()!=1 || pitr1 == pitr3 || pitr2 == pitr3 ) continue;

		  // Pick electrons or muons with Pt > m_inPt and |eta| < m_maxEta
		  int pdgId3((*pitr3)->pdg_id());
		  if (!(std::abs(pdgId3) == 11 || std::abs(pdgId3) == 13)) continue;
		  if (!((*pitr3)->momentum().perp() >= m_minPt && std::abs((*pitr3)->momentum().pseudoRapidity()) <= m_maxEta)) continue;

          // Loop over all remaining particles in the event
		  auto pitr4 = pitr3;
		  pitr4++;

		  for (; pitr4 != genEvt_particles_end; ++pitr4) {
            if ((*pitr4)->status()!=1 || pitr1 == pitr4 || pitr2 == pitr4 || pitr3 == pitr4) continue;

            // Pick electrons or muons with Pt > m_inPt and |eta| < m_maxEta
            int pdgId4((*pitr4)->pdg_id());
            if (!(std::abs(pdgId4) == 11 || abs(pdgId4) == 13)) continue;
            if (!((*pitr4)->momentum().perp() >= m_minPt && std::abs((*pitr4)->momentum().pseudoRapidity()) <= m_maxEta)) continue;

            decltype(pitr1) apitr[4] = {pitr1,pitr2,pitr3,pitr4};
            int pdgIds[4]={pdgId1,pdgId2,pdgId3,pdgId4};
            for (int ii = 0; ii < 4; ii++) {
			  for (int jj = 0; jj < 4; jj++) {
                if (jj == ii) continue;
                for (int kk = 0; kk < 4; kk++) {
                  if (kk == jj || kk == ii) continue;
                  for (int ll = 0; ll < 4; ll++) {
                    if (ll == kk || ll == jj|| ll == ii) continue;
                    if (!m_allowElecMu && (std::abs(pdgIds[ii])!=abs(pdgIds[jj]) ||  abs(pdgIds[kk])!=abs(pdgIds[ll]))) continue;
                    if (!m_allowSameCharge && (pdgIds[ii]*pdgIds[jj]>0. ||  pdgIds[kk]*pdgIds[ll]>0.)) continue;

                    // Leading dilepton pair
                    HepMC::FourVector vec((*(apitr[ii]))->momentum().px() + (*(apitr[jj]))->momentum().px(),
                                          (*(apitr[ii]))->momentum().py() + (*(apitr[jj]))->momentum().py(),
                                          (*(apitr[ii]))->momentum().pz() + (*(apitr[jj]))->momentum().pz(),
                                          (*(apitr[ii]))->momentum().e()  + (*(apitr[jj]))->momentum().e() );
                    double invMass1 = vec.m();
                    if (invMass1 < m_minMass1 || invMass1 > m_maxMass1) continue;
                    ATH_MSG_DEBUG("PASSED FILTER1 " << invMass1);

                    // Sub-leading dilepton pair
                    HepMC::FourVector vec2((*(apitr[kk]))->momentum().px() + (*(apitr[ll]))->momentum().px(),
                                           (*(apitr[kk]))->momentum().py() + (*(apitr[ll]))->momentum().py(),
                                           (*(apitr[kk]))->momentum().pz() + (*(apitr[ll]))->momentum().pz(),
                                           (*(apitr[kk]))->momentum().e()  + (*(apitr[ll]))->momentum().e() );
                    double invMass2 = vec2.m();
                    if (invMass2 < m_minMass2 || invMass2 > m_maxMass2) continue;
                    ATH_MSG_DEBUG("PASSED FILTER2 " << invMass2);

                    return StatusCode::SUCCESS;
                  }
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
