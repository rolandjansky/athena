/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
// FileName : TileHitVecToCntTool.cxx
// Author   : Vishnu Zutshi
// Created  : Dec. 2009
//************************************************************

// Tile includes
#include "TileSimAlgs/TileHitVecToCntTool.h"
#include "TileIdentifier/TileHWID.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileEvent/TileHitNonConstContainer.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCablingSvc.h"

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"

// Athena includes
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "StoreGate/WriteHandle.h"

// Pile up
#include "PileUpTools/PileUpMergeSvc.h"
// Trigger time
#include "AthenaKernel/ITriggerTime.h"
#include "AthenaKernel/errorcheck.h"
// For the Athena-based random numbers.
#include "AthenaKernel/IAtRndmGenSvc.h"

// Gaudi includes
#include "GaudiKernel/SystemOfUnits.h"


#include "CLHEP/Random/Randomize.h"
#include "CLHEP/Random/RandomEngine.h"

#include <algorithm>

using CLHEP::RandFlat;
using CLHEP::RandGaussQ; // FIXME CLHEP::RandGaussZiggurat is faster and more accurate.
using CLHEP::RandPoissonT;
using CLHEP::RandGeneral;

TileHitVecToCntTool::TileHitVecToCntTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
    : PileUpToolBase(type,name,parent)
    , m_infoName("TileInfo")
    , m_run2(false)
    , m_pileUp(false)
    , m_deltaT(1.0 * Gaudi::Units::nanosecond)
    , m_timeFlag(0)
    , m_triggerTime(0.0)
    , m_maxHitTime(150.5 * Gaudi::Units::nanosecond)
    , m_photoStatisticsWindow(62.5  * Gaudi::Units::nanosecond)
    , m_photoElectronStatistics(2)
    , m_skipNoHit(false)
    , m_rndmEvtOverlay(false)
    , m_useTriggerTime(false)
    , m_triggerTimeTool("")
    , m_mergeSvc(0)
    , m_tileID(0)
    , m_tileTBID(0)
    , m_tileInfo(0)
    , m_tileMgr(0)
    , m_pHRengine(0)
    , m_rndmSvc("AtRndmGenSvc",name)
    , m_hits(0)
    , m_doChecks(false)
    , m_doChecksTB(false)
    , m_mbtsOffset(0)
    , m_cablingSvc("TileCablingSvc", name)
    , m_cabling(0)
{
    declareInterface<IPileUpTool>(this);

    m_hitVectorNames.push_back("TileHitVec");

    declareProperty("TileHitVectors", m_hitVectorNames,  "Name of input hit vectors (default=TileHitVec)" );
    declareProperty("TileInfoName", m_infoName,          "Name of TileInfo store (default=TileInfo");
    declareProperty("PileUp",m_pileUp,                   "To switch on pileup (default=false)");
    declareProperty("DeltaT",m_deltaT,                   "Minimal Time granularity in TileHit (default=1ns)");
    declareProperty("HitTimeFlag",m_timeFlag,            "Specail options to deal with times of hits for cosmics and TB (default=0)");
    declareProperty("TriggerTime",m_triggerTime,         "Fixed trigger time value (default=0)");
    declareProperty("UseTriggerTime",m_useTriggerTime,   "Take trigger time from external tool (default=false)");
    declareProperty("TriggerTimeTool",m_triggerTimeTool, "Name of trigger time tool (default='')");
    declareProperty("MaxHitTime", m_maxHitTime,           "All sub-hits with time above m_maxHitTime will be ignored");
    declareProperty("PhotostatWindow", m_photoStatisticsWindow, "Sum up energy in [-m_photoStatWindow,+m_photoStatWindow] and use it for photostatistics");
    declareProperty("PhotostatType", m_photoElectronStatistics,          "Method to apply photostatistics (default=2)");
    declareProperty("RndmSvc", m_rndmSvc,                 "Random Number Service used in TileHitVecToCnt");
    declareProperty("SkipNoHit",m_skipNoHit,              "Skip events with no Tile hits (default=false)");
    declareProperty("RndmEvtOverlay",m_rndmEvtOverlay = false, "Pileup and/or noise added by overlaying random events (default=false)");
}

