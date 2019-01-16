//Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// Tile includes
#include "TileBadChannelsCondAlg.h"
#include "TileConditions/TileCalibData.h"
#include "TileConditions/TileCablingService.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileHWID.h"

// Athena includes
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"


TileBadChannelsCondAlg::TileBadChannelsCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_condSvc("CondSvc", name),
  m_useOflBch(true)
{
}


TileBadChannelsCondAlg::~TileBadChannelsCondAlg() {

  for (const TileBchDecoder* tileBchDecoder : m_tileBchDecoder) {
    delete tileBchDecoder;
  }

}


StatusCode TileBadChannelsCondAlg::initialize() {

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );

  ATH_CHECK( m_badChannelsKey.initialize() );
  // Register write handle
  if (m_condSvc->regHandle(this, m_badChannelsKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_badChannelsKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

    //=== initialize bit pattern decoders for all possible versions
  m_tileBchDecoder.resize(TileBchDecoder::MaxVersion);
  for (unsigned int i = 0; i < TileBchDecoder::MaxVersion; ++i) {
    ATH_MSG_DEBUG( "Creating TileBchDecoder(" << i << ")" );
    m_tileBchDecoder[i] = new TileBchDecoder(static_cast<TileBchDecoder::BitPatVer>(i));
  }


  //=== TileCondIdTransforms
  CHECK( m_tileIdTrans.retrieve() );

  ATH_CHECK( m_onlBchProxy.retrieve() );

  m_useOflBch = !(m_oflBchProxy.empty());
  if (m_useOflBch) {
    //=== retrieve offline proxy
    CHECK( m_oflBchProxy.retrieve() );
    ATH_MSG_INFO("ProxyOnlBch and ProxyOflBch will be used for bad channel status");
  } else {
    m_oflBchProxy.disable();
    ATH_MSG_INFO("Only ProxyOnlBch will be used for bad channel status");
  }


  return StatusCode::SUCCESS;
}


StatusCode TileBadChannelsCondAlg::execute() {

  SG::WriteCondHandle<TileBadChannels> badChannels{m_badChannelsKey};

  if (badChannels.isValid()) {
    ATH_MSG_DEBUG("Found valid TileBadChannels: " << badChannels.key());
    return StatusCode::SUCCESS;
  }

  EventIDRange eventRange;

  // Define validity of the output cond object
  std::unique_ptr<TileCalibData<TileCalibDrawerBch>> onlBchData
    = std::make_unique<TileCalibData<TileCalibDrawerBch>>();

  ATH_CHECK( m_onlBchProxy->fillCalibData(*onlBchData, eventRange) );


  std::unique_ptr<TileCalibData<TileCalibDrawerBch>> oflBchData
    = std::make_unique<TileCalibData<TileCalibDrawerBch>>();

  if (m_useOflBch) {
    EventIDRange oflBchRange;
    ATH_CHECK( m_oflBchProxy->fillCalibData(*oflBchData, oflBchRange) );
    eventRange = EventIDRange::intersect(eventRange, oflBchRange);
  }


  std::unique_ptr<TileBadChannels> badChannelsData = std::make_unique<TileBadChannels>();

  try {

    //=== loop over the whole detector, hash affected ADCs
    uint32_t adcBits(0), channelBits(0);

    const TileHWID* tileHWID = m_tileIdTrans->getTileHWID();

    IdContext adcContext = tileHWID->adc_context();
    unsigned int maxAdcHash = tileHWID->adc_hash_max();

    unsigned int drawerIdx(0);
    unsigned int channel(0);
    unsigned int adc(0);

    for (IdentifierHash adcHash = 0; adcHash < maxAdcHash; adcHash += 1) {
      HWIdentifier adcId;
      if (tileHWID->get_id(adcHash, adcId, &adcContext) == 0) {

        if (tileHWID->ros(adcId) == 0) continue;

        m_tileIdTrans->getIndices(adcId, drawerIdx, channel, adc);

        //=== online status ...
        const TileCalibDrawerBch* calibDrawer = onlBchData->getCalibDrawer(drawerIdx);
        TileBchDecoder::BitPatVer bitPatVer = calibDrawer->getBitPatternVersion();
        calibDrawer->getStatusWords(channel, adc, adcBits, channelBits);
        TileBchStatus adcStatus(m_tileBchDecoder[bitPatVer]->decode(channelBits, adcBits));

        if (m_useOflBch) {
          //=== ... add offline status
          calibDrawer = oflBchData->getCalibDrawer(drawerIdx);
          bitPatVer = calibDrawer->getBitPatternVersion();
          calibDrawer->getStatusWords(channel, adc, adcBits, channelBits);
          adcStatus += m_tileBchDecoder[bitPatVer]->decode(channelBits, adcBits);
        }

        if (!adcStatus.isGood()) {
          //=== only add problematic adcs to map
          HWIdentifier channelId = tileHWID->channel_id(adcId);
          badChannelsData->addAdcStatus(channelId, adcId, adcStatus);
        }
      }
    }


    //============================================================
    //=== Set definition of bad and noisy channel if specified.
    //--- These definitions are stored in drawerIdx=1, since
    //--- drawers 1,...,3 are not used in the default chain.
    //---
    //--- drawerIdx=1, channel=0: definition of bad channel
    //--- drawerIdx=1, channel=1: definition of noisy channel
    //--- drawerIdx=1, channel=2: definition of NoGainLevel1 channel
    //--- drawerIdx=1, channel=3: definition of bad timing channel
    //--- .... (this list could be extended if needed)
    //============================================================
    //=== Reset defintion to hard-coded defaults
    TileBchStatus::initClassifierDefinitions();

    const TileCalibDrawerBch* definitionsCalibDrawer;
    if (m_useOflBch) definitionsCalibDrawer = oflBchData->getCalibDrawer(TileCalibUtils::DEFINITIONS_DRAWERIDX);
    else definitionsCalibDrawer = onlBchData->getCalibDrawer(TileCalibUtils::DEFINITIONS_DRAWERIDX);

    TileBchDecoder::BitPatVer bitPatVer = definitionsCalibDrawer->getBitPatternVersion();

    //=== TileBchStatus.isBad() definition
    definitionsCalibDrawer->getStatusWords(TileCalibUtils::BAD_DEFINITION_CHAN, 0, adcBits, channelBits);
    TileBchStatus channelStatus(m_tileBchDecoder[bitPatVer]->decode(channelBits, adcBits));
    if (channelStatus.isAffected()) {
      ATH_MSG_INFO( "Updating TileBchStatus::isBad() definition from DB" );
      TileBchStatus::defineBad(channelStatus);
    } else {
      ATH_MSG_INFO( "No TileBchStatus::isBad() definition found in DB, using defaults" );
    }

    //=== TileBchStatus.isNoisy() definition
    definitionsCalibDrawer->getStatusWords(TileCalibUtils::NOISY_DEFINITION_CHAN, 0, adcBits, channelBits);
    channelStatus = m_tileBchDecoder[bitPatVer]->decode(channelBits, adcBits);
    if (channelStatus.isAffected()) {
      ATH_MSG_INFO( "Updating TileBchStatus::isNoisy() definition from DB" );
      TileBchStatus::defineNoisy(channelStatus);
    } else {
      ATH_MSG_INFO( "No TileBchStatus::isNoisy() definition found in DB, using defaults" );
    }

    //=== TileBchStatus.isNoGainL1() definition
    definitionsCalibDrawer->getStatusWords(TileCalibUtils::NOGAINL1_DEFINITION_CHAN, 0, adcBits, channelBits);
    channelStatus = m_tileBchDecoder[bitPatVer]->decode(channelBits, adcBits);
    if (channelStatus.isAffected()) {
      ATH_MSG_INFO( "Updating TileBchStatus::isNoGainL1() definition from DB" );
      TileBchStatus::defineNoGainL1(channelStatus);
    } else {
      ATH_MSG_INFO( "No TileBchStatus::isNoGainL1() definition found in DB, using defaults" );
    }


    //=== TileBchStatus.isBadTiming() definition
    definitionsCalibDrawer->getStatusWords(TileCalibUtils::BADTIMING_DEFINITION_CHAN, 0, adcBits, channelBits);
    channelStatus = m_tileBchDecoder[bitPatVer]->decode(channelBits, adcBits);
    if (channelStatus.isAffected()) {
      ATH_MSG_INFO( "Updating TileBchStatus::isBadTiming() definition from DB" );
      TileBchStatus::defineBadTiming(channelStatus);
    } else {
      ATH_MSG_INFO( "No TileBchStatus::isBadTiming() definition found in DB, using defaults" );
    }


    //=== report current definitions
    ATH_MSG_INFO( "TileBchStatus::isBad() is defined by: "
                 << TileBchStatus::getDefinitionBad().getString() );
    ATH_MSG_INFO( "TileBchStatus::isNoisy() is defined by: "
                 << TileBchStatus::getDefinitionNoisy().getString() );
    ATH_MSG_INFO( "TileBchStatus::isNoGainL1() is defined by: "
                 << TileBchStatus::getDefinitionNoGainL1().getString() );
    ATH_MSG_INFO( "TileBchStatus::isBadTiming() is defined by: "
                 << TileBchStatus::getDefinitionBadTiming().getString() );


    // Find Tile drawers masked completely
    std::vector<int> maskedDrawers;
    TileCablingService* cabling = TileCablingService::getInstance();
    unsigned int maxChannels = cabling->getMaxChannels();

    for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
      for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
        unsigned int channel = 0;
        for ( ; channel < maxChannels; ++channel) {
          int index(-1);
          int pmt(-1);
          HWIdentifier channelID = tileHWID->channel_id(ros, drawer, channel);
          cabling->h2s_cell_id_index(channelID, index, pmt);
          if (index >= 0 && !badChannelsData->getChannelStatus(channelID).isBad()) { // good normal cell
            break;
          }
        }
        if (channel == maxChannels) maskedDrawers.push_back(tileHWID->frag(ros, drawer));
      }
    }


    if (msgLvl(MSG::DEBUG)) {
      if (!maskedDrawers.empty()) {
        msg(MSG::DEBUG) << "List of fully masked drawers: " << MSG::hex;
        for(int maskedDrawer : maskedDrawers) {
          msg(MSG::DEBUG) << " 0x" << maskedDrawer;
        }
        msg(MSG::DEBUG) << MSG::dec << endmsg;
      } else {
        msg(MSG::DEBUG) << "No bad drawers found" << endmsg;
      }
    }

    if (!maskedDrawers.empty()) badChannelsData->setMaskedDrawers(std::move(maskedDrawers));

    // Check if drawer trips probabilities for simulation are exist in DB.
    // By special convention trips probabilities are stored in drawer number: 2
    // like integers number plus one denominator per ros
    //--- drawerIdx=2, channel=1: 64 integers + denominator for LBA
    //--- drawerIdx=2, channel=2: 64 integers + denominator for LBC
    //--- drawerIdx=2, channel=3: 64 integers + denominator for EBA
    //--- drawerIdx=2, channel=4: 64 integers + denominator for EBC
    //============================================================
    if (m_useOflBch) {
      const TileCalibDrawerBch* tripsCalibDrawer = oflBchData->getCalibDrawer(TileCalibUtils::TRIPS_DRAWERIDX);

      if (tripsCalibDrawer->getObjSizeUint32() == TileCalibUtils::MAX_DRAWER + 1
	  && tripsCalibDrawer->getNChans() == TileCalibUtils::MAX_ROS) {


	ATH_MSG_INFO("Found drawer trips probabilities in DB");

        std::vector<std::vector<float>> tripsProbs(TileCalibUtils::MAX_ROS - 1, std::vector<float>(TileCalibUtils::MAX_DRAWER, 0));

	// Fill up arrays with trips probabilities for simulation purposes
	// Trips probabilities are cached like floats: number / denominator.

	for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
	  const float denominator = (float) tripsCalibDrawer->getData(ros, 0, TileCalibUtils::MAX_DRAWER);
          const float inv_denominator = 1. / denominator;
	  for (unsigned int mod = 0; mod < TileCalibUtils::MAX_DRAWER; ++mod) {
	    tripsProbs[ros - 1][mod] = ((float) tripsCalibDrawer->getData(ros, 0, mod)) * inv_denominator;
	  }
	}

        badChannelsData->setTripsProbabilities(std::move(tripsProbs));

      } else {
	ATH_MSG_INFO("No drawer trips probabilities found in DB");
      }
    }
  } catch (TileCalib::Exception& e) {
    ATH_MSG_ERROR( "Caught exception: " << e.what() );
    return StatusCode::FAILURE;
  }




  if(badChannels.record(eventRange, badChannelsData.release()).isFailure()) {
    ATH_MSG_ERROR("Could not record TileBadChannels object with "
                  << badChannels.key()
                  << " with EventRange " << eventRange
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  } else {

    ATH_MSG_VERBOSE("Recorded TileBadChannels object with "
                   << badChannels.key()
                   << " with EventRange " << eventRange
                   << " into Conditions Store");
  }



  return StatusCode::SUCCESS;

}
