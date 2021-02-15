/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

// Tile includes
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileCondToolAutoCr.h"
#include "TileConditions/TileCondToolPulseShape.h"
#include "TileConditions/TileCondToolDspThreshold.h"
#include "TileConditions/TileCondToolIntegrator.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileConditions/TileCalibData.h"

#include "TestTools/FLOATassert.h"
#include "TestTools/initGaudi.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/SGtests.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadCondHandle.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/EventIDRange.h"

#include "CoralBase/Blob.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

static const std::string TILE_JO_NAME("jobOptions_TileCondToolsTest.py");
static const std::string TILE_TEST_CONDITION("TileTest");
static const std::string TILE_PULSE_TEST_CONDITION("TilePulseTest");
static const unsigned int OBJ_VERSION(0);
static const unsigned int PULSE_OBJ_VERSION(200);
static const unsigned int DEF_DRAWER_IDX(0);
static const unsigned int DRAWER_IDX(100);
static const unsigned int MAX_CHANNEL(64);
static const unsigned int MAX_GAIN(3);

static const std::vector<float> DEF_CONDITIONS{1.0F, 2.0F, 3.0F, 4.0F, 5.0F, 6.0F, 7.0F, 8.0F, 9.0F, 10.0F};
static const  std::vector<std::vector<float> > DEF_CONDDATA = {
  DEF_CONDITIONS
};

static const unsigned int MAX_INDEX(DEF_CONDITIONS.size());

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


float getCondtionsData(unsigned channel, unsigned int gain, unsigned int index) {
  return gain + channel / 100.0F + (100.0F * index);
}


void initTileTestCondtions(std::string conditionName, unsigned int version) {


  SG::WriteCondHandleKey<TileCalibDataFlt> calibDataKey{conditionName};
  assert(calibDataKey.initialize().isSuccess());

  SG::WriteCondHandle<TileCalibDataFlt> calibData{calibDataKey};

  std::unique_ptr<TileCalibDataFlt> data = std::make_unique<TileCalibDataFlt>();

  std::unique_ptr<coral::Blob> defaultBlob = std::make_unique<coral::Blob>(0);
  std::unique_ptr<TileCalibDrawerFlt> tmpDefaultCalibDrawer(TileCalibDrawerFlt::getInstance(*defaultBlob, DEF_CONDDATA, 1, version));
  // Trick to make calib drawer to copy and own blob
  std::unique_ptr<TileCalibDrawerFlt> defaultCalibDrawer = std::make_unique<TileCalibDrawerFlt>(*tmpDefaultCalibDrawer);
  data->setCalibDrawer(DEF_DRAWER_IDX, defaultCalibDrawer.release());


  std::vector<std::vector<float>> channelData{MAX_GAIN, std::vector<float>(MAX_INDEX, 0.0F)};

  std::unique_ptr<coral::Blob> drawerBlob = std::make_unique<coral::Blob>(0);
  std::unique_ptr<TileCalibDrawerFlt> tmpCalibDrawer(TileCalibDrawerFlt::getInstance(*drawerBlob, channelData, MAX_CHANNEL, version));
  // Trick to make calib drawer to own blob
  std::unique_ptr<TileCalibDrawerFlt> calibDrawer = std::make_unique<TileCalibDrawerFlt>(*tmpCalibDrawer);


  std::vector<float> gainData(MAX_INDEX, 0.0F);
  for (unsigned int channel = 0; channel < MAX_CHANNEL; ++channel) {
    for (unsigned int gain = 0; gain < MAX_GAIN; ++gain) {
      for (unsigned int index = 0; index < MAX_INDEX; ++index) {
        gainData[index] = getCondtionsData(channel, gain, index);
      }
      calibDrawer->setData(channel, gain, gainData);
    }
  }

  data->setCalibDrawer(DRAWER_IDX, calibDrawer.release());

  for (unsigned int drawerIdx = 1; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++drawerIdx) {
    if (drawerIdx == DRAWER_IDX) continue;
    unsigned int defaultDrawerIdx = TileCalibUtils::getDefaultDrawerIdx(drawerIdx);
    data->setCalibDrawer(drawerIdx, data->getCalibDrawer(defaultDrawerIdx));
  }


  assert(calibData.record(EVENT_RANGE, data.release()).isSuccess());

}


