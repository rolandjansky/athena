/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header for this module
#include "GeneratorFilters/VBFMjjIntervalFilter.h"

#include "StoreGate/DataHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h" // For random numbers...
#include "CLHEP/Random/RandomEngine.h"
#include "EventInfo/EventInfo.h"        // For setting the weight
#include "EventInfo/EventType.h"        // From event info - the real holder of the event weight


// Pt  High --> Low
class High2LowByJetClassPt {
public:
  bool operator () (const xAOD::Jet *t1, const xAOD::Jet *t2) const {
    return (t1->pt() > t2->pt());
  }
};


VBFMjjIntervalFilter::VBFMjjIntervalFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator),
    m_rand("AtRndmGenSvc", name),
    m_norm(1.) //< @todo Scalefactor always set to 1.0! Remove?
{
  declareProperty("RapidityAcceptance", m_yMax = 5.0);
  declareProperty("MinSecondJetPT", m_pTavgMin = 15.0*CLHEP::GeV);
  declareProperty("MinOverlapPT", m_olapPt = 15.0*CLHEP::GeV);
  declareProperty("TruthJetContainerName", m_TruthJetContainerName = "AntiKt4TruthJets");
  //declareProperty("DoShape", m_doShape = true);
  declareProperty("NoJetProbability", m_prob0 = 0.0002);
  declareProperty("OneJetProbability", m_prob1 = 0.001);
  declareProperty("LowMjjProbability", m_prob2low = 0.005);
  declareProperty("HighMjjProbability", m_prob2high = 1.0);
  declareProperty("LowMjj", m_mjjlow=100.0*CLHEP::GeV);
  declareProperty("HighMjj", m_mjjhigh=800.0*CLHEP::GeV);
  declareProperty("Alpha",m_alpha=log(m_prob2low/m_prob2high)/log(m_mjjlow/m_mjjhigh));
}


StatusCode VBFMjjIntervalFilter::filterInitialize() {
  ATH_MSG_INFO("Configured for jets in " << m_TruthJetContainerName << " inside |y|<" << m_yMax);
  if (m_rand.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve AtRndmGenSvc " << m_rand);
    return StatusCode::FAILURE;
  }

  m_alpha = log(m_prob2low/m_prob2high)/log(m_mjjlow/m_mjjhigh);
  ATH_MSG_INFO("m_alpha set to" << m_alpha);
  return StatusCode::SUCCESS;
}


StatusCode VBFMjjIntervalFilter::filterEvent() {
  // Get random number engine
  CLHEP::HepRandomEngine* rndm = m_rand->GetEngine("VBFMjjIntervalFilter");
  if (!rndm) {
    ATH_MSG_ERROR("Failed to retrieve random number engine VBFMjjIntervalFilter");
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  // Retrieve jet container
  const DataHandle<xAOD::JetContainer> truthJetCollection = 0;
  if ( !evtStore()->contains<xAOD::JetContainer>( m_TruthJetContainerName ) ||
       evtStore()->retrieve( truthJetCollection, m_TruthJetContainerName).isFailure() ||
       !truthJetCollection ) {
    ATH_MSG_ERROR("No xAOD::JetContainer found in StoreGate with key " << m_TruthJetContainerName);
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  // Find overlap objects
  std::vector<HepMC::GenParticle*> MCTruthElectronList;
  for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin();	pitr != genEvt->particles_end(); ++pitr) {
      // electron
      if (abs((*pitr)->pdg_id()) == 11 && (*pitr)->status() == 1 &&
          (*pitr)->momentum().perp() >= m_olapPt &&
          fabs((*pitr)->momentum().pseudoRapidity()) <= m_yMax) {
        MCTruthElectronList.push_back((*pitr));
      }
    }
  }

  // Filter based on rapidity acceptance and sort
  xAOD::JetContainer filteredJets(SG::VIEW_ELEMENTS);
  for (xAOD::JetContainer::const_iterator jitr = truthJetCollection->begin(); jitr != truthJetCollection->end(); ++jitr) {
    if (fabs( (*jitr)->rapidity() ) < m_yMax && (*jitr)->pt() >= m_olapPt) {
      bool notJet = checkOverlap((*jitr)->rapidity(), (*jitr)->phi(), MCTruthElectronList);
      if (!notJet) filteredJets.push_back(const_cast<xAOD::Jet*>(*jitr));
    }
  }
  filteredJets.sort(High2LowByJetClassPt());

  double eventWeight = 1.0;
  eventWeight = getEventWeight(&filteredJets);
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
    return StatusCode::SUCCESS;
  }

  ATH_MSG_INFO("Event passed.  Will weight events " << eventWeight*m_norm);
  McEventCollection* mec = const_cast<McEventCollection*> (&(*mecc));
  for (unsigned int i = 0; i < mec->size(); ++i) {
    if (!(*mec)[i]) continue;
    double existingWeight = (*mec)[i]->weights().size()>0 ? (*mec)[i]->weights()[0] : 1.;
    if ((*mec)[i]->weights().size()>0) {
      (*mec)[i]->weights()[0] = existingWeight*eventWeight*m_norm;
    } else {
      (*mec)[i]->weights().push_back( eventWeight*m_norm*existingWeight );
    }
  }

  // Made it to the end - success!
  setFilterPassed(true);
  return StatusCode::SUCCESS;
}


bool VBFMjjIntervalFilter::checkOverlap(double eta, double phi, std::vector<HepMC::GenParticle*> list) {
  for (size_t i = 0; i < list.size(); ++i) {
    double pt = list[i]->momentum().perp();
    if (pt > m_olapPt) {
      /// @todo Provide a helper function for this (and similar)
      double dphi = phi-list[i]->momentum().phi();
      double deta = eta-list[i]->momentum().pseudoRapidity();
      if (dphi >  M_PI) { dphi -= 2.*M_PI; }
      if (dphi < -M_PI) { dphi += 2.*M_PI; }
      double dr = sqrt(deta*deta+dphi*dphi);
      if (dr < 0.3) return true;
    }
  }
  return false;
}


double VBFMjjIntervalFilter::getEventWeight(xAOD::JetContainer *jets) {
  double weight = 1.0;
  if (jets->size() == 0) {
    weight /= m_prob0;
    ATH_MSG_DEBUG("Event in 0-jet weighting. Weight is " << weight);
  } else if (jets->size() == 1) {
    weight /= m_prob1;
    ATH_MSG_DEBUG("Event in 1-jet weighting. Weight is " << weight);
  } else {
    double mjj = (jets->at(0)->p4() + jets->at(1)->p4()).M();
    if (mjj < m_mjjlow) {
      weight /= m_prob2low;
    } else if (mjj > m_mjjhigh) {
      weight /= m_prob2high;
    } else {
      weight = weight * pow(m_mjjlow/mjj, m_alpha) / m_prob2low;
      ATH_MSG_DEBUG("WEIGHTING:: " << mjj << "\t" << weight);
    }
  }
  return weight;
}
