#undef NDEBUG

#include "ISF_FatrasInterfaces/IPhotonConversionTool.h"
#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Event/ISFParticleContainer.h"
#include "ISF_Interfaces/ISimulationSelector.h"
#include "ISF_Event/ISFParticleVector.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TestTools/initGaudi.h"
#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>

// TestTruthSvc class
class TestTruthSvc 
   : public extends<AthService, ISF::ITruthSvc> 
{
   public:
     TestTruthSvc(const std::string& name, ISvcLocator* svc)
       : base_class(name,svc)
     {  };
 
     // dummy methods implementing in pure virtual interface methods (to make class non-abstract)
     virtual StatusCode initialize() {
       ATH_MSG_INFO ("initializing TestTruthSvc: " << name());
       return StatusCode::SUCCESS;
     };
     void registerTruthIncident(ISF::ITruthIncident&) const { };
     StatusCode initializeTruthCollection() { return StatusCode::SUCCESS; };
     StatusCode releaseEvent() { return StatusCode::SUCCESS; };
};
DECLARE_COMPONENT( TestTruthSvc )

// TestParticleBroker class
class TestParticleBroker
   : public extends<AthService, ISF::IParticleBroker> 
{
   public:
     TestParticleBroker(const std::string& name, ISvcLocator* svc)
       : base_class(name,svc)
     {  };
 
     StatusCode registerSimSelector(ISF::SimSelectorToolArray &, AtlasDetDescr::AtlasRegion){ return StatusCode::SUCCESS; };
     StatusCode initializeEvent(ISF::ISFParticleContainer&&) { return StatusCode::SUCCESS; };
     StatusCode finalizeEvent() { return StatusCode::SUCCESS; };
     void push(ISF::ISFParticle*, const ISF::ISFParticle*) { };
     const ISF::ConstISFParticleVector& popVector(size_t) { return m_cv; };
     size_t numParticles() const { return 0; };
     StatusCode dump() const { return StatusCode::SUCCESS; };

   private:

     ISF::ConstISFParticleVector m_cv;

};
DECLARE_COMPONENT( TestParticleBroker )


void test(iFatras::IPhotonConversionTool& tool)
{
  std::cout << "test: "  << tool.name() << "\n";
  Trk::Material mat1(0.1,  0.2,  0.3,  0.4,  0.5);
  Trk::MaterialProperties trkmat(mat1, 10.0);
  bool res = tool.pairProduction(trkmat, 0.01, 0.1);
  std::cout << "res : " << res << "\n";
  assert ( res == 0 );

}

int main()
{

   std::cout << "ISF_FatrasTools/PhotonConversionTool_test.cxx \n";
   ISvcLocator* pSvcLoc;
   if (!Athena_test::initGaudi("PhotonConversionTool_test.txt", pSvcLoc)) {
     std::cerr << "This test can not be run" << std::endl;
     return 0;
   }

   ToolHandle<iFatras::IPhotonConversionTool> tool("iFatras::PhotonConversionTool/TestFatrasPhotonConversionTool");
   assert( tool.retrieve().isSuccess() );

   ServiceHandle<ISF::ITruthSvc> tsvc ("TestTruthSvc", "testtruthsvc");
   assert(tsvc.retrieve().isSuccess());

   ServiceHandle<ISF::IParticleBroker> pbroker ("TestParticleBroker", "testbroker");
   assert (pbroker.retrieve().isSuccess());
 
   test(*tool);

   return 0;
}