void testTileCondToolTiming(ISvcLocator* svcLoc) {

  std::cout << "\nTest TileCondToolTiming\n";

  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithm", svcLoc);
  alg->addRef();

  std::unique_ptr<TileCondToolTiming> tool = std::make_unique<TileCondToolTiming>("TileCondToolTiming",
                                                                                  "TileCondToolTimingTest",
                                                                                  alg.get());
  tool->addRef();

  assert(tool->setProperty("TileTiming", TILE_TEST_CONDITION));
  assert(tool->initialize().isSuccess());

  unsigned int defaultDrawerIdx(200);
  unsigned int defaultChannel(0);
  unsigned int defaultAdc(0);
  assert(Athena_test::isEqual(tool->getChannelOffset(defaultDrawerIdx, defaultChannel, defaultAdc), DEF_CONDITIONS[0]));


  for (unsigned int channel = 0; channel < MAX_CHANNEL; ++channel) {
    for (unsigned int gain = 0; gain < MAX_GAIN; ++gain) {
      float channelOffset = getCondtionsData(channel, gain, 0);
      assert(Athena_test::isEqual(tool->getChannelOffset(DRAWER_IDX, channel, gain), channelOffset));
    }
  }

}

void testTileCondToolAutoCr(ISvcLocator* svcLoc) {

  std::cout << "\nTest TileCondToolAutoCr\n";

  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithm", svcLoc);
  alg->addRef();

  std::unique_ptr<TileCondToolAutoCr> tool = std::make_unique<TileCondToolAutoCr>("TileCondToolAutoCr",
                                                                                  "TileCondToolAutoCrTest",
                                                                                   alg.get());
  tool->addRef();

  assert(tool->setProperty("TileAutoCorrelation", TILE_TEST_CONDITION));
  assert(tool->initialize().isSuccess());

  unsigned int defaultDrawerIdx(200);
  unsigned int defaultChannel(0);
  unsigned int defaultAdc(0);

  std::vector<float> referenceAutoCorr(DEF_CONDITIONS);
  std::vector<float> testedAutoCorr(MAX_INDEX);

  tool->getAutoCorr(defaultDrawerIdx, defaultChannel, defaultAdc, testedAutoCorr);
  assert(testedAutoCorr == referenceAutoCorr);

  for (unsigned int channel = 0; channel < MAX_CHANNEL; ++channel) {
    for (unsigned int gain = 0; gain < MAX_GAIN; ++gain) {

      for (unsigned int index = 0; index < MAX_INDEX; ++index) {
        referenceAutoCorr[index] = getCondtionsData(channel, gain, index);
      }

      tool->getAutoCorr(DRAWER_IDX, channel, gain, testedAutoCorr);
      assert(testedAutoCorr == referenceAutoCorr);

    }
  }

}



void testTileCondToolPulseShape(ISvcLocator* svcLoc, const EventContext& ctx) {

  std::cout << "\nTest TileCondToolPulseShape\n";

  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithm", svcLoc);
  alg->addRef();

  std::unique_ptr<TileCondToolPulseShape> tool = std::make_unique<TileCondToolPulseShape>("TileCondToolPulseShape",
                                                                                          "TileCondToolPulseShapeTest",
                                                                                          alg.get());
  tool->addRef();

  assert(tool->setProperty("TilePulseShape", TILE_PULSE_TEST_CONDITION));
  assert(tool->initialize().isSuccess());

  unsigned int defaultDrawerIdx(200);
  unsigned int defaultChannel(0);
  unsigned int defaultAdc(0);

  unsigned int nTimes(MAX_INDEX / 2);
  for(unsigned int timeIndex = 0; timeIndex < nTimes; ++timeIndex) {
    float time = DEF_CONDITIONS[timeIndex];
    float y(0.0F);
    float dy(0.0F);
    tool->getPulseShapeYDY(defaultDrawerIdx, defaultChannel, defaultAdc, time, y, dy, ctx);
    assert(Athena_test::isEqual(y, DEF_CONDITIONS[timeIndex + nTimes]));
  }

  for (unsigned int channel = 0; channel < MAX_CHANNEL; ++channel) {
    for (unsigned int gain = 0; gain < MAX_GAIN; ++gain) {
      for(unsigned int timeIndex = 0; timeIndex < nTimes; ++timeIndex) {
        float time = getCondtionsData(channel, gain, timeIndex);
        float refY = getCondtionsData(channel, gain, timeIndex + nTimes);
        float y(0.0F);
        float dy(0.0F);
        tool->getPulseShapeYDY(DRAWER_IDX, channel, gain, time, y, dy, ctx);
        assert(Athena_test::isEqual(y, refY));
      }
    }
  }

}


