/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "TileConditions/TileDCSState.h"
#include "TileConditions/TileCalibData.h"
#include "../src/TileDCSCondAlg.h"
#include "../src/TileDCSTool.h"
//#include "TileIdentifier/TileHWID.h"
#include "TileCablingService_common_test.cxx"

#include "CaloIdentifier/CaloLVL1_ID.h"

#include "TestTools/FLOATassert.h"
#include "TestTools/initGaudi.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"

#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"


#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


static const std::string TILE_JO_NAME("jobOptions_TileDCSTest.txt");
static const std::string TILE_DCS_CH_HV_IN_COOL("TileDCSChHvInCool.dat");
static const std::string TILE_DCS_CH_STATES_IN_COOL("TileDCSChStatesInCool.dat");

static const std::string TILE_DCS_HV("/TILE/TEST/DCS/HV");
static const std::string TILE_DCS_HVSET("/TILE/TEST/DCS/HVSET");
static const std::string TILE_DCS_STATES("/TILE/TEST/DCS/STATES");

static const std::string TILE_TEST_EMS("TileTestEms");
static const std::string TILE_TEST_DCS("TileTestDCS");
static const std::string TILE_TEST_TOOL_DCS("TileTestToolDCS");

static const unsigned int ROS(2);
static const unsigned int MAX_CHANNEL(48);
static const unsigned int DEF_DRAWER_IDX(0);

static const unsigned int GOOD_DRAWER(5);
static const unsigned int WARNING_DRAWER(6);
static const unsigned int BAD_DRAWER(7);

static const std::vector<unsigned int> DRAWERS{
  GOOD_DRAWER, WARNING_DRAWER, BAD_DRAWER
};

static const unsigned int DRAWER_SHIFT_IN_COOL(10);

static const int GOOD_DCS_STATE(111);
static const int WARNING1_DCS_STATE(150);
static const int WARNING2_DCS_STATE(250);
static const int BAD_DCS_STATE(50);

static const std::vector<int> WARNING_DCS_STATE{
  WARNING1_DCS_STATE, WARNING2_DCS_STATE
};


static const unsigned int LASER_REF_CHANNEL(10);
static const unsigned int WARNING_CHANNEL(11);
static const unsigned int BAD_CHANNEL(12);

static const float CES_HV_SHIFT(50.0F);
static const float WARNING_HV_SHIFT(5.0F);
static const float BAD_HV_SHIFT(15.0F);

static const std::vector<TileDCSState::TileDCSStatus> DCS_STATUS{
    TileDCSState::NON_EXISTING,
    TileDCSState::UNKNOWN,
    TileDCSState::OK_DRAWER,
    TileDCSState::OK_KNOWNBADPMT,
    TileDCSState::OK,
    TileDCSState::WARNING_DRAWER,
    TileDCSState::WARNING_KNOWNBADPMT,
    TileDCSState::WARNING,
    TileDCSState::ALERT_DRAWER,
    TileDCSState::ALERT_KNOWNBADPMT,
    TileDCSState::ALERT
};


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

/*
static const EventIDRange ALL_EVENT_RANGE {
  EventIDBase{
    0, // Run number
    EventIDBase::UNDEFEVT,
    EventIDBase::UNDEFNUM,
    EventIDBase::UNDEFNUM,
    0}, // Lumi block

  EventIDBase{
    EventIDBase::UNDEFNUM - 1,  // Run number
    EventIDBase::UNDEFEVT,
    EventIDBase::UNDEFNUM,
    EventIDBase::UNDEFNUM,
    EventIDBase::UNDEFNUM - 1}  // Lumi block
};

*/

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


/*
// Dummy Tile detector descrition manager
class TileDetDescrManager {

};


CLASS_DEF(TileDetDescrManager, 2941, 1)
*/

unsigned int channelKey(unsigned int ros, unsigned int drawer, unsigned int channel) {
  return ( ((((ros - 1) << 6) | drawer) << 6) | channel );
}


float getChannelHV(unsigned int ros, unsigned int drawer, unsigned int channel) {
  return ros * 100.0F + drawer + (channel / 100.0F);
}

float getChannelHVSet(unsigned int ros, unsigned int drawer, unsigned int channel) {
  return ros * 100.0F + drawer + (channel / 100.0F) + 0.005F;
}

int getDrawerStates(unsigned int ros, unsigned int drawer) {
  return ((ros << 6) | drawer);
}

