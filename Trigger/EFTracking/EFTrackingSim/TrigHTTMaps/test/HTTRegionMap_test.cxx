/**
 * @file HTTRegionMap_test.cxx
 * @brief Unit tests for HTTRegionMap
 * @author Riley Xu - rixu@cern.ch
 * @date 2020-01-15
 */

#undef NDEBUG
#include <cassert>
#include <string>
#include <iostream>
#include <filesystem>

#include "TestTools/initGaudi.h"
#include "AthenaKernel/getMessageSvc.h"
#include "TrigHTTMaps/HTTRegionMap.h"

using namespace std;




void test(HTTRegionMap & rmap)
{
    // Hard-coded values from above files.
    // Make sure to change these if those files are changed
    //assert(rmap.getNRegions() == 96);
    assert(rmap.getRegionBoundaries(0, 2, 0).phi_min == 0);
    //assert(rmap.getRegionBoundaries(0, 2, 0).phi_max == 39);
}


void test_LUT(HTTRegionMap & rmap)
{
    (void)rmap; // TODO
}


int main(int, char**)
{
    ISvcLocator* pSvcLoc;
    if (!Athena_test::initGaudi(pSvcLoc))
    {
        std::cerr << "Gaudi failed to initialize. Test failing." << std::endl;
        return 1;
    }

    string pmap_path="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/step3_01eta03_03phi05.pmap";
    string rmap_path="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/HTT/TrigHTTMaps/V1/map_file/rmaps/eta0103phi0305_ATLAS-P2-ITK-23-00-01.rmap";

    HTTPlaneMap pmap(pmap_path, 0, 1);
    HTTRegionMap rmap(&pmap, rmap_path);

    test(rmap);
    test_LUT(rmap);

    return 0;
}
