/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MagFieldElements/BFieldCache.h"
#include "MagFieldElements/BFieldZone.h"
#include <iostream>
#include <unistd.h>

/**
 * @brief Specimen of service caching expensive calculations.
 * It uses internally std::map which is not thread-safely expandable.
 **/
class BFieldTest
{
public:
  int runTest(bool doDebug = false)
  {

    // Create a field zone

    double fieldz[] = {
      19487, 19487, 19488, 19488, 19487, 19487, 19531, 19531, 19532, 19532,
      19531, 19531, 6399,  6400,  6400,  6400,  6399,  -1561, -1561, -1560,
      -1560, -1560, -1561, -1516, -1516, -1515, -1515, -1516, -1516, 20310,
      20310, 20311, 20311, 20311, 20310, 20329, 20329, 20329, 20330, 20329,
      20329, 7172,  7173,  7173,  7172,  7172,  -814,  -814,  -813,  -812,
      -813,  -814,  -795,  -795,  -794,  -793,  -794,  -795,  20310, 20310,
      20311, 20312, 20311, 20310, 20329, 20329, 20330, 20330, 20330, 20329,
      7172,  7172,  7173,  7173,  7173,  7172,  -813,  -813,  -812,  -812,
      -813,  -813,  -794,  -794,  -793,  -793,  -793,  -794,  19487, 19487,
      19488, 19488, 19488, 19487, 19531, 19531, 19532, 19532, 19532, 19531,
      6400,  6399,  6400,  6401,  6400,  6400,  -1561, -1561, -1560, -1559,
      -1560, -1561, -1516, -1516, -1515, -1515, -1515, -1516, -1516, -1516
    };

    double fieldr[] = {
      -1357, -1356, -1353, -1354, -1354, -1357, -1366, -1366, -1362, -1363,
      -1363, -1366, -1378, -1374, -1375, -1375, -1378, -1388, -1388, -1385,
      -1386, -1386, -1388, -1394, -1394, -1390, -1391, -1391, -1394, -318,
      -318,  -314,  -315,  -316,  -318,  -321,  -321,  -317,  -318,  -319,
      -321,  -325,  -321,  -322,  -322,  -325,  -328,  -328,  -324,  -325,
      -326,  -328,  -330,  -331,  -326,  -327,  -328,  -330,  312,   312,
      316,   315,   315,   312,   315,   315,   319,   318,   318,   315,
      319,   318,   323,   322,   321,   319,   322,   322,   326,   325,
      325,   322,   324,   324,   328,   327,   327,   324,   1351,  1351,
      1356,  1354,  1354,  1351,  1360,  1360,  1365,  1363,  1363,  1360,
      1372,  1372,  1377,  1375,  1375,  1372,  1383,  1383,  1387,  1386,
      1386,  1383,  1388,  1388,  1393,  1391,  1391,  1388,  1388,  1388
    };

    double fieldphi[] = {
      -2, 7,  3,  1,  6, -2, -2, 7, 3,  1,  6, -2, -2, 3, 1,  6,  -2, -2, 7, 3,
      1,  6,  -2, -2, 7, 3,  1,  6, -2, -1, 7, 3,  1,  6, -1, -1, 7,  3,  1, 6,
      -1, -1, 3,  1,  6, -1, -1, 7, 3,  1,  6, -1, -1, 8, 3,  1,  6,  -1, 1, 7,
      3,  2,  6,  1,  1, 7,  3,  2, 6,  1,  1, 7,  3,  2, 6,  1,  1,  7,  3, 2,
      6,  1,  0,  8,  3, 2,  6,  0, 2,  7,  3, 2,  6,  2, 2,  7,  3,  2,  6, 2,
      2,  7,  3,  2,  6, 2,  2,  7, 3,  2,  6, 2,  2,  8, 3,  2,  6,  2,  2, 2
    };

    int id{ 5 };
    double zmin{ -1400 }, zmax{ 1400 }, rmin{ 1200 }, rmax{ 1300 }, phimin{ 0 },
      phimax{ 6.28319 }, bscale{ 1e-07 };
    BFieldZone zone(id, zmin, zmax, rmin, rmax, phimin, phimax, bscale);
    // add in field
    int nmeshz{ 4 }, nmeshr{ 5 }, nmeshphi{ 6 };
    int nfield = nmeshz * nmeshr * nmeshphi;

    zone.reserve(nmeshz, nmeshr, nmeshphi);

    double meshz[] = { -1400, -466.93, 466.14, 1400 };

    for (int j = 0; j < nmeshz; j++) {
      zone.appendMesh(0, meshz[j]);

      if (doDebug){
        std::cout << "j, meshz " << j << ", " << meshz[j] <<'\n';
      }
    }

    if (doDebug){
      std::cout << "did  meshz " << '\n';
    }

    double meshr[] = { 1200, 1225, 1250, 1275, 1300 };

    for (int j = 0; j < nmeshr; j++) {
      zone.appendMesh(1, meshr[j]);
    }

    if (doDebug){
      std::cout << "did  meshr " << '\n';
    }

    double meshphi[] = { 0, 1.25664, 2.51327, 3.76991, 5.02655, 6.28318 };

    for (int j = 0; j < nmeshphi; j++) {
      zone.appendMesh(2, meshphi[j]);
    }

    if (doDebug){
      std::cout << "did  meshphi " << '\n';
    }

    for (int j = 0; j < nfield; j++) {
      BFieldVector<short> field(fieldz[j], fieldr[j], fieldphi[j]);
      zone.appendField(field);
    }

    if (doDebug){
      std::cout << "did  field " << '\n';
    }

    // build (trivial) look up table for zone
    zone.buildLUT();

    if (doDebug){
      std::cout << "did  buildLUT " << '\n';
    }

    // fill the cache, pass in current scale factor
    BFieldCache cache3d;
    double z{ 0 }, r{ 1250 }, phi{ 1.6 };

    // get field at steps of 10 mm from 1200 to 1300
    int status{ 0 };

    double z0 = z;
    double r0 = 1200;
    double phi0 = phi;
    double xyz[3] = { 0, 0, 0 };
    double bxyz[3] = { 0, 0, 0 };
    double bxyz_std[3][10] = { { -2.83727e-07,
                                 -2.81403e-07,
                                 -2.79079e-07,
                                 -2.76755e-07,
                                 -2.74431e-07,
                                 -2.72107e-07,
                                 -2.69782e-07,
                                 -2.67458e-07,
                                 -2.65134e-07,
                                 -2.6281e-07 },
                               { 9.47007e-08,
                                 7.49033e-08,
                                 5.51058e-08,
                                 3.53084e-08,
                                 1.5511e-08,
                                 -4.28645e-09,
                                 -2.40839e-08,
                                 -4.38813e-08,
                                 -6.36787e-08,
                                 -8.34762e-08 },
                               { 0.00308551,
                                 0.00255923,
                                 0.00203296,
                                 0.00150669,
                                 0.000980422,
                                 0.000454151,
                                 -7.21201e-05,
                                 -0.000598391,
                                 -0.00112466,
                                 -0.00165093 } };

    // get field std: i, bxyz_std 0 -2.83727e-07, 9.47007e-08, 0.00308551
    // get field new: i, bxyz_new 0 -2.83727e-07, 9.47007e-08, 0.00308551
    // get field std: i, bxyz_std 1 -2.81403e-07, 7.49033e-08, 0.00255923
    // get field new: i, bxyz_new 1 -2.81403e-07, 7.49033e-08, 0.00255923
    // get field std: i, bxyz_std 2 -2.79079e-07, 5.51058e-08, 0.00203296
    // get field new: i, bxyz_new 2 -2.79079e-07, 5.51058e-08, 0.00203296
    // get field std: i, bxyz_std 3 -2.76755e-07, 3.53084e-08, 0.00150669
    // get field new: i, bxyz_new 3 -2.76755e-07, 3.53084e-08, 0.00150669
    // get field std: i, bxyz_std 4 -2.74431e-07, 1.5511e-08, 0.000980422
    // get field new: i, bxyz_new 4 -2.74431e-07, 1.5511e-08, 0.000980422
    // get field std: i, bxyz_std 5 -2.72107e-07, -4.28645e-09, 0.000454151
    // get field new: i, bxyz_new 5 -2.72107e-07, -4.28645e-09, 0.000454151
    // get field std: i, bxyz_std 6 -2.69782e-07, -2.40839e-08, -7.21201e-05
    // get field new: i, bxyz_new 6 -2.69782e-07, -2.40839e-08, -7.21201e-05
    // get field std: i, bxyz_std 7 -2.67458e-07, -4.38813e-08, -0.000598391
    // get field new: i, bxyz_new 7 -2.67458e-07, -4.38813e-08, -0.000598391
    // get field std: i, bxyz_std 8 -2.65134e-07, -6.36787e-08, -0.00112466
    // get field new: i, bxyz_new 8 -2.65134e-07, -6.36787e-08, -0.00112466
    // get field std: i, bxyz_std 9 -2.6281e-07, -8.34762e-08, -0.00165093
    // get field new: i, bxyz_new 9 -2.6281e-07, -8.34762e-08, -0.00165093

    for (unsigned int i = 0; i < 10; ++i) {
      double r1 = r0 + 5 + i * 10.;

      xyz[0] = r1 * cos(phi0);
      xyz[1] = r1 * sin(phi0);
      xyz[2] = z0;

      // do interpolation (cache3d has correct scale factor)
      zone.getCache(z, r, phi, cache3d, 1);
      cache3d.getB(xyz, r1, phi, bxyz, nullptr);

      std::cout << "get field std: i, bxyz " << i << " " << bxyz[0] << ", "
                << bxyz[1] << ", " << bxyz[2] << " fractional diff gt 10^-5: "
                << int(fabs(bxyz[0] - bxyz_std[0][i]) / bxyz[0] > 0.00001)
                << ", "
                << int(fabs(bxyz[1] - bxyz_std[1][i]) / bxyz[1] > 0.00001)
                << ", "
                << int(fabs(bxyz[2] - bxyz_std[2][i]) / bxyz[2] > 0.00001)
                << '\n';

      if (fabs(bxyz[0] - bxyz_std[0][i]) > 0.00001) {
        std::cout << "failed bz comparison - bz, bz std " << bxyz[0] << ", "
                  << bxyz_std[0][i] << '\n';
        status = 1;
      }
      if (fabs(bxyz[1] - bxyz_std[1][i]) > 0.00001) {
        std::cout << "failed br comparison" << '\n';
        std::cout << "failed br comparison - br, br std " << bxyz[1] << ", "
                  << bxyz_std[1][i] << '\n';
        status = 1;
      }
      if (fabs(bxyz[2] - bxyz_std[2][i]) > 0.00001) {
        std::cout << "failed bphi comparison" << '\n';
        std::cout << "failed bphi comparison - bphi, bphi std " << bxyz[2]
                  << ", " << bxyz_std[2][i] << '\n';
        status = 1;
      }
    }

    std::cout << "runTest: status " << status << '\n';

    return status;
  }

private:
};

int
main()
{

  std::cout << "start BFieldExample test" << '\n';

  BFieldTest bftest;
  int status = bftest.runTest(true);

  if (status == 0) {
    std::cout << "Test passed OK" << '\n';
    return 0;
  } else {
    std::cout << "Test failed" << '\n';
    return 1;
  }
}