TileDCSState::TileDCSStatus getDCSHVStatus(unsigned int ros,
                                         unsigned int drawer,
                                         unsigned int channel) {

  return DCS_STATUS[channelKey(ros, drawer, channel) % DCS_STATUS.size()];

}


TileDCSState::TileDCSStatus getDCSStatus(unsigned int ros,
                                         unsigned int drawer,
                                         unsigned int channel) {

  return DCS_STATUS[channelKey(ros, drawer, channel + 1) % DCS_STATUS.size()];

}


template<typename T>
using FolderData = std::map<unsigned int, std::vector<std::pair<std::string, T>>>;

template<typename T>
void prepareDCSFolder(const std::string& key, const FolderData<T>& folderData) {

  IOVRange range(EVENT_RANGE);

  SG::WriteCondHandleKey<CondAttrListCollection> folderKey(key);
  assert(folderKey.initialize().isSuccess());

  SG::WriteCondHandle<CondAttrListCollection> folder(folderKey);

  std::unique_ptr<CondAttrListCollection> data = std::make_unique<CondAttrListCollection>(true);

  for (const auto& coolChannelData : folderData) {

    unsigned int coolChannel = coolChannelData.first;
    const auto nameValuePairs = coolChannelData.second;

    coral::AttributeList attributeList;

    for (const std::pair<std::string, T>& nameValuePair : nameValuePairs) {
      std::string name = nameValuePair.first;
      T value = nameValuePair.second;
      attributeList.extend<T>(name);
      attributeList[name].data<T>() = value;
    }

    data->add(coolChannel, attributeList);
  }


  std::cout << "FOLDER [" << EVENT_RANGE << "]: " << std::endl;
  data->dump();

   assert(folder.record(EVENT_RANGE, data.release()).isSuccess());
}



void prepareDCSHVFolder(void) {

  const TileCablingService* cabling = TileCablingService::getInstance();

  FolderData<float> hvFolder;

  for (unsigned int drawer : DRAWERS) {

    std::vector<std::pair<std::string, float>> drawerHV;
    drawerHV.resize(55);

    unsigned int channel = 0;
    for (; channel < TileCalibUtils::MAX_CHAN; ++channel) {
      int pmt = std::abs(cabling->channel2hole(ROS, channel));
      std::string name = (pmt < 10) ? "HVOUT0" : "HVOUT";
      name += std::to_string(pmt);
      drawerHV[pmt - 1].first = name;
      drawerHV[pmt - 1].second = getChannelHV(ROS, drawer, channel);
    }

    for (; channel < drawerHV.size(); ++channel) {
      std::string name = "TEMP";
      name += std::to_string(channel - TileCalibUtils::MAX_CHAN + 1);
      drawerHV[channel].first = name;
      drawerHV[channel].second = channel;
    }

    hvFolder[drawer + DRAWER_SHIFT_IN_COOL] = drawerHV;

  }

  prepareDCSFolder(TILE_DCS_HV, hvFolder);

}


void prepareDCSHVSetFolder(void) {

  const TileCablingService* cabling = TileCablingService::getInstance();

  FolderData<float> hvSetFolder;

  for (unsigned int drawer : DRAWERS) {

    std::vector<std::pair<std::string, float>> drawerHV;
    drawerHV.resize(68);

    unsigned int channel = 0;
    for (; channel < TileCalibUtils::MAX_CHAN; ++channel) {
      int pmt = std::abs(cabling->channel2hole(ROS, channel));
      std::string name = "hvOut";
      name += std::to_string(pmt);
      drawerHV[pmt - 1].first = name;
      drawerHV[pmt - 1].second = getChannelHVSet(ROS, drawer, channel);
    }

    for (int i = 1; i < 5; ++i) {
      std::string name = "hvIn";
      name += std::to_string(i);
      drawerHV[channel].first = name;
      drawerHV[channel].second = 0.0F;
      ++channel;
    }

    for (int i = 1; i < 8; ++i) {
      std::string name = "temp";
      name += std::to_string(i);
      drawerHV[channel].first = name;
      drawerHV[channel].second = i * 5.0F;
      ++channel;
    }

    for (int i = 1; i < 8; ++i) {
      std::string name = "volt";
      name += std::to_string(i);
      drawerHV[channel].first = name;
      drawerHV[channel].second = i * 2.0F;
      ++channel;
    }

    for (int i = 1; i < 3; ++i) {
      std::string name = "vFix";
      name += std::to_string(i);
      drawerHV[channel].first = name;
      drawerHV[channel].second = 1230.0F;
      ++channel;
    }


    hvSetFolder[drawer + DRAWER_SHIFT_IN_COOL] = drawerHV;

  }

  prepareDCSFolder(TILE_DCS_HVSET, hvSetFolder);

}