void testTileCondToolDspThreshold(ISvcLocator* svcLoc) {

  std::cout << "\nTest TileCondToolDspThreshold\n";

  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithm", svcLoc);
  alg->addRef();

  std::unique_ptr<TileCondToolDspThreshold> tool = std::make_unique<TileCondToolDspThreshold>("TileCondToolDspThreshold",
                                                                                              "TileCondToolDspThresholdTest",
                                                                                              alg.get());
  tool->addRef();

  assert(tool->setProperty("TileDSP_Threshold", TILE_TEST_CONDITION));
  assert(tool->initialize().isSuccess());

  unsigned int defaultDrawerIdx(200);
  unsigned int defaultChannel(0);
  unsigned int defaultAdc(0);

  float minThreshold = tool->getMinimumAmplitudeThreshold(defaultDrawerIdx, defaultChannel, defaultAdc);
  assert(Athena_test::isEqual(minThreshold, DEF_CONDITIONS[0]));

  float maxThreshold = tool->getMaximumAmplitudeThreshold(defaultDrawerIdx, defaultChannel, defaultAdc);
  assert(Athena_test::isEqual(maxThreshold, DEF_CONDITIONS[1]));

  minThreshold = 0.0;
  maxThreshold = 0.0;

  tool->getAmplitudeThresholds(defaultDrawerIdx, defaultChannel, defaultAdc, minThreshold, maxThreshold);
  assert(Athena_test::isEqual(minThreshold, DEF_CONDITIONS[0]));
  assert(Athena_test::isEqual(maxThreshold, DEF_CONDITIONS[1]));

  float dspThreshold = tool->getDspThreshold(defaultDrawerIdx, defaultChannel, defaultAdc);
  assert(Athena_test::isEqual(dspThreshold, DEF_CONDITIONS[2]));


  for (unsigned int channel = 0; channel < MAX_CHANNEL; ++channel) {
    for (unsigned int gain = 0; gain < MAX_GAIN; ++gain) {

      minThreshold = tool->getMinimumAmplitudeThreshold(DRAWER_IDX, channel, gain);
      assert(Athena_test::isEqual(minThreshold, getCondtionsData(channel, gain, 0)));

      maxThreshold = tool->getMaximumAmplitudeThreshold(DRAWER_IDX, channel, gain);
      assert(Athena_test::isEqual(maxThreshold, getCondtionsData(channel, gain, 1)));

      minThreshold = 0.0;
      maxThreshold = 0.0;

      tool->getAmplitudeThresholds(DRAWER_IDX, channel, gain, minThreshold, maxThreshold);
      assert(Athena_test::isEqual(minThreshold, getCondtionsData(channel, gain, 0)));
      assert(Athena_test::isEqual(maxThreshold, getCondtionsData(channel, gain, 1)));

      dspThreshold = tool->getDspThreshold(DRAWER_IDX, channel, gain);
      assert(Athena_test::isEqual(dspThreshold, getCondtionsData(channel, gain, 2)));

    }
  }

}


