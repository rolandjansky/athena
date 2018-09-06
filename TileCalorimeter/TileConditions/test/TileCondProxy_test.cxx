/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

// Tile includes
#include "TileCalibBlobObjs/Exception.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"

#include "TileConditions/ITileCondProxy.h"
#include "TileConditions/TileCondProxyFile.h"
#include "TileConditions/TileCondProxyWrapper.h"
#include "TileConditions/TileCalibData.h"


#include "TestTools/FLOATassert.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"

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
#include <memory>

static const std::string TILE_JO_NAME("jobOptions_TileCondProxyTest.py");
static const std::string TILE_FILE_NAME("TileConditions.test");
static const std::string TILE_BCH_FILE_NAME("TileBadChannels.test");
static const std::string TILE_FOLDER("/TILE/TEST");
static const unsigned int OBJ_VERSION(0);
static const unsigned int OBJ_BCH_VERSION(0);
static const unsigned int DEF_DRAWER_IDX(0);
static const unsigned int FRAG_ID(0x100);
static const unsigned int MAX_CHANNEL(64);
static const unsigned int MAX_GAIN(3);

static const std::vector<float> DEF_CONDITIONS{0.1F, 0.01F, 0.001F};
static const  std::vector<std::vector<float> > DEF_COND_DATA = {
  DEF_CONDITIONS
};

static const unsigned int MAX_INDEX(DEF_CONDITIONS.size());

static const std::vector<unsigned int> DEF_BAD_STATUS{1, 2, 3};

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

float getBadChannelStatus(unsigned channel, unsigned int gain) {
  return gain * 100 + channel;
}


void testCalibDataFlt(const TileCalibData<TileCalibDrawerFlt>& calibData) {

  // Default drawer with calibrations constants should present always
  const TileCalibDrawerFlt* defaultCalibDrawer = calibData.getCalibDrawer(DEF_DRAWER_IDX);
  assert(defaultCalibDrawer);

  defaultCalibDrawer->dump();

  unsigned int channel(0);
  unsigned int gain(0);

  assert(defaultCalibDrawer->getObjVersion() == OBJ_VERSION);
  assert(defaultCalibDrawer->getNChans() == 1U);
  assert(defaultCalibDrawer->getNGains() == DEF_COND_DATA.size());
  assert(defaultCalibDrawer->getObjSizeUint32() == MAX_INDEX * sizeof(float) / sizeof(uint32_t));

  for (unsigned int index = 0; index < MAX_INDEX; ++index) {
    assert(Athena_test::isEqual(defaultCalibDrawer->getData(channel, gain, index), DEF_CONDITIONS[index]));
  }

  // Default policy is to return default calib drawer in the case of absence of required one
  assert(defaultCalibDrawer == calibData.getCalibDrawer(10));
  assert(defaultCalibDrawer == calibData.getCalibDrawer(200));

  // Expected exeption because maximum drawerIdx is 275
  EXPECT_EXCEPTION(TileCalib::IndexOutOfRange, calibData.getCalibDrawer(276));


  unsigned int drawerIdx( TileCalibUtils::getDrawerIdxFromFragId(FRAG_ID) );
  const TileCalibDrawerFlt* calibDrawer = calibData.getCalibDrawer(drawerIdx);
  assert(calibDrawer);

  assert(calibDrawer->getObjVersion() == OBJ_VERSION);
  assert(calibDrawer->getNChans() == MAX_CHANNEL);
  assert(calibDrawer->getNGains() == MAX_GAIN);
  assert(calibDrawer->getObjSizeUint32() == MAX_INDEX * sizeof(float) / sizeof(uint32_t));

  for (unsigned int channel = 0; channel < MAX_CHANNEL; ++channel) {
    for (unsigned int gain = 0; gain < MAX_GAIN; ++gain) {
      for (unsigned int index = 0; index < MAX_INDEX; ++index) {
        float data = calibDrawer->getData(channel, gain, index);
        assert(Athena_test::isEqual(data, getCondtionsData(channel, gain, index)));
      }
    }
  }


}


