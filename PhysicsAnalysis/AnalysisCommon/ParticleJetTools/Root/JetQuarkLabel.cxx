/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "AsgDataHandles/ReadHandle.h"
#include <algorithm>


#define GeVtoMeV 1000.0

namespace Analysis {

JetQuarkLabel::JetQuarkLabel(const std::string& name)
    : AsgTool(name),
      m_deltaRCut(0.3),
      m_ptCut(5.*GeVtoMeV),
      m_noDoc(true),
      m_inTime(-1)
{
    declareProperty("deltaRCut",    m_deltaRCut);
    declareProperty("pTmin",        m_ptCut);
    declareProperty("NoUseDoc",     m_noDoc);
    declareProperty("PileUpInTime", m_inTime);
}

JetQuarkLabel::~JetQuarkLabel() {}

StatusCode JetQuarkLabel::initialize() {
  ATH_CHECK(m_truthEventContainerKey.initialize());
  return StatusCode::SUCCESS;
}

bool JetQuarkLabel::matchJet(const xAOD::Jet& myJet,
                             MatchInfo* info /*= nullptr*/) const
{
  if (info)
    *info = MatchInfo();

  SG::ReadHandle<xAOD::TruthEventContainer> truthEventContainerReadHandle(m_truthEventContainerKey);

  if (!truthEventContainerReadHandle.isValid()){
    ATH_MSG_DEBUG(" Invalid ReadHandle for xAOD::TruthEventContainer with key: " << truthEventContainerReadHandle.key());
    return false;
  }

  return testJet(myJet,truthEventContainerReadHandle.cptr(), info);

}

  bool JetQuarkLabel::testJet(const xAOD::Jet& myJet, 
                              const xAOD::TruthEventContainer* truthEventContainer,
                              MatchInfo* info)  const
{

  TLorentzVector jet_hlv = myJet.p4();
  
  ATH_MSG_DEBUG("Truth matching for jet " << myJet.pt() << " " << myJet.eta() << " " << myJet.phi() << " using pile-up event selection " << m_inTime);



  int NEventInCollection = truthEventContainer->size();
  if (info)
    info->NEventInCollection =NEventInCollection;

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

  xAOD::TruthEventContainer::const_iterator itEvt = truthEventContainer->begin(); // see if this works for now
  xAOD::TruthEventContainer::const_iterator itEvtE = truthEventContainer->end();
  const xAOD::TruthParticle* LabellingParticle(0);
  //const xAOD::TruthEvent* LabellingEvent(0);
  for (; itEvt != itEvtE; itEvt++) {
    const xAOD::TruthEvent* GenEvent = *itEvt;
    std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator ELpitr = GenEvent->truthParticleLinks().begin();
    std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator pitrE = GenEvent->truthParticleLinks().end();
    for (; ELpitr != pitrE; ++ELpitr) {
      ElementLink<xAOD::TruthParticleContainer> pitr = (*ELpitr);
      if(!pitr.isValid() || !(*pitr)) {
	// Allow for possibility that truth content has been thinned.
	ATH_MSG_VERBOSE("Invalid ElementLink -- skipping.");
	continue;
      }
      int pdg = (*pitr)->pdgId();
      
      // We want to use some special functions from HepLorentzVector
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
	  // do not label by quark before FSR
	  bool afterFSR = true;
	  if ((*pitr)->hasDecayVtx()) {
	    std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator firstChild = (*pitr)->decayVtx()->outgoingParticleLinks().begin();
	    std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator endChild = (*pitr)->decayVtx()->outgoingParticleLinks().begin();
	    std::vector<ElementLink<xAOD::TruthParticleContainer> >::const_iterator thisChild = firstChild;
	    for(; thisChild != endChild; ++thisChild){
	      if(!thisChild->isValid() || !(**thisChild)) {
		// Allow for possibility that truth content has been thinned.
		ATH_MSG_VERBOSE("Invalid ElementLink -- skipping.");
		continue;
	      }
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
  ATH_MSG_DEBUG("Number of events in the EventCollection : " <<  NEventInCollection << " and used for labelling " << nLab);

  if (info) {
    info->distanceToQuarks.insert(std::make_pair("B",deltaRB));
    info->distanceToQuarks.insert(std::make_pair("C",deltaRC));
    info->distanceToQuarks.insert(std::make_pair("T",deltaRT)); //it is not a quark !
  }
  ATH_MSG_VERBOSE("DeltaR " << deltaRB << " " << deltaRC << " " << deltaRT);
  if (deltaRB < m_deltaRCut) {
    if (info) {
      info->pdg      = 5;
      info->barcode  = barcb;
      info->jetLabel = 5;
    }
    ATH_MSG_VERBOSE("Jet matched with a b "<<barcb<<" after FSR, dR: " << deltaRB);
    //JBdV Try  Herwig !!! If Bhadron --> quark, the corresponding decay vertex is the primary !! 
    //                     (but the the following Hadron vertices are OK)
    if (LabellingParticle == 0) {
      ATH_MSG_WARNING("A b labelled jet without a labelling particle ? Should not exist");
      return true;
    }
    return true;
  } else if (deltaRC < m_deltaRCut) {
    if (info) {
      info->pdg      = 4;
      info->barcode  = barcc;
      info->jetLabel = 4;
    }
    ATH_MSG_VERBOSE("Jet matched with a c "<<barcc<<" after FSR, dR: " << deltaRC);
    return true;
  } else if (deltaRT < m_deltaRCut) {
    ATH_MSG_VERBOSE("Jet matched with a tau dR: " << deltaRT);
    if (info) {
      info->pdg      = 15;
      info->jetLabel = 15;
    }
    return true;
  } else {
    if (info) {
      info->pdg      = 0;
      info->barcode  = 0;
      info->jetLabel = 0;
    }
    return true;
  }
  return false;
}

void JetQuarkLabel::printParameterSettings() const {
}

} // namespace Analysis
