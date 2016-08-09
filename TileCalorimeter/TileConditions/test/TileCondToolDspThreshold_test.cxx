/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

//=== AttributeList
//#include "CoralBase/Attribute.h"
#include "TileConditions/TileCondToolDspThreshold.h"
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
static const int NCHANNELS(48);



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


static const std::vector<std::vector<float> > THRESHOLDS = {
  {-1, 2, 3}, // Low gain (min, max, dsp)
  {-10, 20, 30} // High gain (min, max, dsp)
};


static const std::vector<std::vector<float> > CHANNEL2_THRESHOLDS = {
  {-100, 200, 300}, // Low gain (min, max, dsp)
  {-1000, 2000, 3000} // High gain (min, max, dsp)
};


template<typename T = TileCalibDrawerFlt>
class TileCondProxyMock: public AthAlgTool, virtual public ITileCondProxy<T> {
  public:
  
    TileCondProxyMock(const std::string& type, const std::string& name, const IInterface* parent)
      :AthAlgTool(type, name, parent)
      , m_list(nullptr)
      , m_drawer(nullptr)
    {
        declareInterface<ITileCondProxy<T> >(this);
    };

    virtual ~TileCondProxyMock() {};
  
    StatusCode initialize() { 


      coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
      spec->extend("TileCalibBlob", "blob");
      m_list = new coral::AttributeList(*spec);
      coral::Blob& blob = (*m_list)["TileCalibBlob"].data<coral::Blob>();
      m_drawer = TileCalibDrawerFlt::getInstance(blob, THRESHOLDS, NCHANNELS, OBJVERSION);

      unsigned int channel(2);
      unsigned int adc(0);
      m_drawer->setData(channel, adc, CHANNEL2_THRESHOLDS[adc]);
      adc = 1;
      m_drawer->setData(channel, adc, CHANNEL2_THRESHOLDS[adc]);

      return StatusCode::SUCCESS; 
    };

    StatusCode finalize() { 
      delete m_drawer;
      delete m_list;

      return StatusCode::SUCCESS; 
    };
  
    const T* getCalibDrawer(unsigned int /* drawerIdx */ ) const { return m_drawer;};

  private:
    coral::AttributeList* m_list;
    T* m_drawer;
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

  TileCondToolDspThreshold* tool = new TileCondToolDspThreshold("TileCondToolDspThreshold", "TileCondToolDspThresholdTest", alg);
  tool->addRef();
  assert(tool->setProperty("ProxyDspThreshold", "TileCondProxyMock<TileCalibDrawerFlt>/TileCondProxyMockFlt"));
  assert(tool->initialize());


  unsigned int drawerIdx(0);
  unsigned int channel(0);
  unsigned int adc(0);

  float minThreshold = tool->getMinimumAmplitudeThreshold(drawerIdx, channel, adc);
  assert(Athena_test::isEqual(minThreshold, THRESHOLDS[adc][0]));

  float maxThreshold = tool->getMaximumAmplitudeThreshold(drawerIdx, channel, adc);
  assert(Athena_test::isEqual(maxThreshold, THRESHOLDS[adc][1]));



  minThreshold = 0.0;
  maxThreshold = 0.0;


  tool->getAmplitudeThresholds(drawerIdx, channel, adc, minThreshold, maxThreshold);
  assert(Athena_test::isEqual(minThreshold, THRESHOLDS[adc][0]));
  assert(Athena_test::isEqual(maxThreshold, THRESHOLDS[adc][1]));


  float dspThreshold = tool->getDspThreshold(drawerIdx, channel, adc);
  assert(Athena_test::isEqual(dspThreshold, THRESHOLDS[adc][2]));


  adc = 1;
  minThreshold = 0.0;
  maxThreshold = 0.0;

  minThreshold = tool->getMinimumAmplitudeThreshold(drawerIdx, channel, adc);
  assert(Athena_test::isEqual(minThreshold, THRESHOLDS[adc][0]));

  maxThreshold = tool->getMaximumAmplitudeThreshold(drawerIdx, channel, adc);
  assert(Athena_test::isEqual(maxThreshold, THRESHOLDS[adc][1]));

  minThreshold = 0.0;
  maxThreshold = 0.0;


  tool->getAmplitudeThresholds(drawerIdx, channel, adc, minThreshold, maxThreshold);
  assert(Athena_test::isEqual(minThreshold, THRESHOLDS[adc][0]));
  assert(Athena_test::isEqual(maxThreshold, THRESHOLDS[adc][1]));

  dspThreshold = tool->getDspThreshold(drawerIdx, channel, adc);
  assert(Athena_test::isEqual(dspThreshold, THRESHOLDS[adc][2]));


  channel = 2;
  adc = 0;
  minThreshold = 0.0;
  maxThreshold = 0.0;
  dspThreshold = 0.0;

  tool->getAmplitudeThresholds(drawerIdx, channel, adc, minThreshold, maxThreshold);
  assert(Athena_test::isEqual(minThreshold, CHANNEL2_THRESHOLDS[adc][0]));
  assert(Athena_test::isEqual(maxThreshold, CHANNEL2_THRESHOLDS[adc][1]));

  adc = 1;
  minThreshold = 0.0;
  maxThreshold = 0.0;


  tool->getAmplitudeThresholds(drawerIdx, channel, adc, minThreshold, maxThreshold);
  assert(Athena_test::isEqual(minThreshold, CHANNEL2_THRESHOLDS[adc][0]));
  assert(Athena_test::isEqual(maxThreshold, CHANNEL2_THRESHOLDS[adc][1]));

  dspThreshold = tool->getDspThreshold(drawerIdx, channel, adc);
  assert(Athena_test::isEqual(dspThreshold, CHANNEL2_THRESHOLDS[adc][2]));



  delete tool;
  delete alg;

}


int main() {


  test1();

  return 0;
}
