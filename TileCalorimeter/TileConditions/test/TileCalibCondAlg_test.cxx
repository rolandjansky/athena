/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

// Tile includes
#include "../src/TileCalibCondAlg.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileConditions/TileCalibData.h"
#include "TileConditions/ITileCondProxy.h"

#include "TestTools/FLOATassert.h"
#include "TestTools/initGaudi.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/SGtests.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadCondHandle.h"

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

static const std::string TILE_JO_NAME("jobOptions_TileCalibCondAlgTest.py");
static const std::string TILE_TEST_CONDITION("TileTest");
static const unsigned int OBJ_VERSION(0);
static const unsigned int DEF_DRAWER_IDX(0);

static const std::vector<float> DEF_CONDITIONS{0.0F, 1.0F, 2.0F, 3.0F};
static const  std::vector<std::vector<float> > DEF_COND_DATA = {
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



template<typename T = TileCalibDrawerFlt>
class TileCondProxyMock: public AthAlgTool, virtual public ITileCondProxy<T> {
  public:

    TileCondProxyMock(const std::string& type, const std::string& name, const IInterface* parent)
      :AthAlgTool(type, name, parent)
    {
        declareInterface<ITileCondProxy<T> >(this);
    };

    virtual ~TileCondProxyMock() {};

    virtual StatusCode fillCalibData(TileCalibData<T>& calibData, EventIDRange& eventRange) const override {

      coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
      spec->extend("TileCalibBlobFlt", "blob");
      coral::AttributeList defaultList(*spec);
      coral::Blob& defaultBlob = defaultList["TileCalibBlobFlt"].data<coral::Blob>();

      std::unique_ptr<TileCalibDrawerFlt> calibDrawer(TileCalibDrawerFlt::getInstance(defaultBlob, DEF_COND_DATA, 1, OBJ_VERSION));
        // Trick to make calib drawer to own blob
      std::unique_ptr<TileCalibDrawerFlt> defaultCalibDrawer = std::make_unique<TileCalibDrawerFlt>(*calibDrawer);
      calibData.setCalibDrawer(DEF_DRAWER_IDX, defaultCalibDrawer.release());

      for (unsigned int drawerIdx = 1; drawerIdx < TileCalibUtils::MAX_DRAWERIDX; ++drawerIdx) {
        unsigned int defaultDrawerIdx = TileCalibUtils::getDefaultDrawerIdx(drawerIdx);
        calibData.setCalibDrawer(drawerIdx, calibData.getCalibDrawer(defaultDrawerIdx));
      }


      spec->release();

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



typedef TileCondProxyMock< TileCalibDrawerFlt > TileCondProxyFltMock;
DECLARE_COMPONENT( TileCondProxyFltMock )


void test1(ISvcLocator* svcLoc) {

  std::cout << "test TileCalibCondAlg\n";

  ServiceHandle<StoreGateSvc> conditionStore("ConditionStore", "");
  assert(conditionStore.retrieve().isSuccess());

  //TileCalibFltCondAlg
  std::unique_ptr<TileCalibFltCondAlg> alg = std::make_unique<TileCalibFltCondAlg>("TileCalibFltCondAlgTest", svcLoc);
  alg->addRef();

  assert( (alg->setProperty("ConditionsProxy", "TileCondProxyMock<TileCalibDrawerFlt>/TileCondProxyFltMock")).isSuccess() );
  assert( (alg->setProperty("TileCalibData", TILE_TEST_CONDITION)).isSuccess() );
  assert( (alg->initialize()).isSuccess() );

  assert( (alg->execute()).isSuccess() );

  EventIDBase eventId;
  eventId.set_run_number(EventIDBase::UNDEFNUM / 4);
  eventId.set_lumi_block(EventIDBase::UNDEFNUM / 4);

  EventContext ctx;
  ctx.setEventID(eventId);
  ctx.setExtension( Atlas::ExtendedEventContext(&*conditionStore) );
  Gaudi::Hive::setCurrentContext(ctx);

  SG::ReadCondHandleKey<TileCalibData<TileCalibDrawerFlt>> calibDataKey{TILE_TEST_CONDITION};
  assert(calibDataKey.initialize().isSuccess());


  SG::ReadCondHandle<TileCalibDataFlt> calibData{calibDataKey};
  //  SG::ReadCondHandle<TileCalibData<TileCalibDrawerFlt>> calibData{calibDataKey};

  assert(calibData.isValid());

  EventIDRange eventRange;
  calibData.range(eventRange);

  assert( eventRange == EVENT_RANGE);

  // Default drawer with calibrations constants should present always
  const TileCalibDrawerFlt* defaultCalibDrawer = calibData->getCalibDrawer(DEF_DRAWER_IDX);
  assert(defaultCalibDrawer);

  defaultCalibDrawer->dump();

  assert(defaultCalibDrawer->getObjVersion() == OBJ_VERSION);
  assert(defaultCalibDrawer->getNChans() == 1U);
  assert(defaultCalibDrawer->getNGains() == DEF_COND_DATA.size());
  assert(defaultCalibDrawer->getObjSizeUint32() == MAX_INDEX * sizeof(float) / sizeof(uint32_t));

  unsigned int channel(0);
  unsigned int gain(0);

  for (unsigned int index = 0; index < MAX_INDEX; ++index) {
    assert(Athena_test::isEqual(defaultCalibDrawer->getData(channel, gain, index), DEF_CONDITIONS[index]));
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


  test1(svcLoc);

  return 0;
}
