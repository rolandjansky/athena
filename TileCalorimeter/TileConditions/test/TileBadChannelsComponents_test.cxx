/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "TileConditions/TileBadChannels.h"
#include "TileConditions/TileBadChanTool.h"
#include "TileConditions/TileCalibData.h"
#include "TileConditions/ITileCondProxy.h"
#include "TileCalibBlobObjs/TileBchStatus.h"
#include "TileCalibBlobObjs/TileCalibDrawerBch.h"
#include "TileCalibBlobObjs/TileBchDecoder.h"
#include "TileCalibBlobObjs/TileBchPrbs.h"
#include "../src/TileBadChannelsCondAlg.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCablingService_common_test.cxx"

#include "TestTools/FLOATassert.h"
#include "TestTools/initGaudi.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


static const std::string TILE_JO_NAME("jobOptions_TileBadChannelsTest.py");
static const std::string TILE_ONL_BAD_CHANNELS("TileOnlineBadChannelsTest");
static const std::string TILE_OFL_BAD_CHANNELS("TileOflineBadChannelsTest");
static const std::string TILE_BAD_CHANNELS("TileBadChannelsTest");
static const unsigned int DEF_DRAWER_IDX(0);
static const unsigned int N_CHANNELS(48);
static const unsigned int ROS(2);
static const unsigned int DRAWER(3);
static const unsigned int CHANNEL(4);
static const unsigned int ONLINE_ADC(0);
static const unsigned int OFLINE_ADC(1);

static const TileBchStatus::PrbSet ONLINE_ADC_PRBSET{TileBchPrbs::OnlineGeneralMaskAdc};
static const TileBchStatus::PrbSet OFLINE_ADC_PRBSET{TileBchPrbs::NoData};

static const TileBchStatus::PrbSet ONLINE_CHANNEL_PRBSET{TileBchPrbs::IgnoredInHlt};
static const TileBchStatus::PrbSet OFLINE_CHANNEL_PRBSET{TileBchPrbs::GeneralMaskChannel};

static const EventIDRange EVENT_RANGE {
  EventIDBase{
    EventIDBase::UNDEFNUM / 8, // Run number
    EventIDBase::UNDEFEVT,
    EventIDBase::UNDEFNUM,
    EventIDBase::UNDEFNUM,
    EventIDBase::UNDEFNUM / 8}, // Lumi block

  EventIDBase{
    EventIDBase::UNDEFNUM / 2,  // Run number
    EventIDBase::UNDEFEVT,
    EventIDBase::UNDEFNUM,
    EventIDBase::UNDEFNUM,
    EventIDBase::UNDEFNUM / 2}  // Lumi block
};



/** Class provides dummy algorithm
 *  to help construct Tool to be tested.
 */
class DummyAlgorithm: public AthAlgorithm {
  public:
    DummyAlgorithm(const std::string& name, ISvcLocator* svcloc)
      : AthAlgorithm(name, svcloc) {

    }

    virtual StatusCode execute() override {
      return StatusCode::SUCCESS;
    }
};



// Dummy Tile detector descrition manager
class TileDetDescrManager {

};

CLASS_DEF(TileDetDescrManager, 2941, 1)


TileBchStatus getOnlineAdcStatus(void) {
  TileBchStatus onlineAdcStatus(ONLINE_ADC_PRBSET);
  onlineAdcStatus += ONLINE_CHANNEL_PRBSET;
  onlineAdcStatus += OFLINE_CHANNEL_PRBSET;
  return onlineAdcStatus;
}

TileBchStatus getOflineAdcStatus(void) {
  TileBchStatus oflineAdcStatus(OFLINE_ADC_PRBSET);
  oflineAdcStatus += ONLINE_CHANNEL_PRBSET;
  oflineAdcStatus += OFLINE_CHANNEL_PRBSET;
  return oflineAdcStatus;
}

TileBchStatus getChannelStatus(void) {
  TileBchStatus channelStatus(getOnlineAdcStatus());
  channelStatus += getOflineAdcStatus();
  return channelStatus;
}


template<typename T = TileCalibDrawerBch,
         TileBchDecoder::BitPatVer VERSION = TileBchDecoder::BitPat_onl01>
