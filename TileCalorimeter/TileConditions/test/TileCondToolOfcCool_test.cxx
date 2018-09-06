/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "TileConditions/TileCondToolOfcCool.h"
#include "TileCalibBlobObjs/TileCalibDrawerOfc.h"
#include "TileConditions/TileCalibData.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TestTools/FLOATassert.h"
#include "TestTools/initGaudi.h"

#include "GaudiKernel/EventIDRange.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolHandle.h"

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

const std::vector<float> PHASES = {-2.0, 0.0, 0.1, 0.2, 0.5, 1.0};
const int N_PHASES = PHASES.size();

static const std::string TILE_JO_NAME("jobOptions_TileCondToolOfcTest.py");
static const std::string TILE_OFC("TileOFC_Test");
static const unsigned int OBJ_VERSION(3);
static const int N_CHANNELS(1);
static const int N_GAINS(1);
static const unsigned int N_SAMPLES(1);
 static const unsigned int DEF_DRAWER_IDX(0);


static const EventIDRange EVENTRANGE {
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
  *  to help construct CaloCellFastCopyTool to be tested.
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



void test1() {

  std::cout << "test1 TileCondToolOfcCool\n";

  std::ofstream jo(TILE_JO_NAME);
  jo << "ApplicationMgr.ExtSvc += { \"StoreGateSvc/ConditionStore\" };" << std::endl;
  jo.close();

  ISvcLocator* svcLoc;
  if (!Athena_test::initGaudi(TILE_JO_NAME, svcLoc)) {
    std::cerr << "ERROR This test can not be run, can not get ServiceLocator" << std::endl;
  }

  ServiceHandle<StoreGateSvc> conditionStore("ConditionStore", "");
  assert(conditionStore.retrieve().isSuccess());


  SG::WriteCondHandleKey<TileCalibDataOfc> calibDataKey{TILE_OFC};
  assert(calibDataKey.initialize().isSuccess());

  SG::WriteCondHandle<TileCalibDataOfc> calibData{calibDataKey};

  std::unique_ptr<TileCalibDataOfc> data = std::make_unique<TileCalibDataOfc>();
  /*
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("TileCalibBlobOfc", "blob");
  coral::AttributeList ofcList(*spec);
  coral::Blob& blob = ofcList["TileCalibBlobOfc"].data<coral::Blob>();
  */

  std::unique_ptr<coral::Blob> blob = std::make_unique<coral::Blob>(0);
  std::unique_ptr<TileCalibDrawerOfc> calibDrawer(TileCalibDrawerOfc::getInstance(*blob,
                                                                                  OBJ_VERSION,
                                                                                  N_SAMPLES,
                                                                                  -N_PHASES,
                                                                                  N_CHANNELS,
                                                                                  N_GAINS));
  // Trick to make calib drawer to copy and own blob
  std::unique_ptr<TileCalibDrawerOfc> ofcDrawer = std::make_unique<TileCalibDrawerOfc>(*calibDrawer);

  ofcDrawer->setPhases(0, 0, PHASES);

  for (unsigned int channel = 0; channel < N_CHANNELS; ++channel) {
    for (unsigned int gain = 0; gain < N_GAINS; ++gain) {
      for (float phase : PHASES) {
        for (unsigned int isam = 0; isam < N_SAMPLES; ++isam) {
          ofcDrawer->setOfc(TileCalibDrawerOfc::FieldA, channel, gain, phase, isam, phase + 10.0F);
          ofcDrawer->setOfc(TileCalibDrawerOfc::FieldB, channel, gain, phase, isam, phase + 20.0F);
          ofcDrawer->setOfc(TileCalibDrawerOfc::FieldG, channel, gain, phase, isam, phase + 30.0F);
          if (OBJ_VERSION == 3) {
            ofcDrawer->setOfc(TileCalibDrawerOfc::FieldC, channel, gain, phase, isam, phase + 40.0F);
            ofcDrawer->setOfc(TileCalibDrawerOfc::FieldDG, channel, gain, phase, isam, phase + 50.0F);
          }
        }
      }
    }
  }

  data->setCalibDrawer(DEF_DRAWER_IDX, ofcDrawer.release());

  for (unsigned int drawerIdx = 1; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++drawerIdx) {
    unsigned int defaultDrawerIdx = TileCalibUtils::getDefaultDrawerIdx(drawerIdx);
    data->setCalibDrawer(drawerIdx, data->getCalibDrawer(defaultDrawerIdx));
  }

  //  spec->release();

  assert(calibData.record(EVENTRANGE, data.release()).isSuccess());


  EventIDBase eventId;
  eventId.set_run_number(EventIDBase::UNDEFNUM / 4);
  eventId.set_lumi_block(EventIDBase::UNDEFNUM / 4);

  EventContext ctx;
  ctx.setEventID(eventId);
  ctx.setExtension( Atlas::ExtendedEventContext(&*conditionStore) );
  Gaudi::Hive::setCurrentContext(ctx);


  std::unique_ptr<DummyAlgorithm> alg = std::make_unique<DummyAlgorithm>("DummyAlgorithm", svcLoc);
  alg->addRef();

  std::unique_ptr<TileCondToolOfcCool> tool = std::make_unique<TileCondToolOfcCool>("TileCondToolOfcCool",
                                                                                    "TileCondToolOfcCoolTest",
                                                                                    alg.get());
  tool->addRef();

  assert(tool->setProperty("TileOfc", TILE_OFC));
  assert(tool->initialize());

  int drawerIdx(0);
  int channel(0);
  int gain(0);

  float phase(0.0);

  const TileOfcWeightsStruct* weights;
  weights = tool->getOfcWeights(drawerIdx, channel, gain, phase, true);

  assert(Athena_test::isEqual(weights->w_a[0], 10.0));
  assert(Athena_test::isEqual(weights->w_b[0], 20.0));
  assert(Athena_test::isEqual(weights->g[0], 30.0));
  assert(Athena_test::isEqual(weights->w_c[0], 40.0));
  assert(Athena_test::isEqual(weights->dg[0], 50.0));
  assert(Athena_test::isEqual(phase, 0.0));

  phase = 0.1;
  weights = tool->getOfcWeights(drawerIdx, channel, gain, phase, true);
  assert(Athena_test::isEqual(weights->w_a[0], 10.1));
  assert(Athena_test::isEqual(phase, 0.1));

  phase = 0.2;
  weights = tool->getOfcWeights(drawerIdx, channel, gain, phase, true);
  assert(Athena_test::isEqual(weights->w_a[0], 10.2));
  assert(Athena_test::isEqual(phase, 0.2));

  phase = 0.3;
  weights = tool->getOfcWeights(drawerIdx, channel, gain, phase, true);
  assert(Athena_test::isEqual(weights->w_a[0], 10.2));
  assert(Athena_test::isEqual(phase, 0.2));

  phase = 0.6;
  weights = tool->getOfcWeights(drawerIdx, channel, gain, phase, true);
  assert(Athena_test::isEqual(weights->w_a[0], 10.5));
  assert(Athena_test::isEqual(phase, 0.5));

  phase = 0.8;
  weights = tool->getOfcWeights(drawerIdx, channel, gain, phase, true);
  assert(Athena_test::isEqual(weights->w_a[0], 11));
  assert(Athena_test::isEqual(phase, 1.0));

  phase = 2.0;
  weights = tool->getOfcWeights(drawerIdx, channel, gain, phase, true);
  assert(Athena_test::isEqual(weights->w_a[0], 11));
  assert(Athena_test::isEqual(phase, 1.0));

  phase = -1.0;
  weights = tool->getOfcWeights(drawerIdx, channel, gain, phase, true);
  assert(Athena_test::isEqual(weights->w_a[0], 10.0));
  assert(Athena_test::isEqual(phase, 0.0));

  phase = -1.1;
  weights = tool->getOfcWeights(drawerIdx, channel, gain, phase, true);
  assert(Athena_test::isEqual(weights->w_a[0], 8.0));
  assert(Athena_test::isEqual(phase, -2.0));


  phase = -20.0;
  weights = tool->getOfcWeights(drawerIdx, channel, gain, phase, true);
  assert(Athena_test::isEqual(weights->w_a[0], 8.0));
  assert(Athena_test::isEqual(phase, -2.0));

}


int main() {


  test1();

  return 0;
}
