/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JetQuarkLabel.cxx
//   Source file for class JetQuarkLabel
///////////////////////////////////////////////////////////////////
/**
 * <b>Purpose:</b> label jets with b or c quarks.<br>
 * <b>Author:</b> Jean-Baptiste de Vivie<br>
 * <b>Email:</b> devivie@cppm.in2p3.fr <br>
 * <b>Created:</b> 2005/02/25 <br>
 */
#include "ParticleJetTools/JetQuarkLabel.h"
#include "ParticleJetTools/HadronUtils.h"

//#include "GeneratorObjects/McEventCollection.h"
//#include "HepMC/GenEvent.h"
//#include "HepMC/GenVertex.h"
//#include "HepMC/GenParticle.h"
#include <algorithm>

// new
//#include "TruthHelper/PileUpType.h"

#define GeVtoMeV 1000.0

namespace Analysis {

JetQuarkLabel::JetQuarkLabel(const std::string& name)
    : AsgTool(name),
      m_mcEventCollection("TruthEvents"),
      m_deltaRCut(0.3),
      m_ptCut(5.*GeVtoMeV),
      m_noDoc(true),
      m_inTime(-1),
      m_jetLabel(0),
      m_pdg(0),
      m_barcode(0),
      m_Bpdg(0),
      m_NEventInCollection(0)
{
    declareProperty("McEventCollection", m_mcEventCollection);
    declareProperty("deltaRCut",    m_deltaRCut);
    declareProperty("pTmin",        m_ptCut);
    declareProperty("NoUseDoc",     m_noDoc);
    declareProperty("PileUpInTime", m_inTime);
}

JetQuarkLabel::~JetQuarkLabel() {}

StatusCode JetQuarkLabel::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode JetQuarkLabel::finalize() {
  return StatusCode::SUCCESS;
}

