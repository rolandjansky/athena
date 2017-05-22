/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

//=== AttributeList
//#include "CoralBase/Attribute.h"
#include "TileConditions/TileCondToolTMDB.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileConditions/ITileCondProxy.h"

#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"

#include "TestTools/FLOATassert.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


static const unsigned int OBJVERSION(0);
static const int NCHANNELS(4);
//static const int NGAINS(1);


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

static const std::vector<std::vector<float> > defaults = {
  {10}, // DELAY
  {1, 2}, // CALIB
  {100}, // THRESHOLD
  {1, 2, 4, 8, 4, 2, 1} // TMF
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
  
    StatusCode initialize() { 

      for ( const std::vector<float>& def : defaults) {
        coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
        spec->extend("TileCalibBlob", "blob");
        m_lists.push_back( new coral::AttributeList(*spec) );
        coral::Blob& blob = (*m_lists.back())["TileCalibBlob"].template data<coral::Blob>();
        std::vector<std::vector<float> > defs(1, def);
        m_drawers.push_back(TileCalibDrawerFlt::getInstance(blob, defs, NCHANNELS, OBJVERSION));
      }

      m_drawers.at(DELAY_DRAWER_IDX)->setData(TMDB::D5L, 0, 0, 10);
      m_drawers.at(DELAY_DRAWER_IDX)->setData(TMDB::D5R, 0, 0, 20);
      m_drawers.at(DELAY_DRAWER_IDX)->setData(TMDB::D6L, 0, 0, 30);
      m_drawers.at(DELAY_DRAWER_IDX)->setData(TMDB::D6R, 0, 0, 40);

      m_drawers.at(THRESHOLD_DRAWER_IDX)->setData(TMDB::D6LOW, 0, 0, 100);
      m_drawers.at(THRESHOLD_DRAWER_IDX)->setData(TMDB::D6HIGH, 0, 0, 200);
      m_drawers.at(THRESHOLD_DRAWER_IDX)->setData(TMDB::D5D6LOW, 0, 0, 1000);
      m_drawers.at(THRESHOLD_DRAWER_IDX)->setData(TMDB::D5D6HIGH, 0, 0, 2000);

      return StatusCode::SUCCESS; 
    };

    StatusCode finalize() { 
      for (T* drawer : m_drawers) delete drawer;
      for (coral::AttributeList* list : m_lists) delete list;

      return StatusCode::SUCCESS; 
    };
  
  const T* getCalibDrawer(unsigned int drawerIdx ) const { return m_drawers.at(drawerIdx);};

  private:
  std::vector<coral::AttributeList*> m_lists;
  std::vector<T*> m_drawers;
};



typedef TileCondProxyMock< TileCalibDrawerFlt > TileCondProxyMockFlt;
DECLARE_TOOL_FACTORY( TileCondProxyMockFlt )



void test1() {

  std::cout << "test1\n";


  ISvcLocator* svcLoc;
  if (!Athena_test::initGaudi(svcLoc)) {
    std::cerr << "ERROR This test can not be run, can not get ServiceLocator" << std::endl;
  }


  DummyAlgorithm* alg = new DummyAlgorithm("DummyAlgorithm", svcLoc);
  alg->addRef();

  TileCondToolTMDB* tool = new TileCondToolTMDB("TileCondToolTMDB", "TileCondToolTMDBTest", alg);
  tool->addRef();
  assert(tool->setProperty("ProxyThreshold", "TileCondProxyMock<TileCalibDrawerFlt>/TileCondProxyMockFlt"));
  assert(tool->setProperty("ProxyDelay", "TileCondProxyMock<TileCalibDrawerFlt>/TileCondProxyMockFlt"));
  assert(tool->setProperty("ProxyCalib", "TileCondProxyMock<TileCalibDrawerFlt>/TileCondProxyMockFlt"));
  assert(tool->setProperty("ProxyTMF", "TileCondProxyMock<TileCalibDrawerFlt>/TileCondProxyMockFlt"));
  assert(tool->initialize());


  float delay = tool->getDelay(DELAY_DRAWER_IDX, TMDB::D5L);
  assert(Athena_test::isEqual(delay, 10.0));

  delay = tool->getDelay(DELAY_DRAWER_IDX, TMDB::D5R);
  assert(Athena_test::isEqual(delay, 20.0));

  delay = tool->getDelay(DELAY_DRAWER_IDX, TMDB::D6L);
  assert(Athena_test::isEqual(delay, 30.0));

  delay = tool->getDelay(DELAY_DRAWER_IDX, TMDB::D6R);
  assert(Athena_test::isEqual(delay, 40.0));



  float threshold = tool->getThreshold(THRESHOLD_DRAWER_IDX, TMDB::D6LOW);
  assert(Athena_test::isEqual(threshold, 100.0));

  threshold = tool->getThreshold(THRESHOLD_DRAWER_IDX, TMDB::D6HIGH);
  assert(Athena_test::isEqual(threshold, 200.0));

  threshold = tool->getThreshold(THRESHOLD_DRAWER_IDX, TMDB::D5D6LOW);
  assert(Athena_test::isEqual(threshold, 1000.0));

  threshold = tool->getThreshold(THRESHOLD_DRAWER_IDX, TMDB::D5D6HIGH);
  assert(Athena_test::isEqual(threshold, 2000.0));


  float a;
  float b;
  tool->getCalib(CALIB_DRAWER_IDX, TMDB::D5L, a, b);
  assert(Athena_test::isEqual(a, 1.0));
  assert(Athena_test::isEqual(b, 2.0));

  TMDB::Weights weights;
  unsigned int nWeights = tool->getWeights(TMF_DRAWER_IDX, TMDB::D5L, weights);
  assert(Athena_test::isEqual(nWeights, 7));
  assert(Athena_test::isEqual(weights[0], 1.0));
  assert(Athena_test::isEqual(weights[1], 2.0));
  assert(Athena_test::isEqual(weights[2], 4.0));
  assert(Athena_test::isEqual(weights[3], 8.0));
  assert(Athena_test::isEqual(weights[4], 4.0));
  assert(Athena_test::isEqual(weights[5], 2.0));
  assert(Athena_test::isEqual(weights[6], 1.0));


  float amplitude = tool->channelCalib(CALIB_DRAWER_IDX, TMDB::D5L, 1.0);
  assert(Athena_test::isEqual(amplitude, 3.0));

  std::vector<float> samples = {10, 20, 40, 80, 40, 20, 10};
  amplitude = tool->channelCalib(TMF_DRAWER_IDX, TMDB::D5L, samples);
  assert(Athena_test::isEqual(amplitude, 2121.0));

  delete tool;
  delete alg;

}


int main() {


  test1();

  return 0;
}
