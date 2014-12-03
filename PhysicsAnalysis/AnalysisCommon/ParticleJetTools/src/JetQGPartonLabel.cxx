/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JetQGPartonLabel.cxx
//   Source file for class JetQGPartonLabel
///////////////////////////////////////////////////////////////////
/**
 * <b>Purpose:</b> label jets as quark or gluon - adapted from JetQuarkLabel<br>
 * <b>Author:</b> Jean-Baptiste de Vivie<br>
 * <b>Email:</b> devivie@cppm.in2p3.fr <br>
 * <b>Author:</b> Lawrence Lee<br>
 * <b>Email:</b> leejr@cern.ch <br>
 * <b>Created:</b> 2005/02/25 <br>
 * <b>Created:</b> 2012/03 <br>
 */
#include "ParticleJetTools/JetQGPartonLabel.h"
#include "ParticleJetTools/HadronUtils.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include <algorithm>

// new
#include "JetEvent/Jet.h"
#include "TruthHelper/PileUpType.h"

namespace Analysis {

JetQGPartonLabel::JetQGPartonLabel(const std::string& t, const std::string& n, const IInterface* p) :
        AthAlgTool(t,n,p),
        m_mcEventCollection("TruthEvents"),
        m_deltaRCut(0.4),
	m_noDoc(true),
	m_inTime(-1),
	m_jetLabel("N/A")
{
    declareInterface<IJetTruthMatching>(this);  
    declareProperty("McEventCollection", m_mcEventCollection);
    declareProperty("deltaRCut",    m_deltaRCut);
    declareProperty("NoUseDoc",     m_noDoc);
    declareProperty("PileUpInTime", m_inTime);
}

JetQGPartonLabel::~JetQGPartonLabel() {}

StatusCode JetQGPartonLabel::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode JetQGPartonLabel::finalize() {
  return StatusCode::SUCCESS;
}

bool JetQGPartonLabel::matchJet(const Jet& myJet) 
{
  m_jetLabel = "N/A";

  /* ----------------------------------------------------------------------------------- */
  /*                     Retrieve McEventCollection                                      */
  /* ----------------------------------------------------------------------------------- */
  const McEventCollection* myMcEventCollection(0);
  StatusCode sc = evtStore()->retrieve(myMcEventCollection, m_mcEventCollection);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG(m_mcEventCollection << " not found in StoreGate.");
    return false;
  }
  return m_testJet(myJet,myMcEventCollection);
}

bool JetQGPartonLabel::m_testJet(const Jet& myJet, 
              const McEventCollection* myMcEventCollection) 
{

  ATH_MSG_DEBUG("Truth matching for jet " << myJet.pt() << " " << myJet.eta() << " " << myJet.phi() << " using pile-up event selection " << m_inTime);

  m_NEventInCollection = myMcEventCollection->size();

  m_barcode = 0;
  m_pdg     = 0;

  unsigned int nLab = 0;
  PileUpType pileupType( myMcEventCollection );
  McEventCollection::const_iterator itEvt, itEvtE;
  if (m_inTime == -1) {
    // Label with signal event only
    itEvt  = myMcEventCollection->begin();
    itEvtE = pileupType.in_time_minimum_bias_event_begin();
  } else if (m_inTime == 0) {
    // Label with all in time events
    itEvt  = myMcEventCollection->begin();
    itEvtE = pileupType.in_time_event_end();
  } else if (m_inTime == 1) {
    // Label with in time pile-up : removing the signal event...
    itEvt  = pileupType.in_time_minimum_bias_event_begin();
    itEvtE = pileupType.in_time_event_end();
  } else if (m_inTime == 2) {
    //Does one really want to allow for this possibility ? Or in a given BC range ?
    // should use PileUpEventInfo maybe for a time selection...
    // (Cavern and halo are there, but hopefully there is no heavy quark there ;-) )
    itEvt  = myMcEventCollection->begin();
    itEvtE = myMcEventCollection->end(); 
  }

  m_maxEnergy = 0.0;
  m_matchPdgId = 0;
  m_matchBarcode = -1;
  m_matchDeltaR = 9999.;

  for (; itEvt != itEvtE; itEvt++) {
    const HepMC::GenEvent* GenEvent = *itEvt;
    HepMC::GenEvent::particle_const_iterator pitr = GenEvent->particles_begin();
    for (; pitr != GenEvent->particles_end(); ++pitr) {
      int pdg = (*pitr)->pdg_id();

      //Don't look at beam particles. Causes annoying FPE with eta printout
      if( (*pitr)->momentum().px()==0.0 && (*pitr)->momentum().py()==0.0 ) continue; 

      // We want to use some special functions from HepLorentzVector
      // HepMC::Fourvector does not have these functions. 
      // => transform to HepLorentzVector
      CLHEP::HepLorentzVector part_momentum_lv((*pitr)->momentum().px(),
					       (*pitr)->momentum().py(),
					       (*pitr)->momentum().pz(),
					       (*pitr)->momentum().e());
      
      float dR = part_momentum_lv.deltaR(myJet.hlv() );

      if( dR > m_deltaRCut ){
	continue;
      }

      if( abs(pdg) > 21 ) continue; //need to manually veto photons offline
      if( abs(pdg) == 0 ) continue;
      if( abs(pdg) == 6 ) continue; //not interested in top jets for QG tagging

      if( abs(pdg) > 10 && 
	  abs(pdg) < 17 ) continue;

      double E = part_momentum_lv.e();
      if (E > m_maxEnergy) {
	ATH_MSG_VERBOSE( "MCTruth: part " << (*pitr)->barcode() << " PDG= " << pdg
			 << " pT= "  <<part_momentum_lv.perp()
			 << " eta= " <<part_momentum_lv.pseudoRapidity()
			 << " phi= " <<part_momentum_lv.phi()
			 << " dR= "  <<part_momentum_lv.deltaR(myJet.hlv()) );
	m_maxEnergy = E;
	m_matchPdgId = abs(pdg);
	m_matchBarcode = (*pitr)->barcode();
	m_matchDeltaR = dR;
      }
    }
    nLab++;
  }
  ATH_MSG_DEBUG("Number of events in the EventCollection : " <<  m_NEventInCollection << " and used for QG Parton labelling " << nLab);

  m_pdg = m_matchPdgId;
  m_barcode = m_matchBarcode;

  if( m_pdg < 4 ){
    m_jetLabel = "LQ";
    return true;
  } else if ( m_pdg==4 ){
    m_jetLabel = "C";
    return true;
  } else if ( m_pdg==5 ){
    m_jetLabel = "B";
    return true;
  } else if ( m_pdg==21 || m_pdg==9 ){
    m_jetLabel = "G";
    return true;
  } else {
    m_jetLabel = "N/A";
    return false;
  }

}

void JetQGPartonLabel::m_printParameterSettings() {
}

double JetQGPartonLabel::deltaRToParton() const {
  return m_matchDeltaR;
}

}