StatusCode TileHitVecToCntTool::initialize() {

  ATH_MSG_DEBUG("TileHitVecToCntTool initialization started");

  bool error = false;

  CHECK(m_rndmSvc.retrieve());
  m_pHRengine = m_rndmSvc->GetEngine("Tile_HitVecToCnt");

  // retrieve Tile detector manager, TileID helper and TileInfo from det store

  CHECK(detStore()->retrieve(m_tileMgr));

  CHECK(detStore()->retrieve(m_tileID));

  CHECK(detStore()->retrieve(m_tileTBID));

  CHECK(detStore()->retrieve(m_tileInfo, m_infoName));

  CHECK(m_cablingSvc.retrieve());
  m_cabling = m_cablingSvc->cablingService();

  m_run2 = (m_cabling->getCablingType() == TileCablingService::RUN2Cabling);

  for (int i = 0; i < 7; ++i) {
    Identifier pmt_id;
    switch (i) { // invent pmt_id in a given part(sampling) of the calorimeter
      case 6:
        //pmt_id = m_tileTBID->channel_id(-1/*side*/, 0/*phi*/, 2/*radius*/);
        // use the same ID as for normal E-cell - to obtain the same Npe at the end
        pmt_id = m_tileID->pmt_id(3/*section*/, 1/*side*/, 0/*module*/, 11/*tower*/, 3/*sample*/, 0);
        break; // E4' cell uses special ID, similar to MBTS
      case 5:
        pmt_id = m_tileTBID->channel_id(1/*side*/, 0/*phi*/, 1/*radius*/);
        break; // MBTS outer cell
      case 4:
        pmt_id = m_tileTBID->channel_id(1/*side*/, 0/*phi*/, 0/*radius*/);
        break; // MBTS inner cell
      case 3:
        pmt_id = m_tileID->pmt_id(3/*section*/, 1/*side*/, 0/*module*/, 11/*tower*/, 3/*sample*/, 0);
        break; // gap/crack
      default:
        pmt_id = m_tileID->pmt_id(1/*section*/, 1/*side*/, 0/*module*/, 0/*tower*/, i/*sample*/, 0); // normal cell
        break;
    }
    // take number of photoelectrons per GeV divide by 1000 to go to MeV
    // and multiply by inverted sampling fraction (35.9)
    // to get number of photoelectrons per 1 MeV energy in scintillator
    m_nPhotoElectrons[i] = (m_tileInfo->NPhElec(i)) / (Gaudi::Units::GeV / Gaudi::Units::MeV)
        * m_tileInfo->HitCalib(pmt_id);

    //ATH_MSG_DEBUG( i << " " << m_tileInfo->NPhElec(i) << " " << m_tileInfo->HitCalib(pmt_id) );

  }

  if (m_nPhotoElectrons[0] <= 0) {
    std::fill(m_nPhotoElectrons, m_nPhotoElectrons + 7, 0);
    ATH_MSG_DEBUG("No photostatistics effect will be simulated");
  }

  ATH_MSG_DEBUG( " numPhElec/MeV for TileHit energy: "
                << " A "  << m_nPhotoElectrons[0]
                << " BC " << m_nPhotoElectrons[1]
                << " D "  << m_nPhotoElectrons[2]
                << " E "  << m_nPhotoElectrons[3]
                << " E4' "  << m_nPhotoElectrons[6]
                << " MBTSout "  << m_nPhotoElectrons[5]
                << " MBTSin "  << m_nPhotoElectrons[4]
  );

  if (m_rndmEvtOverlay) {
    m_pileUp = false;
    ATH_MSG_INFO("Zero-luminosity pile-up selected");
    ATH_MSG_INFO("Taking hits from original event only");
  }

  if (m_pileUp || m_rndmEvtOverlay) {
    ATH_MSG_INFO("take events from PileUp service");

    //
    // locate the PileUpMergeSvc and initialize our local ptr
    //
    if (service("PileUpMergeSvc", m_mergeSvc).isFailure()) {
      error = true;
      ATH_MSG_ERROR("Can not retrive PileUpMergeSvc");
      ATH_MSG_ERROR("Setting PileUp and RndmOverlay flags to FALSE ");
      m_pileUp = m_rndmEvtOverlay = false;
    } else {
      ATH_MSG_INFO("PileUpMergeSvc successfully initialized");
    }

    if (m_useTriggerTime) {
      ATH_MSG_INFO(" In case of pileup, the trigger time subtraction is done in PileUpSvc");
      ATH_MSG_INFO("  => TileHitVecToCnt will not apply Trigger Time ");
      m_useTriggerTime = false;
    }
    m_timeFlag = 0;

    if (m_pileUp) {
      // prepare vector with all hits
      m_mbtsOffset = m_tileID->pmt_hash_max();
      if (m_run2)
        m_allHits.resize(m_mbtsOffset + N_MBTS_CELLS + N_E4PRIME_CELLS);
      else
        m_allHits.resize(m_mbtsOffset + N_MBTS_CELLS);
      Identifier hit_id;
      IdContext pmt_context = m_tileID->pmt_context();
      for (int i = 0; i < m_mbtsOffset; ++i) {
        m_tileID->get_id((IdentifierHash) i, hit_id, &pmt_context);
        TileHit * pHit = new TileHit(hit_id, 0., 0.);
        pHit->reserve(71); // reserve max possible size for pileup
        m_allHits[i] = pHit;
      }
      for (int side = 0; side < N_SIDE; ++side) {
        for (int phi = 0; phi < N_PHI; ++phi) {
          for (int eta = 0; eta < N_ETA; ++eta) {
            hit_id = m_tileTBID->channel_id((side > 0) ? 1 : -1, phi, eta);
            TileHit * pHit = new TileHit(hit_id, 0., 0.);
            pHit->reserve(71); // reserve max possible size for pileup
            m_allHits[mbts_index(side, phi, eta)] = pHit;
          }
        }
      }
      if (m_run2) {
        for (int phi = 0; phi < E4_N_PHI; ++phi) {
          hit_id = m_tileTBID->channel_id(E4_SIDE, phi, E4_ETA);
          TileHit * pHit = new TileHit(hit_id, 0., 0.);
          pHit->reserve(71); // reserve max possible size for pileup
          m_allHits[e4pr_index(phi)] = pHit;
        }
      }
    }

  } else {

    ATH_MSG_INFO("no pile up");

    if (m_useTriggerTime) {

      m_timeFlag = 2;

      if (!m_triggerTimeTool.empty()) {
        ATH_MSG_INFO( "Trigger time is taken from external tool '" << m_triggerTimeTool.name()
                     << "'; therefore set HitTimeFlag to 2");
        if (m_triggerTimeTool.retrieve().isFailure()) {
          error = true;
          ATH_MSG_ERROR("Unable to find tool for " << m_triggerTimeTool.name());
          ATH_MSG_ERROR("Take average time from all hits in event as trigger time");
          m_useTriggerTime = false;
          m_triggerTimeTool.setTypeAndName("");
        }
      }
    }

    switch (m_timeFlag) {
      case 2:
        if (m_triggerTimeTool.empty()) {
          if (m_triggerTime > 0.0) {
            m_useTriggerTime = true;
            ATH_MSG_INFO("Fixed trigger time of " << m_triggerTime << " ns will be used");
          } else if (m_triggerTime < 0.0) {
            m_useTriggerTime = false;
            ATH_MSG_INFO( "Minimal hit time will be used as trigger time"
                         << " with random additional shift between 0 and " << -m_triggerTime << " ns");
          } else {
            m_useTriggerTime = false;
            ATH_MSG_INFO("Average time will be calculated in every event");
          }
        }
        break;
      case 1:
        ATH_MSG_INFO("Time of all hits will be reset to zero");
        break;
      default:
        ATH_MSG_INFO("Time of all hits will be preserved during copy");
        m_timeFlag = 0;
        break;
    }
  }

  if (m_run2) {
    const TileHWID* tileHWID;
    CHECK(detStore()->retrieve(tileHWID));

    m_fragHashFunc.initialize(tileHWID);

    m_E1merged.resize(tileHWID->drawer_hash_max());
    m_MBTSmerged.resize(tileHWID->drawer_hash_max());

    for (int ros = 3; ros < 5; ++ros) {
      for (int drawer = 0; drawer < 64; ++drawer) {
        int frag_id = tileHWID->frag(ros, drawer);
        IdentifierHash frag_hash = m_fragHashFunc(frag_id);
        m_E1merged[frag_hash] = (m_cabling->E1_merged_with_run2(ros, drawer) != 0);
        m_MBTSmerged[frag_hash] = (m_cabling->is_MBTS_merged_run2(drawer));
      }
    }
    ATH_MSG_INFO("Number of E1 cell to be merged: " << std::count (m_E1merged.begin(), m_E1merged.end(), true));
    ATH_MSG_INFO("Number of MBTS cell to be merged: " << std::count (m_MBTSmerged.begin(), m_MBTSmerged.end(), true));
  }


  ATH_CHECK( m_hitContainerKey.initialize() );

  ATH_MSG_DEBUG("TileHitVecToCntTool initialization completed");

  if (error)
    return StatusCode::RECOVERABLE;
  else
    return StatusCode::SUCCESS;
}

