/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

// Tile includes
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileConditions/TileCalibData.h"
#include "TileConditions/TileEMScale.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileConditions/TileCondToolNoiseRawChn.h"

#include "TestTools/FLOATassert.h"
#include "TestTools/initGaudi.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/SGtests.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/WriteCondHandle.h"


#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/EventIDRange.h"


#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

static const std::string TILE_JO_NAME("jobOptions_TileNoiseToolsTest.py");
static const std::string TILE_SAMPLE_EMS("TileSampleEms");
static const std::string TILE_RAW_CHANNEL_EMS("TileRawChannelEms");
static const std::string TILE_SAMPLE_NOISE("TileSampleNoiseTest");
static const std::string TILE_ONLINE_SAMPLE_NOISE("TileOnlineSampleNoiseTest");
static const std::string TILE_RAW_CHANNEL_NOISE("TileRawChannelNoiseTest");

static const unsigned int DEF_DRAWER_IDX(0);
static const unsigned int MAX_CHANNEL(1);

static const unsigned int DRAWER_IDX(20);
static const unsigned int CHANNEL(0);
static const unsigned int ADC(0);

static const float LIN_FACTOR(2.0F);
static const float NLN_FACTOR(2.0F);
static const float RANGE(1000.0F);
static const std::vector<std::vector<float>> NLN_CORRECTIONS{
  {0.0F, RANGE, 0.0F, RANGE * NLN_FACTOR}
};

static const float PEDESTAL(50.0F);
static const float HFN(10.0F);
static const float LFN(5.0F);
static const float HFN1(8.0F);
static const float HFN2(12.0F);
static const float HFN_NORM(15.0F);

static const std::vector<std::vector<float>> SAMPLE_NOISE{
  {PEDESTAL, HFN, LFN, HFN1, HFN2, HFN_NORM}
};

static const float PEDESTAL_DIF(10.0F);
static const float ONLINE_PEDESTAL(PEDESTAL + PEDESTAL_DIF);

static const float ELECTRONIC_NOISE(20.0F);
static const float PILEUP_NOISE(100.0F);

