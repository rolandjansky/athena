/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Header for this module
#include "GeneratorFilters/VBFMjjIntervalFilter.h"

#include "StoreGate/DataHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h" // For random numbers...
#include "CLHEP/Random/RandomEngine.h"
#include "GaudiKernel/PhysicalConstants.h"


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
  declareProperty("MinSecondJetPT", m_pTavgMin = 15.0*Gaudi::Units::GeV);
  declareProperty("MinOverlapPT", m_olapPt = 15.0*Gaudi::Units::GeV);
  declareProperty("TruthJetContainerName", m_TruthJetContainerName = "AntiKt4TruthJets");
  //declareProperty("DoShape", m_doShape = true);
  declareProperty("NoJetProbability", m_prob0 = 0.0002);
  declareProperty("OneJetProbability", m_prob1 = 0.001);
  declareProperty("LowMjjProbability", m_prob2low = 0.005);
  declareProperty("HighMjjProbability", m_prob2high = 1.0);
  declareProperty("LowMjj", m_mjjlow=100.0*Gaudi::Units::GeV);
  declareProperty("TruncateAtLowMjj", m_truncatelowmjj = false);
  declareProperty("HighMjj", m_mjjhigh=800.0*Gaudi::Units::GeV);
  declareProperty("TruncateAtHighMjj", m_truncatehighmjj = false);
  declareProperty("PhotonJetOverlapRemoval", m_photonjetoverlap = false);
  declareProperty("ElectronJetOverlapRemoval", m_electronjetoverlap = true);
  declareProperty("TauJetOverlapRemoval", m_taujetoverlap = false);
  declareProperty("Alpha",m_alpha=log(m_prob2low/m_prob2high)/log(m_mjjlow/m_mjjhigh));
  declareProperty("ApplyNjet",m_ApplyNjet=false); 
  declareProperty("Njets", m_NJetsMin=2);  
  declareProperty("NjetsMax",m_NJetsMax=-1);
  declareProperty("ApplyWeighting",m_ApplyWeighting=true);
  declareProperty("ApplyDphi",m_applyDphi=false);
  declareProperty("dphijjMax",m_dphijj=2.5); 

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
  std::vector<HepMC::ConstGenParticlePtr> MCTruthPhotonList;
  std::vector<HepMC::ConstGenParticlePtr> MCTruthElectronList;
  std::vector<TLorentzVector> MCTruthTauList;
  for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (auto pitr: *genEvt) {
      if (m_photonjetoverlap==true) {
	// photon - copied from VBFForwardJetsFilter.cxx
	if ( pitr->pdg_id() == 22 && pitr->status() == 1 &&
	     pitr->momentum().perp() >= m_olapPt && 
	     std::abs(pitr->momentum().pseudoRapidity()) <= m_yMax) {
	  MCTruthPhotonList.push_back(pitr);
	}
      }
      if (m_electronjetoverlap==true) {
	// electron
	if (std::abs(pitr->pdg_id()) == 11 && pitr->status() == 1 &&
	    pitr->momentum().perp() >= m_olapPt &&
	    std::abs(pitr->momentum().pseudoRapidity()) <= m_yMax) {
	  MCTruthElectronList.push_back(pitr);
	}
      }
      if (m_taujetoverlap==true) {
	// tau - copied from VBFForwardJetsFilter.cxx
	if ( std::abs(pitr->pdg_id()) == 15 && pitr->status() != 3 ) {
	   auto tau = pitr;
	  int leptonic = 0;
	  for ( auto beg:  *(tau->end_vertex()) ) {
	    if (  beg->production_vertex() != tau->end_vertex() ) continue;
	    if ( std::abs( beg->pdg_id() ) == 12 ) leptonic = 1;
	    if ( std::abs( beg->pdg_id() ) == 14 ) leptonic = 2;
	    if ( std::abs( beg->pdg_id() ) == 15 ) leptonic = 11;
	  }
	  
	  if (leptonic == 0) {
	    TLorentzVector nutau = sumDaughterNeutrinos( tau );
	    TLorentzVector tauvis = TLorentzVector(tau->momentum().px()-nutau.Px(),
					           tau->momentum().py()-nutau.Py(),
			        	           tau->momentum().pz()-nutau.Pz(),
			       			   tau->momentum().e()-nutau.E());
	    if (tauvis.Vect().Perp() >= m_olapPt && std::abs(tauvis.Vect().PseudoRapidity()) <= m_yMax) {
	      MCTruthTauList.push_back(tauvis);
	    }
	  }
	}
      }
    }
  }

  // Filter based on rapidity acceptance and sort
  xAOD::JetContainer filteredJets(SG::VIEW_ELEMENTS);
  for (xAOD::JetContainer::const_iterator jitr = truthJetCollection->begin(); jitr != truthJetCollection->end(); ++jitr) {
    if (std::abs( (*jitr)->rapidity() ) < m_yMax && (*jitr)->pt() >= m_olapPt) {
      bool JetOverlapsWithPhoton   = false;
      bool JetOverlapsWithElectron = false;
      bool JetOverlapsWithTau      = false;

      if (m_photonjetoverlap==true) {
	JetOverlapsWithPhoton = checkOverlap((*jitr)->rapidity(), (*jitr)->phi(), MCTruthPhotonList);
      }
      if (m_electronjetoverlap==true) {
	JetOverlapsWithElectron = checkOverlap((*jitr)->rapidity(), (*jitr)->phi(), MCTruthElectronList);
      }
      if (m_taujetoverlap==true) {
	JetOverlapsWithTau = checkOverlap((*jitr)->rapidity(), (*jitr)->phi(), MCTruthTauList);
      }

      if (!JetOverlapsWithPhoton && !JetOverlapsWithElectron && !JetOverlapsWithTau ) {
	filteredJets.push_back(const_cast<xAOD::Jet*>(*jitr));
      }
    }
  }
  filteredJets.sort(High2LowByJetClassPt());

  if(m_ApplyWeighting){

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
 }//Apply weighting 
  else {
    //just compute mjj, dphi etc 
    bool pass = ApplyMassDphi(&filteredJets);
    if(!pass){
      setFilterPassed(false);
      ATH_MSG_DEBUG("Event failed filter");
      return StatusCode::SUCCESS; 
    }
    if(m_ApplyNjet){
      if(filteredJets.size()<m_NJetsMin){
	setFilterPassed(false);
	return StatusCode::SUCCESS;
      }
      if(m_NJetsMax>0){
	if(filteredJets.size()>m_NJetsMax){
	  setFilterPassed(false);
	  return StatusCode::SUCCESS;
	}
      }//Njets < 
    }//Apply Njets filter 
  }
  // Made it to the end - success!
  setFilterPassed(true);
  return StatusCode::SUCCESS;
}