StatusCode TileHitVecToCntTool::createContainers() {

  ATH_MSG_VERBOSE("TileHitVecToCntTool createContainers started");

  if (m_pileUp) {
    m_hits = new TileHitNonConstContainer(SG::VIEW_ELEMENTS);
    std::vector<TileHit *>::iterator iHit = m_allHits.begin();
    std::vector<TileHit *>::iterator lastHit = m_allHits.end();
    for (; iHit != lastHit; ++iHit) {
      TileHit *pHit = (*iHit);
      pHit->setZero();
    }
  } else {
    m_hits = new TileHitNonConstContainer(SG::OWN_ELEMENTS);
  }

  // disable checks for TileID and remember previous state
  m_doChecks = m_tileID->do_checks();
  m_tileID->set_do_checks(false);
  m_doChecksTB = m_tileTBID->do_checks();
  m_tileTBID->set_do_checks(false);

  ATH_MSG_VERBOSE("TileHitVecToCntTool createContainers finished");

  return StatusCode::SUCCESS;

}

StatusCode TileHitVecToCntTool::prepareEvent(unsigned int /*nInputEvents*/) {

  ATH_MSG_DEBUG("TileHitVecToCntTool prepareEvent initialization started");

  CHECK(this->createContainers());

  ATH_MSG_DEBUG("TileHitVecToCntTool prepareEvent finished");

  return StatusCode::SUCCESS;
}

void TileHitVecToCntTool::processHitVectorForOverlay(const TileHitVector* inputHits, int& nHit, double& eHitTot) {

  TileHitVecConstIterator inpItr = inputHits->begin();
  TileHitVecConstIterator end = inputHits->end();

  for (; inpItr != end; ++inpItr) {

    const TileHit * cinp = &(*inpItr);

    eHitTot += cinp->energy(); // not really correct if TileHit contains vector of energies
    // but eHitTot is needed for debug purposes only
    TileHit * pHit = new TileHit(*cinp);
    m_hits->push_back(pHit);
    ++nHit;

    if (msgLvl(MSG::VERBOSE)) {
      int hitsize = cinp->size();
      double eHit = 0.0;
      double tHit = 0.0;
      for (int i = 0; i < hitsize; ++i) {
        eHit += cinp->energy(i);
        tHit += cinp->time(i) * cinp->energy(i);
      }
      if (eHit > 0.0)
        tHit /= eHit;
      else
        tHit = cinp->time();

      eHitTot += eHit - cinp->energy(); // put total energy instead of first hit energy

      msg(MSG::VERBOSE) << " nHit=" << nHit
                        << " id=" << m_tileID->to_string(cinp->identify(), -1)
                        << " eHit=" << eHit
                        << " tHit=" << tHit
                        << " Copy hit: ener=";
      for (int i = 0; i < hitsize; ++i)
        msg(MSG::VERBOSE) << cinp->energy(i) << " ";
      msg(MSG::VERBOSE) << "time=";
      for (int i = 0; i < hitsize; ++i)
        msg(MSG::VERBOSE) << cinp->time(i) << " ";
      msg(MSG::VERBOSE) << endmsg;
    }
  }
  return;
}