class TileCondProxyMock: public AthAlgTool, virtual public ITileCondProxy<T> {
  public:

    TileCondProxyMock(const std::string& type, const std::string& name, const IInterface* parent)
      :AthAlgTool(type, name, parent)
    {
        declareInterface<ITileCondProxy<T> >(this);
    };

    virtual ~TileCondProxyMock() {};

    virtual StatusCode fillCalibData(TileCalibData<T>& calibData, EventIDRange& eventRange) const override {

      TileBchDecoder bchDecoder(VERSION);

      //=== prepare vector with defaults (no bad channels)
      TileCalibDrawerBch::DefType defaultVec{{0}, {0}, {0}};

      //=== create the calibDrawer
      std::unique_ptr<coral::Blob> blob = std::make_unique<coral::Blob>(0);
      std::unique_ptr<TileCalibDrawerBch> tmpCalibDrawer(TileCalibDrawerBch::getInstance(*blob,
                                                                                         defaultVec,
                                                                                         N_CHANNELS,
                                                                                         VERSION));
      // Trick to make calib drawer to copy and own blob
      std::unique_ptr<TileCalibDrawerBch> bchCalibDrawer = std::make_unique<TileCalibDrawerBch>(*tmpCalibDrawer);

      std::unique_ptr<TileCalibDrawerBch> defaultCalibDrawer = std::make_unique<TileCalibDrawerBch>(*bchCalibDrawer);
      calibData.setCalibDrawer(DEF_DRAWER_IDX, defaultCalibDrawer.release());

      unsigned int adc(ONLINE_ADC);
      TileBchStatus::PrbSet adcPrbSet(ONLINE_ADC_PRBSET);
      TileBchStatus::PrbSet channelPrbSet(ONLINE_CHANNEL_PRBSET);

      if (VERSION == TileBchDecoder::BitPat_ofl01) {
        adc = OFLINE_ADC;
        adcPrbSet = OFLINE_ADC_PRBSET;
        channelPrbSet = OFLINE_CHANNEL_PRBSET;
      }

      TileBchStatus bchStatus(adcPrbSet);
      bchStatus += channelPrbSet;

      TileBchWords bchWords = bchDecoder.encode(bchStatus);
      bchCalibDrawer->setData(CHANNEL, adc, 0, bchWords[1]); // Set ADC status
      bchCalibDrawer->setData(CHANNEL, 2, 0, bchWords[0]); // Set channel status
      unsigned int bchDrawerIdx = TileCalibUtils::getDrawerIdx(ROS, DRAWER) ;
      calibData.setCalibDrawer(bchDrawerIdx, bchCalibDrawer.release());

      for (unsigned int drawerIdx = 1; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++drawerIdx) {
        if (calibData.getCalibDrawer(drawerIdx) == nullptr) {
          unsigned int defaultDrawerIdx = TileCalibUtils::getDefaultDrawerIdx(drawerIdx);
          calibData.setCalibDrawer(drawerIdx, calibData.getCalibDrawer(defaultDrawerIdx));
        }
      }


      eventRange = EVENT_RANGE;

      return StatusCode::SUCCESS;
    };

    StatusCode initialize() {
      return StatusCode::SUCCESS;
    };

    StatusCode finalize() {
      return StatusCode::SUCCESS;
    };

  const T* getCalibDrawer(unsigned int /*drawerIdx*/ ) const { return nullptr;};

};



typedef TileCondProxyMock<TileCalibDrawerBch, TileBchDecoder::BitPat_onl01> TileCondProxyOnlBchMock;
DECLARE_COMPONENT_WITH_ID( TileCondProxyOnlBchMock, "TileCondProxyOnlBchMock" )

typedef TileCondProxyMock<TileCalibDrawerBch, TileBchDecoder::BitPat_ofl01> TileCondProxyOflBchMock;
DECLARE_COMPONENT_WITH_ID( TileCondProxyOflBchMock,  "TileCondProxyOflBchMock")



