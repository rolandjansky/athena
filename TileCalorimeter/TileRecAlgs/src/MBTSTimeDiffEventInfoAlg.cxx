/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "MBTSTimeDiffEventInfoAlg.h"
#include "TileIdentifier/TileTBID.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaKernel/errorcheck.h"

#include <memory>

using xAOD::EventInfo;

/* 
 We have actually about five almost-identical copies of the code below in our repository.
 I am adding now the sixth because I need it fast. But we should try to clean that up.
 My suggestion: Adopt the strategy used by LAr, put a object in SG containing MBTS times
 (and counts, possibly energies) usable by all clients.
 */

StatusCode MBTSTimeDiffEventInfoAlg::initialize() {
  if (m_minHitsPerSide == 0u) {
    m_minHitsPerSide = 1; //Avoid div-by-zero later
  }

  ATH_MSG_INFO("Initializing. MBTS_Threshold=" << m_mbts_threshold
               << ", MinHitsPerSide=" << m_minHitsPerSide
               << ", TimeDiffThreshold=" << m_timeDiffThreshold );

  CHECK( detStore()->retrieve(m_tileTBID));

  ATH_CHECK( m_mbtsContainerKey.initialize() );
  ATH_CHECK( m_eventInfoKey.initialize() );
  ATH_CHECK( m_mbtsCollisionTimeKey.initialize() );
  ATH_CHECK( m_eventInfoDecorKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode MBTSTimeDiffEventInfoAlg::execute(const EventContext& ctx) const {

  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey, ctx);

  float eneA = 0.F;
  float eneC = 0.F;
  float timeA = 0.F;
  float timeC = 0.F;
  unsigned countA = 0;
  unsigned countC = 0;

  SG::ReadHandle<TileCellContainer> mbtsContainer (m_mbtsContainerKey, ctx);
  ATH_CHECK( mbtsContainer.isValid() );

  for (const TileCell* mbtsCell : *mbtsContainer) {

    if (mbtsCell->energy() < m_mbts_threshold) continue;
    //Got cell above threshold

    const uint8_t qbit1 = mbtsCell->qbit1();
    const Identifier& id = mbtsCell->ID();

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "Working on MBTS cell side=" << m_tileTBID->type(id)
          << " module_id=" << m_tileTBID->module(id)
          << " channel_id=" << m_tileTBID->channel(id)
          << " Energy=" << mbtsCell->energy()
          << " Time=" << mbtsCell->time()
          << " Qbit1=" << (int) qbit1
          << " Q1Mask(BC|OF|AP|TM)=[" << ((qbit1 & TileCell::MASK_BADCH) != 0)
          << "|" << ((qbit1 & TileCell::MASK_OVER) != 0) << "|"
          << ((qbit1 & TileCell::MASK_AMPL) != 0) << "|"
          << ((qbit1 & TileCell::MASK_TIME) != 0) << "] "
          //	         << " Q2Mask(BC|OF|AP|TM)=[" << (qbit2 & TileCell::MASK_BADCH)==1 << "|" <<  (qbit2 &  TileCell::MASK_OVER)==1 << "|"
          //	         << (qbit2 &  TileCell::MASK_AMPL) << "|" <<  (qbit2 &  TileCell::MASK_TIME) << "]"
          << endmsg;
    }

    //Accept only cells with time computed, not known to be bad and no overflow:
    if ((qbit1 & m_mask) != m_pattern) {
      ATH_MSG_DEBUG( "Rejected based on quality bits" );
      continue;
    }

    // MBTS Id type is  "side"  +/- 1
    const int type_id = m_tileTBID->type(id);
    if (type_id == 1) { //A-side
      eneA += mbtsCell->energy();
      timeA += mbtsCell->time();
      countA++;
    } else if (type_id == -1) { //C-side
      eneC += mbtsCell->energy();
      timeC += mbtsCell->time();
      countC++;
    } else {
      ATH_MSG_WARNING( "Unexpected 'side' from MBTS identifier " );
      continue;
    }
  } //end loop over MBTS cells

  if (countA > 0) timeA /= countA;
  if (countC > 0) timeC /= countC;

  if (countA > m_minHitsPerSide && countC > m_minHitsPerSide) {
    ATH_MSG_DEBUG( "Got MBTS cells above threshold on both sides" );
    const float timediff = fabs(timeA - timeC);
    ATH_MSG_DEBUG( "Time diff " << timediff << "(" << m_timeDiffThreshold << ")");
    
    if (eventInfo.isValid()) {
      if (timediff > m_timeDiffThreshold) {
        ATH_MSG_DEBUG( "Event identified as background, set bit 'MBTSTimeDiffHalo' in EventInfo Background word" );
      
        if (eventInfo->updateEventFlagBit(EventInfo::Background,
                                          EventInfo::MBTSTimeDiffHalo) == false)
        
          ATH_MSG_WARNING( "Failed to set EventInfo Background word!" );
      } else { //end if above theshold
        ATH_MSG_DEBUG( "Event identified as collision, set bit 'MBTSTimeDiffCol' in EventInfo Background word" );
      
        if (eventInfo->updateEventFlagBit(EventInfo::Background,
                                          EventInfo::MBTSTimeDiffCol) == false)
          ATH_MSG_WARNING( "Failed to set EventInfo Background word!" );
      } //end if below threshold
    } else {
      ATH_MSG_WARNING( " cannot retrieve EventInfo, will not set Tile information " );
    }
  } else {
    ATH_MSG_DEBUG( "Not enough hits above threshold to distinguish halo from collision event");
  }

  SG::WriteHandle<MBTSCollisionTime> mbtsTime(m_mbtsCollisionTimeKey, ctx);

  if (mbtsTime.record(std::make_unique<MBTSCollisionTime>(countA, countC, eneA, eneC, timeA, timeC)).isFailure()) {
    ATH_MSG_WARNING( "Cannot record MBTSCollisionTime " );
  } else {
    ATH_MSG_DEBUG( "MBTSCollisionTime recorded in event store" );
    ATH_MSG_DEBUG( " countA=" << countA
                  << " countC=" << countC
                  << " eneA=" << eneA
                  << " eneC=" << eneC
                  << " timeA=" << timeA
                  << " timeC=" << timeC );
  }

  return StatusCode::SUCCESS;
}
