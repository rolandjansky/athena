/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonDressingTool
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "MuonDressingTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"

namespace MuonCombined {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  MuonDressingTool::MuonDressingTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
	m_hitSummaryTool("Muon::MuonHitSummaryTool/MuonHitSummaryTool"),
	m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")

  {
    declareInterface<xAOD::IMuonDressingTool>(this);
    declareProperty("MuonHitSummaryTool",m_hitSummaryTool );
    declareProperty("MuonIdHelperTool",m_idHelper );
  }

  MuonDressingTool::~MuonDressingTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode MuonDressingTool::initialize() {

    ATH_CHECK(m_hitSummaryTool.retrieve());
    ATH_CHECK(m_idHelper.retrieve());

    return StatusCode::SUCCESS;
  }

  StatusCode MuonDressingTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void MuonDressingTool::addMuonHitSummary( xAOD::Muon& muon, const Trk::TrackSummary* trackSummary ) const {
    
    uint8_t mainSector = 0;
    uint8_t secondSector = 0;

    uint8_t innerSmallHits = 0;
    uint8_t innerLargeHits = 0;
    uint8_t middleSmallHits = 0;
    uint8_t middleLargeHits = 0;
    uint8_t outerSmallHits = 0;
    uint8_t outerLargeHits = 0;
    uint8_t extendedSmallHits = 0;
    uint8_t extendedLargeHits = 0;

    uint8_t innerSmallHoles = 0;
    uint8_t innerLargeHoles = 0;
    uint8_t middleSmallHoles = 0;
    uint8_t middleLargeHoles = 0;
    uint8_t outerSmallHoles = 0;
    uint8_t outerLargeHoles = 0;
    uint8_t extendedSmallHoles = 0;
    uint8_t extendedLargeHoles = 0;

    uint8_t phiLayer1Hits = 0;
    uint8_t phiLayer2Hits = 0;
    uint8_t phiLayer3Hits = 0;
    uint8_t phiLayer4Hits = 0;

    uint8_t etaLayer1Hits = 0;
    uint8_t etaLayer2Hits = 0;
    uint8_t etaLayer3Hits = 0;
    uint8_t etaLayer4Hits = 0;

    uint8_t phiLayer1Holes = 0;
    uint8_t phiLayer2Holes = 0;
    uint8_t phiLayer3Holes = 0;
    uint8_t phiLayer4Holes = 0;

    uint8_t etaLayer1Holes = 0;
    uint8_t etaLayer2Holes = 0;
    uint8_t etaLayer3Holes = 0;
    uint8_t etaLayer4Holes = 0;

    if( !trackSummary ){
      // get link to track particle
      ElementLink< xAOD::TrackParticleContainer > tpLink = muon.combinedTrackParticleLink();
      if( !tpLink.isValid() ) tpLink = muon.muonSpectrometerTrackParticleLink();
      if( tpLink.isValid() ){
	
	// check link to track
	if( (*tpLink)->trackLink().isValid() ) trackSummary = (*tpLink)->track()->trackSummary();
      }
    }
    
    if( trackSummary ) {

      Muon::IMuonHitSummaryTool::CompactSummary summary = m_hitSummaryTool->summary(*trackSummary);
      mainSector = summary.mainSector;
      secondSector = mainSector;
      for( auto sec : summary.sectors ){
	if( sec != summary.mainSector ) secondSector = sec;
      }

      // hack for now to get hit counts onto the track particle
      ElementLink< xAOD::TrackParticleContainer > tpLink = muon.combinedTrackParticleLink();
      if( !tpLink.isValid() ) tpLink = muon.muonSpectrometerTrackParticleLink();
      if( tpLink.isValid() ){
	xAOD::TrackParticle* tp = const_cast<xAOD::TrackParticle*>(*tpLink);
	if( tp ){
	  uint8_t numberOfPrecisionLayers = summary.nprecisionLayers;
	  uint8_t numberOfPrecisionHoleLayers = summary.nprecisionHoleLayers;
	  uint8_t numberOfPhiLayers = summary.nphiLayers;
	  uint8_t numberOfPhiHoleLayers = summary.nphiHoleLayers;
	  uint8_t numberOfTriggerEtaLayers = summary.ntrigEtaLayers;
	  uint8_t numberOfTriggerEtaHoleLayers = summary.ntrigEtaHoleLayers;
	  tp->setSummaryValue(numberOfPrecisionLayers,xAOD::numberOfPrecisionLayers);
	  tp->setSummaryValue(numberOfPrecisionHoleLayers,xAOD::numberOfPrecisionHoleLayers);
	  tp->setSummaryValue(numberOfPhiLayers,xAOD::numberOfPhiLayers);
	  tp->setSummaryValue(numberOfPhiHoleLayers,xAOD::numberOfPhiHoleLayers);
	  tp->setSummaryValue(numberOfTriggerEtaLayers,xAOD::numberOfTriggerEtaLayers);
	  tp->setSummaryValue(numberOfTriggerEtaHoleLayers,xAOD::numberOfTriggerEtaHoleLayers);
	}
      }

      if( trackSummary->muonTrackSummary() ){
	const Trk::MuonTrackSummary& mts = *trackSummary->muonTrackSummary();
	
	// loop over chambers
	std::vector<Trk::MuonTrackSummary::ChamberHitSummary>::const_iterator chit = mts.chamberHitSummary().begin();
	std::vector<Trk::MuonTrackSummary::ChamberHitSummary>::const_iterator chit_end = mts.chamberHitSummary().end();
	for( ;chit!=chit_end;++chit ) {
	  const Identifier& chId = chit->chamberId();
	  bool measPhi = m_idHelper->measuresPhi(chId);
	  bool isMdt = m_idHelper->isMdt(chId);
	  bool isCsc = m_idHelper->isCsc(chId);
	  bool isMM   = m_idHelper->isMM(chId);
	  bool issTgc = m_idHelper->issTgc(chId);

	  // first deal with precision hits
	  if( isMdt || isMM || (!measPhi && (isCsc || issTgc ) ) ){
	    Muon::MuonStationIndex::ChIndex index = m_idHelper->chamberIndex(chId);
	    uint8_t* hits = 0;
	    uint8_t* holes = 0;
	    if( index == Muon::MuonStationIndex::BIS || index == Muon::MuonStationIndex::EIS || index == Muon::MuonStationIndex::CSS ) {
	      hits = &innerSmallHits;
	      holes = &innerSmallHoles;
	    }else if( index == Muon::MuonStationIndex::BIL || index == Muon::MuonStationIndex::EIL || index == Muon::MuonStationIndex::CSL ){
	      hits = &innerLargeHits;
	      holes = &innerLargeHoles;
	    }else if( index == Muon::MuonStationIndex::BMS || index == Muon::MuonStationIndex::EMS ){
	      hits = &middleSmallHits;
	      holes = &middleSmallHoles;
	    }else if( index == Muon::MuonStationIndex::BML || index == Muon::MuonStationIndex::EML ){
	      hits = &middleLargeHits;
	      holes = &middleLargeHoles;
	    }else if( index == Muon::MuonStationIndex::BOS || index == Muon::MuonStationIndex::EOS ){
	      hits = &outerSmallHits;
	      holes = &outerSmallHoles;
	    }else if( index == Muon::MuonStationIndex::BOL || index == Muon::MuonStationIndex::EOL ){
	      hits = &outerLargeHits;
	      holes = &outerLargeHoles;
	    }else if( index == Muon::MuonStationIndex::BEE || index == Muon::MuonStationIndex::EES ){
	      hits = &extendedSmallHits;
	      holes = &extendedSmallHoles;
	    }else if( index == Muon::MuonStationIndex::EEL ){
	      hits = &extendedLargeHits;
	      holes = &extendedLargeHoles;
	    }else{
	      ATH_MSG_WARNING("Unknown ChamberIndex" << Muon::MuonStationIndex::chName(index) );
	    }
	    if( hits ){
	      *hits  += chit->netaHits();
	      *holes += chit->etaProjection().nholes;
	    }
	  }
	  if( !isMdt && !isMM ){

	    uint8_t* phiHits = 0;
	    uint8_t* phiHoles = 0;
	    uint8_t* etaHits = 0;
	    uint8_t* etaHoles = 0;
	    Muon::MuonStationIndex::PhiIndex index = m_idHelper->phiIndex(chId);
	    if( index == Muon::MuonStationIndex::BM1 || index == Muon::MuonStationIndex::T4 || 
		index == Muon::MuonStationIndex::CSC || index == Muon::MuonStationIndex::STGC1 ||
		index == Muon::MuonStationIndex::STGC2 ) {
	      phiHits = &phiLayer1Hits;
	      phiHoles = &phiLayer1Holes;
	      etaHits = &etaLayer1Hits;
	      etaHoles = &etaLayer1Holes;
	    }else if( index == Muon::MuonStationIndex::BM2 || index == Muon::MuonStationIndex::T1 ){
	      phiHits = &phiLayer2Hits;
	      phiHoles = &phiLayer2Holes;
	      etaHits = &etaLayer2Hits;
	      etaHoles = &etaLayer2Holes;
	    }else if( index == Muon::MuonStationIndex::BO1 || index == Muon::MuonStationIndex::T2 ){
	      phiHits = &phiLayer3Hits;
	      phiHoles = &phiLayer3Holes;
	      etaHits = &etaLayer3Hits;
	      etaHoles = &etaLayer3Holes;
	    }else if( index == Muon::MuonStationIndex::T3 ){
	      phiHits = &phiLayer4Hits;
	      phiHoles = &phiLayer4Holes;
	      etaHits = &etaLayer4Hits;
	      etaHoles = &etaLayer4Holes;
	    }else{
	      ATH_MSG_WARNING("Unknown ChamberIndex" << Muon::MuonStationIndex::phiName(index) );
	    }
	    if( phiHits ){
	      *phiHits += chit->nphiHits();
	      *phiHoles += chit->phiProjection().nholes;
	    }
	    if( etaHits && !isCsc && !issTgc ){
	      *etaHits += chit->netaHits();
	      *etaHoles += chit->etaProjection().nholes;
	    }
	  }
	}
      }
    }
    muon.setSummaryValue(mainSector,xAOD::primarySector);
    muon.setSummaryValue(secondSector,xAOD::secondarySector);
    muon.setSummaryValue(innerSmallHits,xAOD::innerSmallHits);
    muon.setSummaryValue(innerLargeHits,xAOD::innerLargeHits);
    muon.setSummaryValue(middleSmallHits,xAOD::middleSmallHits);
    muon.setSummaryValue(middleLargeHits,xAOD::middleLargeHits);
    muon.setSummaryValue(outerSmallHits,xAOD::outerSmallHits);
    muon.setSummaryValue(outerLargeHits,xAOD::outerLargeHits);
    muon.setSummaryValue(extendedSmallHits,xAOD::extendedSmallHits);
    muon.setSummaryValue(extendedLargeHits,xAOD::extendedLargeHits);

    muon.setSummaryValue(innerSmallHoles,xAOD::innerSmallHoles);
    muon.setSummaryValue(innerLargeHoles,xAOD::innerLargeHoles);
    muon.setSummaryValue(middleSmallHoles,xAOD::middleSmallHoles);
    muon.setSummaryValue(middleLargeHoles,xAOD::middleLargeHoles);
    muon.setSummaryValue(outerSmallHoles,xAOD::outerSmallHoles);
    muon.setSummaryValue(outerLargeHoles,xAOD::outerLargeHoles);
    muon.setSummaryValue(extendedSmallHoles,xAOD::extendedSmallHoles);
    muon.setSummaryValue(extendedLargeHoles,xAOD::extendedLargeHoles);

    muon.setSummaryValue(phiLayer1Hits,xAOD::phiLayer1Hits);
    muon.setSummaryValue(phiLayer2Hits,xAOD::phiLayer2Hits);
    muon.setSummaryValue(phiLayer3Hits,xAOD::phiLayer3Hits);
    muon.setSummaryValue(phiLayer4Hits,xAOD::phiLayer4Hits);

    muon.setSummaryValue(etaLayer1Hits,xAOD::etaLayer1Hits);
    muon.setSummaryValue(etaLayer2Hits,xAOD::etaLayer2Hits);
    muon.setSummaryValue(etaLayer3Hits,xAOD::etaLayer3Hits);
    muon.setSummaryValue(etaLayer4Hits,xAOD::etaLayer4Hits);

    muon.setSummaryValue(phiLayer1Holes,xAOD::phiLayer1Holes);
    muon.setSummaryValue(phiLayer2Holes,xAOD::phiLayer2Holes);
    muon.setSummaryValue(phiLayer3Holes,xAOD::phiLayer3Holes);
    muon.setSummaryValue(phiLayer4Holes,xAOD::phiLayer4Holes);

    muon.setSummaryValue(etaLayer1Holes,xAOD::etaLayer1Holes);
    muon.setSummaryValue(etaLayer2Holes,xAOD::etaLayer2Holes);
    muon.setSummaryValue(etaLayer3Holes,xAOD::etaLayer3Holes);
    muon.setSummaryValue(etaLayer4Holes,xAOD::etaLayer4Holes);

  }

}	// end of namespace