void testTileBadChannels(void) {

  std::cout << "\nTest TileBadChannels\n";

  const TileHWID* tileHWID = TileCablingService::getInstance()->getTileHWID();


  HWIdentifier adcLowGainId = tileHWID->adc_id(1, 2, 3, 0);
  TileBchStatus::PrbSet adcLowGainPrbSet{TileBchPrbs::GeneralMaskAdc};
  const TileBchStatus adcLowGainStatus(adcLowGainPrbSet);

  HWIdentifier adcHighGainId = tileHWID->adc_id(1, 2, 3, 1);
  TileBchStatus::PrbSet adcHighGainPrbSet{TileBchPrbs::NoData};
  const TileBchStatus adcHighGainStatus(adcHighGainPrbSet);

  HWIdentifier channelId = tileHWID->channel_id(1, 2, 3);
  TileBchStatus::PrbSet channelPrbSet{TileBchPrbs::GeneralMaskChannel};
  const TileBchStatus channelStatus(channelPrbSet);



  TileBadChannels badChannels;
  const TileBchStatus goodStatus;

  assert(goodStatus != adcLowGainStatus);
  assert(goodStatus != adcHighGainStatus);
  assert(goodStatus != channelStatus);

  // All status are good by by default
  assert(badChannels.getAdcStatus(adcLowGainId) == goodStatus);
  assert(badChannels.getAdcStatus(adcHighGainId) == goodStatus);
  assert(badChannels.getChannelStatus(channelId) == goodStatus);

  badChannels.addAdcStatus(channelId, adcLowGainId, adcLowGainStatus);
  assert(badChannels.getAdcStatus(adcLowGainId) == adcLowGainStatus);
  assert(badChannels.getAdcStatus(adcHighGainId) == goodStatus);
  assert(badChannels.getChannelStatus(channelId) == adcLowGainStatus);

  TileBchStatus adcLowAndHighGainStatus(adcLowGainStatus);
  adcLowAndHighGainStatus += adcHighGainStatus;
  badChannels.addAdcStatus(channelId, adcHighGainId, adcHighGainStatus);
  assert(badChannels.getAdcStatus(adcLowGainId) == adcLowGainStatus);
  assert(badChannels.getAdcStatus(adcHighGainId) == adcHighGainStatus);
  assert(badChannels.getChannelStatus(channelId) == adcLowAndHighGainStatus);

  TileBchStatus finalChannelStatus(channelStatus);
  finalChannelStatus += adcLowAndHighGainStatus;

  TileBchStatus adcLowGainAndChannelStatus(channelStatus);
  adcLowGainAndChannelStatus += adcLowGainStatus;

  badChannels.addAdcStatus(channelId, adcLowGainId, channelStatus);
  assert(badChannels.getAdcStatus(adcLowGainId) == adcLowGainAndChannelStatus);
  assert(badChannels.getAdcStatus(adcHighGainId) == adcHighGainStatus);
  assert(badChannels.getChannelStatus(channelId) == finalChannelStatus);

  TileBchStatus adcHighGainAndChannelStatus(channelStatus);
  adcHighGainAndChannelStatus += adcHighGainStatus;

  badChannels.addAdcStatus(channelId, adcHighGainId, channelStatus);
  assert(badChannels.getAdcStatus(adcLowGainId) == adcLowGainAndChannelStatus);
  assert(badChannels.getAdcStatus(adcHighGainId) == adcHighGainAndChannelStatus);
  assert(badChannels.getChannelStatus(channelId) == finalChannelStatus);

  std::vector<std::vector<float>> noTrips = badChannels.getTripsProbabilities();
  assert(noTrips.size() == 0);

  std::vector<std::vector<float>> trips{{1.0F, 2.0F}};
  const std::vector<std::vector<float>> refTrips(trips);

  badChannels.setTripsProbabilities(trips);
  std::vector<std::vector<float>> tripsProbs = badChannels.getTripsProbabilities();

  assert(tripsProbs == refTrips);
}