void prepareDCSStatesFolder(void) {

  FolderData<int32_t> statesFolder;

  std::map<unsigned int, unsigned int> drawerState{
    {GOOD_DRAWER, GOOD_DCS_STATE},
    {WARNING_DRAWER, WARNING1_DCS_STATE},
    {BAD_DRAWER, BAD_DCS_STATE},
  };

  for (unsigned int drawer : DRAWERS) {
    std::vector<std::pair<std::string, int32_t>> drawerStates{
      {"FORDAQ_MBHV", drawerState[drawer]}
    };
    statesFolder[drawer + DRAWER_SHIFT_IN_COOL] = drawerStates;
  }

  prepareDCSFolder(TILE_DCS_STATES, statesFolder);
}


void prepareDrawerInCoolMapping(void) {

  std::map<unsigned int, std::string> computer{
    {1, "ATLTILLV01"},
    {2, "ATLTILLV02"},
    {3, "ATLTILLV00"},
    {4, "ATLTILLV03"},
  };

  std::ofstream dcsChHVInCool(TILE_DCS_CH_HV_IN_COOL);
  for (unsigned int drawer : DRAWERS) {
    unsigned int coolChannel(drawer + DRAWER_SHIFT_IN_COOL);
    std::string drawerInCool = std::to_string(coolChannel - 1) + ": "
      + std::to_string(coolChannel) + " "
      + computer[ROS] + "_Drawer" + std::to_string(drawer + 1);

    dcsChHVInCool << drawerInCool << std::endl;
  }
  dcsChHVInCool.close();


  std::ofstream dcsChStatesInCool(TILE_DCS_CH_STATES_IN_COOL);
  for (unsigned int drawer : DRAWERS) {
    unsigned int coolChannel(drawer + DRAWER_SHIFT_IN_COOL);
    std::string drawerInCool = std::to_string(coolChannel - 1) + ": "
      + std::to_string(coolChannel) + " "
      + computer[ROS] + "_ELMB_LVCAN1_LVPS_" + std::to_string(drawer + 1) + "_states";

    dcsChStatesInCool << drawerInCool << std::endl;
  }

  dcsChStatesInCool.close();

}


std::unique_ptr<TileCalibDataFlt> getCalibOflCes() {

  const unsigned int version(1);
  const unsigned int adc(0);

  std::unique_ptr<TileCalibDataFlt> calibData = std::make_unique<TileCalibDataFlt>();

  std::vector<std::vector<float>> defaultData{{0.0F, 0.0F, 0.0F, 0.0F}};
  std::unique_ptr<coral::Blob> blob = std::make_unique<coral::Blob>(0);
  std::unique_ptr<TileCalibDrawerFlt> tmpCalibDrawer(TileCalibDrawerFlt::getInstance(*blob,
                                                                                     defaultData,
                                                                                     MAX_CHANNEL,
                                                                                     version));
  // Trick to make calib drawer to copy and own blob
  std::unique_ptr<TileCalibDrawerFlt> defaultCalibDrawer = std::make_unique<TileCalibDrawerFlt>(*tmpCalibDrawer);
  calibData->setCalibDrawer(DEF_DRAWER_IDX, defaultCalibDrawer.release());

  std::unique_ptr<TileCalibDrawerFlt> goodCalibDrawer = std::make_unique<TileCalibDrawerFlt>(*tmpCalibDrawer);
  std::unique_ptr<TileCalibDrawerFlt> warningCalibDrawer = std::make_unique<TileCalibDrawerFlt>(*tmpCalibDrawer);
  std::unique_ptr<TileCalibDrawerFlt> badCalibDrawer = std::make_unique<TileCalibDrawerFlt>(*tmpCalibDrawer);

  for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
    float hvShift(0.0F);
    if (channel == WARNING_CHANNEL) hvShift = WARNING_HV_SHIFT;
    if (channel == BAD_CHANNEL) hvShift = BAD_HV_SHIFT;

    goodCalibDrawer->setData(channel, adc, 2, getChannelHVSet(ROS, GOOD_DRAWER, channel) - hvShift);
    warningCalibDrawer->setData(channel, adc, 2, getChannelHVSet(ROS, WARNING_DRAWER, channel) - hvShift);
    badCalibDrawer->setData(channel, adc, 2, getChannelHVSet(ROS, BAD_DRAWER, channel) - hvShift);
  }

  // Put 'bad' value, should be recovered by laser refference
  goodCalibDrawer->setData(LASER_REF_CHANNEL, adc, 2,
                           getChannelHVSet(ROS, GOOD_DRAWER, LASER_REF_CHANNEL) - CES_HV_SHIFT);

  /*
  goodCalibDrawer->setData(WARNING_CHANNEL, adc, 2,
                           getChannelHVSet(ROS, GOOD_DRAWER, WARNING_CHANNEL) - WARNING_HV_SHIFT);

  goodCalibDrawer->setData(BAD_CHANNEL, adc, 2,
                           getChannelHVSet(ROS, GOOD_DRAWER, BAD_CHANNEL) - BAD_HV_SHIFT);

  */

  unsigned int goodDrawerIdx = TileCalibUtils::getDrawerIdx(ROS, GOOD_DRAWER);
  calibData->setCalibDrawer(goodDrawerIdx, goodCalibDrawer.release());

  unsigned int warningDrawerIdx = TileCalibUtils::getDrawerIdx(ROS, WARNING_DRAWER);
  calibData->setCalibDrawer(warningDrawerIdx, warningCalibDrawer.release());

  unsigned int badDrawerIdx = TileCalibUtils::getDrawerIdx(ROS, BAD_DRAWER);
  calibData->setCalibDrawer(badDrawerIdx, badCalibDrawer.release());

  for (unsigned int drawerIdx = 1; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++drawerIdx) {
    if(!calibData->getCalibDrawer(drawerIdx)) {
      unsigned int defaultDrawerIdx = TileCalibUtils::getDefaultDrawerIdx(drawerIdx);
      calibData->setCalibDrawer(drawerIdx, calibData->getCalibDrawer(defaultDrawerIdx));
    }
  }

  return calibData;

}