void TileHitVecToCntTool::processHitVectorForPileUp(const TileHitVector* inputHits, double SubEvtTimOffset, int& nHit,
    double& eHitTot) {

  IdContext pmt_context = m_tileID->pmt_context();
  IdContext tbchannel_context = m_tileTBID->channel_context();
  IdentifierHash hit_idhash;

  const bool inTimeEvent(fabs(SubEvtTimOffset) < m_deltaT);
  // Loop over hits in this HitVector
  TileHitVecConstIterator inpItr = inputHits->begin();
  TileHitVecConstIterator end = inputHits->end();

  for (; inpItr != end; ++inpItr) {

    const TileHit * cinp = &(*inpItr);
    Identifier hit_id = cinp->identify();

    if (m_tileTBID->is_tiletb(hit_id)) {
      int side = std::max(0, m_tileTBID->type(hit_id));
      int phi = m_tileTBID->module(hit_id);
      int eta = m_tileTBID->channel(hit_id);
      if (eta < 2)
        hit_idhash = mbts_index(side, phi, eta);
      else
        hit_idhash = e4pr_index(phi);
    } else {
      m_tileID->get_hash(hit_id, hit_idhash, &pmt_context);
    }

    double ener = cinp->energy();
    double time = cinp->time() + SubEvtTimOffset;

    ++nHit;
    eHitTot += ener;

    TileHit * pHit = m_allHits[hit_idhash];

    if (0 == pHit) {

      // keep this "if" just in case there is a bug somewhere ... ( will be removed soon)
      ATH_MSG_ERROR(" new hit AND MEMORY LEAK HERE!!!");

      if (inTimeEvent) {
        pHit = new TileHit(hit_id, ener, time, m_deltaT);
      } else {
        pHit = new TileHit(hit_id, 0.0, 0.0); // create in-time hit with zero energy
        pHit->add(ener, time, m_deltaT);
      }
      m_allHits[hit_idhash] = pHit;

      if (msgLvl(MSG::VERBOSE)) {
        HWIdentifier channel_id = pHit->pmt_HWID();
        msg(MSG::VERBOSE) << " nH=" << nHit
                          << " id=" << m_tileID->to_string(hit_id, -1)
                          << " HWid=" << m_tileID->to_string(channel_id)
                          << " e=" << ener
                          << " time=" << time
                          << " offs=" << SubEvtTimOffset
                          << " new hit" << endmsg;
      }

    } else {

      if (time < m_maxHitTime) pHit->add(ener, time, m_deltaT);

      if (msgLvl(MSG::VERBOSE)) {
        if (pHit->size() > 1 || pHit->energy() != 0.0)
          msg(MSG::VERBOSE) << " nHit=" << nHit
                            << " id=" << m_tileID->to_string(hit_id, -1)
                            << " ener=" << ener
                            << " time=" << time
                            << " offs=" << SubEvtTimOffset
                            << " double hit" << endmsg;
        else
          msg(MSG::VERBOSE) << " nH=" << nHit
                            << " id=" << m_tileID->to_string(hit_id, -1)
                            << " HWid=" << m_tileID->to_string(pHit->pmt_HWID())
                            << " e=" << ener
                            << " time=" << time
                            << " offs=" << SubEvtTimOffset
                            << " new hit" << endmsg;
      }
    }

    int hitsize = cinp->size();
    for (int ind = 1; ind < hitsize; ++ind) { // if we have double hits in original hit
      ener = cinp->energy(ind);           // merge all of them with appropriate time
      time = cinp->time(ind) + SubEvtTimOffset;

      ++nHit;
      eHitTot += ener;

      if (time < m_maxHitTime) pHit->add(ener, time, m_deltaT);

      if (msgLvl(MSG::VERBOSE))
        msg(MSG::VERBOSE) << " nHit=" << nHit
                          << " id=" << m_tileID->to_string(hit_id, -1)
                          << " ener=" << ener
                          << " time=" << time
                          << " double hit from single hit" << endmsg;
    }
  }         // loop over hits in one vector
  return;
}