void testOnlineTileBadChannelsCondAlg(ISvcLocator* svcLoc) {

  std::cout << "\nTest online TileBadChannelsCondAlg\n";

  //TileBadChannelsCondAlg
  std::unique_ptr<TileBadChannelsCondAlg> alg = std::make_unique<TileBadChannelsCondAlg>("TileOnlBadChannelsCondAlgTest", svcLoc);
  alg->addRef();

  assert( (alg->setProperty("OnlBchProxy", "TileCondProxyOnlBchMock")).isSuccess() );
  assert( (alg->setProperty("TileBadChannels", TILE_ONL_BAD_CHANNELS)).isSuccess() );
  assert( (alg->initialize()).isSuccess() );

  assert( (alg->execute()).isSuccess() );


  SG::ReadCondHandleKey<TileBadChannels> badChannelsKey{TILE_ONL_BAD_CHANNELS};
  assert(badChannelsKey.initialize().isSuccess());


  SG::ReadCondHandle<TileBadChannels> badChannels{badChannelsKey};
  assert(badChannels.isValid());

  EventIDRange eventRange;
  badChannels.range(eventRange);

  assert( eventRange == EVENT_RANGE);

  const TileHWID* tileHWID = TileCablingService::getInstance()->getTileHWID();

  HWIdentifier adcId = tileHWID->adc_id(1, 2, 3, 0);
  HWIdentifier channelId = tileHWID->channel_id(1, 2, 3);
  // All status are good by by default
  assert(badChannels->getAdcStatus(adcId).isGood());
  assert(badChannels->getAdcStatus(adcId).isGood());
  assert(badChannels->getChannelStatus(channelId).isGood());

  std::vector<std::vector<float>> noTrips = badChannels->getTripsProbabilities();
  assert(noTrips.size() == 0);

  HWIdentifier badOnlineAdcId = tileHWID->adc_id(ROS, DRAWER, CHANNEL, ONLINE_ADC);
  HWIdentifier badOflineAdcId = tileHWID->adc_id(ROS, DRAWER, CHANNEL, OFLINE_ADC);
  HWIdentifier badChannelId = tileHWID->channel_id(ROS, DRAWER, CHANNEL);

  std::cout << "ADC STATUS[" << tileHWID->to_string(badOnlineAdcId)
            << "]: " << badChannels->getAdcStatus(badOnlineAdcId).getString() << std::endl;
  std::cout << "ADC STATUS[" << tileHWID->to_string(badOflineAdcId)
            << "]: " << badChannels->getAdcStatus(badOflineAdcId).getString() << std::endl;
  std::cout << "CHANNEL STATUS[" << tileHWID->to_string(badChannelId)
            << "]: "  << badChannels->getChannelStatus(badChannelId).getString() << std::endl;


  TileBchStatus onlineAdcStatus(ONLINE_ADC_PRBSET);
  onlineAdcStatus += ONLINE_CHANNEL_PRBSET;

  TileBchStatus oflineAdcStatus(ONLINE_CHANNEL_PRBSET);

  TileBchStatus channelStatus(onlineAdcStatus);

  assert(badChannels->getAdcStatus(badOnlineAdcId) == onlineAdcStatus);
  assert(badChannels->getAdcStatus(badOflineAdcId) == oflineAdcStatus);
  assert(badChannels->getChannelStatus(badChannelId) == channelStatus);

}


