/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

//=== AttributeList
//#include "CoralBase/Attribute.h"
#include "TileConditions/TileCondToolOfcCool.h"
#include "TileCalibBlobObjs/TileCalibDrawerOfc.h"
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

const std::vector<float> phases = {-2.0, 0.0, 0.1, 0.2, 0.5, 1.0};
const int nPhases = phases.size();

static const unsigned int OBJVERSION(3);
static const int NCHANNELS(1);
static const int NGAINS(1);
static const unsigned int NSAMPLES(1);


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



template<typename T = TileCalibDrawerOfc>
class TileCondProxyMock: public AthAlgTool, virtual public ITileCondProxy<T> {
  public:
  
    TileCondProxyMock(const std::string& type, const std::string& name, const IInterface* parent)
      :AthAlgTool(type, name, parent),
       m_ofcList(nullptr),
       m_drawerOfc(nullptr)
    {
        declareInterface<ITileCondProxy<T> >(this);
    };

    virtual ~TileCondProxyMock() {};
  
    StatusCode initialize() { 
      coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
      spec->extend("TileCalibBlobOfc", "blob");
      m_ofcList = new coral::AttributeList(*spec);
      coral::Blob& blob = (*m_ofcList)["TileCalibBlobOfc"].data<coral::Blob>();
  
      m_drawerOfc = TileCalibDrawerOfc::getInstance(blob, OBJVERSION, NSAMPLES, -nPhases, NCHANNELS, NGAINS);
      m_drawerOfc->setPhases(0, 0, phases);
      
      for (unsigned int channel = 0; channel < NCHANNELS; ++channel) {
        for (unsigned int gain = 0; gain < NGAINS; ++gain) {
          for (float phase : phases) {
            for (unsigned int isam = 0; isam < NSAMPLES; ++isam) {
              m_drawerOfc->setOfc(TileCalibDrawerOfc::FieldA, channel, gain, phase, isam, phase + 10.0F);
              m_drawerOfc->setOfc(TileCalibDrawerOfc::FieldB, channel, gain, phase, isam, phase + 20.0F);
              m_drawerOfc->setOfc(TileCalibDrawerOfc::FieldG, channel, gain, phase, isam, phase + 30.0F);
              if (OBJVERSION == 3) {
                m_drawerOfc->setOfc(TileCalibDrawerOfc::FieldC, channel, gain, phase, isam, phase + 40.0F);
                m_drawerOfc->setOfc(TileCalibDrawerOfc::FieldDG, channel, gain, phase, isam, phase + 50.0F);
              }
            }
          }
        }
      }

      return StatusCode::SUCCESS; 
    };

    StatusCode finalize() { 
      delete m_drawerOfc;
      delete m_ofcList;
      return StatusCode::SUCCESS; 
    };
  
    const T* getCalibDrawer(unsigned int /* drawerIdx */) const {return m_drawerOfc; };

  private:
    coral::AttributeList* m_ofcList;
    T* m_drawerOfc;
};



typedef TileCondProxyMock< TileCalibDrawerOfc > TileCondProxyMockOfc;
DECLARE_TOOL_FACTORY( TileCondProxyMockOfc )



void test1() {

  std::cout << "test1\n";


  ISvcLocator* svcLoc;
  if (!Athena_test::initGaudi(svcLoc)) {
    std::cerr << "ERROR This test can not be run, can not get ServiceLocator" << std::endl;
  }


  DummyAlgorithm* alg = new DummyAlgorithm("DummyAlgorithm", svcLoc);
  alg->addRef();

  TileCondToolOfcCool* tool = new TileCondToolOfcCool("TileCondToolOfcCool", "TileCondToolOfcCoolTest", alg);
  tool->addRef();
  assert(tool->setProperty("ProxyOfcCool", "TileCondProxyMock<TileCalibDrawerOfc>/TileCondProxyMockOfc"));
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

  delete tool;
  delete alg;

}


int main() {


  test1();

  return 0;
}