void TileHitVecToCntTool::processHitVectorWithoutPileUp(const TileHitVector* inputHits, int& nHit, double& eHitTot) {

  TileHitVecConstIterator inpItr = inputHits->begin();
  TileHitVecConstIterator end = inputHits->end();

  //**
  //* Iterate over hits, creating new TileHits
  //* Add each TileHit to the TileHitContainer.
  //**

  switch (m_timeFlag) {

    case 0: {
      for (; inpItr != end; ++inpItr) {
        const TileHit * cinp = &(*inpItr);
        eHitTot += cinp->energy(); // not really correct if TileHit contains vector of energies
        // but eHitTot is needed for debug purposes only
        TileHit * pHit = new TileHit(*cinp);
        m_hits->push_back(pHit);
        ++nHit;

        if (msgLvl(MSG::VERBOSE)) {
          int hitsize = cinp->size();
          double eHit = 0.0;
          double tHit = 0.0;
          for (int i = 0; i < hitsize; ++i) {
            eHit += cinp->energy(i);
            tHit += cinp->time(i) * cinp->energy(i);
          }
          if (eHit > 0.0)
            tHit /= eHit;
          else
            tHit = cinp->time();

          eHitTot += eHit - cinp->energy(); // put total energy instead of first hit energy

          msg(MSG::VERBOSE) << " nHit=" << nHit
                            << " id=" << m_tileID->to_string(cinp->identify(), -1)
                            << " eHit=" << eHit
                            << " tHit=" << tHit
                            << " Copy hit: ener=";
          for (int i = 0; i < hitsize; ++i)
            msg(MSG::VERBOSE) << cinp->energy(i) << " ";
          msg(MSG::VERBOSE) << "time=";
          for (int i = 0; i < hitsize; ++i)
            msg(MSG::VERBOSE) << cinp->time(i) << " ";
          msg(MSG::VERBOSE) << endmsg;
        }
      }
      break;
    }

    case 1: {

      for (; inpItr != end; ++inpItr) {
        const TileHit * cinp = &(*inpItr);
        int size = cinp->size();
        double eHit = 0.0;
        for (int i = 0; i < size; ++i) {
          eHit += cinp->energy(i);
        }
        eHitTot += eHit;

        // create hit with total energy at time=0 instead of original one
        Identifier pmID = cinp->pmt_ID();
        TileHit * pHit = new TileHit(pmID, eHit, 0.);

        m_hits->push_back(pHit);
        ++nHit;

        if (msgLvl(MSG::VERBOSE)) {
          int hitsize = cinp->size();
          msg(MSG::VERBOSE) << " nHit=" << nHit
                            << " id=" << m_tileID->to_string(cinp->identify(), -1)
                            << " eHit=" << eHit
                            << " tHit=0.0"
                            << " Input hit: ener=";
          for (int i = 0; i < hitsize; ++i)
            msg(MSG::VERBOSE) << cinp->energy(i) << " ";
          msg(MSG::VERBOSE) << "time=";
          for (int i = 0; i < hitsize; ++i)
            msg(MSG::VERBOSE) << cinp->time(i) << " ";
          msg(MSG::VERBOSE) << endmsg;
        }
      }
      break;

    }

    case 2: {

      double avtime = 0.0;

      if (m_useTriggerTime) {

        if (m_triggerTimeTool.empty()) {
          avtime = m_triggerTime;
        } else {
          avtime = m_triggerTimeTool->time();
        }
        ATH_MSG_DEBUG("Trigger time used : " << avtime);

      } else {

        if (m_triggerTime < 0.0) {

          avtime = 1.0e20;

          // loop to find minimal time
          for (; inpItr != end; ++inpItr) {
            const TileHit * cinp = &(*inpItr);
            int size = cinp->size();
            for (int i = 0; i < size; ++i) {
              if (cinp->time(i) < avtime) avtime = cinp->time(i);
            }
          }
          ATH_MSG_DEBUG("Minimal time in input event " << avtime);
          double shift = RandFlat::shoot(m_pHRengine, m_triggerTime, 0.0);
          ATH_MSG_DEBUG("Minimal time after random shift " << shift);
          avtime -= shift; // subtracting negative shift value here

        } else {

          double weight = 0.0;

          // loop to calculate average time
          for (; inpItr != end; ++inpItr) {
            const TileHit * cinp = &(*inpItr);
            int size = cinp->size();
            for (int i = 0; i < size; ++i) {
              avtime += cinp->time(i) * cinp->energy(i);
              weight += cinp->energy(i);
            }
          }
          if (weight > 0.0)
            avtime /= weight;
          else
            avtime = 0.0;

          ATH_MSG_DEBUG("Average time used : " << avtime);
        }

        // reset iterator to the first hit
        inpItr = inputHits->begin();
      }

      for (; inpItr != end; ++inpItr) {
        const TileHit * cinp = &(*inpItr);
        TileHit * pHit = new TileHit(*cinp);
        // subract average time from all time bins in the hit
        int size = pHit->size();
        for (int i = 0; i < size; ++i) {
          pHit->setTime(pHit->time(i) - avtime, i);
          eHitTot += cinp->energy(i);
        }

        m_hits->push_back(pHit);
        ++nHit;

        if (msgLvl(MSG::VERBOSE)) {
          int hitsize = pHit->size();
          double eHit = 0.0;
          double tHit = 0.0;
          for (int i = 0; i < hitsize; ++i) {
            eHit += pHit->energy(i);
            tHit += pHit->time(i) * cinp->energy(i);
          }
          if (eHit > 0.0)
            tHit /= eHit;
          else
            tHit = cinp->time(); // just first time

          msg(MSG::VERBOSE) << " nHit="  << nHit
                            << " id=" << m_tileID->to_string(pHit->identify(), -1)
                            << " eHit=" << eHit
                            << " tHit=" << tHit
                            << " Output hit: ener=";
          for (int i = 0; i < hitsize; ++i)
            msg(MSG::VERBOSE) << pHit->energy(i) << " ";
          msg(MSG::VERBOSE) << "time=";
          for (int i = 0; i < hitsize; ++i)
            msg(MSG::VERBOSE) << pHit->time(i) << " ";
          msg(MSG::VERBOSE) << endmsg;
        }
      }
      break;
    }

    default:
      ATH_MSG_ERROR("unexpected value m_timeFlag=" << m_timeFlag);
      break;
  }

  return;
}

StatusCode TileHitVecToCntTool::processBunchXing(int bunchXing
                                                 , SubEventIterator bSubEvents
                                                 , SubEventIterator eSubEvents)
{

  ATH_MSG_DEBUG("Inside TileHitVecToCntTool processBunchXing" << bunchXing);
  //  setFilterPassed(true);

  SubEventIterator iEvt(bSubEvents);
  if (m_rndmEvtOverlay && bunchXing != 0) iEvt = eSubEvents; // in overlay skip all events except BC=0

  while (iEvt != eSubEvents) {
    StoreGateSvc& seStore(*iEvt->ptr()->evtStore());
    /* zero all counters and sums */
    int nHit(0);
    double eHitTot(0.0);

    std::vector<std::string>::const_iterator hitVecNamesItr = m_hitVectorNames.begin();
    std::vector<std::string>::const_iterator hitVecNamesEnd = m_hitVectorNames.end();
    for (; hitVecNamesItr != hitVecNamesEnd; ++hitVecNamesItr) {

      const std::string hitVectorName(*hitVecNamesItr);

      if (m_pileUp || m_rndmEvtOverlay) {
        const TileHitVector* inputHits;
        if (!seStore.contains<TileHitVector>(hitVectorName)) {
          ATH_MSG_WARNING("TileHitVector container not found for signal event key " << hitVectorName);
          continue;
        }
        if (seStore.retrieve(inputHits, hitVectorName).isFailure()) {
          continue;
        }

        const double SubEvtTimOffset(iEvt->time());

        if (m_rndmEvtOverlay) { // overlay code
          if (fabs(SubEvtTimOffset) > 0.1) {
            ATH_MSG_ERROR("Wrong time for in-time event: " << SubEvtTimOffset << " Ignoring all hits ");
          } else {
            ATH_MSG_DEBUG(" New HitCont.  TimeOffset=" << SubEvtTimOffset << ", size =" << inputHits->size());
            this->processHitVectorForOverlay(inputHits, nHit, eHitTot);
          }
        } else if (m_pileUp) { // pileup code
          this->processHitVectorForPileUp(inputHits, SubEvtTimOffset, nHit, eHitTot);
        }
      } else {  // no PileUp
        //**
        //* Get TileHits from TileHitVector
        //**
        const TileHitVector * inputHits;
        if (evtStore()->retrieve(inputHits, hitVectorName).isFailure()) {
          ATH_MSG_WARNING("Hit Vector "<< hitVectorName << " not found in StoreGate");
          continue;
        }
        this->processHitVectorWithoutPileUp(inputHits, nHit, eHitTot);
      } // to pile-up or not

    } // end of the loop over different input hitVectorNames (normal hits and MBTS hits)

    ++iEvt;
    if (m_rndmEvtOverlay) iEvt = eSubEvents; // in overlay skip all events except fisrt one
  } // subEvent loop

  ATH_MSG_DEBUG("Exiting processBunchXing in TileHitVecToCntTool");

  return StatusCode::SUCCESS;
}