void testTileCondProxyFileFlt(ISvcLocator* svcLoc) {

  std::cout << "\nTest TileCodProxyFileFlt\n";


  std::ofstream conditionsFile(TILE_FILE_NAME);
  if (!conditionsFile) {
    std::cerr << "ERROR: impossible to create temporary file with conditions for test!" << std::endl;
  } else {

    conditionsFile << "Temporary input file with dummy conditions for test." << std::endl;
    conditionsFile << std::endl;
    conditionsFile << "OBJVERSION " << std::hex << OBJ_VERSION << std::dec << std::endl;
    conditionsFile << std::endl;

    conditionsFile << "FragID Channel Gain \tConditions" << std::endl;
    conditionsFile << "0x" << std::hex << DEF_DRAWER_IDX << std::dec << "\t0 \t0";

    for (float data : DEF_CONDITIONS) {
      conditionsFile << "\t" << data;
    }

    conditionsFile << std::endl;
    conditionsFile << std::endl;

    for (unsigned int channel = 0; channel < MAX_CHANNEL; ++channel) {
      for (unsigned int gain = 0; gain < MAX_GAIN; ++gain) {
        conditionsFile << "0x" << std::hex << FRAG_ID << std::dec
                       << "\t" << channel << "\t" << gain;

        for (unsigned int index = 0; index < MAX_INDEX; ++index) {
          conditionsFile << "\t" << getCondtionsData(channel, gain, index);
        }

        conditionsFile << std::endl;
      }
    }

    conditionsFile << std::endl;
    conditionsFile.close();
  }



  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithm", svcLoc);
  alg->addRef();

  std::unique_ptr<TileCondProxyFileFlt> proxyFile = std::make_unique<TileCondProxyFileFlt>("TileCondProxyFile",
                                                                                      "TileCondProxyFileTest",
                                                                                      alg.get());
  proxyFile->addRef();

  assert(proxyFile->setProperty("Source", TILE_FILE_NAME));
  assert(proxyFile->setProperty("OutputLevel", 1)); // 1 == VERBOSE
  assert(proxyFile->initialize().isSuccess());

  assert(proxyFile->getCalibDrawer(DEF_DRAWER_IDX));

  EventIDRange eventRange;
  std::unique_ptr<TileCalibData<TileCalibDrawerFlt>> calibData = std::make_unique<TileCalibData<TileCalibDrawerFlt>>();
  assert( proxyFile->fillCalibData(*calibData, eventRange) );

  // Calibration data from file are valid for all runs
  assert(eventRange == ALL_EVENT_RANGE);

  testCalibDataFlt(*calibData);
}


