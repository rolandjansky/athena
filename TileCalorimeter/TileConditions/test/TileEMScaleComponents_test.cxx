/*
  Copyright (C) 2002-, 20212021 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

// Tile includes
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileConditions/TileCalibData.h"
#include "TileConditions/TileEMScale.h"
#include "src/TileEMScaleCondAlg.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileCablingService_common_test.cxx"

#include "CaloIdentifier/CaloLVL1_ID.h"

#include "TestTools/FLOATassert.h"
#include "TestTools/initGaudi.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/WriteCondHandle.h"

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

static const std::string TILE_JO_NAME("jobOptions_TileCalibEmsTest.txt");
static const std::string TILE_TEST_EMS("TileEMS_Test");
static const std::string TILE_TEST_ALG_EMS("TileEMS_AlgTest");
static const unsigned int DEF_DRAWER_IDX(0);
static const unsigned int MAX_CHANNEL(1);

static const unsigned int DRAWER_IDX(20);
static const unsigned int CHANNEL(0);
static const unsigned int ADC(0);


static const float AMPLITUDE(5.0F);

static const float ONL_CIS(1.05F);
static const float ONL_LAS(1.005F);
static const float ONL_CES(1.0005F);
static const float ONL_EMS(1.00005F);

static const float OFL_CIS(1.1F);
static const float OFL_LAS(1.01F);
static const float OFL_CES(1.001F);
static const float OFL_EMS(1.0001F);

static const float CES_REF_LAS(1.5F);
static const float CES_REF_HV(650.0F);
static const float CES_REF_TEMP(20.5F);


static const std::vector<std::vector<float>> ONL_CIS_DEF{{ONL_CIS}};
static const std::vector<std::vector<float>> ONL_LAS_DEF{{ONL_LAS}};
static const std::vector<std::vector<float>> ONL_CES_DEF{{ONL_CES}};
static const std::vector<std::vector<float>> ONL_EMS_DEF{{ONL_EMS}};

static const std::vector<std::vector<float>> OFL_CIS_LIN{{OFL_CIS}};
static const std::vector<std::vector<float>> OFL_CIS_NLN{{0.0F, 1000.0F, 0.0F, 1100.0F}};
static const std::vector<std::vector<float>> OFL_LAS_LIN{{OFL_LAS}};
static const std::vector<std::vector<float>> OFL_LAS_NLN{{0.0F, 1000.0F, 0.0F, 1200.0F}};
static const std::vector<std::vector<float>> OFL_CES_FULL{
  {OFL_CES, CES_REF_LAS, CES_REF_HV, CES_REF_TEMP}
};
static const std::vector<std::vector<float>> OFL_EMS_DEF{{OFL_EMS}};



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


void prepareCalibData(TileCalibDataFlt& calibData,
                   const std::vector<std::vector<float>>& defaultData,
                   unsigned int version = 100) {

  std::unique_ptr<coral::Blob> defaultBlob = std::make_unique<coral::Blob>(0);
  std::unique_ptr<TileCalibDrawerFlt> tmpCalibDrawer(TileCalibDrawerFlt::getInstance(*defaultBlob, defaultData, MAX_CHANNEL, version));
  // Trick to make calib drawer to copy and own blob
  std::unique_ptr<TileCalibDrawerFlt> defaultCalibDrawer = std::make_unique<TileCalibDrawerFlt>(*tmpCalibDrawer);

  calibData.setCalibDrawer(DEF_DRAWER_IDX, defaultCalibDrawer.release());

  for (unsigned int drawerIdx = 1; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++drawerIdx) {
    unsigned int defaultDrawerIdx = TileCalibUtils::getDefaultDrawerIdx(drawerIdx);
    calibData.setCalibDrawer(drawerIdx, calibData.getCalibDrawer(defaultDrawerIdx));
  }

}

std::unique_ptr<TileCalibDataFlt> getCalibData(const std::vector<std::vector<float>>& defaultData, unsigned int version = 100) {

  std::unique_ptr<TileCalibDataFlt> calibData = std::make_unique<TileCalibDataFlt>();
  prepareCalibData(*calibData, defaultData, version);

  return calibData;

}


std::unique_ptr<TileCalibDataFlt> getCalibData(float value) {
  std::vector<std::vector<float>> defaultData{{value}};
  return getCalibData(defaultData, 1);
}


std::unique_ptr<TileEMScale> getCalibEms() {

  // Initialize TileCalibEms
  std::unique_ptr<TileEMScale> emScale = std::make_unique<TileEMScale>();
  unsigned int cesVersion(1);

  emScale->setCalibOnlCis(getCalibData(ONL_CIS));
  emScale->setCalibOnlLas(getCalibData(ONL_LAS));
  emScale->setCalibOnlCes(getCalibData(ONL_CES));
  emScale->setCalibOnlEms(getCalibData(ONL_EMS));

  emScale->setCalibOflCisLin(getCalibData(OFL_CIS));
  emScale->setCalibOflLasLin(getCalibData(OFL_LAS));
  emScale->setCalibOflCes(getCalibData(OFL_CES_FULL, cesVersion));
  emScale->setCalibOflEms(getCalibData(OFL_EMS));

  emScale->setCalibOflCisNln(getCalibData(OFL_CIS_NLN));
  emScale->setCalibOflLasNln(getCalibData(OFL_LAS_NLN));

  emScale->setOnlineCacheUnit(TileRawChannelUnit::OnlineMegaElectronVolts);
  emScale->setMaxChannels(48);
  emScale->setMaxGains(2);
  assert(emScale->initialize());

  return emScale;
}



template<class T = TileCalibDrawerFlt,
         const std::vector<std::vector<float>>* CALIBRATIONS_DEF = &ONL_CIS_DEF,
         unsigned int VERSION = 1 >
class TileCondProxyMock: public AthAlgTool, virtual public ITileCondProxy<T> {
  public:

    TileCondProxyMock(const std::string& type, const std::string& name, const IInterface* parent)
      :AthAlgTool(type, name, parent)
    {
        declareInterface<ITileCondProxy<T> >(this);
    };

    virtual ~TileCondProxyMock() {};

    virtual StatusCode fillCalibData(TileCalibData<T>& calibData, EventIDRange& eventRange) const override {

      prepareCalibData(calibData, *CALIBRATIONS_DEF, VERSION);
      eventRange = EVENT_RANGE;

      return StatusCode::SUCCESS;
    };

    virtual StatusCode initialize() override {
      return StatusCode::SUCCESS;
    };

    virtual StatusCode finalize() override {
      return StatusCode::SUCCESS;
    };

};


// Online proxies
typedef TileCondProxyMock<TileCalibDrawerFlt, &ONL_CIS_DEF> TileCondProxyOnlCisMock;
// cppcheck-suppress unknownMacro
DECLARE_COMPONENT_WITH_ID( TileCondProxyOnlCisMock, "TileCondProxyOnlCisMock" )

typedef TileCondProxyMock<TileCalibDrawerFlt, &ONL_LAS_DEF> TileCondProxyOnlLasMock;
DECLARE_COMPONENT_WITH_ID( TileCondProxyOnlLasMock, "TileCondProxyOnlLasMock" )

typedef TileCondProxyMock<TileCalibDrawerFlt, &ONL_CES_DEF> TileCondProxyOnlCesMock;
DECLARE_COMPONENT_WITH_ID( TileCondProxyOnlCesMock, "TileCondProxyOnlCesMock" )

typedef TileCondProxyMock<TileCalibDrawerFlt, &ONL_EMS_DEF> TileCondProxyOnlEmsMock;
DECLARE_COMPONENT_WITH_ID( TileCondProxyOnlEmsMock, "TileCondProxyOnlEmsMock" )

// Offline proxies
typedef TileCondProxyMock<TileCalibDrawerFlt, &OFL_CIS_LIN> TileCondProxyOflCisLinMock;
DECLARE_COMPONENT_WITH_ID( TileCondProxyOflCisLinMock, "TileCondProxyOflCisLinMock" )

typedef TileCondProxyMock<TileCalibDrawerFlt, &OFL_CIS_NLN, 100U> TileCondProxyOflCisNlnMock;
DECLARE_COMPONENT_WITH_ID( TileCondProxyOflCisNlnMock, "TileCondProxyOflCisNlnMock" )

typedef TileCondProxyMock<TileCalibDrawerFlt, &OFL_LAS_LIN> TileCondProxyOflLasLinMock;
DECLARE_COMPONENT_WITH_ID( TileCondProxyOflLasLinMock, "TileCondProxyOflLasLinMock" )

typedef TileCondProxyMock<TileCalibDrawerFlt, &OFL_LAS_NLN, 100U> TileCondProxyOflLasNlnMock;
DECLARE_COMPONENT_WITH_ID( TileCondProxyOflLasNlnMock, "TileCondProxyOflLasNlnMock" )

typedef TileCondProxyMock<TileCalibDrawerFlt, &OFL_CES_FULL> TileCondProxyOflCesMock;
DECLARE_COMPONENT_WITH_ID( TileCondProxyOflCesMock, "TileCondProxyOflCesMock" )

typedef TileCondProxyMock<TileCalibDrawerFlt, &OFL_EMS_DEF> TileCondProxyOflEmsMock;
DECLARE_COMPONENT_WITH_ID( TileCondProxyOflEmsMock, "TileCondProxyOflEmsMock" )



void checkEMScale(const TileEMScale* emScale) {

  // Test online calibration
  assert(Athena_test::isEqual(emScale->applyOnlineChargeCalibration(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE), (AMPLITUDE / ONL_CIS), 1E-5));
  assert(Athena_test::isEqual(emScale->applyOnlineLaserCalibration(DRAWER_IDX, CHANNEL, AMPLITUDE), (AMPLITUDE / ONL_LAS), 1E-5));
  assert(Athena_test::isEqual(emScale->applyOnlineCesiumCalibration(DRAWER_IDX, CHANNEL, AMPLITUDE), (AMPLITUDE / ONL_CES), 1E-5));
  assert(Athena_test::isEqual(emScale->applyOnlineEMScaleCalibration(DRAWER_IDX, CHANNEL, AMPLITUDE), (AMPLITUDE / ONL_EMS), 1E-5));

  const float onlAdcAmplitude(emScale->calibrateOnlineChannel(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE,
                                                              TileRawChannelUnit::OnlineADCcounts));
  assert(Athena_test::isEqual(onlAdcAmplitude, AMPLITUDE));

  const float onlPicoCoulombsAmplitude(emScale->calibrateOnlineChannel(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE,
                                                                       TileRawChannelUnit::OnlinePicoCoulombs));
  assert(Athena_test::isEqual(onlPicoCoulombsAmplitude, AMPLITUDE / ONL_CIS, 1E-5));

  const float onlCesPicoCoulombsAmplitude(emScale->calibrateOnlineChannel(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE,
                                                                          TileRawChannelUnit::OnlineCesiumPicoCoulombs));
  assert(Athena_test::isEqual(onlCesPicoCoulombsAmplitude, AMPLITUDE / (ONL_CIS * ONL_LAS * ONL_CES), 1E-5));


  const float onlMevAmplitude(emScale->calibrateOnlineChannel(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE,
                                                              TileRawChannelUnit::OnlineMegaElectronVolts));
  assert(Athena_test::isEqual(onlMevAmplitude, AMPLITUDE / (ONL_CIS * ONL_LAS * ONL_CES * ONL_EMS), 1E-5));


  // Test offline calibration
  assert(Athena_test::isEqual(emScale->applyCesiumCalibration(DRAWER_IDX, CHANNEL, AMPLITUDE), (AMPLITUDE / OFL_CES), 1E-5));
  assert(Athena_test::isEqual(emScale->applyEMScaleCalibration(DRAWER_IDX, CHANNEL, AMPLITUDE), (AMPLITUDE / OFL_EMS), 1E-5));

  assert(Athena_test::isEqual(emScale->undoOnlineChannelCalibration(DRAWER_IDX, CHANNEL, ADC, onlAdcAmplitude,
                                                                    TileRawChannelUnit::OnlineADCcounts), AMPLITUDE, 1E-5));

  assert(Athena_test::isEqual(emScale->undoOnlineChannelCalibration(DRAWER_IDX, CHANNEL, ADC, onlPicoCoulombsAmplitude,
                                                                    TileRawChannelUnit::OnlinePicoCoulombs), AMPLITUDE, 1E-5));

  assert(Athena_test::isEqual(emScale->undoOnlineChannelCalibration(DRAWER_IDX, CHANNEL, ADC, onlCesPicoCoulombsAmplitude,
                                                                    TileRawChannelUnit::OnlineCesiumPicoCoulombs), AMPLITUDE, 1E-5));

  assert(Athena_test::isEqual(emScale->undoOnlineChannelCalibration(DRAWER_IDX, CHANNEL, ADC, onlMevAmplitude,
                                                                    TileRawChannelUnit::OnlineMegaElectronVolts), AMPLITUDE, 1E-5));


  const float adcAmplitude(emScale->calibrateChannel(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE,
                                                 TileRawChannelUnit::OnlineADCcounts,
                                                 TileRawChannelUnit::ADCcounts));
  assert(Athena_test::isEqual(adcAmplitude, AMPLITUDE));


  const float nlnCis((OFL_CIS_NLN[0][3] - OFL_CIS_NLN[0][2]) / (OFL_CIS_NLN[0][1] - OFL_CIS_NLN[0][0]));
  const float nlnLas((OFL_LAS_NLN[0][3] - OFL_LAS_NLN[0][2]) / (OFL_LAS_NLN[0][1] - OFL_LAS_NLN[0][0]));

  const float picoCoulombsAmplitude(emScale->calibrateChannel(DRAWER_IDX, CHANNEL, ADC, onlPicoCoulombsAmplitude,
                                                              TileRawChannelUnit::OnlinePicoCoulombs,
                                                              TileRawChannelUnit::PicoCoulombs));
  assert(Athena_test::isEqual(picoCoulombsAmplitude, (AMPLITUDE / OFL_CIS) * nlnCis, 1E-5));

  const float cesPicoCoulombsAmplitude(emScale->calibrateChannel(DRAWER_IDX, CHANNEL, ADC, onlCesPicoCoulombsAmplitude,
                                                                 TileRawChannelUnit::OnlineCesiumPicoCoulombs,
                                                                 TileRawChannelUnit::CesiumPicoCoulombs));
  assert(Athena_test::isEqual(cesPicoCoulombsAmplitude,
                              (AMPLITUDE / OFL_CIS / OFL_LAS / OFL_CES) * nlnCis * nlnLas, 1E-5));

  const float mevAmplitude(emScale->calibrateChannel(DRAWER_IDX, CHANNEL, ADC, onlMevAmplitude,
                                                     TileRawChannelUnit::OnlineMegaElectronVolts,
                                                     TileRawChannelUnit::MegaElectronVolts));
  assert(Athena_test::isEqual(mevAmplitude,
                              (AMPLITUDE / OFL_CIS / OFL_LAS / OFL_CES / OFL_EMS) * nlnCis * nlnLas, 1E-5));

  assert(Athena_test::isEqual(emScale->getCesiumReferenceLaserGain(DRAWER_IDX, CHANNEL, ADC), CES_REF_LAS));
  assert(Athena_test::isEqual(emScale->getCesiumReferenceHV(DRAWER_IDX, CHANNEL), CES_REF_HV));
  assert(Athena_test::isEqual(emScale->getCesiumReferenceTemperature(DRAWER_IDX, CHANNEL), CES_REF_TEMP));

}


void checkEMScaleTool(const TileCondToolEmscale* emScaleTool) {

  // Test online calibration
  assert(Athena_test::isEqual(emScaleTool->doCalibCisOnl(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE), (AMPLITUDE / ONL_CIS), 1E-5));
  assert(Athena_test::isEqual(emScaleTool->doCalibLasOnl(DRAWER_IDX, CHANNEL, AMPLITUDE), (AMPLITUDE / ONL_LAS), 1E-5));
  assert(Athena_test::isEqual(emScaleTool->doCalibCesOnl(DRAWER_IDX, CHANNEL, AMPLITUDE), (AMPLITUDE / ONL_CES), 1E-5));
  assert(Athena_test::isEqual(emScaleTool->doCalibEmsOnl(DRAWER_IDX, CHANNEL, AMPLITUDE), (AMPLITUDE / ONL_EMS), 1E-5));

  const float onlAdcAmplitude(emScaleTool->channelCalibOnl(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE,
                                                           TileRawChannelUnit::OnlineADCcounts));
  assert(Athena_test::isEqual(onlAdcAmplitude, AMPLITUDE));

  const float onlPicoCoulombsAmplitude(emScaleTool->channelCalibOnl(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE,
                                                                    TileRawChannelUnit::OnlinePicoCoulombs));
  assert(Athena_test::isEqual(onlPicoCoulombsAmplitude, AMPLITUDE / ONL_CIS, 1E-5));

  const float onlCesPicoCoulombsAmplitude(emScaleTool->channelCalibOnl(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE,
                                                                       TileRawChannelUnit::OnlineCesiumPicoCoulombs));
  assert(Athena_test::isEqual(onlCesPicoCoulombsAmplitude, AMPLITUDE / (ONL_CIS * ONL_LAS * ONL_CES), 1E-5));


  const float onlMevAmplitude(emScaleTool->channelCalibOnl(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE,
                                                           TileRawChannelUnit::OnlineMegaElectronVolts));
  assert(Athena_test::isEqual(onlMevAmplitude, AMPLITUDE / (ONL_CIS * ONL_LAS * ONL_CES * ONL_EMS), 1E-5));


  // Test offline calibration
  assert(Athena_test::isEqual(emScaleTool->doCalibCes(DRAWER_IDX, CHANNEL, AMPLITUDE), (AMPLITUDE / OFL_CES), 1E-5));
  assert(Athena_test::isEqual(emScaleTool->doCalibEms(DRAWER_IDX, CHANNEL, AMPLITUDE), (AMPLITUDE / OFL_EMS), 1E-5));

  assert(Athena_test::isEqual(emScaleTool->undoOnlCalib(DRAWER_IDX, CHANNEL, ADC, onlAdcAmplitude,
                                                        TileRawChannelUnit::OnlineADCcounts), AMPLITUDE, 1E-5));

  assert(Athena_test::isEqual(emScaleTool->undoOnlCalib(DRAWER_IDX, CHANNEL, ADC, onlPicoCoulombsAmplitude,
                                                        TileRawChannelUnit::OnlinePicoCoulombs), AMPLITUDE, 1E-5));

  assert(Athena_test::isEqual(emScaleTool->undoOnlCalib(DRAWER_IDX, CHANNEL, ADC, onlCesPicoCoulombsAmplitude,
                                                        TileRawChannelUnit::OnlineCesiumPicoCoulombs), AMPLITUDE, 1E-5));

  assert(Athena_test::isEqual(emScaleTool->undoOnlCalib(DRAWER_IDX, CHANNEL, ADC, onlMevAmplitude,
                                                        TileRawChannelUnit::OnlineMegaElectronVolts), AMPLITUDE, 1E-5));


  const float adcAmplitude(emScaleTool->channelCalib(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE,
                                                     TileRawChannelUnit::OnlineADCcounts,
                                                     TileRawChannelUnit::ADCcounts));
  assert(Athena_test::isEqual(adcAmplitude, AMPLITUDE));


  const float nlnCis((OFL_CIS_NLN[0][3] - OFL_CIS_NLN[0][2]) / (OFL_CIS_NLN[0][1] - OFL_CIS_NLN[0][0]));
  const float nlnLas((OFL_LAS_NLN[0][3] - OFL_LAS_NLN[0][2]) / (OFL_LAS_NLN[0][1] - OFL_LAS_NLN[0][0]));

  const float picoCoulombsAmplitude(emScaleTool->channelCalib(DRAWER_IDX, CHANNEL, ADC, onlPicoCoulombsAmplitude,
                                                              TileRawChannelUnit::OnlinePicoCoulombs,
                                                              TileRawChannelUnit::PicoCoulombs));
  assert(Athena_test::isEqual(picoCoulombsAmplitude, (AMPLITUDE / OFL_CIS) * nlnCis, 1E-5));

  const float cesPicoCoulombsAmplitude(emScaleTool->channelCalib(DRAWER_IDX, CHANNEL, ADC, onlCesPicoCoulombsAmplitude,
                                                                 TileRawChannelUnit::OnlineCesiumPicoCoulombs,
                                                                 TileRawChannelUnit::CesiumPicoCoulombs));
  assert(Athena_test::isEqual(cesPicoCoulombsAmplitude,
                              (AMPLITUDE / OFL_CIS / OFL_LAS / OFL_CES) * nlnCis * nlnLas, 1E-5));

  const float mevAmplitude(emScaleTool->channelCalib(DRAWER_IDX, CHANNEL, ADC, onlMevAmplitude,
                                                     TileRawChannelUnit::OnlineMegaElectronVolts,
                                                     TileRawChannelUnit::MegaElectronVolts));
  assert(Athena_test::isEqual(mevAmplitude,
                              (AMPLITUDE / OFL_CIS / OFL_LAS / OFL_CES / OFL_EMS) * nlnCis * nlnLas, 1E-5));

  assert(Athena_test::isEqual(emScaleTool->getCesRefLas(DRAWER_IDX, CHANNEL, ADC), CES_REF_LAS));
  assert(Athena_test::isEqual(emScaleTool->getCesRefHv(DRAWER_IDX, CHANNEL), CES_REF_HV));
  assert(Athena_test::isEqual(emScaleTool->getCesRefTemp(DRAWER_IDX, CHANNEL), CES_REF_TEMP));

}



void testTileCalibEms(void) {

  std::cout << "\nTest TileCalibEms\n";

  std::unique_ptr<TileEMScale> emScale = getCalibEms();

  std::cout << "" << std::endl;
  std::cout << "ONL CIS: " << (1.0F / emScale->applyOnlineChargeCalibration(DRAWER_IDX, CHANNEL, ADC, 1.0F)) << std::endl;
  std::cout << "ONL LAS: " << (1.0F / emScale->applyOnlineLaserCalibration(DRAWER_IDX, CHANNEL, 1.0F)) << std::endl;
  std::cout << "ONL CES: " << (1.0F / emScale->applyOnlineCesiumCalibration(DRAWER_IDX, CHANNEL, 1.0F)) << std::endl;
  std::cout << "ONL EMS: " << (1.0F / emScale->applyOnlineEMScaleCalibration(DRAWER_IDX, CHANNEL, 1.0F)) << std::endl;

  std::cout << std::endl;

  std::cout << "OFL CIS: " << (1.0F / emScale->applyChargLinearCalibration(DRAWER_IDX, CHANNEL, ADC, 1.0F)) << std::endl;
  std::cout << "OFL CIS NLN [" << AMPLITUDE << "]: "
            << (emScale->applyChargeNonLinearCalibration(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE)) << std::endl;

  std::cout << "OFL LAS: " << (1.0F / emScale->applyLaserLinearCalibration(DRAWER_IDX, CHANNEL, 1.0F)) << std::endl;
  std::cout << "OFL LAS NLN [" << AMPLITUDE << "]: "
            << (emScale->applyLaserNonLinearCalibration(DRAWER_IDX, CHANNEL, AMPLITUDE)) << std::endl;

  std::cout << "OFL CES: " << (1.0F / emScale->applyCesiumCalibration(DRAWER_IDX, CHANNEL, 1.0F)) << std::endl;
  std::cout << "OFL EMS: " << (1.0F / emScale->applyEMScaleCalibration(DRAWER_IDX, CHANNEL, 1.0F)) << std::endl;
  std::cout << std::endl;

  checkEMScale(emScale.get());

  assert(Athena_test::isEqual(emScale->applyChargLinearCalibration(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE), (AMPLITUDE / OFL_CIS), 1E-5));
  assert(Athena_test::isEqual(emScale->applyLaserLinearCalibration(DRAWER_IDX, CHANNEL, AMPLITUDE), (AMPLITUDE / OFL_LAS), 1E-5));

}



void testTileCalibEmsCondAlg(ISvcLocator* svcLoc) {

  std::cout << "\nTest TileCalibEmsCondAlg\n";

  ServiceHandle<StoreGateSvc> detStore("DetectorStore", "");
  assert(detStore.retrieve().isSuccess());

  std::unique_ptr<CaloLVL1_ID> caloLVL1_ID = std::make_unique<CaloLVL1_ID>();
  assert(detStore->record(caloLVL1_ID.release(), "CaloLVL1_ID").isSuccess());


  std::unique_ptr<TileEMScaleCondAlg> alg = std::make_unique<TileEMScaleCondAlg>("TileCalibEmsCondAlg", svcLoc);
  alg->addRef();

  assert( (alg->setProperty("OnlCisProxy", "TileCondProxyOnlCisMock")).isSuccess() );
  assert( (alg->setProperty("OnlLasProxy", "TileCondProxyOnlLasMock")).isSuccess() );
  assert( (alg->setProperty("OnlCesProxy", "TileCondProxyOnlCesMock")).isSuccess() );
  assert( (alg->setProperty("OnlEmsProxy", "TileCondProxyOnlEmsMock")).isSuccess() );

  assert( (alg->setProperty("OflCisLinProxy", "TileCondProxyOflCisLinMock")).isSuccess() );
  assert( (alg->setProperty("OflCisNlnProxy", "TileCondProxyOflCisNlnMock")).isSuccess() );
  assert( (alg->setProperty("OflLasLinProxy", "TileCondProxyOflLasLinMock")).isSuccess() );
  assert( (alg->setProperty("OflLasNlnProxy", "TileCondProxyOflLasNlnMock")).isSuccess() );
  assert( (alg->setProperty("OflCesProxy", "TileCondProxyOflCesMock")).isSuccess() );
  assert( (alg->setProperty("OflEmsProxy", "TileCondProxyOflEmsMock")).isSuccess() );

  assert( (alg->setProperty("OnlCacheUnit", "OnlineMegaElectronVolts")).isSuccess() );
  assert( (alg->setProperty("TileEMScale", TILE_TEST_ALG_EMS)).isSuccess() );
  assert( (alg->initialize()).isSuccess() );

  assert( (alg->execute()).isSuccess() );

  SG::ReadCondHandleKey<TileEMScale> emScaleKey{TILE_TEST_ALG_EMS};
  assert(emScaleKey.initialize().isSuccess());

  SG::ReadCondHandle<TileEMScale> emScale{emScaleKey};
  assert(emScale.isValid());

  EventIDRange eventRange;
  emScale.range(eventRange);

  assert( eventRange == EVENT_RANGE);

  checkEMScale(emScale.retrieve());

  assert(Athena_test::isEqual(emScale->applyChargLinearCalibration(DRAWER_IDX, CHANNEL, ADC, AMPLITUDE), (AMPLITUDE / OFL_CIS), 1E-5));
  assert(Athena_test::isEqual(emScale->applyLaserLinearCalibration(DRAWER_IDX, CHANNEL, AMPLITUDE), (AMPLITUDE / OFL_LAS), 1E-5));

}



void testTileCondToolEmscale(ISvcLocator* svcLoc) {

  std::cout << "\nTest TileCondToolEmscale\n";

  {
    SG::WriteCondHandleKey<TileEMScale> emScaleKey{TILE_TEST_EMS};
    assert(emScaleKey.initialize().isSuccess());

    SG::WriteCondHandle<TileEMScale> emScale{emScaleKey};
    std::unique_ptr<TileEMScale> calibData = getCalibEms();
    assert(emScale.record(EVENT_RANGE, calibData.release()).isSuccess());
  }

  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithm", svcLoc);
  alg->addRef();

  std::unique_ptr<TileCondToolEmscale> toolEmscale
    =  std::make_unique<TileCondToolEmscale>("TileCondToolEmscale",
                                             "TileCondToolEmscaleTest",
                                             alg.get());
  toolEmscale->addRef();

  assert(toolEmscale->setProperty("TileEMScale", TILE_TEST_EMS));
  assert(toolEmscale->initialize().isSuccess());


  checkEMScaleTool(toolEmscale.get());


}


int main ATLAS_NOT_THREAD_SAFE () {


  std::ofstream jo(TILE_JO_NAME);
  jo << "ApplicationMgr.ExtSvc += {\"StoreGateSvc/DetectorStore\",  \"StoreGateSvc/ConditionStore\" };" << std::endl;
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


  testTileCalibEms();
  testTileCalibEmsCondAlg(svcLoc);
  testTileCondToolEmscale(svcLoc);

  return 0;
}