std::unique_ptr<TileCalibDataFlt> getCalibOflLasLin() {

  const unsigned int version(1);
  const unsigned int adc(0);

  std::unique_ptr<TileCalibDataFlt> calibData = std::make_unique<TileCalibDataFlt>();

  std::vector<std::vector<float>> defaultData{{0.0F, -1.0F}};
  std::unique_ptr<coral::Blob> blob = std::make_unique<coral::Blob>(0);
  std::unique_ptr<TileCalibDrawerFlt> tmpCalibDrawer(TileCalibDrawerFlt::getInstance(*blob,
                                                                                     defaultData,
                                                                                     MAX_CHANNEL,
                                                                                     version));
  // Trick to make calib drawer to copy and own blob
  std::unique_ptr<TileCalibDrawerFlt> defaultCalibDrawer = std::make_unique<TileCalibDrawerFlt>(*tmpCalibDrawer);
  calibData->setCalibDrawer(DEF_DRAWER_IDX, defaultCalibDrawer.release());

  std::unique_ptr<TileCalibDrawerFlt> goodCalibDrawer = std::make_unique<TileCalibDrawerFlt>(*tmpCalibDrawer);
  std::unique_ptr<TileCalibDrawerFlt> warningCalibDrawer = std::make_unique<TileCalibDrawerFlt>(*tmpCalibDrawer);
  std::unique_ptr<TileCalibDrawerFlt> badCalibDrawer = std::make_unique<TileCalibDrawerFlt>(*tmpCalibDrawer);

  goodCalibDrawer->setData(LASER_REF_CHANNEL, adc, 1, getChannelHVSet(ROS, GOOD_DRAWER, LASER_REF_CHANNEL));

  unsigned int goodDrawerIdx = TileCalibUtils::getDrawerIdx(ROS, GOOD_DRAWER);
  calibData->setCalibDrawer(goodDrawerIdx, goodCalibDrawer.release());

  unsigned int warningDrawerIdx = TileCalibUtils::getDrawerIdx(ROS, WARNING_DRAWER);
  calibData->setCalibDrawer(warningDrawerIdx, warningCalibDrawer.release());

  unsigned int badDrawerIdx = TileCalibUtils::getDrawerIdx(ROS, BAD_DRAWER);
  calibData->setCalibDrawer(badDrawerIdx, badCalibDrawer.release());

  for (unsigned int drawerIdx = 1; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++drawerIdx) {
    if(!calibData->getCalibDrawer(drawerIdx)) {
      unsigned int defaultDrawerIdx = TileCalibUtils::getDefaultDrawerIdx(drawerIdx);
      calibData->setCalibDrawer(drawerIdx, calibData->getCalibDrawer(defaultDrawerIdx));
    }
  }

  return calibData;

}