void testTileCondToolIntegrator(ISvcLocator* svcLoc) {

  std::cout << "\nTest TileCondToolIntegrator\n";

  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithm", svcLoc);
  alg->addRef();

  std::unique_ptr<TileCondToolIntegrator> tool = std::make_unique<TileCondToolIntegrator>("TileCondToolIntegrator",
                                                                                          "TileCondToolIntegratorTest",
                                                                                          alg.get());
  tool->addRef();

  assert(tool->setProperty("TileIntegrator", TILE_TEST_CONDITION));
  assert(tool->initialize().isSuccess());

  unsigned int defaultDrawerIdx(200);
  unsigned int defaultChannel(0);
  unsigned int defaultAdc(0);


  assert(Athena_test::isEqual(tool->getGain(defaultDrawerIdx, defaultChannel, defaultAdc), DEF_CONDITIONS[0]));
  assert(Athena_test::isEqual(tool->getGainError(defaultDrawerIdx, defaultChannel, defaultAdc), DEF_CONDITIONS[1]));
  assert(Athena_test::isEqual(tool->getChi2(defaultDrawerIdx, defaultChannel, defaultAdc), DEF_CONDITIONS[2]));
  assert(Athena_test::isEqual(tool->getPedestal(defaultDrawerIdx, defaultChannel, defaultAdc), DEF_CONDITIONS[3]));
  assert(Athena_test::isEqual(tool->getDACForPed(defaultDrawerIdx, defaultChannel, defaultAdc), DEF_CONDITIONS[4]));
  assert(Athena_test::isEqual(tool->getSigmaOfPed(defaultDrawerIdx, defaultChannel, defaultAdc), DEF_CONDITIONS[5]));
  assert(Athena_test::isEqual(tool->getRMSOfPed(defaultDrawerIdx, defaultChannel, defaultAdc), DEF_CONDITIONS[6]));
  assert(Athena_test::isEqual(tool->getSigmaOfRMS(defaultDrawerIdx, defaultChannel, defaultAdc), DEF_CONDITIONS[7]));


  for (unsigned int channel = 0; channel < MAX_CHANNEL; ++channel) {
    for (unsigned int gain = 0; gain < MAX_GAIN; ++gain) {

      assert(Athena_test::isEqual(tool->getGain(DRAWER_IDX, channel, gain), getCondtionsData(channel, gain, 0)));
      assert(Athena_test::isEqual(tool->getGainError(DRAWER_IDX, channel, gain), getCondtionsData(channel, gain, 1)));

      assert(Athena_test::isEqual(tool->getChi2(DRAWER_IDX, channel, gain), getCondtionsData(channel, gain, 2)));
      assert(Athena_test::isEqual(tool->getPedestal(DRAWER_IDX, channel, gain), getCondtionsData(channel, gain, 3)));
      assert(Athena_test::isEqual(tool->getDACForPed(DRAWER_IDX, channel, gain), getCondtionsData(channel, gain, 4)));
      assert(Athena_test::isEqual(tool->getSigmaOfPed(DRAWER_IDX, channel, gain), getCondtionsData(channel, gain, 5)));
      assert(Athena_test::isEqual(tool->getRMSOfPed(DRAWER_IDX, channel, gain), getCondtionsData(channel, gain, 6)));
      assert(Athena_test::isEqual(tool->getSigmaOfRMS(DRAWER_IDX, channel, gain), getCondtionsData(channel, gain, 7)));

    }
  }

}


int main() {


  std::ofstream jo(TILE_JO_NAME);
  jo << "ApplicationMgr.ExtSvc += { \"StoreGateSvc/ConditionStore\" };" << std::endl;
  jo.close();

  ISvcLocator* svcLoc;
  if (!Athena_test::initGaudi(TILE_JO_NAME, svcLoc)) {
    std::cerr << "ERROR This test can not be run, can not get ServiceLocator" << std::endl;
  }

  ServiceHandle<StoreGateSvc> conditionStore("ConditionStore", "");
  assert(conditionStore.retrieve().isSuccess());

  initTileTestCondtions(TILE_TEST_CONDITION, OBJ_VERSION);
  initTileTestCondtions(TILE_PULSE_TEST_CONDITION, PULSE_OBJ_VERSION);

  EventIDBase eventId;
  eventId.set_run_number(EventIDBase::UNDEFNUM / 4);
  eventId.set_lumi_block(EventIDBase::UNDEFNUM / 4);

  EventContext ctx;
  ctx.setEventID(eventId);
  ctx.setExtension( Atlas::ExtendedEventContext(&*conditionStore) );
  Gaudi::Hive::setCurrentContext(ctx);

  testTileCondToolTiming(svcLoc);
  testTileCondToolAutoCr(svcLoc);
  testTileCondToolPulseShape(svcLoc, ctx);
  testTileCondToolDspThreshold(svcLoc);
  testTileCondToolIntegrator(svcLoc);

  return 0;
}
