/**
 * @file HTTSSMap_test.cxx
 * @brief Unit tests for HTTSSMap
 * @author Riley Xu - rixu@cern.ch
 * @date 2020-01-15
 */

#undef NDEBUG
#include <cassert>
#include <string>
#include <iostream>
#include <numeric>
#include <filesystem>

#include "TestTools/initGaudi.h"
#include "AthenaKernel/getMessageSvc.h"
#include "TrigHTTMaps/HTTPlaneMap.h"

using namespace std;



// Hard-coded values from step3_01eta03_03phi05.pmap
// Make sure to change these if the file is changed
void test(HTTPlaneMap & pmap)
{
    //assert(pmap.getNDetLayers() == 227);
    assert(pmap.getNLogiLayers() == 8);
    assert(pmap.getNCoords() == 9);

    assert(pmap.isPixel(0));
    assert(pmap.isSCT(3));

    assert(pmap.getNSections(5) == 1);
    assert(pmap.getCoordOffset(1) == 2);
    assert(pmap.getDim(0) == 2);

    assert(pmap.getLayerInfo(0, 0).zone == DetectorZone::barrel);
    assert(pmap.getLayerInfo(3, 0).physLayer == 3);
    assert(pmap.getLayerSection(SiliconTech::strip, DetectorZone::barrel, 0).layer == 1);
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
    HTTPlaneMap pmap(pmap_path, 0, 1);

    test(pmap);

    return 0;
}