bool VBFMjjIntervalFilter::checkOverlap(double eta, double phi, const std::vector<HepMC::ConstGenParticlePtr> &list ) {
  for (size_t i = 0; i < list.size(); ++i) {
    double pt = list[i]->momentum().perp();
    if (pt > m_olapPt) {
      /// @todo Provide a helper function for this (and similar)
      double dphi = phi-list[i]->momentum().phi();
      double deta = eta-list[i]->momentum().pseudoRapidity();
      if (dphi >  M_PI) { dphi -= 2.*M_PI; }
      if (dphi < -M_PI) { dphi += 2.*M_PI; }
      double dr = std::sqrt(deta*deta+dphi*dphi);
      if (dr < 0.3) return true;
    }
  }
  return false;
}



bool VBFMjjIntervalFilter::checkOverlap(double eta, double phi, const std::vector<TLorentzVector> &list ) {
  for (size_t i = 0; i < list.size(); ++i) {
    double pt = list[i].Vect().Perp();
    if (pt > m_olapPt) {
      /// @todo Provide a helper function for this (and similar)
      double dphi = phi-list[i].Phi();
      double deta = eta-list[i].Vect().PseudoRapidity();
      if (dphi >  M_PI) { dphi -= 2.*M_PI; }
      if (dphi < -M_PI) { dphi += 2.*M_PI; }
      double dr = std::sqrt(deta*deta+dphi*dphi);
      if (dr < 0.3) return true;
    }
  }
  return false;
}

bool VBFMjjIntervalFilter::ApplyMassDphi(xAOD::JetContainer *jets){
  if(jets->size()<2) return false; 
  double mjj = (jets->at(0)->p4() + jets->at(1)->p4()).M();
  double dphi = std::abs(jets->at(0)->p4().DeltaPhi(jets->at(1)->p4()));
  ATH_MSG_INFO("mjj " << mjj << " dphi " << dphi); 
  bool pass = true; 
  if(mjj<m_mjjlow)pass=false; 
  if(mjj>m_mjjhigh)pass=false;
  if(m_applyDphi && dphi > m_dphijj) pass = false; 
  
  return pass; 
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
      if (m_truncatelowmjj==false) {
	weight /= m_prob2low;
      }
      else {
	weight = -1.0;
      }
    } else if (mjj > m_mjjhigh) {
      if (m_truncatehighmjj==false) {
	weight /= m_prob2high;
      }
      else {
	weight = -1.0;
      }
    } else {
      weight = weight * std::pow(m_mjjlow/mjj, m_alpha) / m_prob2low;
      ATH_MSG_DEBUG("WEIGHTING:: " << mjj << "\t" << weight);
    }
  }
  return weight;
}


 TLorentzVector VBFMjjIntervalFilter::sumDaughterNeutrinos( HepMC::ConstGenParticlePtr part ) {
  TLorentzVector nu( 0, 0, 0, 0);

  if ( ( std::abs( part->pdg_id() ) == 12 ) || ( std::abs( part->pdg_id() ) == 14 ) || ( std::abs( part->pdg_id() ) == 16 ) ) {
    nu.SetPx(part->momentum().px());
    nu.SetPy(part->momentum().py());
    nu.SetPz(part->momentum().pz());
    nu.SetE(part->momentum().e());
    return nu;
  }

  if ( !part->end_vertex() ) return nu;

  for (auto beg: *(part->end_vertex()) ) nu += sumDaughterNeutrinos( beg );
  return nu;
}
