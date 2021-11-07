/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/xAODDiLeptonMassFilter.h"
#include "GaudiKernel/PhysicalConstants.h"

xAODDiLeptonMassFilter::xAODDiLeptonMassFilter(const std::string& name, ISvcLocator* pSvcLocator)
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


StatusCode xAODDiLeptonMassFilter::filterInitialize() {
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

StatusCode xAODDiLeptonMassFilter::filterFinalize() {
  ATH_MSG_INFO(
    "\n########## xAODDiLeptonMassFilter Statistics ##########\n"          <<
    "Filter has the following parameters:\n"                         <<
    "    Minimum pT (GeV) : " << m_minPt / Gaudi::Units::GeV << "\n" <<
    "    Maximum eta : " << m_maxEta << "\n"                         <<
    "    Mass range : (" << m_minMass << ", " << m_maxMass << ")\n"  <<
    "    Dilepton pT (GeV) : " << m_minDilepPt << "\n"               <<
    "    Allow el+mu : " << m_allowElecMu << "\n"                    <<
    "    Allow same-sign : " << m_allowSameCharge << "\n"
  );

  if (m_AthenaCalls == 0) {
    ATH_MSG_DEBUG("xAODDiLeptonMassFilter filter is not interfaced/called.");
  } else {
    ATH_MSG_INFO(
      "\nAfter the filtering you have " << m_nPass << " events passing.\n"    <<
      "xAODDiLeptonMassFilter efficiency = " << m_nPass << " / " << m_AthenaCalls <<
      " (Accepted/Generated) = " << m_nPass / (double)(m_AthenaCalls) << "\n" <<
      "########## xAODDiLeptonMassFilter finished ##########\n"
    );
  }

  return StatusCode::SUCCESS;
}


StatusCode xAODDiLeptonMassFilter::filterEvent() {
  // Retrieve TruthLightLepton container from xAOD LightLepton slimmer, contains (electrons and muons ) particles
  const xAOD::TruthParticleContainer* xTruthParticleContainer;
  if (evtStore()->retrieve(xTruthParticleContainer, "TruthLightLeptons").isFailure()) {
      ATH_MSG_ERROR("No TruthParticle collection with name " << "TruthLightLepton" << " found in StoreGate!");
      return StatusCode::FAILURE;
  }


  unsigned int nParticles = xTruthParticleContainer->size();
  //loop over all particles
  for (unsigned int iPart=0; iPart<nParticles; ++iPart) {
    const xAOD::TruthParticle* lightLeptonParticle = (*xTruthParticleContainer)[iPart];
    int pdgId1 = lightLeptonParticle->pdgId();
    if (lightLeptonParticle->status()!=1) continue;

    // Pick electrons or muons with Pt > m_inPt and |eta| < m_maxEta
      if (std::abs(pdgId1) == 11 || std::abs(pdgId1) == 13) {
        if (lightLeptonParticle->pt() >= m_minPt && std::abs(lightLeptonParticle->eta()) <= m_maxEta){
        //loop over all remaining particles in the event

          for(unsigned int iPart2=iPart + 1; iPart2<nParticles; ++iPart2) {
            const xAOD::TruthParticle* lightLeptonParticle2 = (*xTruthParticleContainer)[iPart2];
            int pdgId2 = lightLeptonParticle2->pdgId();
            if(lightLeptonParticle->status()!=1 || iPart == iPart2) continue;

            // Pick electrons or muons with Pt > m_inPt and |eta| < m_maxEta
            // If m_allowSameChagrge is not true only pick those with opposite charge to the first particle
            // If m_allowElecMu is true allow also Z -> emu compinations (with charge requirements as above)
            if ((m_allowSameCharge  && (std::abs(pdgId2) == std::abs(pdgId1) || (m_allowElecMu && (std::abs(pdgId2) == 11 || std::abs(pdgId2) == 13) ) ) ) ||
                (!m_allowSameCharge && (pdgId2 == -1*pdgId1 || (m_allowElecMu && (pdgId2 == (pdgId1 < 0 ? 1 : -1) * 11 || (pdgId1 < 0 ? 1 : -1) * pdgId2 == 13) ) ) ) ) {
              if (lightLeptonParticle2->pt() >= m_minPt && std::abs(lightLeptonParticle2->eta()) <= m_maxEta){
                // Calculate invariant mass and apply cut
                HepMC::FourVector vec(lightLeptonParticle->px() + lightLeptonParticle2->px(),
                                      lightLeptonParticle->py() + lightLeptonParticle2->py(),
                                      lightLeptonParticle->pz() + lightLeptonParticle2->pz(),
                                      lightLeptonParticle->e() + lightLeptonParticle2->e());

                double invMass = vec.m();
                double dilepPt = vec.perp();

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

  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