void testTileCondProxyCool(ISvcLocator* svcLoc) {

  std::cout << "\nTest TileCodProxyCool\n";


  ServiceHandle<StoreGateSvc> detStore("DetectorStore", "");
  assert(detStore.retrieve().isSuccess());

  ServiceHandle<StoreGateSvc> conditionStore("ConditionStore", "");
  assert(conditionStore.retrieve().isSuccess());


  SG::WriteCondHandleKey<CondAttrListCollection> folderKey{TILE_FOLDER};
  assert(folderKey.initialize().isSuccess());

  SG::WriteCondHandle<CondAttrListCollection> folder{folderKey};

  CondAttrListCollection* attrListColl = new CondAttrListCollection(true);
  IOVRange range(ALL_EVENT_RANGE);

  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("TileCalibBlobFlt", "blob");
  coral::AttributeList defaultList(*spec);
  coral::Blob& defaultBlob = defaultList["TileCalibBlobFlt"].data<coral::Blob>();

  std::unique_ptr<TileCalibDrawerFlt> defaultCalibDrawerFlt(TileCalibDrawerFlt::getInstance(defaultBlob, DEF_COND_DATA, 1, OBJ_VERSION));

  attrListColl->add(DEF_DRAWER_IDX, defaultList);
  attrListColl->add(DEF_DRAWER_IDX, range);

  std::vector<std::vector<float>> channelData{MAX_GAIN, std::vector<float>(MAX_INDEX, 0.0F)};

  coral::AttributeList drawerList(*spec);
  coral::Blob& drawerBlob = drawerList["TileCalibBlobFlt"].data<coral::Blob>();

  unsigned int filledDrawerIdx( TileCalibUtils::getDrawerIdxFromFragId(FRAG_ID) );
  std::unique_ptr<TileCalibDrawerFlt> calibDrawerFlt(TileCalibDrawerFlt::getInstance(drawerBlob, channelData, MAX_CHANNEL, OBJ_VERSION));

  std::vector<float> gainData(MAX_INDEX, 0.0F);
  for (unsigned int channel = 0; channel < MAX_CHANNEL; ++channel) {
    for (unsigned int gain = 0; gain < MAX_GAIN; ++gain) {
      for (unsigned int index = 0; index < MAX_INDEX; ++index) {
        gainData[index] = getCondtionsData(channel, gain, index);
      }
      calibDrawerFlt->setData(channel, gain, gainData);
    }
  }

  attrListColl->add(filledDrawerIdx, drawerList);
  attrListColl->add(filledDrawerIdx, range);


  for (unsigned int drawerIdx = 1; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++drawerIdx) {
    if (drawerIdx == filledDrawerIdx) continue;
    //=== create attribute list for this drawer
    coral::AttributeList emptyList(*spec);
    attrListColl->add(drawerIdx, emptyList);
    attrListColl->add(drawerIdx, range);
  }

  assert(folder.record(ALL_EVENT_RANGE, attrListColl).isSuccess());
  //  assert(detStore->record(attrListColl, TILE_FOLDER));

  spec->release();
  // cppcheck-suppress memleak
  spec = nullptr;

  EventIDBase eventId;
  eventId.set_run_number(EventIDBase::UNDEFNUM / 2);
  eventId.set_lumi_block(EventIDBase::UNDEFNUM / 2);


  EventContext ctx;
  ctx.setEventID(eventId);
  ctx.setExtension( Atlas::ExtendedEventContext(&*conditionStore) );
  Gaudi::Hive::setCurrentContext(ctx);


  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithmCondAlg", svcLoc);
  alg->addRef();

  std::unique_ptr<TileCondProxyCoolFlt> proxyCool = std::make_unique<TileCondProxyCoolFlt>("TileCondProxyCool",
                                                                                           "TileCondProxyCoolTest",
                                                                                           alg.get());
  proxyCool->addRef();

  assert(proxyCool->setProperty("Source", TILE_FOLDER));
  assert(proxyCool->setProperty("OutputLevel", 1)); // 1 == VERBOSE
  assert(proxyCool->initialize().isSuccess());
  assert(proxyCool->getCalibDrawer(DEF_DRAWER_IDX) == nullptr);

  EventIDRange eventRange;
  std::unique_ptr<TileCalibData<TileCalibDrawerFlt>> calibData = std::make_unique<TileCalibData<TileCalibDrawerFlt>>();
  assert( proxyCool->fillCalibData(*calibData, eventRange) );

  assert(eventRange == ALL_EVENT_RANGE);

  testCalibDataFlt(*calibData);

}


