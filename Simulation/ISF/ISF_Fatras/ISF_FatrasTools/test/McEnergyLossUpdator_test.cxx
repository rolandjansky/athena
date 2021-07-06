#undef NDEBUG

#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ISvcLocator.h"
#include "TestTools/initGaudi.h"
#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>


void test(Trk::IEnergyLossUpdator& tool)
{
  std::cout << "test: "  << tool.name() << "\n";
  Trk::Material mat1(31.5, 32.5, 33.5, 34.5, 35.5);
  Trk::MaterialProperties trkmat(mat1, 4);
  Trk::EnergyLoss* eloss = tool.energyLoss(trkmat, 5000., 5.);

  double dE = eloss->deltaE();
  double dEneg = eloss->sigmaMinusDeltaE();
  double dEpos = eloss->sigmaPlusDeltaE();
  
  printf("deltaE: %f\n", dE);
  printf("dEneg: %f\n", dEneg);
  printf("dEpos: %f\n", dEpos);

  double dedx = tool.dEdX(trkmat, 5000.);
  printf("dEdX: %f\n", dedx);

  assert ( (dE - 537734.4523) < 0.0001 );
  assert ( (dEneg - 11187.92) < 0.0001 );
  assert ( (dEpos - 11187.92) < 0.0001 );
  assert ( (dedx - 9197.0873) < 0.0001 );
}

int main()
{

   std::cout << "ISF_FatrasTools/McEnergyLossUpdator_test.cxx \n";
   ISvcLocator* pSvcLoc;
   if (!Athena_test::initGaudi("EnergyLoss_test.txt", pSvcLoc)) {
     std::cerr << "This test can not be run" << std::endl;
     return 0;
   }

   ToolHandle<Trk::IEnergyLossUpdator> tool("iFatras::McEnergyLossUpdator/TestMcEnergyLossUpdator");
   assert( tool.retrieve().isSuccess() );

   test(*tool);

   return 0;
}