StatusCode TileHitVecToCntTool::processAllSubEvents() {

  ATH_MSG_DEBUG("TileHitVecToCntTool processAllSubEvents started");
  typedef PileUpMergeSvc::TimedList<TileHitVector>::type TimedHitContList;

  CHECK(this->createContainers());

  /* zero all counters and sums */
  int nHit(0);
  double eHitTot(0.0);

  std::vector<std::string>::const_iterator hitVecNamesItr = m_hitVectorNames.begin();
  std::vector<std::string>::const_iterator hitVecNamesEnd = m_hitVectorNames.end();
  for (; hitVecNamesItr != hitVecNamesEnd; ++hitVecNamesItr) {

    const std::string hitVectorName(*hitVecNamesItr);

    if (m_pileUp || m_rndmEvtOverlay) {
      TimedHitContList hitContList;
      // retrive list of pairs (time,container) from PileUp service
      if (!(m_mergeSvc->retrieveSubEvtsData(hitVectorName, hitContList).isSuccess()) || hitContList.size() == 0) {
        ATH_MSG_WARNING("Could not fill TimedHitContList for hit vector " << hitVectorName);
        continue; // continue to the next hit vector
      }

      // loop over this list
      TimedHitContList::iterator iCont(hitContList.begin());
      TimedHitContList::iterator iEndCont(hitContList.end());

      if (m_rndmEvtOverlay) {  // overlay code
        if (iCont != iEndCont) { // use only hits from first container
          // get time for this subevent
          const double SubEvtTimOffset(iCont->first.time());
          if (fabs(SubEvtTimOffset) > 0.1) {
            ATH_MSG_ERROR("Wrong time for in-time event: " << SubEvtTimOffset << " Ignoring all hits ");
          } else {
            // get HitVector for this subevent
            const TileHitVector* inputHits = &(*(iCont->second));
            ATH_MSG_DEBUG(" New HitCont.  TimeOffset=" << SubEvtTimOffset << ", size =" << inputHits->size());
            this->processHitVectorForOverlay(inputHits, nHit, eHitTot);
          }
        }
      } else if (m_pileUp) {  // pileup code

        for (; iCont != iEndCont; ++iCont) {
          // get time for this subevent
          const double SubEvtTimOffset(iCont->first.time());
          // get HitVector for this subevent
          const TileHitVector* inputHits = &(*(iCont->second));
          ATH_MSG_VERBOSE(" New HitCont.  TimeOffset=" << SubEvtTimOffset << ", size =" << inputHits->size());
          this->processHitVectorForPileUp(inputHits, SubEvtTimOffset, nHit, eHitTot);
        }
      }           // loop over subevent list
    } else {  // no PileUp

      //**
      //* Get TileHits from TileHitVector
      //**
      const TileHitVector * inputHits;
      if (evtStore()->retrieve(inputHits, hitVectorName).isFailure()) {
        ATH_MSG_WARNING("Hit Vector "<< hitVectorName << " not found in StoreGate");
        continue; // continue to the next hit vector
      }
      this->processHitVectorWithoutPileUp(inputHits, nHit, eHitTot);
    }

  } // end of the loop over different input hitVectorNames (normal hits and MBTS hits)

  CHECK(this->mergeEvent());

  return StatusCode::SUCCESS;
}

