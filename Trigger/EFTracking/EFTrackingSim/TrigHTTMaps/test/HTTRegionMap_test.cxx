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
#include "../src/HTTRegionMap.h"
#include "PathResolver/PathResolver.h"

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

    // remove local file since filesystem copy overwrite doesn't always work if there is a duplicate, even with flags
    std::filesystem::remove("./step3_01eta03_03phi05_region.pmap");

    // find the file
    std::string pmap_path = PathResolver::find_file("step3_01eta03_03phi05.pmap","DATAPATH", PathResolver::RecursiveSearch);

    //copy it over
    std::filesystem::copy(pmap_path,"./step3_01eta03_03phi05_region.pmap",std::filesystem::copy_options::overwrite_existing);

    // remove local file since filesystem copy overwrite doesn't always work if there is a duplicate, even with flags
    std::filesystem::remove("./eta0103phi0305_ATLAS-P2-ITK-23-00-01.rmap");

    // find the file
    std::string rmap_path = PathResolver::find_file("eta0103phi0305_ATLAS-P2-ITK-23-00-01.rmap","DATAPATH", PathResolver::RecursiveSearch);

    //copy it over
    std::filesystem::copy(rmap_path,"./eta0103phi0305_ATLAS-P2-ITK-23-00-01.rmap",std::filesystem::copy_options::overwrite_existing);


    HTTPlaneMap pmap("./step3_01eta03_03phi05_region.pmap", 0, 1);
    HTTRegionMap rmap(&pmap, "./eta0103phi0305_ATLAS-P2-ITK-23-00-01.rmap");

    test(rmap);
    test_LUT(rmap);

    return 0;
}
