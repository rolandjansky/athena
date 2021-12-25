/*
  Copyright (C) 2020-2021 CERN for the benefit of the ATLAS collaboration
*/

// Header for this module
#include "GeneratorFilters/xAODM4MuIntervalFilter.h"

#include "StoreGate/DataHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h" // For random numbers...
#include "CLHEP/Random/RandomEngine.h"
#include "EventInfo/EventInfo.h"        // For setting the weight
#include "EventInfo/EventType.h"        // From event info - the real holder of the event weight



xAODM4MuIntervalFilter::xAODM4MuIntervalFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  
}

StatusCode xAODM4MuIntervalFilter::filterInitialize() {
  if (m_rand.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve AtRndmGenSvc " << m_rand);
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "MaxEta           "  << m_maxEta);
  ATH_MSG_DEBUG( "MinPt          "  << m_minPt);
  ATH_MSG_DEBUG( "LowM4muProbability         "  << m_prob2low);
  ATH_MSG_DEBUG( "MediumMj4muProbability         "  << m_prob2medium);
  ATH_MSG_DEBUG( "HighM4muProbability         "  << m_prob2high);
  ATH_MSG_DEBUG( "LowM4mu         "  << m_m4mulow);
  ATH_MSG_DEBUG( "HighM4mu         "  << m_m4muhigh);
  ATH_MSG_DEBUG( "ApplyReWeighting         "  << m_ApplyReWeighting);
  return StatusCode::SUCCESS;
}

StatusCode xAODM4MuIntervalFilter::filterFinalize() {
  return StatusCode::SUCCESS;
}

StatusCode xAODM4MuIntervalFilter::filterEvent() {
  // Get random number engine
  CLHEP::HepRandomEngine* rndm = m_rand->GetEngine("xAODM4MuIntervalFilter");
  if (!rndm) {
    ATH_MSG_ERROR("Failed to retrieve random number engine xAODM4MuIntervalFilter");
    setFilterPassed(false);
    return StatusCode::FAILURE;
  }

  // Find overlap objects
  std::vector<HepMC::FourVector> MCTruthMuonList;
  
// Retrieve full TruthEvent container
const xAOD::TruthEventContainer *xTruthEventContainer;
if (evtStore()->retrieve(xTruthEventContainer, "TruthEvents").isFailure())
{
    ATH_MSG_ERROR("No TruthEvents collection with name " << "TruthEvents" << " found in StoreGate!");
    return StatusCode::FAILURE;
}
xAOD::TruthEventContainer::const_iterator itr;
for (itr = xTruthEventContainer->begin(); itr!=xTruthEventContainer->end(); ++itr) {
    const xAOD::TruthEvent *genEvt = (*itr);
    unsigned int nPart = genEvt->nTruthParticles();
    for (unsigned int iPart = 0; iPart < nPart; ++iPart) {
     const xAOD::TruthParticle* pitr =  genEvt->truthParticle(iPart);
	   // muon
	   if (std::abs((pitr)->pdgId()) == 13 && (pitr)->status() == 1 &&
	      (pitr)->pt() >= m_minPt &&
	       std::abs((pitr)->eta()) <= m_maxEta) {
           HepMC::FourVector tmp((pitr)->px(), (pitr)->py(), (pitr)->pz(), (pitr)->e());
           MCTruthMuonList.push_back(tmp);
           
	       }
    }
   
  }
  
 std::sort(MCTruthMuonList.begin(), MCTruthMuonList.end(), High2LowByPt());

  if(MCTruthMuonList.size()<4){
    setFilterPassed(false);
    ATH_MSG_DEBUG("Less than 4 muons. The muon number is " << MCTruthMuonList.size());
    return StatusCode::SUCCESS;
  }
  
  if(m_ApplyReWeighting) {
    HepMC::FourVector vec4mu(MCTruthMuonList.at(0).px() + MCTruthMuonList.at(1).px() + MCTruthMuonList.at(2).px() + MCTruthMuonList.at(3).px(), 
                   MCTruthMuonList.at(0).py() + MCTruthMuonList.at(1).py() + MCTruthMuonList.at(2).py() + MCTruthMuonList.at(3).py(),
                   MCTruthMuonList.at(0).pz() + MCTruthMuonList.at(1).pz() + MCTruthMuonList.at(2).pz() + MCTruthMuonList.at(3).pz(),
                   MCTruthMuonList.at(0).e() + MCTruthMuonList.at(1).e() + MCTruthMuonList.at(2).e() + MCTruthMuonList.at(3).e() );
    
    double m4mu = vec4mu.m();
    double eventWeight = 1.0;
    eventWeight = getEventWeight(m4mu);
    double rnd = rndm->flat();
    if (1.0/eventWeight < rnd) {
      setFilterPassed(false);
      ATH_MSG_DEBUG("Event failed weighting. Weight is " << eventWeight);
      return StatusCode::SUCCESS;
    }

    // Get MC event collection for setting weight
    const DataHandle<McEventCollection> mecc = 0;
    if ( evtStore()->retrieve( mecc ).isFailure() || !mecc ){
      setFilterPassed(false);
      ATH_MSG_ERROR("Could not retrieve MC Event Collection - weight might not work");
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Event passed.  Will weight events " << eventWeight);
    McEventCollection* mec = const_cast<McEventCollection*> (&(*mecc));
    for (unsigned int i = 0; i < mec->size(); ++i) {
      if (!(*mec)[i]) continue;
      double existingWeight = (*mec)[i]->weights().size()>0 ? (*mec)[i]->weights()[0] : 1.;
      if ((*mec)[i]->weights().size()>0) {
        (*mec)[i]->weights()[0] = existingWeight*eventWeight;
      } else {
        (*mec)[i]->weights().push_back( eventWeight*existingWeight );
      }
    }
  }
  // Made it to the end - success!
  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

double xAODM4MuIntervalFilter::getEventWeight(double mass) {
  double weight = 1.0;
  if (mass < m_m4mulow) {
       	weight /= m_prob2low;

  } else if (mass > m_m4muhigh) {
	       weight /= m_prob2high;

  } else {
         weight /= m_prob2medium;
    }
  ATH_MSG_DEBUG("WEIGHTING:: " << mass << "\t" << weight);
  return weight;
}