void testOflineTileBadChannelsCondAlg(ISvcLocator* svcLoc) {

 std::cout << "\nTest ofline TileBadChannelsCondAlg\n";

  //TileBadChannelsCondAlg
  std::unique_ptr<TileBadChannelsCondAlg> alg = std::make_unique<TileBadChannelsCondAlg>("TileOflBadChannelsCondAlgTest", svcLoc);
  alg->addRef();

  assert( (alg->setProperty("OnlBchProxy", "TileCondProxyOnlBchMock")).isSuccess() );
  assert( (alg->setProperty("OflBchProxy", "TileCondProxyOflBchMock")).isSuccess() );
  assert( (alg->setProperty("TileBadChannels", TILE_OFL_BAD_CHANNELS)).isSuccess() );
  assert( (alg->initialize()).isSuccess() );

  assert( (alg->execute()).isSuccess() );


  SG::ReadCondHandleKey<TileBadChannels> badChannelsKey{TILE_OFL_BAD_CHANNELS};
  assert(badChannelsKey.initialize().isSuccess());


  SG::ReadCondHandle<TileBadChannels> badChannels{badChannelsKey};
  assert(badChannels.isValid());

  EventIDRange eventRange;
  badChannels.range(eventRange);

  assert( eventRange == EVENT_RANGE);

  const TileHWID* tileHWID = TileCablingService::getInstance()->getTileHWID();

  HWIdentifier lowGainAdcId = tileHWID->adc_id(1, 2, 3, 0);
  HWIdentifier highGainAdcId = tileHWID->adc_id(1, 2, 3, 1);
  HWIdentifier channelId = tileHWID->channel_id(1, 2, 3);
  // All status are good by by default
  assert(badChannels->getAdcStatus(lowGainAdcId).isGood());
  assert(badChannels->getAdcStatus(highGainAdcId).isGood());
  assert(badChannels->getChannelStatus(channelId).isGood());

  std::vector<std::vector<float>> noTrips = badChannels->getTripsProbabilities();
  assert(noTrips.size() == 0);

  HWIdentifier badOnlineAdcId = tileHWID->adc_id(ROS, DRAWER, CHANNEL, ONLINE_ADC);
  HWIdentifier badOflineAdcId = tileHWID->adc_id(ROS, DRAWER, CHANNEL, OFLINE_ADC);
  HWIdentifier badChannelId = tileHWID->channel_id(ROS, DRAWER, CHANNEL);

  std::cout << "ADC STATUS[" << tileHWID->to_string(badOnlineAdcId)
            << "]: " << badChannels->getAdcStatus(badOnlineAdcId).getString() << std::endl;
  std::cout << "ADC STATUS[" << tileHWID->to_string(badOflineAdcId)
            << "]: " << badChannels->getAdcStatus(badOflineAdcId).getString() << std::endl;
  std::cout << "CHANNEL STATUS[" << tileHWID->to_string(badChannelId)
            << "]: "  << badChannels->getChannelStatus(badChannelId).getString() << std::endl;

  TileBchStatus onlineAdcStatus(getOnlineAdcStatus());
  TileBchStatus oflineAdcStatus(getOflineAdcStatus());

  TileBchStatus channelStatus(getChannelStatus());

  assert(badChannels->getAdcStatus(badOnlineAdcId) == onlineAdcStatus);
  assert(badChannels->getAdcStatus(badOflineAdcId) == oflineAdcStatus);
  assert(badChannels->getChannelStatus(badChannelId) == channelStatus);

}