void testTileCondProxyFileBch(ISvcLocator* svcLoc) {

  std::cout << "\nTest TileCodProxyFileBch\n";

  std::ofstream bchFile(TILE_BCH_FILE_NAME);
  if (!bchFile) {
    std::cerr << "ERROR: impossible to create temporary file with Tile bad channels for test!" << std::endl;
  } else {

    bchFile << "Temporary input file with dummy Tile bad channel status for test." << std::endl;
    bchFile << std::endl;
    bchFile << "OBJVERSION " << std::hex << OBJ_BCH_VERSION << std::dec << std::endl;
    bchFile << std::endl;

    bchFile << "FragID Channel Gain \tStatus" << std::endl;
    bchFile << "0x" << std::hex << DEF_DRAWER_IDX << std::dec << "\t0 \t0";

    for (unsigned int status : DEF_BAD_STATUS) {
      bchFile << "\t" << status;
    }

    bchFile << std::endl;
    bchFile << std::endl;

    for (unsigned int channel = 0; channel < MAX_CHANNEL; ++channel) {
      bchFile << "0x" << std::hex << FRAG_ID << std::dec
              << "\t" << channel << "\t" << 0;
        for (unsigned int gain = 0; gain < 3; ++gain) {
          bchFile << "\t" << getBadChannelStatus(channel, gain);
        }

        bchFile << std::endl;
    }

    bchFile << std::endl;
    bchFile.close();
  }



  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithmCondAlg", svcLoc);
  alg->addRef();

  std::unique_ptr<TileCondProxyFileBch> proxyFile = std::make_unique<TileCondProxyFileBch>("TileCondProxyFile",
                                                                                           "TileCondProxyFileBchTest",
                                                                                           alg.get());
  proxyFile->addRef();

  assert(proxyFile->setProperty("Source", TILE_BCH_FILE_NAME));
  assert(proxyFile->setProperty("OutputLevel", 1)); // 1 == VERBOSE
  assert(proxyFile->initialize().isSuccess());
  assert(proxyFile->getCalibDrawer(DEF_DRAWER_IDX) == nullptr);

  EventIDRange eventRange;
  std::unique_ptr<TileCalibData<TileCalibDrawerBch>> calibData = std::make_unique<TileCalibData<TileCalibDrawerBch>>();
  assert( proxyFile->fillCalibData(*calibData, eventRange) );

  // Calibration data from file are valid for all runs
  assert(eventRange == ALL_EVENT_RANGE);


  // Default drawer with calibrations constants should present always
  const TileCalibDrawerBch* defaultCalibDrawer = calibData->getCalibDrawer(DEF_DRAWER_IDX);
  assert(defaultCalibDrawer);

  defaultCalibDrawer->dump();

  assert(defaultCalibDrawer->getObjVersion() == OBJ_BCH_VERSION);
  assert(defaultCalibDrawer->getNChans() == 1U);
  assert(defaultCalibDrawer->getNGains() == 3U);
  assert(defaultCalibDrawer->getObjSizeUint32() == 1U);

  unsigned int channel(0);
  unsigned int adcStatus;
  unsigned int channelStatus;

  for (unsigned int gain = 0; gain < 2; ++gain) {
    defaultCalibDrawer->getStatusWords(channel, gain, adcStatus, channelStatus);
    assert(Athena_test::isEqual(adcStatus, DEF_BAD_STATUS[gain]));
    assert(Athena_test::isEqual(channelStatus, DEF_BAD_STATUS[2]));
  }

  // Default policy is to return default calib drawer in the case of absence of required one
  assert(defaultCalibDrawer == calibData->getCalibDrawer(10));
  assert(defaultCalibDrawer == calibData->getCalibDrawer(200));

  // Expected exeption because maximum drawerIdx is 275
  EXPECT_EXCEPTION(TileCalib::IndexOutOfRange, calibData->getCalibDrawer(276));


  unsigned int drawerIdx( TileCalibUtils::getDrawerIdxFromFragId(FRAG_ID) );
  const TileCalibDrawerBch* calibDrawer = calibData->getCalibDrawer(drawerIdx);
  assert(calibDrawer);

  assert(calibDrawer->getObjVersion() == OBJ_BCH_VERSION);
  assert(calibDrawer->getNChans() == MAX_CHANNEL);
  assert(calibDrawer->getNGains() == 3U);
  assert(calibDrawer->getObjSizeUint32() == 1U);

  for (unsigned int channel = 0; channel < MAX_CHANNEL; ++channel) {
    for (unsigned int gain = 0; gain < 2; ++gain) {

      calibDrawer->getStatusWords(channel, gain, adcStatus, channelStatus);
      assert(Athena_test::isEqual(adcStatus, getBadChannelStatus(channel, gain)));
      assert(Athena_test::isEqual(channelStatus, getBadChannelStatus(channel, 2)));

    }
  }



}

int main() {

  std::ofstream jo(TILE_JO_NAME);
  jo << "ApplicationMgr.ExtSvc += { \"StoreGateSvc/DetectorStore\", \"StoreGateSvc/ConditionStore\" };" << std::endl;
  jo.close();

  ISvcLocator* svcLoc;
  if (!Athena_test::initGaudi(TILE_JO_NAME, svcLoc)) {
    std::cerr << "ERROR This test can not be run, can not get ServiceLocator" << std::endl;
  }


  testTileCondProxyFileFlt(svcLoc);

  testTileCondProxyCool(svcLoc);

  testTileCondProxyFileBch(svcLoc);

  return 0;
}