static const std::vector<std::vector<float>> RAW_CHANNEL_NOISE{
  {ELECTRONIC_NOISE, PILEUP_NOISE}
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


std::unique_ptr<TileCalibDataFlt> getCalibData(const std::vector<std::vector<float>>& defaultData, unsigned int version = 100) {

  //  std::vector<std::vector<float> defaultData{{data}};
  std::unique_ptr<coral::Blob> defaultBlob = std::make_unique<coral::Blob>(0);
  std::unique_ptr<TileCalibDrawerFlt> tmpCalibDrawer(TileCalibDrawerFlt::getInstance(*defaultBlob, defaultData, MAX_CHANNEL, version));
  // Trick to make calib drawer to copy and own blob
  std::unique_ptr<TileCalibDrawerFlt> defaultCalibDrawer = std::make_unique<TileCalibDrawerFlt>(*tmpCalibDrawer);

  std::unique_ptr<TileCalibDataFlt> calibData = std::make_unique<TileCalibDataFlt>();
  calibData->setCalibDrawer(DEF_DRAWER_IDX, defaultCalibDrawer.release());

  for (unsigned int drawerIdx = 1; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++drawerIdx) {
    unsigned int defaultDrawerIdx = TileCalibUtils::getDefaultDrawerIdx(drawerIdx);
    calibData->setCalibDrawer(drawerIdx, calibData->getCalibDrawer(defaultDrawerIdx));
  }

  return calibData;

}

std::unique_ptr<TileCalibDataFlt> getCalibData(float value) {
  std::vector<std::vector<float>> defaultData{{value}};
  return getCalibData(defaultData, 1);
}


std::unique_ptr<TileEMScale> getCalibEms() {

  // Initialize TileCalibEms
  std::unique_ptr<TileEMScale> emScale = std::make_unique<TileEMScale>();

  emScale->setCalibOnlCis(getCalibData(LIN_FACTOR));
  emScale->setCalibOnlLas(getCalibData(LIN_FACTOR));
  emScale->setCalibOnlCes(getCalibData(LIN_FACTOR));
  emScale->setCalibOnlEms(getCalibData(LIN_FACTOR));

  emScale->setCalibOflCisLin(getCalibData(LIN_FACTOR));
  emScale->setCalibOflLasLin(getCalibData(LIN_FACTOR));
  emScale->setCalibOflCes(getCalibData(LIN_FACTOR));
  emScale->setCalibOflEms(getCalibData(LIN_FACTOR));

  emScale->setCalibOflCisNln(getCalibData(NLN_CORRECTIONS));
  emScale->setCalibOflLasNln(getCalibData(NLN_CORRECTIONS));

  emScale->setOnlineCacheUnit(TileRawChannelUnit::OnlineMegaElectronVolts);
  emScale->setMaxChannels(48);
  emScale->setMaxGains(2);
  assert(emScale->initialize());

  return emScale;
}


void testTileCondToolNoiseSample(ISvcLocator* svcLoc, const EventContext &ctx) {

  std::cout << "\nTest TileCondToolNoiseSample\n";

  {
    SG::WriteCondHandleKey<TileEMScale> emScaleKey{TILE_SAMPLE_EMS};
    assert(emScaleKey.initialize().isSuccess());

    SG::WriteCondHandle<TileEMScale> emScale{emScaleKey, ctx};
    std::unique_ptr<TileEMScale> emScaleData = getCalibEms();
    assert(emScale.record(EVENT_RANGE, emScaleData.release()).isSuccess());


    SG::WriteCondHandleKey<TileCalibDataFlt> calibSampleNoiseKey{TILE_SAMPLE_NOISE};
    assert(calibSampleNoiseKey.initialize().isSuccess());

    SG::WriteCondHandle<TileCalibDataFlt> calibSampleNoise{calibSampleNoiseKey, ctx};
    std::unique_ptr<TileCalibDataFlt> calibSampleNoiseData = getCalibData(SAMPLE_NOISE);
    assert(calibSampleNoise.record(EVENT_RANGE, calibSampleNoiseData.release()).isSuccess());


    SG::WriteCondHandleKey<TileCalibDataFlt> calibOnlineSampleNoiseKey{TILE_ONLINE_SAMPLE_NOISE};
    assert(calibOnlineSampleNoiseKey.initialize().isSuccess());

    SG::WriteCondHandle<TileCalibDataFlt> calibOnlineSampleNoise{calibOnlineSampleNoiseKey, ctx};
    std::unique_ptr<TileCalibDataFlt> calibOnlineSampleNoiseData = getCalibData(ONLINE_PEDESTAL);
    assert(calibOnlineSampleNoise.record(EVENT_RANGE, calibOnlineSampleNoiseData.release()).isSuccess());

  }

  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithm", svcLoc);
  alg->addRef();

  std::unique_ptr<TileCondToolNoiseSample> sampleNoise
    =  std::make_unique<TileCondToolNoiseSample>("TileCondToolNoiseSample",
                                                 "TileCondToolNoiseSampleTest",
                                                 alg.get());
  sampleNoise->addRef();

  assert(sampleNoise->setProperty("TileEMScale", TILE_SAMPLE_EMS));
  assert(sampleNoise->setProperty("TileSampleNoise", TILE_SAMPLE_NOISE));
  assert(sampleNoise->setProperty("TileOnlineSampleNoise", TILE_ONLINE_SAMPLE_NOISE));
  assert(sampleNoise->initialize().isSuccess());

  const float adcPedestal(sampleNoise->getPed(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::ADCcounts, ctx));
  assert(Athena_test::isEqual(adcPedestal, PEDESTAL));

  const float picoCoulombsPedestal(sampleNoise->getPed(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::PicoCoulombs, ctx));
  assert(Athena_test::isEqual(picoCoulombsPedestal, (adcPedestal / LIN_FACTOR) * NLN_FACTOR));

  const float cesPicoCoulombsPedestal(sampleNoise->getPed(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::CesiumPicoCoulombs, ctx));
  assert(Athena_test::isEqual(cesPicoCoulombsPedestal, picoCoulombsPedestal / LIN_FACTOR / LIN_FACTOR * NLN_FACTOR));

  const float mevPedestal(sampleNoise->getPed(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::MegaElectronVolts, ctx));
  assert(Athena_test::isEqual(mevPedestal, cesPicoCoulombsPedestal / LIN_FACTOR));



  const float adcHfn(sampleNoise->getHfn(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::ADCcounts, ctx));
  assert(Athena_test::isEqual(adcHfn, HFN));

  const float picoCoulombsHfn(sampleNoise->getHfn(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::PicoCoulombs, ctx));
  assert(Athena_test::isEqual(picoCoulombsHfn, (adcHfn / LIN_FACTOR) * NLN_FACTOR));

  const float cesPicoCoulombsHfn(sampleNoise->getHfn(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::CesiumPicoCoulombs, ctx));
  assert(Athena_test::isEqual(cesPicoCoulombsHfn, picoCoulombsHfn / LIN_FACTOR / LIN_FACTOR * NLN_FACTOR));

  const float mevHfn(sampleNoise->getHfn(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::MegaElectronVolts, ctx));
  assert(Athena_test::isEqual(mevHfn, cesPicoCoulombsHfn / LIN_FACTOR));



  const float adcNoise(sampleNoise->getNoise(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::ADCcounts, ctx));
  assert(Athena_test::isEqual(adcNoise, HFN));

  const float picoCoulombsNoise(sampleNoise->getNoise(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::PicoCoulombs, ctx));
  assert(Athena_test::isEqual(picoCoulombsNoise, (adcNoise / LIN_FACTOR) * NLN_FACTOR));

  const float cesPicoCoulombsNoise(sampleNoise->getNoise(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::CesiumPicoCoulombs, ctx));
  assert(Athena_test::isEqual(cesPicoCoulombsNoise, picoCoulombsNoise / LIN_FACTOR / LIN_FACTOR * NLN_FACTOR));

  const float mevNoise(sampleNoise->getNoise(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::MegaElectronVolts, ctx));
  assert(Athena_test::isEqual(mevNoise, cesPicoCoulombsNoise / LIN_FACTOR));



  const float adcLfn(sampleNoise->getLfn(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::ADCcounts, ctx));
  assert(Athena_test::isEqual(adcLfn, LFN));

  const float picoCoulombsLfn(sampleNoise->getLfn(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::PicoCoulombs, ctx));
  assert(Athena_test::isEqual(picoCoulombsLfn, (adcLfn / LIN_FACTOR) * NLN_FACTOR));

  const float cesPicoCoulombsLfn(sampleNoise->getLfn(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::CesiumPicoCoulombs, ctx));
  assert(Athena_test::isEqual(cesPicoCoulombsLfn, picoCoulombsLfn / LIN_FACTOR / LIN_FACTOR * NLN_FACTOR));

  const float mevLfn(sampleNoise->getLfn(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::MegaElectronVolts, ctx));
  assert(Athena_test::isEqual(mevLfn, cesPicoCoulombsLfn / LIN_FACTOR));



  const float hfn1(sampleNoise->getHfn1(DRAWER_IDX, CHANNEL, ADC, ctx));
  assert(Athena_test::isEqual(hfn1, HFN1));

  const float hfn2(sampleNoise->getHfn2(DRAWER_IDX, CHANNEL, ADC, ctx));
  assert(Athena_test::isEqual(hfn2, HFN2));

  const float hfnNorm(sampleNoise->getHfnNorm(DRAWER_IDX, CHANNEL, ADC, ctx));
  assert(Athena_test::isEqual(hfnNorm, HFN_NORM));



  const float adcPedestalDifferene(sampleNoise->getOnlinePedestalDifference(DRAWER_IDX, CHANNEL, ADC,
                                                                            TileRawChannelUnit::OnlineADCcounts, ctx));
  assert(Athena_test::isEqual(adcPedestalDifferene, PEDESTAL_DIF));

  const float picoCoulombsPedestalDifference =
    sampleNoise->getOnlinePedestalDifference(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::OnlinePicoCoulombs, ctx);

  assert(Athena_test::isEqual(picoCoulombsPedestalDifference, (adcPedestalDifferene / LIN_FACTOR)));


  const float cesPicoCoulombsPedestalDifference =
    sampleNoise->getOnlinePedestalDifference(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::OnlineCesiumPicoCoulombs, ctx);

  assert(Athena_test::isEqual(cesPicoCoulombsPedestalDifference,
                              picoCoulombsPedestalDifference / LIN_FACTOR / LIN_FACTOR));

  const float mevPedestalDifference =
    sampleNoise->getOnlinePedestalDifference(DRAWER_IDX, CHANNEL, ADC, TileRawChannelUnit::OnlineMegaElectronVolts, ctx);

  assert(Athena_test::isEqual(mevPedestalDifference, cesPicoCoulombsPedestalDifference / LIN_FACTOR));

}



void testTileCondToolNoiseRawChn(ISvcLocator* svcLoc, const EventContext &ctx) {

  std::cout << "\nTest TileCondToolNoiseRawChn\n";

  {
    SG::WriteCondHandleKey<TileEMScale> emScaleKey{TILE_RAW_CHANNEL_EMS};
    assert(emScaleKey.initialize().isSuccess());

    SG::WriteCondHandle<TileEMScale> emScale{emScaleKey, ctx};
    std::unique_ptr<TileEMScale> emScaleData = getCalibEms();
    assert(emScale.record(EVENT_RANGE, emScaleData.release()).isSuccess());


    SG::WriteCondHandleKey<TileCalibDataFlt> calibSampleNoiseKey{TILE_RAW_CHANNEL_NOISE};
    assert(calibSampleNoiseKey.initialize().isSuccess());

    SG::WriteCondHandle<TileCalibDataFlt> calibSampleNoise{calibSampleNoiseKey, ctx};
    std::unique_ptr<TileCalibDataFlt> calibSampleNoiseData = getCalibData(RAW_CHANNEL_NOISE);
    assert(calibSampleNoise.record(EVENT_RANGE, calibSampleNoiseData.release()).isSuccess());

  }

  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithm", svcLoc);
  alg->addRef();

  std::unique_ptr<TileCondToolNoiseRawChn> rawChannelNoise
    =  std::make_unique<TileCondToolNoiseRawChn>("TileCondToolNoiseRawChn",
                                                 "TileCondToolNoiseRawChnTest",
                                                 alg.get());
  rawChannelNoise->addRef();

  assert(rawChannelNoise->setProperty("TileEMScale", TILE_RAW_CHANNEL_EMS));
  assert(rawChannelNoise->setProperty("TileRawChannelNoise", TILE_RAW_CHANNEL_NOISE));
  assert(rawChannelNoise->initialize().isSuccess());


  const float adcElectronicNoise(rawChannelNoise->getElectronicNoise(DRAWER_IDX, CHANNEL, ADC,
                                                                     TileRawChannelUnit::ADCcounts, ctx));
  assert(Athena_test::isEqual(adcElectronicNoise, ELECTRONIC_NOISE));

  const float picoCoulombsElectronicNoise(rawChannelNoise->getElectronicNoise(DRAWER_IDX, CHANNEL, ADC,
                                                                              TileRawChannelUnit::PicoCoulombs, ctx));
  assert(Athena_test::isEqual(picoCoulombsElectronicNoise, ELECTRONIC_NOISE / LIN_FACTOR * NLN_FACTOR));

  const float cesPicoCoulombsElectronicNoise(rawChannelNoise->getElectronicNoise(DRAWER_IDX, CHANNEL, ADC,
                                                                                 TileRawChannelUnit::CesiumPicoCoulombs, ctx));
  assert(Athena_test::isEqual(cesPicoCoulombsElectronicNoise,
                              picoCoulombsElectronicNoise / LIN_FACTOR / LIN_FACTOR * NLN_FACTOR));


  const float mevElectronicNoise(rawChannelNoise->getElectronicNoise(DRAWER_IDX, CHANNEL, ADC,
                                                                     TileRawChannelUnit::MegaElectronVolts, ctx));
  assert(Athena_test::isEqual(mevElectronicNoise, cesPicoCoulombsElectronicNoise / LIN_FACTOR));


  const float pileUpNoise(rawChannelNoise->getPileUpNoise(DRAWER_IDX, CHANNEL, ADC, ctx));
  assert(Athena_test::isEqual(pileUpNoise, PILEUP_NOISE));

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

  EventIDBase eventId;
  eventId.set_run_number(EventIDBase::UNDEFNUM / 4);
  eventId.set_lumi_block(EventIDBase::UNDEFNUM / 4);

  EventContext ctx;
  ctx.setEventID(eventId);
  ctx.setExtension( Atlas::ExtendedEventContext(&*conditionStore) );
  Gaudi::Hive::setCurrentContext(ctx);


  testTileCondToolNoiseSample(svcLoc, ctx);
  testTileCondToolNoiseRawChn(svcLoc, ctx);

  return 0;
}