void prepareEms(void) {

  // Initialize TileCalibEms
  std::unique_ptr<TileEMScale> emScale = std::make_unique<TileEMScale>();

  emScale->setCalibOflLasLin(getCalibOflLasLin());
  emScale->setCalibOflCes(getCalibOflCes());

  SG::WriteCondHandleKey<TileEMScale> folderEmsKey{TILE_TEST_EMS};
  assert(folderEmsKey.initialize().isSuccess());

  SG::WriteCondHandle<TileEMScale> folderEms{folderEmsKey};

  assert(folderEms.record(EVENT_RANGE, emScale.release()).isSuccess());

}

std::unique_ptr<TileDCSState> getDCSState() {

  // Initialize TileDscState
  std::unique_ptr<TileDCSState> dcsState = std::make_unique<TileDCSState>();

  dcsState->setGoodDrawer(GOOD_DCS_STATE);
  dcsState->setWarningDrawer(WARNING_DCS_STATE);

  // Initialize TileDCSState
  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {

      dcsState->setDrawerStates(ros, drawer, getDrawerStates(ros, drawer));

      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
        dcsState->setChannelHV(ros, drawer, channel, getChannelHV(ros, drawer, channel));
        dcsState->setChannelHVSet(ros, drawer, channel, getChannelHVSet(ros, drawer, channel));

        dcsState->setDCSStatus(ros, drawer, channel, getDCSStatus(ros, drawer, channel));
        dcsState->setDCSHVStatus(ros, drawer, channel, getDCSHVStatus(ros, drawer, channel));

      }

    }
  }


  return dcsState;
}


template<class T>
void checkDCSState(const T* dcsState) {

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {

      const int referenceDrawerStates(getDrawerStates(ros, drawer));
      assert(dcsState->getDrawerStates(ros, drawer) == referenceDrawerStates);

      TileDCSState::TileDCSStatus referenceDrawerDCSStatus(TileDCSState::ALERT_DRAWER);
      if (referenceDrawerStates == GOOD_DCS_STATE) {
        referenceDrawerDCSStatus = TileDCSState::OK_DRAWER;
      } else if (referenceDrawerStates == WARNING1_DCS_STATE
                 || referenceDrawerStates == WARNING2_DCS_STATE) {
        referenceDrawerDCSStatus = TileDCSState::WARNING_DRAWER;
      }

      assert(dcsState->getDCSStatus(ros, drawer) == referenceDrawerDCSStatus);

      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {

        const float channelHV(getChannelHV(ros, drawer, channel));
        assert(Athena_test::isEqual(dcsState->getChannelHV(ros, drawer, channel), channelHV));

        const float channelHVSet(getChannelHVSet(ros, drawer, channel));
        assert(Athena_test::isEqual(dcsState->getChannelHVSet(ros, drawer, channel), channelHVSet));


        assert(dcsState->getDCSStatus(ros, drawer, channel) == getDCSStatus(ros, drawer, channel));
        assert(dcsState->getDCSHVStatus(ros, drawer, channel) == getDCSHVStatus(ros, drawer, channel));

        const bool referenceDCSStatus(getDCSStatus(ros, drawer, channel) > TileDCSState::WARNING);
        assert(dcsState->isStatusBad(ros, drawer, channel) == referenceDCSStatus);

        const bool referenceDCSHVStatus(getDCSHVStatus(ros, drawer, channel) > TileDCSState::WARNING);
        assert(dcsState->isStatusHVBad(ros, drawer, channel) == referenceDCSHVStatus);


      }
    }
  }

}



void testTileDCSState(void) {

  std::cout << "\nTest TileDCSState\n";


  TileDCSState initialDCSState;

  // Test initial values
  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {

      assert(initialDCSState.getDrawerStates(ros, drawer) < 0);
      assert(initialDCSState.getDCSStatus(ros, drawer) == TileDCSState::UNKNOWN);
      assert(!initialDCSState.isStatusBad(ros, drawer));

      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {

        assert(initialDCSState.getChannelHV(ros, drawer, channel) < 0);
        assert(initialDCSState.getChannelHVSet(ros, drawer, channel) < 0);
        assert(initialDCSState.getDCSStatus(ros, drawer, channel) == TileDCSState::UNKNOWN);

        assert(!initialDCSState.isStatusBad(ros, drawer, channel));
        assert(!initialDCSState.isStatusHVBad(ros, drawer, channel));
      }
    }
  }


  std::unique_ptr<TileDCSState> dcsState = getDCSState();
  checkDCSState(dcsState.get());

}