void testTileBadChanTool(ISvcLocator* svcLoc) {

  std::cout << "\nTest TileBadChanTool\n";

  TileCablingService* cabling = TileCablingService::getInstance();
  const TileHWID* tileHWID = cabling->getTileHWID();

  {
    SG::WriteCondHandleKey<TileBadChannels> badChannelsKey{TILE_BAD_CHANNELS};
    assert(badChannelsKey.initialize().isSuccess());

    SG::WriteCondHandle<TileBadChannels> badChannels{badChannelsKey};

    std::unique_ptr<TileBadChannels> badChannelsData = std::make_unique<TileBadChannels>();

    TileBchStatus onlineAdcStatus(getOnlineAdcStatus());
    TileBchStatus oflineAdcStatus(getOflineAdcStatus());

    TileBchStatus channelStatus(getChannelStatus());

    HWIdentifier badOnlineAdcId = tileHWID->adc_id(ROS, DRAWER, CHANNEL, ONLINE_ADC);
    HWIdentifier badOflineAdcId = tileHWID->adc_id(ROS, DRAWER, CHANNEL, OFLINE_ADC);
    HWIdentifier badChannelId = tileHWID->channel_id(ROS, DRAWER, CHANNEL);

    badChannelsData->addAdcStatus(badChannelId, badOnlineAdcId, onlineAdcStatus);
    badChannelsData->addAdcStatus(badChannelId, badOflineAdcId, oflineAdcStatus);

    assert(badChannels.record(EVENT_RANGE, badChannelsData.release()).isSuccess());

  }


  ServiceHandle<StoreGateSvc> detStore("DetectorStore", "");
  assert(detStore.retrieve().isSuccess());


  std::unique_ptr<TileDetDescrManager> tileMgr = std::make_unique<TileDetDescrManager>();
  assert(detStore->record(tileMgr.release(), "TileMgr").isSuccess());


  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithm", svcLoc);
  alg->addRef();

  std::unique_ptr<TileBadChanTool> tool = std::make_unique<TileBadChanTool>("TileBadChanTool",
                                                                            "TileBadChanToolTest",
                                                                            alg.get());
  tool->addRef();

  assert(tool->setProperty("TileBadChannels", TILE_BAD_CHANNELS));
  assert(tool->initialize().isSuccess());


  HWIdentifier lowGainAdcId = tileHWID->adc_id(1, 2, 3, 0);
  HWIdentifier highGainAdcId = tileHWID->adc_id(1, 2, 3, 1);
  HWIdentifier channelId = tileHWID->channel_id(1, 2, 3);

  // All status are good by by default
  assert(tool->getAdcStatus(lowGainAdcId).isGood());
  assert(tool->getAdcStatus(highGainAdcId).isGood());
  assert(tool->getChannelStatus(channelId).isGood());


  HWIdentifier badOnlineAdcId = tileHWID->adc_id(ROS, DRAWER, CHANNEL, ONLINE_ADC);
  HWIdentifier badOflineAdcId = tileHWID->adc_id(ROS, DRAWER, CHANNEL, OFLINE_ADC);
  HWIdentifier badChannelId = tileHWID->channel_id(ROS, DRAWER, CHANNEL);


  std::cout << "ADC STATUS[" << tileHWID->to_string(badOnlineAdcId)
            << "]: " << tool->getAdcStatus(badOnlineAdcId).getString() << std::endl;
  std::cout << "ADC STATUS[" << tileHWID->to_string(badOflineAdcId)
            << "]: " << tool->getAdcStatus(badOflineAdcId).getString() << std::endl;
  std::cout << "CHANNEL STATUS[" << tileHWID->to_string(badChannelId)
            << "]: "  << tool->getChannelStatus(badChannelId).getString() << std::endl;


  TileBchStatus onlineAdcStatus(getOnlineAdcStatus());
  TileBchStatus oflineAdcStatus(getOflineAdcStatus());

  TileBchStatus channelStatus(getChannelStatus());

  unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ROS, DRAWER);
  IdentifierHash badChannelHash = tileHWID->get_channel_hash(badChannelId);

  assert(tool->getAdcStatus(badOnlineAdcId) == onlineAdcStatus);
  assert(tool->getAdcStatus(drawerIdx, CHANNEL, ONLINE_ADC) == onlineAdcStatus);
  assert(tool->getAdcStatus(badChannelHash, ONLINE_ADC) == onlineAdcStatus);

  assert(tool->getAdcStatus(badOflineAdcId) == oflineAdcStatus);
  assert(tool->getAdcStatus(drawerIdx, CHANNEL, OFLINE_ADC) == oflineAdcStatus);
  assert(tool->getAdcStatus(badChannelHash, OFLINE_ADC) == oflineAdcStatus);

  assert(tool->getChannelStatus(badChannelId) == channelStatus);
  assert(tool->getChannelStatus(drawerIdx, CHANNEL) == channelStatus);
  assert(tool->getChannelStatus(badChannelHash) == channelStatus);


}


int main() {

  std::ofstream jo(TILE_JO_NAME);
  jo << "ApplicationMgr.ExtSvc += { \"StoreGateSvc/DetectorStore\", \"StoreGateSvc/ConditionStore\" };" << std::endl;
  jo.close();

  ISvcLocator* svcLoc;
  if (!Athena_test::initGaudi(TILE_JO_NAME, svcLoc)) {
    std::cerr << "ERROR This test can not be run, can not get ServiceLocator" << std::endl;
  }


  IdDictParser parser;
  TileCablingSvc::init_idhelpers (parser);


  ServiceHandle<StoreGateSvc> conditionStore("ConditionStore", "");
  assert(conditionStore.retrieve().isSuccess());

  EventIDBase eventId;
  eventId.set_run_number(EventIDBase::UNDEFNUM / 4);
  eventId.set_lumi_block(EventIDBase::UNDEFNUM / 4);

  EventContext ctx;
  ctx.setEventID(eventId);
  ctx.setExtension( Atlas::ExtendedEventContext(&*conditionStore) );
  Gaudi::Hive::setCurrentContext(ctx);


  testTileBadChannels();
  testOnlineTileBadChannelsCondAlg(svcLoc);
  testOflineTileBadChannelsCondAlg(svcLoc);
  testTileBadChanTool(svcLoc);

  return 0;
}