StatusCode TileHitVecToCntTool::mergeEvent() {

  ATH_MSG_DEBUG("Entering mergeEvent in TileHitVecToCntTool");

  if (m_pileUp) {

    std::vector<TileHit *>::iterator iHit = m_allHits.begin();
    std::vector<TileHit *>::iterator lastHit = m_allHits.end();

    int nHitUni = 0;
    double eHitInTime = 0.0;

    ATH_MSG_DEBUG("Hits being stored in container");

    for (; iHit != lastHit; ++iHit) {
      TileHit *pHit = (*iHit);
      if (pHit->size() > 1 || pHit->energy() != 0.0) {       // hit exists
        m_hits->push_back(pHit);   // store hit in container
        ++nHitUni;
        eHitInTime += pHit->energy();
      }
    }

    ATH_MSG_DEBUG(" nHitUni=" << nHitUni << " eHitInTime="<< eHitInTime);
  } else {
    //    ATH_MSG_DEBUG ( " nHit=" << nHit
    //                    << " eHitTot=" << eHitTot );
  }

  m_tileID->set_do_checks(m_doChecks); // set back this flag to TileID
  m_tileTBID->set_do_checks(m_doChecksTB); // set back this flag to TileTBID

  if (m_run2) {
    // Merge MBTS and E1 where it is needed.

    for (std::unique_ptr<TileHitCollection>& coll : *m_hits ) {
      int frag_id = coll->identify();
      IdentifierHash frag_hash = m_fragHashFunc(frag_id);
      if (m_E1merged[frag_hash])
        findAndMergeE1(coll.get(), frag_id);
      else if (m_MBTSmerged[frag_hash]) findAndMergeMBTS(coll.get(), frag_id);
    }
  }

  //photoelectron statistics.
  //loop over all hits in TileHitContainer and take energy deposited in certain period of time
  //std::vector<std::string>::const_iterator hitVecNamesEnd = m_hitVectorNames.end();

  for (std::unique_ptr<TileHitCollection>& coll : *m_hits ) {
    for (TileHit* pHit : *coll) {
      double ehit = 0.0;
      int hitsize = pHit->size();
      for (int i = 0; i < hitsize; ++i) {
        double thit = pHit->time(i);
        if (fabs(thit) < m_photoStatisticsWindow) ehit += pHit->energy(i);
      }

      Identifier pmt_id = pHit->pmt_ID();
      //HWIdentifier channel_id = (*hitItr)->pmt_HWID();
      // for gap/crack scintillators
      if (m_tileID->sample(pmt_id) == 3) {
        pmt_id = m_tileID->pmt_id(m_tileID->cell_id(pmt_id), 0);
        //channel_id = m_cabling->s2h_channel_id(pmt_id);
      }

      pHit->scale(applyPhotoStatistics(ehit, pmt_id));
    }
  }

  /* Register the set of TileHits to the event store. */
  auto hits = std::make_unique<TileHitContainer>
                 (false, m_pileUp ? SG::VIEW_ELEMENTS : SG::OWN_ELEMENTS);
  size_t hashId = 0;
  for (std::unique_ptr<TileHitCollection>& coll : *m_hits ) {
    CHECK(hits->addCollection (coll.release(), hashId++));
  }

  SG::WriteHandle<TileHitContainer> hitContainer(m_hitContainerKey);
  ATH_CHECK( hitContainer.record(std::move(hits)) );

  ATH_MSG_DEBUG("TileHit container registered to the TES with name" << m_hitContainerKey.key());

  //  if (m_skipNoHit && nHit==0) {
  //    setFilterPassed(false);
  //    ATH_MSG_DEBUG ( " No hits, skip this event "  );
  //  }

  ATH_MSG_DEBUG("Exiting mergeEvent in TileHitVecToCntTool");

  return StatusCode::SUCCESS;
}

StatusCode TileHitVecToCntTool::finalize() {

  ATH_MSG_DEBUG("Finalizing TileHitVecToCntTool");

  if (m_pileUp) {
    std::vector<TileHit *>::iterator iHit = m_allHits.begin();
    std::vector<TileHit *>::iterator lastHit = m_allHits.end();
    for (; iHit != lastHit; ++iHit) {
      delete (*iHit);
    }
  }

  ATH_MSG_DEBUG("TileHitVecToCntTool finalized");

  return StatusCode::SUCCESS;

}

double TileHitVecToCntTool::applyPhotoStatistics(double energy, Identifier pmt_id) {
  // pmt_sample = 0-3 for normal cells 4 for inner MBTS, 5 for outer MBTS, 6 for E4'
  int pmt_sample =
      (m_tileTBID->is_tiletb(pmt_id)) ? TileID::SAMP_X + m_tileTBID->channel(pmt_id) : m_tileID->sample(pmt_id);
  double pe = energy * m_nPhotoElectrons[pmt_sample];
  double pe_scale = 1., RndmPois = 1.;

  switch (m_photoElectronStatistics) {
    case 2:
      if (pe > 20.0) {
        RndmPois = std::max(0.0, RandGaussQ::shoot(m_pHRengine, pe, sqrt(pe))); // FIXME CLHEP::RandGaussZiggurat is faster and more accurate.
        pe_scale = RndmPois / pe;
      } else { // pe<=20

        if (pe > 0.) {
          double singleMEAN = 1.0;  //Parameterization of monoelectron spectra
          double singleSIGMA = 1.0;
          RndmPois = RandPoissonT::shoot(m_pHRengine, pe);

          if (RndmPois > 0) {
            pe_scale = 0;
            for (int i = 0; i < RndmPois; i++)
              pe_scale += 1 / (1.08332) * std::max(0., RandGaussQ::shoot(m_pHRengine, singleMEAN, singleSIGMA)); // FIXME CLHEP::RandGaussZiggurat is faster and more accurate.

            pe_scale /= RndmPois;
          } else
            pe_scale = 0;  //RndmPois==0
        }
      }
      break;

    case 0:
      if (pe > 0.0) {
        RndmPois = RandPoissonT::shoot(m_pHRengine, pe);
        pe_scale = RndmPois / pe;
      }
      break;

    case 1:
      if (pe > 0.0) {
        if (pe > 10.0) {
          RndmPois = std::max(0.0, RandGaussQ::shoot(m_pHRengine, pe, sqrt(pe))); // FIXME CLHEP::RandGaussZiggurat is faster and more accurate.
        } else {
          int nn = std::max(10, (int) (pe * 10.0));
          double * ProbFunc = new double[nn];
          ProbFunc[0] = exp(-pe);
          for (int i = 1; i < nn; ++i) {
            ProbFunc[i] = ProbFunc[i - 1] * pe / i;
          }
          RandGeneral* RandG = new RandGeneral(ProbFunc, nn, 0);
          RndmPois = RandG->shoot(m_pHRengine) * nn;
          //here RndmPois is continuously distributed random value obtained from Poisson
          //distribution by approximation.
          delete RandG;
          delete[] ProbFunc;
        }
        pe_scale = RndmPois / pe;
      }
      break;
  } //end switch(m_PhElStat)

  ATH_MSG_VERBOSE( "PhotoElec: id=" << m_tileID->to_string(pmt_id,-1)
                  << " totEne=" << energy
                  << ", numPhElec[" << pmt_sample << "]=" << m_nPhotoElectrons[pmt_sample]
                  << ", pe=" << pe
                  << ", rndmPoisson=" << RndmPois
                  << ", pe_scale=" << pe_scale);

  return pe_scale;
}


