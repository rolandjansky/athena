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
  : AthAlgTool(type, name, parent),
    m_hitSummaryTool("Muon::MuonHitSummaryTool/MuonHitSummaryTool"),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")

{
  declareInterface<xAOD::IMuonDressingTool>(this);
  declareProperty("MuonHitSummaryTool", m_hitSummaryTool );
  declareProperty("MuonIdHelperTool", m_idHelper );
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

  uint8_t phiLayer1RPCHits = 0;
  uint8_t phiLayer2RPCHits = 0;
  uint8_t phiLayer3RPCHits = 0;
  uint8_t phiLayer4RPCHits = 0;

  uint8_t etaLayer1RPCHits = 0;
  uint8_t etaLayer2RPCHits = 0;
  uint8_t etaLayer3RPCHits = 0;
  uint8_t etaLayer4RPCHits = 0;

  uint8_t phiLayer1RPCHoles = 0;
  uint8_t phiLayer2RPCHoles = 0;
  uint8_t phiLayer3RPCHoles = 0;
  uint8_t phiLayer4RPCHoles = 0;

  uint8_t etaLayer1RPCHoles = 0;
  uint8_t etaLayer2RPCHoles = 0;
  uint8_t etaLayer3RPCHoles = 0;
  uint8_t etaLayer4RPCHoles = 0;

  uint8_t phiLayer1TGCHits = 0;
  uint8_t phiLayer2TGCHits = 0;
  uint8_t phiLayer3TGCHits = 0;
  uint8_t phiLayer4TGCHits = 0;

  uint8_t etaLayer1TGCHits = 0;
  uint8_t etaLayer2TGCHits = 0;
  uint8_t etaLayer3TGCHits = 0;
  uint8_t etaLayer4TGCHits = 0;

  uint8_t phiLayer1TGCHoles = 0;
  uint8_t phiLayer2TGCHoles = 0;
  uint8_t phiLayer3TGCHoles = 0;
  uint8_t phiLayer4TGCHoles = 0;

  uint8_t etaLayer1TGCHoles = 0;
  uint8_t etaLayer2TGCHoles = 0;
  uint8_t etaLayer3TGCHoles = 0;
  uint8_t etaLayer4TGCHoles = 0;

  // New Small Wheel
  // STGC
  uint8_t phiLayer1STGCHits = 0;
  uint8_t phiLayer2STGCHits = 0;

  uint8_t etaLayer1STGCHits = 0;
  uint8_t etaLayer2STGCHits = 0;

  uint8_t phiLayer1STGCHoles = 0;
  uint8_t phiLayer2STGCHoles = 0;

  uint8_t etaLayer1STGCHoles = 0;
  uint8_t etaLayer2STGCHoles = 0;

  // MM
  uint8_t MMHits = 0;
  uint8_t MMHoles = 0;
  // Note: there is currently no MuonStationIndex for the two MM layers
  // In the future it might be needed to add them in order to have 
  // separate counters as follows
  // uint8_t Layer1MMHits = 0;
  // uint8_t Layer2MMHits = 0;
  // uint8_t Layer1MMHoles = 0;
  // uint8_t Layer2MMHoles = 0;


  uint8_t innerCloseHits = 0;
  uint8_t middleCloseHits = 0;
  uint8_t outerCloseHits = 0;
  uint8_t extendedCloseHits = 0;

  uint8_t innerOutBoundsHits = 0;
  uint8_t middleOutBoundsHits = 0;
  uint8_t outerOutBoundsHits = 0;
  uint8_t extendedOutBoundsHits = 0;

  uint8_t combinedTrackOutBoundsHits = 0;

  uint8_t numberOfPrecisionLayers = 0;
  uint8_t numberOfPrecisionHoleLayers = 0;
  uint8_t numberOfPhiLayers = 0;
  uint8_t numberOfPhiHoleLayers = 0;
  uint8_t numberOfTriggerEtaLayers = 0;
  uint8_t numberOfTriggerEtaHoleLayers = 0;
  uint8_t numberOfGoodPrecisionLayers = 0;

  uint8_t isEndcap = 0;
  uint8_t isSmall = 0;

  uint8_t cscEtaHits = 0;
  uint8_t cscUnspoiledEtaHits = 0;

  const Trk::TrackSummary* mstpTrackSummary = 0;

  if ( !trackSummary ) {
    // get link to track particle
    ElementLink< xAOD::TrackParticleContainer > tpLink = muon.combinedTrackParticleLink();
    if ( !tpLink.isValid() ) tpLink = muon.muonSpectrometerTrackParticleLink();
    if ( tpLink.isValid() ) {

      // check link to track
      if ( (*tpLink)->trackLink().isValid() ) trackSummary = (*tpLink)->track()->trackSummary();
    }
  }

  if ( trackSummary ) {

    Muon::IMuonHitSummaryTool::CompactSummary summary = m_hitSummaryTool->summary(*trackSummary);
    mainSector = summary.mainSector;
    secondSector = mainSector;
    for ( auto sec : summary.sectors ) {
      if ( sec != summary.mainSector ) secondSector = sec;
    }

    numberOfPrecisionLayers = summary.nprecisionLayers;
    numberOfPrecisionHoleLayers = summary.nprecisionHoleLayers;
    numberOfPhiLayers = summary.nphiLayers;
    numberOfPhiHoleLayers = summary.nphiHoleLayers;
    numberOfTriggerEtaLayers = summary.ntrigEtaLayers;
    numberOfTriggerEtaHoleLayers = summary.ntrigEtaHoleLayers;
    numberOfGoodPrecisionLayers = summary.nprecisionGoodLayers;

    if (summary.isEndcap) isEndcap = 1;
    if (summary.isSmall) isSmall = 1;

    for (auto layer : summary.stationLayers) {
      combinedTrackOutBoundsHits += layer.second.noutBoundsHits;
      if (layer.first == Muon::MuonStationIndex::BI || layer.first == Muon::MuonStationIndex::EI) {
        innerCloseHits += layer.second.nprecisionCloseHits;
      }
      if (layer.first == Muon::MuonStationIndex::BM || layer.first == Muon::MuonStationIndex::EM) {
        middleCloseHits += layer.second.nprecisionCloseHits;
      }
      if (layer.first == Muon::MuonStationIndex::BO || layer.first == Muon::MuonStationIndex::EO) {
        outerCloseHits += layer.second.nprecisionCloseHits;
      }
      if (layer.first == Muon::MuonStationIndex::BE || layer.first == Muon::MuonStationIndex::EE) {
        extendedCloseHits += layer.second.nprecisionCloseHits;
      }
    }

    //get out-of-bounds from ME track, shouldn't be any on combined track anyway
    ElementLink< xAOD::TrackParticleContainer > mstpLink = muon.muonSpectrometerTrackParticleLink();
    if (mstpLink.isValid()) {
      if ( (*mstpLink)->trackLink().isValid() ) mstpTrackSummary = (*mstpLink)->track()->trackSummary();

      if (mstpTrackSummary) {
        for (auto layer : m_hitSummaryTool->summary(*mstpTrackSummary).stationLayers) {
          if (layer.first == Muon::MuonStationIndex::BI || layer.first == Muon::MuonStationIndex::EI) {
            innerOutBoundsHits += layer.second.noutBoundsHits;
          }
          if (layer.first == Muon::MuonStationIndex::BM || layer.first == Muon::MuonStationIndex::EM) {
            middleOutBoundsHits += layer.second.noutBoundsHits;
          }
          if (layer.first == Muon::MuonStationIndex::BO || layer.first == Muon::MuonStationIndex::EO) {
            outerOutBoundsHits += layer.second.noutBoundsHits;
          }
          if (layer.first == Muon::MuonStationIndex::BE || layer.first == Muon::MuonStationIndex::EE) {
            extendedOutBoundsHits += layer.second.noutBoundsHits;
          }
        }
      }
    }

    if ( trackSummary->muonTrackSummary() ) {
      const Trk::MuonTrackSummary& mts = *trackSummary->muonTrackSummary();

      // loop over chambers
      std::vector<Trk::MuonTrackSummary::ChamberHitSummary>::const_iterator chit = mts.chamberHitSummary().begin();
      std::vector<Trk::MuonTrackSummary::ChamberHitSummary>::const_iterator chit_end = mts.chamberHitSummary().end();
      for ( ; chit != chit_end; ++chit ) {
        const Identifier& chId = chit->chamberId();
        bool isMdt = m_idHelper->isMdt(chId);
        bool isCsc = m_idHelper->isCsc(chId);
        bool isMM   = m_idHelper->isMM(chId);
        bool issTgc = m_idHelper->issTgc(chId);
        if ( isMdt || isMM || isCsc || issTgc ) {
          Muon::MuonStationIndex::ChIndex index = m_idHelper->chamberIndex(chId);
          uint8_t* hits = 0;
          uint8_t* holes = 0;
          if ( index == Muon::MuonStationIndex::BIS || index == Muon::MuonStationIndex::EIS || index == Muon::MuonStationIndex::CSS ) {
            hits = &innerSmallHits;
            holes = &innerSmallHoles;
          } else if ( index == Muon::MuonStationIndex::BIL || index == Muon::MuonStationIndex::EIL || index == Muon::MuonStationIndex::CSL ) {
            hits = &innerLargeHits;
            holes = &innerLargeHoles;
          } else if ( index == Muon::MuonStationIndex::BMS || index == Muon::MuonStationIndex::EMS ) {
            hits = &middleSmallHits;
            holes = &middleSmallHoles;
          } else if ( index == Muon::MuonStationIndex::BML || index == Muon::MuonStationIndex::EML ) {
            hits = &middleLargeHits;
            holes = &middleLargeHoles;
          } else if ( index == Muon::MuonStationIndex::BOS || index == Muon::MuonStationIndex::EOS ) {
            hits = &outerSmallHits;
            holes = &outerSmallHoles;
          } else if ( index == Muon::MuonStationIndex::BOL || index == Muon::MuonStationIndex::EOL ) {
            hits = &outerLargeHits;
            holes = &outerLargeHoles;
          } else if ( index == Muon::MuonStationIndex::BEE || index == Muon::MuonStationIndex::EES ) {
            hits = &extendedSmallHits;
            holes = &extendedSmallHoles;
          } else if ( index == Muon::MuonStationIndex::EEL ) {
            hits = &extendedLargeHits;
            holes = &extendedLargeHoles;
          } else {
            ATH_MSG_WARNING("Unknown ChamberIndex" << Muon::MuonStationIndex::chName(index) );
          }
          if ( hits ) {
            if (isMdt || isMM) {
              // MM does not have netaHits(), but only nHits() as Mdt
              *hits  += chit->nhits();
              *holes += chit->nholes();
            } else { // any other technology
              *hits += chit->netaHits();
              *holes += chit->etaProjection().nholes;
            }
          }
        }
        // Fill MM hits and holes
        if (isMM) {
          MMHits += chit->nhits();
          MMHoles += chit->nholes();
        }
        if (isCsc) {
          cscEtaHits += chit->etaProjection().nhits;
          cscUnspoiledEtaHits += chit->etaProjection().ngoodHits;
        }

        if ( !isMdt && !isMM ) {

          uint8_t* phiHits = 0;
          uint8_t* phiHoles = 0;
          uint8_t* etaHits = 0;
          uint8_t* etaHoles = 0;
          Muon::MuonStationIndex::PhiIndex index = m_idHelper->phiIndex(chId);
          if ( index == Muon::MuonStationIndex::BM1 || index == Muon::MuonStationIndex::T4 ||
               index == Muon::MuonStationIndex::CSC || index == Muon::MuonStationIndex::STGC1 ) {
            phiHits = &phiLayer1Hits;
            phiHoles = &phiLayer1Holes;
            etaHits = &etaLayer1Hits;
            etaHoles = &etaLayer1Holes;
          } else if ( index == Muon::MuonStationIndex::BM2 || index == Muon::MuonStationIndex::T1 ||
                      index == Muon::MuonStationIndex::STGC2  ) {
            phiHits = &phiLayer2Hits;
            phiHoles = &phiLayer2Holes;
            etaHits = &etaLayer2Hits;
            etaHoles = &etaLayer2Holes;
          } else if ( index == Muon::MuonStationIndex::BO1 || index == Muon::MuonStationIndex::T2 ) {
            phiHits = &phiLayer3Hits;
            phiHoles = &phiLayer3Holes;
            etaHits = &etaLayer3Hits;
            etaHoles = &etaLayer3Holes;
          } else if ( index == Muon::MuonStationIndex::BO2 || index == Muon::MuonStationIndex::T3 ) {
            phiHits = &phiLayer4Hits;
            phiHoles = &phiLayer4Holes;
            etaHits = &etaLayer4Hits;
            etaHoles = &etaLayer4Holes;
          } else {
            ATH_MSG_WARNING("Unknown ChamberIndex" << Muon::MuonStationIndex::phiName(index) );
          }
          // split into RPC and TGC
          // First RPC layer
          if (index == Muon::MuonStationIndex::BM1) {
            phiLayer1RPCHits  += chit->nphiHits();
            etaLayer1RPCHits  += chit->netaHits();
            phiLayer1RPCHoles += chit->phiProjection().nholes;
            etaLayer1RPCHoles += chit->etaProjection().nholes;
          }
          // Second RPC layer
          else if (index == Muon::MuonStationIndex::BM2) {
            phiLayer2RPCHits  += chit->nphiHits();
            etaLayer2RPCHits  += chit->netaHits();
            phiLayer2RPCHoles += chit->phiProjection().nholes;
            etaLayer2RPCHoles += chit->etaProjection().nholes;
          }
          // Third RPC layer
          else if (index == Muon::MuonStationIndex::BO1) {
            phiLayer3RPCHits  += chit->nphiHits();
            etaLayer3RPCHits  += chit->netaHits();
            phiLayer3RPCHoles += chit->phiProjection().nholes;
            etaLayer3RPCHoles += chit->etaProjection().nholes;
          }
          // Fourth RPC layer
          else if (index == Muon::MuonStationIndex::BO2) {
            phiLayer4RPCHits  += chit->nphiHits();
            etaLayer4RPCHits  += chit->netaHits();
            phiLayer4RPCHoles += chit->phiProjection().nholes;
            etaLayer4RPCHoles += chit->etaProjection().nholes;
          }
          // First TGC layer
          else if (index == Muon::MuonStationIndex::T4) {
            phiLayer1TGCHits  += chit->nphiHits();
            etaLayer1TGCHits  += chit->netaHits();
            phiLayer1TGCHoles += chit->phiProjection().nholes;
            etaLayer1TGCHoles += chit->etaProjection().nholes;
          }
          // Second TGC layer
          else if (index == Muon::MuonStationIndex::T1) {
            phiLayer2TGCHits  += chit->nphiHits();
            etaLayer2TGCHits  += chit->netaHits();
            phiLayer2TGCHoles += chit->phiProjection().nholes;
            etaLayer2TGCHoles += chit->etaProjection().nholes;
          }
          // Third TGC layer
          else if (index == Muon::MuonStationIndex::T2) {
            phiLayer3TGCHits  += chit->nphiHits();
            etaLayer3TGCHits  += chit->netaHits();
            phiLayer3TGCHoles += chit->phiProjection().nholes;
            etaLayer3TGCHoles += chit->etaProjection().nholes;
          }
          // Fourth TGC layer
          else if (index == Muon::MuonStationIndex::T3) {
            phiLayer4TGCHits  += chit->nphiHits();
            etaLayer4TGCHits  += chit->netaHits();
            phiLayer4TGCHoles += chit->phiProjection().nholes;
            etaLayer4TGCHoles += chit->etaProjection().nholes;
          }
          // First STGC layer
          else if (index == Muon::MuonStationIndex::STGC1) {
            phiLayer1STGCHits  += chit->nphiHits();
            etaLayer1STGCHits  += chit->netaHits();
            phiLayer1STGCHoles += chit->phiProjection().nholes;
            etaLayer1STGCHoles += chit->etaProjection().nholes;
          }
          // Second STGC layer
          else if (index == Muon::MuonStationIndex::STGC2) {
            phiLayer2STGCHits  += chit->nphiHits();
            etaLayer2STGCHits  += chit->netaHits();
            phiLayer2STGCHoles += chit->phiProjection().nholes;
            etaLayer2STGCHoles += chit->etaProjection().nholes;              
          }

          if ( phiHits ) {
            *phiHits += chit->nphiHits();
            *phiHoles += chit->phiProjection().nholes;
          }
          if ( etaHits && !isCsc && !issTgc ) {
            *etaHits += chit->netaHits();
            *etaHoles += chit->etaProjection().nholes;
          }
        }
      }
    }
  }

  muon.setSummaryValue(numberOfPrecisionLayers, xAOD::numberOfPrecisionLayers);
  muon.setSummaryValue(numberOfPrecisionHoleLayers, xAOD::numberOfPrecisionHoleLayers);
  muon.setSummaryValue(numberOfPhiLayers, xAOD::numberOfPhiLayers);
  muon.setSummaryValue(numberOfPhiHoleLayers, xAOD::numberOfPhiHoleLayers);
  muon.setSummaryValue(numberOfTriggerEtaLayers, xAOD::numberOfTriggerEtaLayers);
  muon.setSummaryValue(numberOfTriggerEtaHoleLayers, xAOD::numberOfTriggerEtaHoleLayers);
  muon.setSummaryValue(numberOfGoodPrecisionLayers, xAOD::numberOfGoodPrecisionLayers);

  muon.setSummaryValue(mainSector, xAOD::primarySector);
  muon.setSummaryValue(secondSector, xAOD::secondarySector);
  muon.setSummaryValue(innerSmallHits, xAOD::innerSmallHits);
  muon.setSummaryValue(innerLargeHits, xAOD::innerLargeHits);
  muon.setSummaryValue(middleSmallHits, xAOD::middleSmallHits);
  muon.setSummaryValue(middleLargeHits, xAOD::middleLargeHits);
  muon.setSummaryValue(outerSmallHits, xAOD::outerSmallHits);
  muon.setSummaryValue(outerLargeHits, xAOD::outerLargeHits);
  muon.setSummaryValue(extendedSmallHits, xAOD::extendedSmallHits);
  muon.setSummaryValue(extendedLargeHits, xAOD::extendedLargeHits);

  muon.setSummaryValue(innerSmallHoles, xAOD::innerSmallHoles);
  muon.setSummaryValue(innerLargeHoles, xAOD::innerLargeHoles);
  muon.setSummaryValue(middleSmallHoles, xAOD::middleSmallHoles);
  muon.setSummaryValue(middleLargeHoles, xAOD::middleLargeHoles);
  muon.setSummaryValue(outerSmallHoles, xAOD::outerSmallHoles);
  muon.setSummaryValue(outerLargeHoles, xAOD::outerLargeHoles);
  muon.setSummaryValue(extendedSmallHoles, xAOD::extendedSmallHoles);
  muon.setSummaryValue(extendedLargeHoles, xAOD::extendedLargeHoles);

  muon.setSummaryValue(phiLayer1Hits, xAOD::phiLayer1Hits);
  muon.setSummaryValue(phiLayer2Hits, xAOD::phiLayer2Hits);
  muon.setSummaryValue(phiLayer3Hits, xAOD::phiLayer3Hits);
  muon.setSummaryValue(phiLayer4Hits, xAOD::phiLayer4Hits);

  muon.setSummaryValue(etaLayer1Hits, xAOD::etaLayer1Hits);
  muon.setSummaryValue(etaLayer2Hits, xAOD::etaLayer2Hits);
  muon.setSummaryValue(etaLayer3Hits, xAOD::etaLayer3Hits);
  muon.setSummaryValue(etaLayer4Hits, xAOD::etaLayer4Hits);

  muon.setSummaryValue(phiLayer1Holes, xAOD::phiLayer1Holes);
  muon.setSummaryValue(phiLayer2Holes, xAOD::phiLayer2Holes);
  muon.setSummaryValue(phiLayer3Holes, xAOD::phiLayer3Holes);
  muon.setSummaryValue(phiLayer4Holes, xAOD::phiLayer4Holes);

  muon.setSummaryValue(etaLayer1Holes, xAOD::etaLayer1Holes);
  muon.setSummaryValue(etaLayer2Holes, xAOD::etaLayer2Holes);
  muon.setSummaryValue(etaLayer3Holes, xAOD::etaLayer3Holes);
  muon.setSummaryValue(etaLayer4Holes, xAOD::etaLayer4Holes);

  muon.setSummaryValue(phiLayer1RPCHits,  xAOD::phiLayer1RPCHits);
  muon.setSummaryValue(phiLayer2RPCHits,  xAOD::phiLayer2RPCHits);
  muon.setSummaryValue(phiLayer3RPCHits,  xAOD::phiLayer3RPCHits);
  muon.setSummaryValue(phiLayer4RPCHits,  xAOD::phiLayer4RPCHits);

  muon.setSummaryValue(etaLayer1RPCHits,  xAOD::etaLayer1RPCHits);
  muon.setSummaryValue(etaLayer2RPCHits,  xAOD::etaLayer2RPCHits);
  muon.setSummaryValue(etaLayer3RPCHits,  xAOD::etaLayer3RPCHits);
  muon.setSummaryValue(etaLayer4RPCHits,  xAOD::etaLayer4RPCHits);

  muon.setSummaryValue(phiLayer1RPCHoles, xAOD::phiLayer1RPCHoles);
  muon.setSummaryValue(phiLayer2RPCHoles, xAOD::phiLayer2RPCHoles);
  muon.setSummaryValue(phiLayer3RPCHoles, xAOD::phiLayer3RPCHoles);
  muon.setSummaryValue(phiLayer4RPCHoles, xAOD::phiLayer4RPCHoles);

  muon.setSummaryValue(etaLayer1RPCHoles, xAOD::etaLayer1RPCHoles);
  muon.setSummaryValue(etaLayer2RPCHoles, xAOD::etaLayer2RPCHoles);
  muon.setSummaryValue(etaLayer3RPCHoles, xAOD::etaLayer3RPCHoles);
  muon.setSummaryValue(etaLayer4RPCHoles, xAOD::etaLayer4RPCHoles);

  muon.setSummaryValue(phiLayer1TGCHits,  xAOD::phiLayer1TGCHits);
  muon.setSummaryValue(phiLayer2TGCHits,  xAOD::phiLayer2TGCHits);
  muon.setSummaryValue(phiLayer3TGCHits,  xAOD::phiLayer3TGCHits);
  muon.setSummaryValue(phiLayer4TGCHits,  xAOD::phiLayer4TGCHits);

  muon.setSummaryValue(etaLayer1TGCHits,  xAOD::etaLayer1TGCHits);
  muon.setSummaryValue(etaLayer2TGCHits,  xAOD::etaLayer2TGCHits);
  muon.setSummaryValue(etaLayer3TGCHits,  xAOD::etaLayer3TGCHits);
  muon.setSummaryValue(etaLayer4TGCHits,  xAOD::etaLayer4TGCHits);

  muon.setSummaryValue(phiLayer1TGCHoles, xAOD::phiLayer1TGCHoles);
  muon.setSummaryValue(phiLayer2TGCHoles, xAOD::phiLayer2TGCHoles);
  muon.setSummaryValue(phiLayer3TGCHoles, xAOD::phiLayer3TGCHoles);
  muon.setSummaryValue(phiLayer4TGCHoles, xAOD::phiLayer4TGCHoles);

  muon.setSummaryValue(etaLayer1TGCHoles, xAOD::etaLayer1TGCHoles);
  muon.setSummaryValue(etaLayer2TGCHoles, xAOD::etaLayer2TGCHoles);
  muon.setSummaryValue(etaLayer3TGCHoles, xAOD::etaLayer3TGCHoles);
  muon.setSummaryValue(etaLayer4TGCHoles, xAOD::etaLayer4TGCHoles);

  // New Small Wheel
  // STGC
  muon.setSummaryValue(phiLayer1STGCHits,  xAOD::phiLayer1STGCHits);
  muon.setSummaryValue(phiLayer2STGCHits,  xAOD::phiLayer2STGCHits);

  muon.setSummaryValue(etaLayer1STGCHits,  xAOD::etaLayer1STGCHits);
  muon.setSummaryValue(etaLayer2STGCHits,  xAOD::etaLayer2STGCHits);

  muon.setSummaryValue(phiLayer1STGCHoles, xAOD::phiLayer1STGCHoles);
  muon.setSummaryValue(phiLayer2STGCHoles, xAOD::phiLayer2STGCHoles);

  muon.setSummaryValue(etaLayer1STGCHoles, xAOD::etaLayer1STGCHoles);
  muon.setSummaryValue(etaLayer2STGCHoles, xAOD::etaLayer2STGCHoles);
  
  // MM
  muon.setSummaryValue(MMHits,  xAOD::MMHits);
  muon.setSummaryValue(MMHoles,  xAOD::MMHoles);
  

  muon.setSummaryValue(innerCloseHits, xAOD::innerClosePrecisionHits);
  muon.setSummaryValue(middleCloseHits, xAOD::middleClosePrecisionHits);
  muon.setSummaryValue(outerCloseHits, xAOD::outerClosePrecisionHits);
  muon.setSummaryValue(extendedCloseHits, xAOD::extendedClosePrecisionHits);

  muon.setSummaryValue(innerOutBoundsHits, xAOD::innerOutBoundsPrecisionHits);
  muon.setSummaryValue(middleOutBoundsHits, xAOD::middleOutBoundsPrecisionHits);
  muon.setSummaryValue(outerOutBoundsHits, xAOD::outerOutBoundsPrecisionHits);
  muon.setSummaryValue(extendedOutBoundsHits, xAOD::extendedOutBoundsPrecisionHits);

  muon.setSummaryValue(combinedTrackOutBoundsHits, xAOD::combinedTrackOutBoundsPrecisionHits);

  muon.setSummaryValue(isEndcap, xAOD::isEndcapGoodLayers);
  muon.setSummaryValue(isSmall, xAOD::isSmallGoodSectors);

  muon.setSummaryValue(cscEtaHits, xAOD::cscEtaHits);
  muon.setSummaryValue(cscUnspoiledEtaHits, xAOD::cscUnspoiledEtaHits);

}

} // end of namespace