void testTileDCSCondAlg(ISvcLocator* svcLoc) {

  std::cout << "\nTest TileDCSCondAlg\n";

  prepareDrawerInCoolMapping();

  ServiceHandle<StoreGateSvc> detStore("DetectorStore", "");
  assert(detStore.retrieve().isSuccess());

  std::unique_ptr<CaloLVL1_ID> caloLVL1_ID = std::make_unique<CaloLVL1_ID>();
  assert(detStore->record(caloLVL1_ID.release(), "CaloLVL1_ID").isSuccess());


  //TileDCSCondAlg
  std::unique_ptr<TileDCSCondAlg> alg = std::make_unique<TileDCSCondAlg>("TileDCSCondAlgTest", svcLoc);
  alg->addRef();

  assert( (alg->setProperty("HV", TILE_DCS_HV)).isSuccess() );
  assert( (alg->setProperty("HVSET", TILE_DCS_HVSET)).isSuccess() );
  assert( (alg->setProperty("STATES", TILE_DCS_STATES)).isSuccess() );
  assert( (alg->setProperty("TileEMScale", TILE_TEST_EMS)).isSuccess() );
  assert( (alg->setProperty("TileDCS", TILE_TEST_DCS)).isSuccess() );

  assert( (alg->setProperty("GoodDrawer", GOOD_DCS_STATE)).isSuccess() );
  assert( (alg->setProperty("WarningDrawer", WARNING_DCS_STATE)).isSuccess() );

  assert( (alg->setProperty("DCSChHVInCOOL", TILE_DCS_CH_HV_IN_COOL)).isSuccess() );
  assert( (alg->setProperty("DCSChHVSETInCOOL", TILE_DCS_CH_HV_IN_COOL)).isSuccess() );
  assert( (alg->setProperty("DCSChStatesInCOOL", TILE_DCS_CH_STATES_IN_COOL)).isSuccess() );

  assert( (alg->initialize()).isSuccess() );
  TileCablingSvcMock::set_cabling_type(TileCablingService::RUN2aCabling);

  //assert(alg->setProperty("OutputLevel", 1)); // 1 == VERBOSE

  assert( (alg->initialize()).isSuccess() );

  const TileCablingService* cabling = TileCablingService::getInstance();
  std::cout << "Tile cabling type: " << cabling->getCablingType() << std::endl;

  prepareDCSHVFolder();
  prepareDCSHVSetFolder();
  prepareDCSStatesFolder();
  prepareEms();

  assert( (alg->execute(Gaudi::Hive::currentContext())).isSuccess() );


  SG::ReadCondHandleKey<TileDCSState> dcsStateKey{TILE_TEST_DCS};
  assert( dcsStateKey.initialize().isSuccess() );

  SG::ReadCondHandle<TileDCSState> dcsState{dcsStateKey};
  assert( dcsState.isValid() );

  assert(dcsState->getDrawerStates(ROS, GOOD_DRAWER) == GOOD_DCS_STATE);
  assert(dcsState->getDCSStatus(ROS, GOOD_DRAWER) == TileDCSState::OK_DRAWER);

  assert(dcsState->getDrawerStates(ROS, WARNING_DRAWER) == WARNING1_DCS_STATE);
  assert(dcsState->getDCSStatus(ROS, WARNING_DRAWER) == TileDCSState::WARNING_DRAWER);

  assert(dcsState->getDrawerStates(ROS, BAD_DRAWER) == BAD_DCS_STATE);
  assert(dcsState->getDCSStatus(ROS, BAD_DRAWER) == TileDCSState::ALERT_DRAWER);


  for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {

    float channelHV(getChannelHV(ROS, WARNING_DRAWER, channel));
    assert(Athena_test::isEqual(dcsState->getChannelHV(ROS, WARNING_DRAWER, channel), channelHV));

    channelHV = getChannelHV(ROS, BAD_DRAWER, channel);
    assert(Athena_test::isEqual(dcsState->getChannelHV(ROS, BAD_DRAWER, channel), channelHV));

    channelHV = getChannelHV(ROS, GOOD_DRAWER, channel);
    assert(Athena_test::isEqual(dcsState->getChannelHV(ROS, GOOD_DRAWER, channel), channelHV));


    float hvShift(0.0F);
    if (channel == WARNING_CHANNEL) {
      hvShift = WARNING_HV_SHIFT;
    } else if (channel == BAD_CHANNEL) {
      hvShift = BAD_HV_SHIFT;
    }

    float channelHVSet(getChannelHVSet(ROS, GOOD_DRAWER, channel) - hvShift);
    assert(Athena_test::isEqual(dcsState->getChannelHVSet(ROS, GOOD_DRAWER, channel), channelHVSet));

    channelHVSet = getChannelHVSet(ROS, WARNING_DRAWER, channel) - hvShift;
    assert(Athena_test::isEqual(dcsState->getChannelHVSet(ROS, WARNING_DRAWER, channel), channelHVSet));

    channelHVSet = getChannelHVSet(ROS, BAD_DRAWER, channel) - hvShift;
    assert(Athena_test::isEqual(dcsState->getChannelHVSet(ROS, BAD_DRAWER, channel), channelHVSet));


    if (cabling->channel2hole(ROS, channel) > 0) {
      assert(dcsState->getDCSStatus(ROS, WARNING_DRAWER, channel) == TileDCSState::WARNING_DRAWER);
      assert(dcsState->getDCSStatus(ROS, BAD_DRAWER, channel) == TileDCSState::ALERT_DRAWER);

      if (channel == WARNING_CHANNEL) {
        assert(dcsState->getDCSHVStatus(ROS, GOOD_DRAWER, channel) == TileDCSState::WARNING);
        assert(dcsState->getDCSStatus(ROS, GOOD_DRAWER, channel) == TileDCSState::WARNING);
        assert(!dcsState->isStatusBad(ROS, GOOD_DRAWER, channel));

        assert(dcsState->getDCSHVStatus(ROS, WARNING_DRAWER, channel) == TileDCSState::WARNING);
        assert(dcsState->getDCSHVStatus(ROS, BAD_DRAWER, channel) == TileDCSState::WARNING);

      } else if(channel == BAD_CHANNEL) {
        assert(dcsState->getDCSHVStatus(ROS, GOOD_DRAWER, channel) == TileDCSState::ALERT);
        assert(dcsState->getDCSStatus(ROS, GOOD_DRAWER, channel) == TileDCSState::ALERT);
        assert(dcsState->isStatusBad(ROS, GOOD_DRAWER, channel));

        assert(dcsState->getDCSHVStatus(ROS, WARNING_DRAWER, channel) == TileDCSState::ALERT);
        assert(dcsState->getDCSHVStatus(ROS, BAD_DRAWER, channel) == TileDCSState::ALERT);

      } else {
        assert(dcsState->getDCSHVStatus(ROS, GOOD_DRAWER, channel) == TileDCSState::OK);
        assert(dcsState->getDCSStatus(ROS, GOOD_DRAWER, channel) == TileDCSState::OK);

        assert(dcsState->getDCSHVStatus(ROS, WARNING_DRAWER, channel) == TileDCSState::OK);
        assert(dcsState->getDCSHVStatus(ROS, BAD_DRAWER, channel) == TileDCSState::OK);

        assert(!dcsState->isStatusBad(ROS, GOOD_DRAWER, channel));
      }

      assert(!dcsState->isStatusBad(ROS, WARNING_DRAWER, channel));
      assert(dcsState->isStatusBad(ROS, BAD_DRAWER, channel));

    } else {
      assert(dcsState->getDCSStatus(ROS, WARNING_DRAWER, channel) == TileDCSState::NON_EXISTING);
      assert(dcsState->getDCSStatus(ROS, BAD_DRAWER, channel) == TileDCSState::NON_EXISTING);
      assert(dcsState->getDCSStatus(ROS, GOOD_DRAWER, channel) == TileDCSState::NON_EXISTING);

      assert(!dcsState->isStatusBad(ROS, WARNING_DRAWER, channel));
      assert(!dcsState->isStatusBad(ROS, BAD_DRAWER, channel));
      assert(!dcsState->isStatusBad(ROS, GOOD_DRAWER, channel));

      assert(dcsState->getDCSHVStatus(ROS, WARNING_DRAWER, channel) == TileDCSState::NON_EXISTING);
      assert(dcsState->getDCSHVStatus(ROS, BAD_DRAWER, channel) == TileDCSState::NON_EXISTING);
      assert(dcsState->getDCSHVStatus(ROS, GOOD_DRAWER, channel) == TileDCSState::NON_EXISTING);

    }

    if (channel == BAD_CHANNEL) {
      assert(dcsState->isStatusHVBad(ROS, GOOD_DRAWER, channel));
      assert(dcsState->isStatusHVBad(ROS, WARNING_DRAWER, channel));
      assert(dcsState->isStatusHVBad(ROS, BAD_DRAWER, channel));
    } else {
      assert(!dcsState->isStatusHVBad(ROS, GOOD_DRAWER, channel));
      assert(!dcsState->isStatusHVBad(ROS, WARNING_DRAWER, channel));
      assert(!dcsState->isStatusHVBad(ROS, BAD_DRAWER, channel));
    }

  }

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {

      if (ros == ROS && (drawer == GOOD_DRAWER || drawer == WARNING_DRAWER || drawer == BAD_DRAWER)) {
        continue;
      }

      assert(dcsState->getDrawerStates(ros, drawer) < 0);
      assert(dcsState->getDCSStatus(ros, drawer) == TileDCSState::UNKNOWN);
      assert(!dcsState->isStatusBad(ros, drawer));

      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {

        assert(dcsState->getChannelHV(ros, drawer, channel) < 0);
        assert(dcsState->getChannelHVSet(ros, drawer, channel) == 0);

        bool isChannelUsed(cabling->channel2hole(ros, channel) > 0);

        if ((ros == 3 && drawer == 14) || (ros == 4 && drawer == 17)) {
          if (channel < 4) isChannelUsed = false;
          else if (channel == 18 || channel == 19) isChannelUsed = true;
        }

        if (ros > 2 && !cabling->C10_connected(drawer) && channel == 4) {
          isChannelUsed = false;
        }

        if (isChannelUsed) {
          assert(dcsState->getDCSStatus(ros, drawer, channel) == TileDCSState::UNKNOWN);
        } else {
          assert(dcsState->getDCSStatus(ros, drawer, channel) == TileDCSState::NON_EXISTING);
        }

        assert(!dcsState->isStatusBad(ros, drawer, channel));
        assert(!dcsState->isStatusHVBad(ros, drawer, channel));
      }
    }
  }




}