void TileHitVecToCntTool::findAndMergeE1(TileHitCollection* coll, int frag_id) {
  int module = frag_id & 0x3F;

  TileHitCollection::iterator hitIt = coll->begin();
  TileHitCollection::iterator endHitIt = coll->end();

  TileHitCollection::iterator fromHitIt = coll->end();
  TileHit* toHit(0);

  for (; hitIt != endHitIt; hitIt++) {
    Identifier pmt_id = (*hitIt)->pmt_ID();
    if (m_tileID->tower(pmt_id) == E1_TOWER && m_tileID->sample(pmt_id) == TileID::SAMP_E) {
      if (module == m_tileID->module(pmt_id)) {
        toHit = *hitIt;
      } else {
        fromHitIt = hitIt; // need iterator to delete this hit later.
      }
    }
  }
  
  if (fromHitIt != coll->end()) {
    ATH_MSG_VERBOSE("Found TileHit (E1 cell) for merging [" << m_tileID->to_string((*fromHitIt)->pmt_ID(), -1) 
		    << "] in module: " << module);
    if (toHit == 0) {
      int side = m_tileID->side((*fromHitIt)->pmt_ID());
      Identifier to_pmt_id = m_tileID->pmt_id(TileID::GAPDET, side, module, E1_TOWER, TileID::SAMP_E, 0);
      toHit = new TileHit(to_pmt_id);
      m_hits->push_back(toHit);
      ATH_MSG_VERBOSE("New TileHit (E1 cell) for merging added Id: " << m_tileID->to_string(toHit->pmt_ID(), -1) );
    } else {
      ATH_MSG_VERBOSE("Found TileHit (E1 cell) for merging Id: " << m_tileID->to_string(toHit->pmt_ID(), -1) );
    }

    ATH_MSG_DEBUG( "TileHit (E1 cell) Id: " << m_tileID->to_string((*fromHitIt)->pmt_ID(), -1) 
		   << " will be merged to " << m_tileID->to_string(toHit->pmt_ID(), -1) );
    
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "Before merging (E1 cell) => " << (std::string) (**fromHitIt) << endmsg;
      msg(MSG::VERBOSE) << "Before merging (E1 cell) => " << (std::string) (*toHit) << endmsg;
    }

    toHit->add(*fromHitIt, 0.1);

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "After merging (E1 cell) => " << (std::string) (*toHit) << endmsg;
      msg(MSG::VERBOSE) << "TileHit to be deleted Id (E1 cell): " << m_tileID->to_string((*fromHitIt)->pmt_ID(), -1) << endmsg;
    }
    
    coll->erase(fromHitIt);
  }
}


void TileHitVecToCntTool::findAndMergeMBTS(TileHitCollection* coll, int frag_id) {
  int module = frag_id & 0x3F;

  TileHitCollection::iterator hitIt = coll->begin();
  TileHitCollection::iterator endHitIt = coll->end();

  TileHitCollection::iterator fromHitIt = coll->end();
  TileHit* toHit(0);

  for (; hitIt != endHitIt; hitIt++) {
    Identifier pmt_id = (*hitIt)->pmt_ID();
    if (m_tileTBID->is_tiletb(pmt_id)) {
      if (m_tileTBID->phi(pmt_id) % 2 == 0) {
        toHit = *hitIt;
      } else {
        fromHitIt = hitIt; // need iterator to delete this hit later.
      }
    }
  }
  
  if (fromHitIt != coll->end()) {
    ATH_MSG_VERBOSE("Found TileHit (MBTS) for merging [" << m_tileTBID->to_string((*fromHitIt)->pmt_ID(), 0) 
		    << "] in module: " << module);
    if (toHit == 0) {
      int side = m_tileTBID->side((*fromHitIt)->pmt_ID());
      int phi = m_tileTBID->phi((*fromHitIt)->pmt_ID()) - 1;
      Identifier to_pmt_id = m_tileTBID->channel_id(side, phi, 1);
      toHit = new TileHit(to_pmt_id);
      m_hits->push_back(toHit);
      ATH_MSG_VERBOSE("New TileHit (MBTS) for merging added Id: " << m_tileTBID->to_string(toHit->pmt_ID(), 0) );
    } else {
      ATH_MSG_VERBOSE("Found TileHit (MBTS) for merging Id: " << m_tileTBID->to_string(toHit->pmt_ID(), 0) );
    }
    
    ATH_MSG_DEBUG( "TileHit (MBTS) Id: " << m_tileTBID->to_string((*fromHitIt)->pmt_ID(), 0) 
		   << " will be merged to " << m_tileTBID->to_string(toHit->pmt_ID(), 0) );

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "Before merging (MBTS) => " << (std::string) (**fromHitIt) << endmsg;
      msg(MSG::VERBOSE) << "Before merging (MBTS) => " <<  (std::string) (*toHit) << endmsg;
    }

    toHit->add(*fromHitIt, 0.1);

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "After merging (MBTS) => " << (std::string) (*toHit) << endmsg;
      msg(MSG::VERBOSE) << "TileHit to be deleted Id (MBTS): " 
			<< m_tileTBID->to_string((*fromHitIt)->pmt_ID(), 0) << endmsg;
    }
    
    coll->erase(fromHitIt);
  }

}
