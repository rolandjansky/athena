/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "TileConditions/TileCondToolTMDB.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"

#include "CoralBase/Blob.h"

#include "TestTools/FLOATassert.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"


#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


static const std::string TILE_JO_NAME("jobOptions_TileCondToolTMDBTest.py");
static const std::string TILE_TMDB("TileTMDB");
static const unsigned int OBJ_VERSION(0);
static const int N_CHANNELS(4);
static const unsigned int DEF_DRAWER_IDX(0);

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

    virtual StatusCode execute() {
      return StatusCode::SUCCESS;
    }
};

enum DRAWER {DELAY_DRAWER_IDX = 0, CALIB_DRAWER_IDX = 1, THRESHOLD_DRAWER_IDX = 2, TMF_DRAWER_IDX = 3};

static const std::vector<std::vector<float> > DEFAULTS = {
  {10}, // DELAY
  {1, 2}, // CALIB
  {100}, // THRESHOLD
  {1, 2, 4, 8, 4, 2, 1} // TMF
};


void test1() {

  std::cout << "test1 TileCondToolTMDB\n";


  std::ofstream jo(TILE_JO_NAME);
  jo << "ApplicationMgr.ExtSvc += { \"StoreGateSvc/ConditionStore\" };" << std::endl;
  jo.close();

  ISvcLocator* svcLoc;
  if (!Athena_test::initGaudi(TILE_JO_NAME, svcLoc)) {
    std::cerr << "ERROR This test can not be run, can not get ServiceLocator" << std::endl;
  }

  ServiceHandle<StoreGateSvc> conditionStore("ConditionStore", "");
  assert(conditionStore.retrieve().isSuccess());

  SG::WriteCondHandleKey<TileCalibDataFlt> calibDataKey{TILE_TMDB};
  assert(calibDataKey.initialize().isSuccess());

  SG::WriteCondHandle<TileCalibDataFlt> calibData{calibDataKey};

  std::unique_ptr<TileCalibDataFlt> data = std::make_unique<TileCalibDataFlt>();

  unsigned int drawerIdx(DEF_DRAWER_IDX);
  for(const std::vector<float>& def : DEFAULTS) {

    std::vector<std::vector<float> > defs(1, def);
    std::unique_ptr<coral::Blob> blob = std::make_unique<coral::Blob>(0);
    std::unique_ptr<TileCalibDrawerFlt> calibDrawer(TileCalibDrawerFlt::getInstance(*blob, defs, N_CHANNELS, OBJ_VERSION));
    // Trick to make calib drawer to own blob
    std::unique_ptr<TileCalibDrawerFlt> tmdbDrawer = std::make_unique<TileCalibDrawerFlt>(*calibDrawer);

    if (drawerIdx == DELAY_DRAWER_IDX) {
      tmdbDrawer->setData(0, 0, 0, 10);
      tmdbDrawer->setData(1, 0, 0, 20);
      tmdbDrawer->setData(2, 0, 0, 30);
      tmdbDrawer->setData(3, 0, 0, 40);
    } else if (drawerIdx == THRESHOLD_DRAWER_IDX) {
      tmdbDrawer->setData(0, 0, 0, 100);
      tmdbDrawer->setData(1, 0, 0, 200);
      tmdbDrawer->setData(2, 0, 0, 1000);
      tmdbDrawer->setData(3, 0, 0, 2000);
    }

    data->setCalibDrawer(drawerIdx, tmdbDrawer.release());
    ++drawerIdx;
  }

  for (unsigned int drawerIdx = 0; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++drawerIdx) {
    if (!data->getCalibDrawer(drawerIdx)) {
      unsigned int defaultDrawerIdx = TileCalibUtils::getDefaultDrawerIdx(drawerIdx);
      data->setCalibDrawer(drawerIdx, data->getCalibDrawer(defaultDrawerIdx));
    }
  }

  assert(calibData.record(EVENT_RANGE, data.release()).isSuccess());

  EventIDBase eventId;
  eventId.set_run_number(EventIDBase::UNDEFNUM / 4);
  eventId.set_lumi_block(EventIDBase::UNDEFNUM / 4);

  EventContext ctx;
  ctx.setEventID(eventId);
  ctx.setExtension( Atlas::ExtendedEventContext(&*conditionStore) );
  Gaudi::Hive::setCurrentContext(ctx);


  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithm", svcLoc);
  alg->addRef();

  std::unique_ptr<TileCondToolTMDB> tool = std::make_unique<TileCondToolTMDB>("TileCondToolTMDB",
                                                                              "TileCondToolTMDBTest",
                                                                              alg.get());
  tool->addRef();

  assert(tool->setProperty("TileTMDBThreshold", TILE_TMDB));
  assert(tool->setProperty("TileTMDBDelay", TILE_TMDB));
  assert(tool->setProperty("TileTMDBTMF", TILE_TMDB));
  assert(tool->setProperty("TileTMDBCalib", TILE_TMDB));
  assert(tool->initialize());


  float delay = tool->getDelay(DELAY_DRAWER_IDX, 0);
  assert(Athena_test::isEqual(delay, 10.0));

  delay = tool->getDelay(DELAY_DRAWER_IDX, 1);
  assert(Athena_test::isEqual(delay, 20.0));

  delay = tool->getDelay(DELAY_DRAWER_IDX, 2);
  assert(Athena_test::isEqual(delay, 30.0));

  delay = tool->getDelay(DELAY_DRAWER_IDX, 3);
  assert(Athena_test::isEqual(delay, 40.0));



  float threshold = tool->getThreshold(THRESHOLD_DRAWER_IDX, 0);
  assert(Athena_test::isEqual(threshold, 100.0));

  threshold = tool->getThreshold(THRESHOLD_DRAWER_IDX, 1);
  assert(Athena_test::isEqual(threshold, 200.0));

  threshold = tool->getThreshold(THRESHOLD_DRAWER_IDX, 2);
  assert(Athena_test::isEqual(threshold, 1000.0));

  threshold = tool->getThreshold(THRESHOLD_DRAWER_IDX, 3);
  assert(Athena_test::isEqual(threshold, 2000.0));


  float a;
  float b;
  tool->getCalib(CALIB_DRAWER_IDX, 0, a, b);
  assert(Athena_test::isEqual(a, 1.0));
  assert(Athena_test::isEqual(b, 2.0));

  TMDB::Weights weights;
  unsigned int nWeights = tool->getWeights(TMF_DRAWER_IDX, 0, weights);
  assert(Athena_test::isEqual(nWeights, 7));
  assert(Athena_test::isEqual(weights[0], 1.0));
  assert(Athena_test::isEqual(weights[1], 2.0));
  assert(Athena_test::isEqual(weights[2], 4.0));
  assert(Athena_test::isEqual(weights[3], 8.0));
  assert(Athena_test::isEqual(weights[4], 4.0));
  assert(Athena_test::isEqual(weights[5], 2.0));
  assert(Athena_test::isEqual(weights[6], 1.0));


  float amplitude = tool->channelCalib(CALIB_DRAWER_IDX, 0, 1.0);
  assert(Athena_test::isEqual(amplitude, 3.0));

  std::vector<float> samples = {10, 20, 40, 80, 40, 20, 10};
  amplitude = tool->channelCalib(TMF_DRAWER_IDX, 0, samples);
  assert(Athena_test::isEqual(amplitude, 1062.0));

}


int main() {

  test1();

  return 0;
}