void testTileDCSTool(ISvcLocator* svcLoc) {

  std::cout << "\nTest TileDCSTool\n";


  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithm", svcLoc);
  alg->addRef();

  std::unique_ptr<TileDCSTool> tool = std::make_unique<TileDCSTool>("TileDCSTool",
                                                                    "TileDCSToolTest",
                                                                    alg.get());
  tool->addRef();


  {

    SG::WriteCondHandleKey<TileDCSState> dcsStateKey{TILE_TEST_TOOL_DCS};
    assert(dcsStateKey.initialize().isSuccess());

    SG::WriteCondHandle<TileDCSState> dcsState{dcsStateKey};
    std::unique_ptr<TileDCSState> dcsData = getDCSState();
    assert(dcsState.record(EVENT_RANGE, dcsData.release()).isSuccess());

  }

  assert(tool->setProperty("TileDCS", TILE_TEST_TOOL_DCS));
  assert(tool->initialize().isSuccess());

  checkDCSState(tool.get());

}



int main() {

  std::ofstream jo(TILE_JO_NAME);
  jo << "ApplicationMgr.ExtSvc += { \"StoreGateSvc/DetectorStore\", \"StoreGateSvc/ConditionStore\" };" << std::endl;
  jo << "GeoModelSvc.SupportedGeometry = 21;" << std::endl;
  jo << "GeoModelSvc.AtlasVersion = \"ATLAS-R2-2016-01-00-01\";" << std::endl;
  jo.close();


  ISvcLocator* svcLoc;
  if (!Athena_test::initGaudi(TILE_JO_NAME, svcLoc)) {
    std::cerr << "ERROR This test can not be run, can not get ServiceLocator" << std::endl;
  }


  IdDictParser parser;
  TileCablingSvcMock::init_idhelpers (parser);

  ServiceHandle<StoreGateSvc> conditionStore("ConditionStore", "");
  assert(conditionStore.retrieve().isSuccess());

  EventIDBase eventId;
  eventId.set_run_number(EventIDBase::UNDEFNUM / 4);
  eventId.set_lumi_block(EventIDBase::UNDEFNUM / 4);

  EventContext ctx;
  ctx.setEventID(eventId);
  ctx.setExtension( Atlas::ExtendedEventContext(&*conditionStore) );
  Gaudi::Hive::setCurrentContext(ctx);


  testTileDCSState();
  testTileDCSCondAlg(svcLoc);
  testTileDCSTool(svcLoc);

  return 0;
}
