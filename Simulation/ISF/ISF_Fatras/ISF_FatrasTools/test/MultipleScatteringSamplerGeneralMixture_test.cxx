#undef NDEBUG

#include "ISF_FatrasInterfaces/IMultipleScatteringSampler.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ISvcLocator.h"
#include "TestTools/initGaudi.h"
#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>


void test(iFatras::IMultipleScatteringSampler& tool)
{
  std::cout << "test\n";
  std::cout << "test: "  << tool.name() << "\n";
  Trk::Material mat1(31.5, 32.5, 33.5, 34.5, 35.5);
  Trk::MaterialProperties trkmat(mat1, 4);

  double simtheta = tool.simTheta(trkmat, 5000., 5.);
  printf("simtheta: %f\n", simtheta);

  assert ( (simtheta - 0.000103) < 0.0001 );
}

int main()
{

   std::cout << "ISF_FatrasTools/MultipleScatteringSamplerGeneralMixture_test.cxx \n";
   ISvcLocator* pSvcLoc;
   if (!Athena_test::initGaudi("EnergyLoss_test.txt", pSvcLoc)) {
     std::cerr << "This test can not be run" << std::endl;
     return 0;
   }

   ToolHandle<iFatras::IMultipleScatteringSampler> tool("iFatras::MultipleScatteringSamplerGeneralMixture/TestMultipleScatteringSamplerGeneralMixture");
   assert( tool.retrieve().isSuccess() );

   test(*tool);

   return 0;
}
