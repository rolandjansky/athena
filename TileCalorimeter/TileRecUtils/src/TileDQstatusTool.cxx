/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileRecUtils/src/TileDQstatusTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2018
 * @brief Construct a TileDQstatus object.
 */


#include "TileDQstatusTool.h"
#include "TileEvent/TileDQstatus.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTBFrag.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "AthenaKernel/RNGWrapper.h"
#include <CLHEP/Random/RandomEngine.h>
#include <CLHEP/Random/RandFlat.h>
#include "GaudiKernel/EventContext.h"


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TileDQstatusTool::TileDQstatusTool (const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent)
  : base_class (type, name, parent),
    m_tileBadChanTool ("TileBadChanTool")
{
  // FIXME: public tool
  declareProperty ("TileBadChanTool", m_tileBadChanTool,
                   "Tile bad channel tool.");
}


/**
 * @brief Gaudi initialize method.
 */
StatusCode TileDQstatusTool::initialize()
{
  ATH_CHECK( detStore()->retrieve(m_tileHWID, "TileHWID") );

  if (m_simulateTrips) {
    ATH_CHECK( m_athRNGSvc.retrieve() );
    ATH_CHECK( m_tileBadChanTool.retrieve() );
  }
  else {
    m_tileBadChanTool.disable();
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Make a new TileDQstatus object.
 * @param ctx Event context.
 * @param rawChannelContainer The TileRawChannelContainer (may be null).
 * @param tileDigitsContainer The TileDigitsContainer (may be null).
 * @param tileBeamElemContainer The TileBeamElemContainer (may be null).
 * @param[out] dqstatus TileDQstatus object to fill.
 */
StatusCode
TileDQstatusTool::makeStatus (const EventContext& ctx,
                              const TileRawChannelContainer* rawChannelContainer,
                              const TileDigitsContainer* tileDigitsContainer,
                              const TileBeamElemContainer* tileBeamElemContainer,
                              TileDQstatus& dqstatus) const
{
  dqstatus.setAllGood();
  dqstatus.setRODBCID (ctx.eventID().bunch_crossing_id());

  ATH_CHECK( doBeamElem (tileBeamElemContainer, dqstatus) );

  if (rawChannelContainer != nullptr) {
    if (tileDigitsContainer != nullptr) {
      bool isCalib = false;
      bool incomplete = false;
      for (const TileDigitsCollection* coll : *tileDigitsContainer) {
        incomplete |= (coll->size() < 48);
        
        if (coll->size() > 0) {
          int dsize = (*(coll->begin()))->NtimeSamples();
          if (4 < dsize && dsize < 15) { // don't use strange fragments
            isCalib |= coll->isCalibMode();
          }
        }

        int frag = coll->identify();
        int partition = (frag >> 8);
        int drawer = (frag & 0x3F);

        std::vector < uint32_t > data = coll->getFragChipHeaderWords();
        unsigned int dataSize = std::min(16u, (unsigned int) data.size());
        for (unsigned int dmu = 0; dmu < dataSize; ++dmu) {
          if (data[dmu] == 0xFFFFFFFF)
            dqstatus.setEmptyEvent(partition, drawer, dmu, 0, 1);
        }

        data = coll->getFragChipHeaderWordsHigh();
        dataSize = std::min(16u, (unsigned int) data.size());
        for (unsigned int dmu = 0; dmu < dataSize; ++dmu) {
          if (data[dmu] == 0xFFFFFFFF)
            dqstatus.setEmptyEvent(partition, drawer, dmu, 1, 1);
        }
      }
      dqstatus.setIncompleteDigits (incomplete);
      dqstatus.setCalibMode (isCalib);
      dqstatus.setBiGain (isCalib);
    }

    TileFragHash::TYPE RChType = rawChannelContainer->get_type();
    if (RChType != TileFragHash::OptFilterDsp
        && RChType != TileFragHash::OptFilterDspCompressed)
    {
      ATH_MSG_DEBUG("RawChannelContainer didn't come from BS - don't check DQ flags");
      ATH_MSG_DEBUG("RChType = " << RChType);
    }
    else {
      for (const TileRawChannelCollection *coll : *rawChannelContainer) {
        ATH_MSG_VERBOSE("RCh collection 0x" << MSG::hex
                       << coll->identify() << MSG::dec
                       << " size=" << coll->size());

        dqstatus.fillArrays(coll, tileDigitsContainer, 0);
        dqstatus.fillArrays(coll, tileDigitsContainer, 1);
      }
      if (dqstatus.nonZeroCounter() == 0) {
        ATH_MSG_DEBUG("all DQ elements are empty - don't check DQ flags");
        dqstatus.setAllGood();
      }
      else {
        ATH_MSG_DEBUG("BiGain mode: " << ((dqstatus.isBiGain()) ? "true" : "false"));
      }
    }
  }

  if (m_simulateTrips) {
    ATHRNG::RNGWrapper* wrapper = m_athRNGSvc->getEngine (this, this->name());
    wrapper->setSeed (this->name(), ctx);
    CLHEP::HepRandomEngine* engine = wrapper->getEngine (ctx);
    double rndmVec[TileCalibUtils::MAX_DRAWER];

    for (unsigned int partition = 1; partition < TileCalibUtils::MAX_ROS; ++partition) {
      CLHEP::RandFlat::shootArray (engine, TileCalibUtils::MAX_DRAWER, rndmVec);
      std::vector<float> trips = m_tileBadChanTool->getTripsProbabilities(partition);
      dqstatus.fillTrips(partition, trips, rndmVec, this->msg());
    }
  }

  dqstatus.setFilled (true);
  return StatusCode::SUCCESS;
}


/**
 * @brief Process BeamElemContainer.
 * @param tileBeamElemContainer The TileBeamElemContainer (may be null).
 * @param dqstatus TileDQstatus object being filled.
 */
StatusCode
TileDQstatusTool::doBeamElem (const TileBeamElemContainer* tileBeamElemContainer,
                              TileDQstatus& dqstatus) const
{
  if (!tileBeamElemContainer) {
    return StatusCode::SUCCESS;
  }

  uint32_t* cispar = dqstatus.cispar();

  for (const TileBeamElemCollection* coll : *tileBeamElemContainer) {
    int frag = coll->identify();

    if (dqstatus.trigType() == 0 && coll->getLvl1Type() != 0) {
      // take it from the ROD header
      // make negative to distinguish from TileCal internal trig types
      dqstatus.setTrigType (- coll->getLvl1Type());
    }

    switch (frag) {

    case LASE_PTN_FRAG: {

      TileBeamElemCollection::const_iterator beamItr = coll->begin();
      TileBeamElemCollection::const_iterator lastBeam = coll->end();

      if (beamItr != lastBeam) {
        std::vector < uint32_t > digits = (*beamItr)->get_digits();

        if (digits.size() > 0) {
          uint32_t laserFlag = digits[0];
          if (laserFlag & 0xFF00)
            dqstatus.setTrigType (laserFlag >> 8);
        }
      }
    }
    break;

    case DIGI_PAR_FRAG: {
      dqstatus.setRODBCID (coll->getRODBCID());

      for (const TileBeamElem* elem : *coll) {
        HWIdentifier id = elem->adc_HWID();
        std::vector < uint32_t > digits = elem->get_digits();
        int cha = m_tileHWID->channel(id);

        if (cha < 15) {
          if (digits.size() > 0) {
            cispar[cha] = digits[0];
            ATH_MSG_VERBOSE("cispar [" << cha << "] = " << cispar[cha]);
          }
        }
        else if (cha == 15) {
          int siz = 15 + digits.size();
          if (siz > 110)
            siz = 110;
          for (int i = 15; i < siz; ++i) {
            cispar[i] = digits[i - 15];
            ATH_MSG_VERBOSE("cispar [" << i << "] = " << cispar[i]);
          }
          switch (cispar[16]) {

          case 0x02: {
            int aux_ext = cispar[17];
            cispar[17] = (aux_ext & 0x00ff); // dac
            cispar[18] = (aux_ext >> 8) & 0x00ff; // 00
            cispar[19] = (aux_ext >> 16) & 0x00ff; // 00
            cispar[20] = (aux_ext >> 24) & 0x00ff; // small/large cap
          }
          break;

          case 0x07: {
            bool badpar = ((cispar[16] == cispar[17]) || (cispar[17] == cispar[18]));
            int aux_ext = cispar[18];
            cispar[18] = (aux_ext & 0x00ff) - 1; // pmt ext cispar starts from 1
            cispar[19] = (aux_ext >> 8) & 0x00ff; // tower
            cispar[20] = (aux_ext >> 16) & 0x00ff; // drawer

            if (badpar || (aux_ext>>24)!=0 || cispar[18]>5 || cispar[19]>15 || cispar[20]>63) {
              ATH_MSG_WARNING("bad cispar[16,17,18]: " << cispar[16] << " " << cispar[17] << " " << aux_ext
                              << "  drawer,tower,pmt: " << cispar[20] << " " << cispar[19] << " " << (int)cispar[18]);
              cispar[16] += 0x100; // flag bad events
              cispar[18] = 5;
              cispar[19] = 0xff;
              cispar[20] = 0xff;
            }

            if (cispar[16] != cispar[17]) {
              // Warning: nonportable.
              union {
                unsigned int i;
                float f;
              } chargeCnv;
              chargeCnv.i = cispar[17];
              cispar[17] = chargeCnv.f;
            }
          }
          break;
          }
        }
      }
    }
    break;

    case LASER_OBJ_FRAG: {

      dqstatus.setRODBCID (coll->getRODBCID());

      // laspar isn't actually used by anything.
      // Leave commented out for now.
#if 0      
      TileBeamElemCollection::const_iterator beamItr = (*collItr)->begin();
      TileBeamElemCollection::const_iterator lastBeam = (*collItr)->end();

      if (beamItr != lastBeam) { // one channel is expected, check that it's really there

        std::vector < uint32_t > digits = (*beamItr)->get_digits();
        int cha = std::min(32, (int) digits.size());

        while (--cha >= 0) {
          m_laspar[cha] = digits[cha];
          ATH_MSG_VERBOSE("laspar [" << cha << "] = " << m_laspar[cha]);
        }
      }
#endif
    }
    break;

    case COIN_TRIG1_FRAG:
    case COIN_TRIG2_FRAG:
    case COIN_TRIG3_FRAG:
    case COIN_TRIG4_FRAG:
    case COIN_TRIG5_FRAG:
    case COIN_TRIG6_FRAG:
    case COIN_TRIG7_FRAG:
    case COIN_TRIG8_FRAG: {
      // coincTrig is not actually used by anything now.
      // Leave commented out.
#if 0
      unsigned int board = frag - COIN_TRIG1_FRAG;
      // make sure that we have enough space
      if (board >= m_coincTrig.size()) {
        m_coincTrig.resize(board + 1);
      }

      TileBeamElemCollection::const_iterator beamItr = (*collItr)->begin();
      TileBeamElemCollection::const_iterator lastBeam = (*collItr)->end();

      // loop over 4 integer words for one board
      for (; beamItr != lastBeam; ++beamItr) {

        HWIdentifier id = (*beamItr)->adc_HWID();
        std::vector < uint32_t > digits = (*beamItr)->get_digits();
        uint32_t amplitude = (digits.size() > 0) ? digits[0] : 0;
        int cha = m_tileHWID->channel(id);

        if (cha < 3) {
          int idx = cha * 32;
          for (int ibit = 0; ibit < 32; ++ibit) {
            m_coincTrig[board].trig[idx++] = (amplitude >> ibit) & 1;
          }
        } else if (cha == 3) {
          m_coincTrig[board].amp = amplitude;
        }
      }
#endif
    }
    break;

    default:
    break;
    }
  }

  return StatusCode::SUCCESS;
}
