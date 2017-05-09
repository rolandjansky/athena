/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileConditions/TileBadChanTool.h"
#include "TileConditions/TileCondProxyWrapper.h"
#include "TileCalibBlobObjs/TileCalibDrawerBch.h"
#include "TileCalibBlobObjs/TileBchDecoder.h"
#include "TileCalibBlobObjs/TileBchPrbs.h"
#include "TileIdentifier/TileHWID.h"
#include "TileDetDescr/TileDetDescrManager.h"

// Calo includes
#include "CaloDetDescr/CaloDetDescrElement.h"

// Athena incldues
#include "Identifier/IdContext.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"

#include <string>

//
//____________________________________________________________________
const InterfaceID&
TileBadChanTool::interfaceID() {
  return ITileBadChanTool::interfaceID();
}

//
//____________________________________________________________________
TileBadChanTool::TileBadChanTool(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
    , m_tileMgr(0)
    , m_channel_context(0)
    , m_tileIdTrans("TileCondIdTransforms")
    , m_pryOnlBch("TileCondProxyFile_TileCalibDrawerBch_/TileCondProxyDefault_onlBch", this)
    , m_pryOflBch("TileCondProxyFile_TileCalibDrawerBch_/TileCondProxyDefault_oflBch", this)
    , m_defaultStatus(TileBchStatus())
    , m_tripsProbs(TileCalibUtils::MAX_ROS - 1, std::vector<float>(TileCalibUtils::MAX_DRAWER, 0))
    , m_useOflBch(true)
    , m_tileHWID(nullptr)
{
  declareInterface<ITileBadChanTool>(this);
  declareInterface<TileBadChanTool>(this);
  declareInterface<ICaloBadChanTool>(this);
  declareProperty("ProxyOnlBch", m_pryOnlBch);
  declareProperty("ProxyOflBch", m_pryOflBch);

}

//
//____________________________________________________________________
TileBadChanTool::~TileBadChanTool() {
  for (std::vector<const TileBchDecoder*>::iterator i = m_tileBchDecoder.begin();
      i != m_tileBchDecoder.end(); ++i) {
    delete *i;
  }
}


//
//____________________________________________________________________
StatusCode TileBadChanTool::initialize() {
  ATH_MSG_DEBUG( "in initialize()" );
  
  //=== TileDetDescrManager
  CHECK( detStore()->retrieve(m_tileMgr) );

  //=== initialize bit pattern decoders for all possible versions
  m_tileBchDecoder.resize(TileBchDecoder::MaxVersion);
  for (unsigned int i = 0; i < TileBchDecoder::MaxVersion; ++i) {
    ATH_MSG_DEBUG( "Creating TileBchDecoder(" << i << ")" );
    m_tileBchDecoder[i] = new TileBchDecoder(static_cast<TileBchDecoder::BitPatVer>(i));
  }

  //=== TileCondIdTransforms
  CHECK( m_tileIdTrans.retrieve() );

  //=== initialize channel context
  m_channel_context = new IdContext(m_tileIdTrans->getTileHWID()->channel_context());

  //=== retrieve online proxy
  CHECK( m_pryOnlBch.retrieve() );

  m_useOflBch = !(m_pryOflBch.empty());
  if (m_useOflBch) {
    //=== retrieve offline proxy
    CHECK( m_pryOflBch.retrieve() );
    ATH_MSG_INFO("ProxyOnlBch and ProxyOflBch will be used for bad channel status");
  } else {
    ATH_MSG_INFO("Only ProxyOnlBch will be used for bad channel status");
  }
  
  //=== recache function needs to be bound to IOV updates only for COOL proxies
  const TileCondProxyCoolBch* ptrPryOnlBch = dynamic_cast<const TileCondProxyCoolBch*>(&(*m_pryOnlBch));
  const TileCondProxyCoolBch* ptrPryOflBch(0);
  if (m_useOflBch) {
    ptrPryOflBch = dynamic_cast<const TileCondProxyCoolBch*>(&(*m_pryOflBch));
  }

  //=== call recache if both proxies are FILE proxies
  if (!ptrPryOnlBch && !ptrPryOflBch) {
    int idx = 0;
    std::list<std::string> keys;
    keys.push_back("ASCII files");
    CHECK( recache(idx, keys) );
  }

  //=== if at least one COOL proxy is loaded, then the linked callback will take care of recaching
  else {
    if (ptrPryOnlBch) {
      CHECK( detStore()->regFcn(&TileCondProxyCoolBch::callback
                               , ptrPryOnlBch
                               , &TileBadChanTool::recache
                               , this, true) );

    }

    if (m_useOflBch && ptrPryOflBch) {
      CHECK( detStore()->regFcn(&TileCondProxyCoolBch::callback
                               , ptrPryOflBch
                               , &TileBadChanTool::recache
                               , this, true) );
    }
  }


  m_roses.resize(TileCalibUtils::MAX_DRAWERIDX);
  m_drawers.resize(TileCalibUtils::MAX_DRAWERIDX);

  for (unsigned int ros = 0; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::getMaxDrawer(ros); ++drawer) {
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
      m_roses[drawerIdx] = ros;
      m_drawers[drawerIdx] = drawer;
    }
  }

  
  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileBadChanTool::finalize() {
  ATH_MSG_DEBUG( "finalize called" );

  if (m_channel_context) delete m_channel_context;
  m_channel_context = 0;

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileBadChanTool::recache(IOVSVC_CALLBACK_ARGS_K(keys)) {
  msg(MSG::INFO) << "TileBadChanTool::recache() has been triggered by: ";
  std::list<std::string>::const_iterator itr;
  for (itr = keys.begin(); itr != keys.end(); ++itr) {
    msg(MSG::INFO) << "\'" << *itr << "' ";
  }
  msg(MSG::INFO) << endmsg;
  
  //=== clear old hash maps
  m_statusMapAdc.clear();
  m_statusMapChn.clear();

  try {

    //=== loop over the whole detector, hash affected ADCs
    uint32_t adcBits(0), channelBits(0);
    
    m_tileHWID = m_tileIdTrans->getTileHWID();

    IdContext adcContext = m_tileHWID->adc_context();
    unsigned int maxAdcHash = m_tileHWID->adc_hash_max();

    unsigned int drawerIdx(0);
    unsigned int channel(0);
    unsigned int adc(0);

    for (IdentifierHash adcHash = 0; adcHash < maxAdcHash; adcHash += 1) {
      HWIdentifier adcId;
      if (m_tileHWID->get_id(adcHash, adcId, &adcContext) == 0) {
        //        ATH_MSG_ALWAYS(m_tileHWID->to_string(adcId));
        if (m_tileHWID->ros(adcId) == 0) continue;

        m_tileIdTrans->getIndices(adcId, drawerIdx, channel, adc);

        //=== online status ...         
        const TileCalibDrawerBch* calibDrawer = m_pryOnlBch->getCalibDrawer(drawerIdx);
        TileBchDecoder::BitPatVer bitPatVer = calibDrawer->getBitPatternVersion();
        calibDrawer->getStatusWords(channel, adc, adcBits, channelBits);
        TileBchStatus adcStatus(m_tileBchDecoder[bitPatVer]->decode(channelBits, adcBits));
        
        if (m_useOflBch) {
          //=== ... add offline status
          calibDrawer = m_pryOflBch->getCalibDrawer(drawerIdx);
          bitPatVer = calibDrawer->getBitPatternVersion();
          calibDrawer->getStatusWords(channel, adc, adcBits, channelBits);
          adcStatus += m_tileBchDecoder[bitPatVer]->decode(channelBits, adcBits);
        }
        
        if (!adcStatus.isGood()) { 
          //=== only add problematic adcs to map
          m_statusMapAdc[adcId] = adcStatus;
          HWIdentifier channelId = m_tileHWID->channel_id(adcId);
          m_statusMapChn[channelId] += adcStatus;
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
    if (m_useOflBch) definitionsCalibDrawer = m_pryOflBch->getCalibDrawer(TileCalibUtils::DEFINITIONS_DRAWERIDX);
    else definitionsCalibDrawer = m_pryOnlBch->getCalibDrawer(TileCalibUtils::DEFINITIONS_DRAWERIDX);

    TileBchDecoder::BitPatVer bitPatVer = definitionsCalibDrawer->getBitPatternVersion();

    //=== TileBchStatus.isBad() definition
    definitionsCalibDrawer->getStatusWords(TileCalibUtils::BAD_DEFINITION_CHAN, 0, adcBits, channelBits);
    TileBchStatus chnStatus(m_tileBchDecoder[bitPatVer]->decode(channelBits, adcBits));
    if (chnStatus.isAffected()) {
      ATH_MSG_INFO( "Updating TileBchStatus::isBad() definition from DB" );
      TileBchStatus::defineBad(chnStatus);
    } else {
      ATH_MSG_INFO( "No TileBchStatus::isBad() definition found in DB, using defaults" );
    }

    //=== TileBchStatus.isNoisy() definition
    definitionsCalibDrawer->getStatusWords(TileCalibUtils::NOISY_DEFINITION_CHAN, 0, adcBits, channelBits);
    chnStatus = m_tileBchDecoder[bitPatVer]->decode(channelBits, adcBits);
    if (chnStatus.isAffected()) {
      ATH_MSG_INFO( "Updating TileBchStatus::isNoisy() definition from DB" );
      TileBchStatus::defineNoisy(chnStatus);
    } else {
      ATH_MSG_INFO( "No TileBchStatus::isNoisy() definition found in DB, using defaults" );
    }

    //=== TileBchStatus.isNoGainL1() definition
    definitionsCalibDrawer->getStatusWords(TileCalibUtils::NOGAINL1_DEFINITION_CHAN, 0, adcBits, channelBits);
    chnStatus = m_tileBchDecoder[bitPatVer]->decode(channelBits, adcBits);
    if (chnStatus.isAffected()) {
      ATH_MSG_INFO( "Updating TileBchStatus::isNoGainL1() definition from DB" );
      TileBchStatus::defineNoGainL1(chnStatus);
    } else {
      ATH_MSG_INFO( "No TileBchStatus::isNoGainL1() definition found in DB, using defaults" );
    }


    //=== TileBchStatus.isBadTiming() definition
    definitionsCalibDrawer->getStatusWords(TileCalibUtils::BADTIMING_DEFINITION_CHAN, 0, adcBits, channelBits);
    chnStatus = m_tileBchDecoder[bitPatVer]->decode(channelBits, adcBits);
    if (chnStatus.isAffected()) {
      ATH_MSG_INFO( "Updating TileBchStatus::isBadTiming() definition from DB" );
      TileBchStatus::defineBadTiming(chnStatus);
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
    

    // Check if drawer trips probabilities for simulation are exist in DB.
    // By special convention trips probabilities are stored in drawer number: 2
    // like integers number plus one denominator per ros
    //--- drawerIdx=2, channel=1: 64 integers + denominator for LBA
    //--- drawerIdx=2, channel=2: 64 integers + denominator for LBC
    //--- drawerIdx=2, channel=3: 64 integers + denominator for EBA
    //--- drawerIdx=2, channel=4: 64 integers + denominator for EBC
    //============================================================
    if (m_useOflBch) {
      const TileCalibDrawerBch* tripsCalibDrawer = m_pryOflBch->getCalibDrawer(TileCalibUtils::TRIPS_DRAWERIDX);

      if (tripsCalibDrawer->getObjSizeUint32() == TileCalibUtils::MAX_DRAWER + 1
	  && tripsCalibDrawer->getNChans() == TileCalibUtils::MAX_ROS) {

	ATH_MSG_INFO("Found drawer trips probabilities in DB");

	// Fill up arrays with trips probabilities for simulation purposes
	// Trips probabilities are cached like floats: number / denominator.

	for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
	  const float denominator = (float) tripsCalibDrawer->getData(ros, 0, TileCalibUtils::MAX_DRAWER);
          const float inv_denominator = 1. / denominator;
	  for (unsigned int mod = 0; mod < TileCalibUtils::MAX_DRAWER; ++mod) {
	    m_tripsProbs[ros - 1][mod] = ((float) tripsCalibDrawer->getData(ros, 0, mod)) * inv_denominator;
	  }
	}

      } else {
	ATH_MSG_INFO("No drawer trips probabilities found in DB");
      }
    }
  } catch (TileCalib::Exception& e) {
    ATH_MSG_ERROR( "Caught exception: " << e.what() );
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
CaloBadChannel TileBadChanTool::caloStatus(Identifier id) const {
  CaloBadChannel::BitWord res = 0;

  //=== get the TileHWIDs of the two channels connected to the caloCell id
  const CaloDetDescrElement* elem = m_tileMgr->get_cell_element(id);

  if (!elem) {
    //=== this should never happen
    ATH_MSG_ERROR( "CaloMgr returns NULL CaloDetDescrElement" );
    std::abort();
  }

  //=== status of 1. connected channel
  TileBchStatus chn1Stat(getChannelStatus(elem->onl1()));

  //=== status of 2. connected channel
  TileBchStatus chn2Stat(getChannelStatus(elem->onl2()));
  
  //=== set cell status depending on channel status
  if (chn1Stat.isBad() && chn2Stat.isBad()) {
    CaloBadChannel::setBit(CaloBadChannel::deadBit, res);
  } else if (chn1Stat.isAffected() || chn2Stat.isAffected()) {
    CaloBadChannel::setBit(CaloBadChannel::affectedBit, res);
  }

  return CaloBadChannel(res);
}

//
//____________________________________________________________________
const TileBchStatus&
TileBadChanTool::getAdcStatus(const HWIdentifier& adc_id) const {

  std::map<HWIdentifier, TileBchStatus>::const_iterator iStatusMap = m_statusMapAdc.find(adc_id);
  if (iStatusMap == m_statusMapAdc.end()) {
    return m_defaultStatus;
  } else {
    return iStatusMap->second;
  }

}

//
//____________________________________________________________________
const TileBchStatus&
TileBadChanTool::getAdcStatus(IdentifierHash hash_id, unsigned int adc) const {
  if (hash_id != TileHWID::NOT_VALID_HASH) {
    HWIdentifier adc_id = m_tileHWID->adc_id(hash_id, adc);
    return getAdcStatus(adc_id);
  } else {
    return m_defaultStatus;
  }
}

//
//____________________________________________________________________
const TileBchStatus&
TileBadChanTool::getChannelStatus(IdentifierHash hash_id) const {
  if (hash_id != TileHWID::NOT_VALID_HASH) {
    HWIdentifier channel_id = m_tileHWID->channel_id(hash_id);
    return getChannelStatus(channel_id);
  } else {
    return m_defaultStatus;
  }
}

//
//____________________________________________________________________
const TileBchStatus&
TileBadChanTool::getChannelStatus(const HWIdentifier& channel_id) const {

  std::map<HWIdentifier, TileBchStatus>::const_iterator iStatusMap = m_statusMapChn.find(channel_id);
  if (iStatusMap == m_statusMapChn.end()) {
    return m_defaultStatus;
  } else {
    return iStatusMap->second;
  }

}

//
//____________________________________________________________________
const TileBchStatus&
TileBadChanTool::getChannelStatus(unsigned int drawerIdx, unsigned int channel) const {

  HWIdentifier channel_id = m_tileHWID->channel_id(m_roses[drawerIdx], m_drawers[drawerIdx], channel);
  return getChannelStatus(channel_id);

}

//
//____________________________________________________________________
const TileBchStatus&
TileBadChanTool::getAdcStatus(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {

  HWIdentifier adc_id =  m_tileHWID->adc_id(m_roses[drawerIdx], m_drawers[drawerIdx], channel, adc);
  return getAdcStatus(adc_id);

}


uint32_t TileBadChanTool::encodeStatus(const TileBchStatus& status) const {
  uint32_t bad;
  
  if (status.isGood())
    bad = 0;
  else if (status.isBad())
    bad = 3;
  else if (status.isNoisy())
    bad = 1;
  else if (status.isAffected())
    bad = 2;
  else
    bad = 4;

  return bad;
}