  bool JetQuarkLabel::matchJet(const xAOD::Jet& myJet) 
{
  m_jetLabel = 0;

  /* ----------------------------------------------------------------------------------- */
  /*                     Retrieve McEventCollection                                      */
  /* ----------------------------------------------------------------------------------- */
/*
  const McEventCollection* myMcEventCollection(0);
  StatusCode sc = evtStore()->retrieve(myMcEventCollection, m_mcEventCollection);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG(m_mcEventCollection << " not found in StoreGate.");
    return false;
  }
  return m_testJet(myJet,myMcEventCollection);
*/

//retrieve xAOD::TruthEvent
  const xAOD::TruthEventContainer* truthEventContainer = NULL;
  StatusCode sc = evtStore()->retrieve(truthEventContainer, m_mcEventCollection);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG(m_mcEventCollection << " not found in StoreGate.");
    return false;
  }
  return m_testJet(myJet,truthEventContainer);

}

  bool JetQuarkLabel::m_testJet(const xAOD::Jet& myJet, 
				const xAOD::TruthEventContainer* truthEventContainer) 
{

  TLorentzVector jet_hlv = myJet.p4();
  
  ATH_MSG_DEBUG("Truth matching for jet " << myJet.pt() << " " << myJet.eta() << " " << myJet.phi() << " using pile-up event selection " << m_inTime);



  m_NEventInCollection = truthEventContainer->size();

  m_barcode = 0;
  m_pdg     = 0;
  m_Bpdg    = 0;
  m_BDecVtx = Eigen::Vector3d();
  m_distanceToQuarks.clear();
  // Tag only jet in the ID acceptance : not anymore...
  // Labelling might be usefull also outside ID acceptance if (fabs(myJet.eta()) > 2.5) return false;
  //
  double deltaRC = 999.;
  double deltaRB = 999.;
  double deltaRT = 999.;
  double deltaR  = 999.;
  int barcc = 0;
  int barcb = 0;
  unsigned int nLab = 0;
  /*
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
  */
  xAOD::TruthEventContainer::const_iterator itEvt = truthEventContainer->begin(); // see if this works for now
  xAOD::TruthEventContainer::const_iterator itEvtE = truthEventContainer->end();
  const xAOD::TruthParticle* LabellingParticle(0);
  //const xAOD::TruthEvent* LabellingEvent(0);
  for (; itEvt != itEvtE; itEvt++) {
    const xAOD::TruthEvent* GenEvent = *itEvt;
    std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator ELpitr = GenEvent->truthParticleLinks().begin();
    std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator pitrE = GenEvent->truthParticleLinks().end();
    //HepMC::GenEvent::particle_const_iterator pitr = GenEvent->particles_begin();
    for (; ELpitr != pitrE; ++ELpitr) {
      ElementLink<xAOD::TruthParticleContainer> pitr = (*ELpitr);
      int pdg = (*pitr)->pdgId();
      
      // We want to use some special functions from HepLorentzVector
      // HepMC::Fourvector does not have these functions. 
      // => transform to HepLorentzVector
      TLorentzVector part_momentum_lv = (*pitr)->p4();
      
      // label b, c and light jets
      if (abs(pdg) == 5 || abs(pdg) == 4) {
	double pt = part_momentum_lv.Pt();
	if (pt > m_ptCut) {
	  // Herwig ! Do not use quark from Hadron decay !
	  //bool fromHadron = false; // put this back in later
	  ATH_MSG_VERBOSE( "MCTruth: part " << (*pitr)->barcode() << " PDG= " << pdg
			   << " pT= "  <<part_momentum_lv.Pt()
			   << " eta= " <<part_momentum_lv.Eta()
			   << " phi= " <<part_momentum_lv.Phi()
			   << " dR= "  <<part_momentum_lv.DeltaR(jet_hlv));
/*	  if ((*pitr)->prodVtx() != NULL) {
	    HepMC::GenVertex::particle_iterator firstParent =
	      (*pitr)->prodVtx()->particles_begin(HepMC::ancestors);
	    HepMC::GenVertex::particle_iterator endParent =
	      (*pitr)->prodVtx()->particles_end(HepMC::ancestors);
	    HepMC::GenVertex::particle_iterator thisParent = firstParent;
	    for(; thisParent != endParent; ++thisParent){
	      if (HadronClass::HeavyHadron((*thisParent)->pdg_id())) 
		fromHadron = true;
	      ATH_MSG_VERBOSE("In the parent loop "<< (*thisParent)->pdg_id() << " " << fromHadron);
	      if (fromHadron) break;
	    }
	    }*/
	  // do not label by quark before FSR
	  bool afterFSR = true;
	  if ((*pitr)->hasDecayVtx()) {
	    std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator firstChild = (*pitr)->decayVtx()->outgoingParticleLinks().begin();
	    std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator endChild = (*pitr)->decayVtx()->outgoingParticleLinks().begin();
	    std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator thisChild = firstChild;
	      //HepMC::GenVertex::particle_iterator firstChild =
	      //(*pitr)->end_vertex()->particles_begin(HepMC::children);
	      //HepMC::GenVertex::particle_iterator endChild =
	      //(*pitr)->end_vertex()->particles_end(HepMC::children);
	      //HepMC::GenVertex::particle_iterator thisChild = firstChild;
	    for(; thisChild != endChild; ++thisChild){
	      if ((**thisChild)->pdgId() == pdg) afterFSR = false;
	    }
	  } else if ( ((*pitr)->status() == 3 && m_noDoc) || ((*pitr)->status() == 10902) ) {
	    // do not label by documentary quark 
	    // (New 27/06/2006, for PYTHIA, with new Shower and maybe dependant on MSTP(128). What a mess !)
	    afterFSR = false;
	  }
	  if (afterFSR) {
	  //if (afterFSR && !fromHadron) {
	    deltaR=part_momentum_lv.DeltaR(jet_hlv);
	    if (abs(pdg) == 4 && deltaR < deltaRC) {deltaRC = deltaR; barcc = (*pitr)->barcode(); }
	    if (abs(pdg) == 5 && deltaR < deltaRB) {deltaRB = deltaR; barcb = (*pitr)->barcode(); LabellingParticle=(*pitr);}
	  }
	}
      }
      if (abs(pdg) == 15) {
	double pt = part_momentum_lv.Pt();
	if (pt > m_ptCut) {
	  ATH_MSG_VERBOSE( "MCTruth: part " << (*pitr)->barcode() << " PDG= " << pdg
			   << " pT= "  <<part_momentum_lv.Pt()
			   << " eta= " <<part_momentum_lv.Eta()
			   << " phi= " <<part_momentum_lv.Phi()
			   << " dR= "  <<part_momentum_lv.DeltaR(jet_hlv) );
	  deltaR=part_momentum_lv.DeltaR(jet_hlv);
	  if (deltaR < deltaRT) {deltaRT = deltaR;}
	}
      }
    }
    nLab++;
  }
  ATH_MSG_DEBUG("Number of events in the EventCollection : " <<  m_NEventInCollection << " and used for labelling " << nLab);

  m_distanceToQuarks.insert(std::make_pair("B",deltaRB));
  m_distanceToQuarks.insert(std::make_pair("C",deltaRC));
  m_distanceToQuarks.insert(std::make_pair("T",deltaRT)); //it is not a quark !
  ATH_MSG_VERBOSE("DeltaR " << deltaRB << " " << deltaRC << " " << deltaRT);
  if (deltaRB < m_deltaRCut) {
    m_pdg      = 5;
    m_barcode  = barcb;
    m_jetLabel = 5;
    ATH_MSG_VERBOSE("Jet matched with a b "<<m_barcode<<" after FSR, dR: " << deltaRB);
    //JBdV Try  Herwig !!! If Bhadron --> quark, the corresponding decay vertex is the primary !! 
    //                     (but the the following Hadron vertices are OK)
    if (LabellingParticle == 0) {
      ATH_MSG_WARNING("A b labelled jet without a labelling particle ? Should not exist");
      return true;
    }
    //HepMC::GenParticle *theB = LabellingEvent->barcode_to_particle( barcb );
    /*    if (theB) {
      ATH_MSG_VERBOSE( 
	      " bquark px = " << theB->momentum().px() 
	   << " py = " << theB->momentum().py() 
	   << " pz = " << theB->momentum().pz() );
      if (theB->hasDecayVertex()) {
	double deltaRbBmin = 9999;
	int barB = -1;
	std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator firstChild = theB->decayVertex()->outgoingParticles().begin();
	std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator endChild = theB->decayVertex()->outgoingParticles().begin();
	std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator ELthisChild = firstChild
	//HepMC::GenVertex::particle_iterator firstChild =
	//theB->end_vertex()->particles_begin(HepMC::descendants);
	//HepMC::GenVertex::particle_iterator endChild =
	//theB->end_vertex()->particles_end(HepMC::descendants);
	//HepMC::GenVertex::particle_iterator thisChild = firstChild;
	for(; thisChild != endChild; ++thisChild){
	  ElementLink<xAOD::TruthParticleContainer> thisChild = (*ELthisChild);
	  int pdg = (*thisChild)->pdgId();
	  short typeH = HadronClass::type(pdg).first;
	  short typeP = HadronClass::type(pdg).second;
	  if ( typeP == 5 && ( (typeH == 1 && (abs(pdg)%10) == 2) || (typeH == 0 && (abs(pdg)%10) == 1) ) ) {
	    CLHEP::HepLorentzVector theB_momentum_lv(theB->momentum().px(),
					      theB->momentum().py(),
					      theB->momentum().pz(),
					      theB->momentum().e());
	    CLHEP::HepLorentzVector child_momentum_lv((*thisChild)->momentum().px(),
						      (*thisChild)->momentum().py(),
						      (*thisChild)->momentum().pz(),
						      (*thisChild)->momentum().e());
	    
	    
	    double deltaRbB = theB_momentum_lv.deltaR(child_momentum_lv);
	    ATH_MSG_VERBOSE("b quark child " << pdg << " barcode = " << (*thisChild)->barcode()); 
	    if (deltaRbB < deltaRbBmin) {
	      if ((*thisChild)->hasDecayVtx()) {
		m_BDecVtx[0] = (*thisChild)->decayVtx()->position().x();
		m_BDecVtx[1] = (*thisChild)->decayVtx()->position().y();
		m_BDecVtx[2] = (*thisChild)->decayVtx()->position().z();
		m_Bpdg    = pdg;
		std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator goodChildEL = (*thisChild)->decayVertex()->outgoingParticles().begin();
		std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator goodChildE = (*thisChild)->decayVertex()->outgoingParticles().begin();
		//HepMC::GenVertex::particle_iterator goodChild  = (*thisChild)->end_vertex()->particles_begin(HepMC::children);
		//HepMC::GenVertex::particle_iterator goodChildE = (*thisChild)->end_vertex()->particles_end(HepMC::children);
		for(; goodChild != goodChildE; ++goodChild){
		  ElementLink<xAOD::TruthParticleContainer> goodChild = (*goodChildEL);
		  if (abs((*goodChild)->pdgId()) == 5) {
		    ATH_MSG_VERBOSE("Decay vertex corrupted !");
		    if ((*goodChild)->hasDecayVtx()) {
		      HepMC::GenVertex::particle_iterator goodbChild  = (*goodChild)->end_vertex()->particles_begin(HepMC::children);
		      while (abs((*goodbChild)->pdg_id()) <= 4) {
			if ((*goodbChild)->end_vertex()) { 
			  goodbChild = (*goodbChild)->end_vertex()->particles_begin(HepMC::children);
			} else {
			  break;
			}
		      }
		      if ((*goodbChild)->end_vertex()) {
			m_BDecVtx[0] = (*goodbChild)->end_vertex()->position().x();
			m_BDecVtx[1] = (*goodbChild)->end_vertex()->position().y();
			m_BDecVtx[2] = (*goodbChild)->end_vertex()->position().z();
			ATH_MSG_VERBOSE( "Trying to recover pdg Good Child = " << (*goodbChild)->pdg_id() 
			     << " Decay vertex " << m_BDecVtx[0] << " " << m_BDecVtx[1] << " " << m_BDecVtx[2] );
			break;
		      }
		    }
		  }
		}
	      }
	      deltaRbBmin = deltaRbB;
	      barB = (*thisChild)->barcode();
	    }
	  }
	  }
	if (msgLvl(MSG::VERBOSE)) {
	  HepMC::GenParticle *theBHad = LabellingEvent->barcode_to_particle( barB );
	  if (theBHad) {
	    ATH_MSG_VERBOSE("The nearest B hadron for a b-labelled jet ");
	    ATH_MSG_VERBOSE("barc = " << barB << " DR = " << deltaRbBmin);
	    ATH_MSG_VERBOSE( 
		    " px = " << theBHad->momentum().px() 
		 << " py = " << theBHad->momentum().py() 
		 << " pz = " << theBHad->momentum().pz() );
	    ATH_MSG_VERBOSE(
		    " vx = " << m_BDecVtx[0] 
		 << " vy = " << m_BDecVtx[1] 
		 << " vz = " << m_BDecVtx[2] );
	  }
	}
	}
      }*/
  
    //
    return true;
  } else if (deltaRC < m_deltaRCut) {
    m_pdg      = 4;
    m_barcode  = barcc;
    m_jetLabel = 4;
    ATH_MSG_VERBOSE("Jet matched with a c "<<m_barcode<<" after FSR, dR: " << deltaRC);
    return true;
  } else if (deltaRT < m_deltaRCut) {
    ATH_MSG_VERBOSE("Jet matched with a tau dR: " << deltaRT);
    m_pdg      = 15;
    m_jetLabel = 15;
    return true;
  } else {
    m_pdg      = 0;
    m_barcode  = 0;
    m_jetLabel = 0;
    return true;
  }
  return false;
}

void JetQuarkLabel::m_printParameterSettings() {
}

double JetQuarkLabel::deltaRMinTo(const std::string& key) const {
  double dR = 9999.;
  std::map<std::string, double>::const_iterator posI = m_distanceToQuarks.find(key);
  std::map<std::string, double>::const_iterator posE = m_distanceToQuarks.end();
  if(posI!=posE) dR = posI->second;
  return dR;
}

}
