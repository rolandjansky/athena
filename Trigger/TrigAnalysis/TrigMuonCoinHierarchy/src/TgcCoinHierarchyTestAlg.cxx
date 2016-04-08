/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonCoinHierarchy/TgcCoinHierarchyTestAlg.h"

#include "TrigMuonCoinHierarchy/ITgcCoinHierarchySvc.h"

#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "muonEvent/MuonContainer.h"
#include "TrkTrack/TrackCollection.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2I.h"

#include <iomanip>

using Muon::TgcCoinData;
using Muon::TgcCoinDataCollection;
using Muon::TgcCoinDataContainer;
using Muon::TgcPrepData;
using Muon::TgcPrepDataCollection;
using Muon::TgcPrepDataContainer;

namespace Trigger { 
  TgcCoinHierarchyTestAlg::TgcCoinHierarchyTestAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator), 
      m_tgcCoinSvc(0), 
      m_thistSvc(0), 
      nEvent(0), 
      nSL(0), m_hnSL(0), 
      nSLWithTrackletWire(0),             m_hnSLWithTrackletWire(0), 
      nSLWithTrackletStrip(0),            m_hnSLWithTrackletStrip(0), 
      nSLWithPT2456(0),                   m_hnSLWithPT2456(0), 
      nSLWithPT2456WithHiPtWire(0),       m_hnSLWithPT2456WithHiPtWire(0), 
      nSLWithPT2456WithHiPtStrip(0),      m_hnSLWithPT2456WithHiPtStrip(0), 
      nHiPtWire(0),                       m_hnHiPtWire(0), 
      nHiPtWireWithTrackletWire(0),       m_hnHiPtWireWithTrackletWire(0), 
      nHiPtWireWithTGC1WireHit(0),        m_hnHiPtWireWithTGC1WireHit(0), 
      nHiPtStrip(0),                      m_hnHiPtStrip(0), 
      nHiPtStripWithTrackletStrip(0),     m_hnHiPtStripWithTrackletStrip(0), 
      nHiPtStripWithTGC1StripHit(0),      m_hnHiPtStripWithTGC1StripHit(0), 
      nTrackletWire(0),                   m_hnTrackletWire(0), 
      nTrackletWireWithTGC23WireHit(0),   m_hnTrackletWireWithTGC23WireHit(0), 
      nTrackletStrip(0),                  m_hnTrackletStrip(0), 
      nTrackletStripWithTGC23StripHit(0), m_hnTrackletStripWithTGC23StripHit(0),
      m_hMuonPt(0)
  {
    // Flags for examples
    // true means running the example 
    declareProperty("showFraction", showFraction=true);
    declareProperty("runEvent", runEvent=true);
    declareProperty("runEventHistos", runEventHistos=true);
    declareProperty("runTgcPrepData", runTgcPrepData=false);
    declareProperty("runTgcCoinData", runTgcCoinData=false);
    declareProperty("runMuon", runMuon=false);
    declareProperty("runTrack", runTrack=false);

    for(unsigned int iType=0; iType<TgcCoinHierarchy::NTYPES; iType++) {
      m_h_nTGC23Hit[iType] = 0;
      m_h_nTGC1Hit[iType] = 0;
    }
  }

  StatusCode TgcCoinHierarchyTestAlg::initialize() {   
    ATH_MSG_INFO("initialize()");

    // Retrieve Trigger::TgcCoinHierarchySvc 
    StatusCode sc = service("Trigger::TgcCoinHierarchySvc", m_tgcCoinSvc);
    if(sc.isFailure()) {
      ATH_MSG_FATAL("Could not find TgcCoinHierarchySvc");
      return sc;
    }

    if(runEventHistos || showFraction || runMuon) {
      // Retrieve THistSvc
      sc = service("THistSvc", m_thistSvc);
      if(!sc.isSuccess() || !m_thistSvc) {
	ATH_MSG_FATAL("Could not find THistSvc");
	return StatusCode::FAILURE;
      }
    }
      
    if(showFraction) {
      nEvent = 0;
      nSL = 0;
      nSLWithTrackletWire = 0;
      nSLWithTrackletStrip = 0;
      nSLWithPT2456 = 0;
      nSLWithPT2456WithHiPtWire = 0;
      nSLWithPT2456WithHiPtStrip = 0;
      nHiPtWire = 0;
      nHiPtWireWithTrackletWire = 0;
      nHiPtWireWithTGC1WireHit = 0;
      nHiPtStrip = 0;
      nHiPtStripWithTrackletStrip = 0;
      nTrackletWire = 0;
      nTrackletWireWithTGC23WireHit = 0;
      nHiPtStripWithTGC1StripHit = 0;
      nTrackletStrip = 0;
      nTrackletStripWithTGC23StripHit = 0;

      m_hnSL = new TH1F("hnSL", "hnSL", NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnSL", 
			     m_hnSL).isFailure()) { 
	return StatusCode::FAILURE;
      }
      
      m_hnSLWithTrackletWire = new TH1F("hnSLWithTrackletWire", "hnSLWithTrackletWire", 
					NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnSLWithTrackletWire", 
			     m_hnSLWithTrackletWire).isFailure()) {
	return StatusCode::FAILURE;
      }

      m_hnSLWithTrackletStrip = new TH1F("hnSLWithTrackletStrip", "hnSLWithTrackletStrip", 
					 NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnSLWithTrackletStrip", 
			     m_hnSLWithTrackletStrip).isFailure()) { 
	return StatusCode::FAILURE;
      }

      m_hnSLWithPT2456 = new TH1F("hnSLWithPT2456", "hnSLWithPT2456", 
				  NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnSLWithPT2456", 
			     m_hnSLWithPT2456).isFailure()) {
	return StatusCode::FAILURE;
      }

      m_hnSLWithPT2456WithHiPtWire = new TH1F("hnSLWithPT2456WithHiPtWire", "hnSLWithPT2456WithHiPtWire", 
					      NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnSLWithPT2456WithHiPtWire", 
			     m_hnSLWithPT2456WithHiPtWire).isFailure()) {
	return StatusCode::FAILURE;
      }

      m_hnSLWithPT2456WithHiPtStrip = new TH1F("hnSLWithPT2456WithHiPtStrip", "hnSLWithPT2456WithHiPtStrip", 
					       NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnSLWithPT2456WithHiPtStrip", 
			     m_hnSLWithPT2456WithHiPtStrip).isFailure()) {
	return StatusCode::FAILURE;
      }

      m_hnHiPtWire = new TH1F("hnHiPtWire", "hnHiPtWire", 
			      NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnHiPtWire", 
			     m_hnHiPtWire).isFailure()) {
	return StatusCode::FAILURE;
      }

      m_hnHiPtWireWithTrackletWire = new TH1F("hnHiPtWireWithTrackletWire", "hnHiPtWireWithTrackletWire", 
					      NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnHiPtWireWithTrackletWire", 
			     m_hnHiPtWireWithTrackletWire).isFailure()) {
	return StatusCode::FAILURE;
      }

      m_hnHiPtWireWithTGC1WireHit = new TH1F("hnHiPtWireWithTGC1WireHit", "hnHiPtWireWithTGC1WireHit", 
					     NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnHiPtWireWithTGC1WireHit", 
			     m_hnHiPtWireWithTGC1WireHit).isFailure()) {
	return StatusCode::FAILURE;
      }

      m_hnHiPtStrip = new TH1F("hnHiPtStrip", "hnHiPtStrip", 
			       NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnHiPtStrip", 
			     m_hnHiPtStrip).isFailure()) {
	return StatusCode::FAILURE;
      }

      m_hnHiPtStripWithTrackletStrip = new TH1F("hnHiPtStripWithTrackletStrip", "hnHiPtStripWithTrackletStrip", 
						NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnHiPtStripWithTrackletStrip", 
			     m_hnHiPtStripWithTrackletStrip).isFailure()) {
	return StatusCode::FAILURE;
      }

      m_hnHiPtStripWithTGC1StripHit = new TH1F("hnHiPtStripWithTGC1StripHit", "hnHiPtStripWithTGC1StripHit", 
					       NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnHiPtStripWithTGC1StripHit", 
			     m_hnHiPtStripWithTGC1StripHit).isFailure()) {
	return StatusCode::FAILURE;
      }

      m_hnTrackletWire = new TH1F("hnTrackletWire", "hnTrackletWire", 
				  NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnTrackletWire", 
			     m_hnTrackletWire).isFailure()) {
	return StatusCode::FAILURE;
      }

      m_hnTrackletWireWithTGC23WireHit = new TH1F("hnTrackletWireWithTGC23WireHit", "hnTrackletWireWithTGC23WireHit", 
						  NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnTrackletWireWithTGC23WireHit", 
			     m_hnTrackletWireWithTGC23WireHit).isFailure()) {
	return StatusCode::FAILURE;
      }

      m_hnTrackletStrip = new TH1F("hnTrackletStrip", "hnTrackletStrip", 
				   NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnTrackletStrip", 
			     m_hnTrackletStrip).isFailure()) {
	return StatusCode::FAILURE;
      }

      m_hnTrackletStripWithTGC23StripHit = new TH1F("hnTrackletStripWithTGC23StripHit", "hnTrackletStripWithTGC23StripHit", 
						    NTRIGGERSECTORS, 0.5, NTRIGGERSECTORS+0.5);
      if(m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hnTrackletStripWithTGC23StripHit", 
			     m_hnTrackletStripWithTGC23StripHit).isFailure()) {
	return StatusCode::FAILURE;
      }
    }

    if(runEventHistos) {
      m_h_nTGC23Hit[TgcCoinHierarchy::WIRE] = new TH2I("h_nTGC23Hit_Wire", "h_nTGC23Hit_Wire", 8, -0.5, 7.5, 8, -0.5, 7.5);
      if(!m_h_nTGC23Hit[TgcCoinHierarchy::WIRE]) {
	ATH_MSG_FATAL("h_nTGC23Hit_Wire could not be created.");
	return StatusCode::FAILURE;
      }
      sc = m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/h_nTGC23Hit_Wire", m_h_nTGC23Hit[TgcCoinHierarchy::WIRE]);
      if(sc.isFailure()) {
	ATH_MSG_FATAL("h_nTGC23Hit_Wire could not be registered.");
	return sc;
      }
      
      m_h_nTGC23Hit[TgcCoinHierarchy::STRIP] = new TH2I("h_nTGC23Hit_Strip", "h_nTGC23Hit_Strip", 8, -0.5, 7.5, 8, -0.5, 7.5); 
      if(!m_h_nTGC23Hit[TgcCoinHierarchy::STRIP]) {
	ATH_MSG_FATAL("h_nTGC23Hit_Strip could not be created.");
	return StatusCode::FAILURE;
      }
      sc = m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/h_nTGC23Hit_Strip", m_h_nTGC23Hit[TgcCoinHierarchy::STRIP]);
      if(sc.isFailure()) {
	ATH_MSG_FATAL("h_nTGC23Hit_Strip could not be registered.");
	return sc;
      }

      m_h_nTGC1Hit[TgcCoinHierarchy::WIRE] = new TH1I("h_nTGC1Hit_Wire", "h_nTGC1Hit_Wire", 101, -0.5, 100.5);
      if(!m_h_nTGC1Hit[TgcCoinHierarchy::WIRE]) {
	ATH_MSG_FATAL("h_nTGC1Hit_Wire could not be created.");
	return StatusCode::FAILURE;
      }
      sc = m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/h_nTGC1Hit_Wire", m_h_nTGC1Hit[TgcCoinHierarchy::WIRE]);
      if(sc.isFailure()) {
	ATH_MSG_FATAL("h_nTGC1Hit_Wire could not be registered.");
	return sc;
      }
      
      m_h_nTGC1Hit[TgcCoinHierarchy::STRIP] = new TH1I("h_nTGC1Hit_Strip", "h_nTGC1Hit_Strip", 101, -0.5, 100.5);
      if(!m_h_nTGC1Hit[TgcCoinHierarchy::STRIP]) {
	ATH_MSG_FATAL("h_nTGC1Hit_Strip could not be created.");
	return StatusCode::FAILURE;
      }
      sc = m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/h_nTGC1Hit_Strip", m_h_nTGC1Hit[TgcCoinHierarchy::STRIP]);
      if(sc.isFailure()) {
	ATH_MSG_FATAL("h_nTGC1Hit_Strip could not be registered.");
	return sc;
      }
    }

    if(runMuon) {
      m_hMuonPt = new TH2I("hMuonPt", "hMuonPt", 200, 0., 100., 8, -1.5, 6.5);
      sc = m_thistSvc->regHist("/TgcCoinHierarchyTestAlg/hMuonPt", m_hMuonPt);
      if(sc.isFailure()) {
        ATH_MSG_FATAL("m_hMuonPt could not be registered.");
        return sc;
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchyTestAlg::execute() {   
    ATH_MSG_DEBUG("execute()");

    // Get all the TgcCoinHierarchy's in this event
    if(runEvent) {
      std::vector<const TgcCoinHierarchy*> hierarchies;
      // Get all the TgcCoinHierarchy's in this event
      StatusCode sc = m_tgcCoinSvc->getHierarchy(&hierarchies);
      if(sc.isFailure()) {
	ATH_MSG_FATAL("getHierarchy failed in execute().");
	return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("The number of TgcCoinHierarchy's in this event is " << hierarchies.size());
      for(unsigned int jh=0; jh<hierarchies.size(); jh++) {
	if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_SL)) {
	  ATH_MSG_INFO("PT value of this TgcCoinHierarchy, which has Sector Logic, is " 
		       << hierarchies.at(jh)->getCoincidence(TgcCoinData::TYPE_SL)->pt() 
		       << ". Detail is as follows:");
	  (hierarchies.at(jh))->dump(msg(), MSG::INFO);
	}
      }
    }

    // Count the numbers of events, SLs, HiPts and Tracklets. 
    if(showFraction) {
      std::vector<const TgcCoinHierarchy*> hierarchies;
      // Get all the TgcCoinHierarchy's in this event
      StatusCode sc = m_tgcCoinSvc->getHierarchy(&hierarchies);
      if(sc.isFailure()) {
	ATH_MSG_FATAL("getHierarchy failed in execute().");
	return StatusCode::FAILURE;
      }
      nEvent++;
      for(unsigned int jh=0; jh<hierarchies.size(); jh++) {
	const TgcCoinData* highestCoin = hierarchies.at(jh)->getCoincidence(hierarchies.at(jh)->highestCoincidence());
        int isAside = highestCoin->isAside();
        int isForward = highestCoin->isForward();
        int phi = highestCoin->phi();
        int myTriggerSectorId = phi;
        if(isForward) myTriggerSectorId += NENDCAPTRIGGERSECTORS;
        if(!isAside) myTriggerSectorId += (NENDCAPTRIGGERSECTORS+NFORWARDTRIGGERSECTORS);

	if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_SL)) { // SL 
	  nSL++;
	  m_hnSL->Fill(myTriggerSectorId);
	  if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_TRACKLET, false)) { // Tracklet Wire
	    nSLWithTrackletWire++; 
	    m_hnSLWithTrackletWire->Fill(myTriggerSectorId);
	  }

	  if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_TRACKLET, true)) { // Tracklet Strip 
	    nSLWithTrackletStrip++;
	    m_hnSLWithTrackletStrip->Fill(myTriggerSectorId);
	  }

	  int pt = hierarchies.at(jh)->getCoincidence(TgcCoinData::TYPE_SL)->pt();
	  if(pt==2 || pt==4 || pt==5 || pt==6) { // SL with PT=2, 4, 5 or 6 
	    nSLWithPT2456++;
	    m_hnSLWithPT2456->Fill(myTriggerSectorId);
	    if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_HIPT, false)) { // HiPt Wire
	      nSLWithPT2456WithHiPtWire++;
	      m_hnSLWithPT2456WithHiPtWire->Fill(myTriggerSectorId);
	    }
	    if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_HIPT, true)) { // HiPt Strip
	      nSLWithPT2456WithHiPtStrip++;
	      m_hnSLWithPT2456WithHiPtStrip->Fill(myTriggerSectorId);
	    }
	  }
	}

	if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_HIPT, false)) { // HiPt Wire
	  nHiPtWire++;
	  m_hnHiPtWire->Fill(myTriggerSectorId);
	  if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_TRACKLET, false)) { // Tracklet Wire
            nHiPtWireWithTrackletWire++;
	    m_hnHiPtWireWithTrackletWire->Fill(myTriggerSectorId);
	  }
	  if(hierarchies.at(jh)->numHits(false, TgcCoinHierarchy::TGC1)>=2) { // TGC1 Hit Wire 
	    nHiPtWireWithTGC1WireHit++; 
	    m_hnHiPtWireWithTGC1WireHit->Fill(myTriggerSectorId);
	  } 
	}

	if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_HIPT, true)) { // HiPt Strip
	  nHiPtStrip++;
	  m_hnHiPtStrip->Fill(myTriggerSectorId);
	  if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_TRACKLET, true)) { // Tracklet Strip
            nHiPtStripWithTrackletStrip++;
	    m_hnHiPtStripWithTrackletStrip->Fill(myTriggerSectorId);
	  } 

	  bool foundTGC1HitStrip = false;
	  if(hierarchies.at(jh)->numHits(true, TgcCoinHierarchy::TGC1)>=1) { // TGC1 Hit Strip 
	    foundTGC1HitStrip = true;
	  } else if(!isForward) { 
	    // HiPt Endcap Strip Board has a bug and one additional (unnecessary) object is created. 
	    // If the partner HiPt has at least one TGC1 Hit Strip, the HiPt is assumed to have the TGC1 Hit Strip.  
	    const TgcCoinData* hiPtStrip = hierarchies.at(jh)->getCoincidence(TgcCoinData::TYPE_HIPT, true);
	    const int trackletId = hiPtStrip->trackletId();
	    if(trackletId==38 || trackletId==39 || trackletId==40 || trackletId==41) { // Problematic trackletId
	      const int associatedTrackletId = ((trackletId<40) ? (trackletId+2) : (trackletId-2)); 
	      // Loop over all the hierarchies to find the partner HiPt object. 
	      for(unsigned int kh=0; kh<hierarchies.size(); kh++) { 
		if(kh==jh) continue;
		if(!hierarchies.at(kh)->hasCoincidence(TgcCoinData::TYPE_HIPT, true)) continue;
		
		const TgcCoinData* tmpHiPt = hierarchies.at(kh)->getCoincidence(TgcCoinData::TYPE_HIPT, true);
		if(isAside!=tmpHiPt->isAside()) continue; // Side
		if(isForward!=tmpHiPt->isForward()) continue; // Region
		if(phi!=tmpHiPt->phi()) continue; // Trigger sector
		if(associatedTrackletId!=tmpHiPt->trackletId()) continue; // TrackletId

		if(hierarchies.at(kh)->numHits(true, TgcCoinHierarchy::TGC1)>=1) {
		  foundTGC1HitStrip = true;
		  break;
		}
	      }
	    }
	  }
	  if(foundTGC1HitStrip) {
	    nHiPtStripWithTGC1StripHit++; 
	    m_hnHiPtStripWithTGC1StripHit->Fill(myTriggerSectorId);
	  }
	}

	if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_TRACKLET, false)) { // Tracklet Wire
	  nTrackletWire++;
	  m_hnTrackletWire->Fill(myTriggerSectorId);
	  unsigned int nWireTGC2Hit = hierarchies.at(jh)->numHits(false, TgcCoinHierarchy::TGC2); 
	  unsigned int nWireTGC3Hit = hierarchies.at(jh)->numHits(false, TgcCoinHierarchy::TGC3);
	  if(nWireTGC2Hit>=1 && nWireTGC3Hit>=1 && nWireTGC2Hit+nWireTGC3Hit>=3) {
	    nTrackletWireWithTGC23WireHit++;
	    m_hnTrackletWireWithTGC23WireHit->Fill(myTriggerSectorId);
	  }
	}

	if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_TRACKLET, true)) { // Tracklet Strip
	  nTrackletStrip++;
	  m_hnTrackletStrip->Fill(myTriggerSectorId);
	  unsigned int nStripTGC2Hit = hierarchies.at(jh)->numHits(true, TgcCoinHierarchy::TGC2); 
	  unsigned int nStripTGC3Hit = hierarchies.at(jh)->numHits(true, TgcCoinHierarchy::TGC3);
	  if(nStripTGC2Hit>=1 && nStripTGC3Hit>=1 && nStripTGC2Hit+nStripTGC3Hit>=3) {
	    nTrackletStripWithTGC23StripHit++;
	    m_hnTrackletStripWithTGC23StripHit->Fill(myTriggerSectorId);
	  }
	}

      }
    }

    // Get all the TgcCoinHierarchy's in this event and check the number of TGC2 and TGC3 hits
    if(runEventHistos) {
      std::vector<const TgcCoinHierarchy*> hierarchies;
      // Get all the TgcCoinHierarchy's in this event
      StatusCode sc = m_tgcCoinSvc->getHierarchy(&hierarchies);
      if(sc.isFailure()) {
	ATH_MSG_FATAL("getHierarchy failed in execute().");
	return StatusCode::FAILURE;
      }
      for(unsigned int jh=0; jh<hierarchies.size(); jh++) {
	// Tracklet Wire
	if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_TRACKLET, false)) { 
	  m_h_nTGC23Hit[TgcCoinHierarchy::WIRE]->Fill(hierarchies.at(jh)->numHits(false, TgcCoinHierarchy::TGC2), 
						      hierarchies.at(jh)->numHits(false, TgcCoinHierarchy::TGC3));
	}
	// Tracklet Strip
	if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_TRACKLET, true)) { 
	  m_h_nTGC23Hit[TgcCoinHierarchy::STRIP]->Fill(hierarchies.at(jh)->numHits(true, TgcCoinHierarchy::TGC2), 
						       hierarchies.at(jh)->numHits(true, TgcCoinHierarchy::TGC3));
	}
	// HiPt Wire
	if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_HIPT, false)) { 
	  m_h_nTGC1Hit[TgcCoinHierarchy::WIRE]->Fill(hierarchies.at(jh)->numHits(false, TgcCoinHierarchy::TGC1)); 
	}
	// HiPt Strip
	if(hierarchies.at(jh)->hasCoincidence(TgcCoinData::TYPE_HIPT, true)) { 
	  m_h_nTGC1Hit[TgcCoinHierarchy::STRIP]->Fill(hierarchies.at(jh)->numHits(true, TgcCoinHierarchy::TGC1)); 
	}
      }
    }

    // Loop over TGC_Measurements container (TGC current hits)
    if(runTgcPrepData) {
      const TgcPrepDataContainer* hitColl = 0;
      StatusCode sc = evtStore()->retrieve(hitColl, "TGC_Measurements");
      if(sc.isFailure() || !hitColl) {
	ATH_MSG_WARNING("No TGC hit PRD container (" << "TGC_Measurements" << ") found");
	return StatusCode::SUCCESS;
      } 
      TgcPrepDataContainer::const_iterator ih   = hitColl->begin();
      TgcPrepDataContainer::const_iterator ih_e = hitColl->end();
      for(; ih!=ih_e; ih++) {
	TgcPrepDataCollection::const_iterator ihh   = (*ih)->begin();
	TgcPrepDataCollection::const_iterator ihh_e = (*ih)->end();
	for(; ihh!=ihh_e; ihh++) {
	  Identifier identify = (*ihh)->identify();
	  std::vector<const TgcCoinHierarchy*> hierarchies;
	  // Get TgcCoinHierarchy's which are associated to identify
	  sc = m_tgcCoinSvc->getHierarchy(&hierarchies, identify);
	  if(sc.isFailure()) {
	    ATH_MSG_FATAL("getHierarchy failed in execute().");
	    return StatusCode::FAILURE;
	  }
	  ATH_MSG_INFO("identify=" << identify << " size of hierarchies=" << hierarchies.size());
	  for(unsigned int jh=0; jh<hierarchies.size(); jh++) {
	    (hierarchies.at(jh))->dump(msg(), MSG::INFO);
	  }

	  // Get TgcCoinHierarchy's which are associated to Trk::PrepRawData*
	  sc = m_tgcCoinSvc->getHierarchy(&hierarchies, dynamic_cast<const Trk::PrepRawData*>(*ihh));
	  if(sc.isFailure()) {
	    ATH_MSG_FATAL("getHierarchy failed in execute().");
	    return StatusCode::FAILURE;
	  }
	  ATH_MSG_DEBUG("PrepRawData" << " size of hierarchies=" << hierarchies.size());
	}
      }
    }

    // Loop over TrigT1CoinDataCollection container (TGC current coincidences)
    if(runTgcCoinData) {
      const TgcCoinDataContainer* coinColl = 0;
      StatusCode sc = evtStore()->retrieve(coinColl, "TrigT1CoinDataCollection");
      if(sc.isFailure() || !coinColl) {
	ATH_MSG_WARNING("No TGC coin PRD container (" << "TrigT1CoinDataCollection" << ") found");
	return StatusCode::SUCCESS;
      } 
      TgcCoinDataContainer::const_iterator ih   = coinColl->begin();
      TgcCoinDataContainer::const_iterator ih_e = coinColl->end();
      for(; ih!=ih_e; ih++) {
	TgcCoinDataCollection::const_iterator ihh   = (*ih)->begin();
	TgcCoinDataCollection::const_iterator ihh_e = (*ih)->end();
	for(; ihh!=ihh_e; ihh++) {
	  std::vector<const TgcCoinHierarchy*> hierarchies;

	  // Get TgcCoinHierarchy's which are associated to TgcCoinData*
	  sc = m_tgcCoinSvc->getHierarchy(&hierarchies, *ihh);
	  if(sc.isFailure()) {
	    ATH_MSG_FATAL("getHierarchy failed in execute().");
	    return StatusCode::FAILURE;
	  }
	  ATH_MSG_INFO(**ihh << " size of hierarchies=" << hierarchies.size());
	  for(unsigned int jh=0; jh<hierarchies.size(); jh++) {
	    (hierarchies.at(jh))->dump(msg(), MSG::INFO);
	  }

	  // Get TgcCoinHierarchy's which are associated to the trigger sector
	  sc = m_tgcCoinSvc->getHierarchy(&hierarchies, (*ihh)->isAside(), (*ihh)->isForward(), (*ihh)->phi());
	  if(sc.isFailure()) {
	    ATH_MSG_FATAL("getHierarchy failed in execute().");
	    return StatusCode::FAILURE;
	  }
	  ATH_MSG_DEBUG("const bool isAside, const bool isForward, const int phi" << " size of hierarchies=" 
			<< hierarchies.size());
	}
      }
    }

    // Loop over StacoMuonCollection container (combined muon tracks) 
    // Muon -> trackParticle -> track 
    if(runMuon) {
      const Analysis::MuonContainer* muonTES = 0;
      StatusCode sc = evtStore()->retrieve(muonTES, "MuidMuonCollection");
      if(sc.isFailure() || !muonTES) {
	ATH_MSG_WARNING("No MuidMuonCollection found");
	return StatusCode::SUCCESS;
      } 
      Analysis::MuonContainer::const_iterator it   = muonTES->begin();
      Analysis::MuonContainer::const_iterator it_e = muonTES->end();
      for(; it!=it_e; it++) {
	if(!(*it)->isCombinedMuon()) continue;
	
	const Rec::TrackParticle* muonSpectrometerTrackParticle = (*it)->muonSpectrometerTrackParticle();
	if(!muonSpectrometerTrackParticle) continue;

	const Trk::Track* track = muonSpectrometerTrackParticle->originalTrack();
	if(!track) continue;
	
	std::vector<const TgcCoinHierarchy*> hierarchies;
	// Get TgcCoinHierarchy's which are associated to the track
	sc = m_tgcCoinSvc->getHierarchy(&hierarchies, track);
	if(sc.isFailure()) {
	  ATH_MSG_FATAL("getHierarchy failed in execute().");
	  return StatusCode::FAILURE;
	}
	ATH_MSG_DEBUG("track=" << *track << " size of hierarchies=" << hierarchies.size());
	int highestPt = -1; 
	for(unsigned int ih=0; ih<hierarchies.size(); ih++) {
	  (hierarchies.at(ih))->dump(msg(), MSG::DEBUG);

	  if(hierarchies.at(ih)->hasCoincidence(Trigger::TgcCoinHierarchy::SL)) {
	    int pt = hierarchies.at(ih)->getCoincidence(Trigger::TgcCoinHierarchy::SL)->pt();
	    if(highestPt<pt) highestPt = pt; 
	  }
	}

	const Rec::TrackParticle* combinedTrackParticle = (*it)->combinedMuonTrackParticle();

	m_hMuonPt->Fill(combinedTrackParticle->pt()/1000., highestPt);
      }
    }

    // Loop over MooreTracks container
    if(runTrack) {
      const TrackCollection *trackCollection = 0;
      StatusCode sc = evtStore()->retrieve(trackCollection, "MooreTracks");
      if(sc.isFailure()) {
	ATH_MSG_FATAL("Cannot retrieve MooreTracks container");
	return sc;
      }
      TrackCollection::const_iterator trit   = trackCollection->begin();
      TrackCollection::const_iterator trit_e = trackCollection->end();
      for(; trit!=trit_e; trit++) {
	if(!(*trit)) continue;
	
	std::vector<const TgcCoinHierarchy*> hierarchies;
	// Get TgcCoinHierarchy's which are associated to the track
	sc = m_tgcCoinSvc->getHierarchy(&hierarchies, *trit);
	if(sc.isFailure()) {
	  ATH_MSG_FATAL("getHierarchy failed in execute().");
	  return StatusCode::FAILURE;
	}
	ATH_MSG_INFO("track=" << **trit << " size of hierarchies=" << hierarchies.size());
	for(unsigned int ih=0; ih<hierarchies.size(); ih++) {
	  (hierarchies.at(ih))->dump(msg(), MSG::INFO);
	}
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchyTestAlg::finalize() {   
    ATH_MSG_INFO("finalize()");

    if(showFraction) {
      ATH_MSG_INFO("************************************************************************");
      ATH_MSG_INFO(nEvent << " events have been processed.");
      if(nSL>0) {
	ATH_MSG_INFO("SL->Tracklet Wire                     : " << std::setw(7) << nSLWithTrackletWire << 
		     " / " << std::setw(7) << nSL << " = " << 
		     std::setw(8) << std::showpoint << std::fixed << std::setprecision(4) << 
		     100.*static_cast<double>(nSLWithTrackletWire)/static_cast<double>(nSL) << "%");
	ATH_MSG_INFO("SL->Tracklet Strip                    : " << std::setw(7) << nSLWithTrackletStrip << 
		     " / " << std::setw(7) << nSL << " = " << 
		     std::setw(8) << std::showpoint << std::fixed << std::setprecision(4) << 
		     100.*static_cast<double>(nSLWithTrackletStrip)/static_cast<double>(nSL) << "%");
      }
      if(nSLWithPT2456) {
	ATH_MSG_INFO("SL (PT=2, 4, 5 or 6)->HiPt Wire       : " << std::setw(7) << nSLWithPT2456WithHiPtWire << 
		     " / " << std::setw(7) << nSLWithPT2456 << " = " << 
		     std::setw(8) << std::showpoint << std::fixed << std::setprecision(4) << 
		     100.*static_cast<double>(nSLWithPT2456WithHiPtWire)/static_cast<double>(nSLWithPT2456) << "%");
	ATH_MSG_INFO("SL (PT=2, 4, 5 or 6)->HiPt Strip      : " << std::setw(7) << nSLWithPT2456WithHiPtStrip << 
		     " / " << std::setw(7) << nSLWithPT2456 << " = " << 
		     std::setw(8) << std::showpoint << std::fixed << std::setprecision(4) << 
		     100.*static_cast<double>(nSLWithPT2456WithHiPtStrip)/static_cast<double>(nSLWithPT2456) << "%");
      }
      if(nHiPtWire) {
	ATH_MSG_INFO("HiPt Wire->Tracklet Wire              : " << std::setw(7) << nHiPtWireWithTrackletWire << 
		     " / " << std::setw(7) << nHiPtWire << " = " << 
		     std::setw(8) << std::showpoint << std::fixed << std::setprecision(4) << 
		     100.*static_cast<double>(nHiPtWireWithTrackletWire)/static_cast<double>(nHiPtWire) << "%");
	ATH_MSG_INFO("HiPt Wire->TGC1 Wire Hits             : " << std::setw(7) << nHiPtWireWithTGC1WireHit <<  
		     " / " << std::setw(7) << nHiPtWire << " = " <<  
		     std::setw(8) << std::showpoint << std::fixed << std::setprecision(4) <<  
		     100.*static_cast<double>(nHiPtWireWithTGC1WireHit)/static_cast<double>(nHiPtWire) << "%"); 
      }
      if(nHiPtStrip) {
	ATH_MSG_INFO("HiPt Strip->Tracklet Strip            : " << std::setw(7) << nHiPtStripWithTrackletStrip << 
		     " / " << std::setw(7) << nHiPtStrip << " = " << 
		     std::setw(8) << std::showpoint << std::fixed << std::setprecision(4) <<
		     100.*static_cast<double>(nHiPtStripWithTrackletStrip)/static_cast<double>(nHiPtStrip) << "%");
	ATH_MSG_INFO("HiPt Strip->TGC1 Strip Hit            : " << std::setw(7) << nHiPtStripWithTGC1StripHit <<  
		     " / " << std::setw(7) << nHiPtStrip << " = " <<  
		     std::setw(8) << std::showpoint << std::fixed << std::setprecision(4) << 
		     100.*static_cast<double>(nHiPtStripWithTGC1StripHit)/static_cast<double>(nHiPtStrip) << "%");
      }
      if(nTrackletWire) {
	ATH_MSG_INFO("Tracklet Wire->TGC2 and 3 Wire Hits   : " << std::setw(7) << nTrackletWireWithTGC23WireHit << 
		     " / " << std::setw(7) << nTrackletWire << " = " << 
		     std::setw(8) << std::showpoint << std::fixed << std::setprecision(4) << 
		     100.*static_cast<double>(nTrackletWireWithTGC23WireHit)/static_cast<double>(nTrackletWire) << "%");
      }
      if(nTrackletStrip) {
	ATH_MSG_INFO("Tracklet Strip->TGC2 and 3 Strip Hits : " << std::setw(7) << nTrackletStripWithTGC23StripHit << 
		     " / " << std::setw(7) << nTrackletStrip << " = " << 
		     std::setw(8) << std::showpoint << std::fixed << std::setprecision(4) << 
		     100.*static_cast<double>(nTrackletStripWithTGC23StripHit)/static_cast<double>(nTrackletStrip) << "%");
      }
      ATH_MSG_INFO("************************************************************************");
    }

    return StatusCode::SUCCESS;
  }

} // end of namespace Trigger
