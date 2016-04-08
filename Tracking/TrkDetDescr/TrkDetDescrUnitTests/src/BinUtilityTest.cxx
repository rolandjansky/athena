/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// BinUtilityTest.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk includes
#include "TrkDetDescrUnitTests/BinUtilityTest.h"
#include "TrkDetDescrUtils/BinUtility.h"

Trk::BinUtilityTest::BinUtilityTest(const std::string& name, ISvcLocator* pSvcLocator) :
 Trk::TrkDetDescrUnitTestBase(name, pSvcLocator),
 m_numberOfSegments(10),
 m_numberOfTestsPerSet(100000)
{
    declareProperty("NumberOfSegments",    m_numberOfSegments);
    declareProperty("NumberOfTetsPerSet",  m_numberOfTestsPerSet);
    
    
}

StatusCode Trk::BinUtilityTest::runTest()
{

    ATH_MSG_VERBOSE("Running the BinUtilityTest Test");

    // Contstruction of bin array test
    // add a closed equidistant binning in Rphi
    Trk::BinUtility testUtil(10, 0., m_flatDist->shoot()*1000.*M_PI, Trk::closed, Trk::binRPhi);
    ATH_MSG_VERBOSE(" Created 1D bin utitlity : " << testUtil );
    // add an equidistant binning in Z
    testUtil += Trk::BinUtility(24, -400., 400., Trk::open, Trk::binZ);
    ATH_MSG_VERBOSE(" Created 2D bin utitlity : " << testUtil );
    // add a bi equidistant binning in X
    testUtil += Trk::BinUtility(5, m_flatDist->shoot()*10., m_flatDist->shoot()*80., 1000.+m_flatDist->shoot()*1000 , Trk::open, Trk::binX);
    ATH_MSG_VERBOSE(" Created 3D bin utitlity : " << testUtil );

    // Arbitrary binning test
    for (size_t it = 0; it < Trk::TrkDetDescrUnitTestBase::m_numTests; ++it) {
         std::vector<float> 		    binVector;
         std::map<float, size_t>	    binMap;
         float                          low;
         float                          high;
         
	     prepareData(binVector, binMap, low, high);
	     
         Trk::BinUtility binUtility(binVector, Trk::open, Trk::binX);
	     
         // run the test n times
         for (size_t itpv = 0; itpv < m_numberOfTestsPerSet; ++itpv) {
            float value = low + (high - low) * Trk::TrkDetDescrUnitTestBase::m_flatDist->shoot();
            Amg::Vector3D testPosition(value, 0., 0.);
            size_t binVec  =  searchInVectorWithBoundary(binVector, testPosition.x());
            size_t binBin  =  binarySearchWithBoundary(binVector, testPosition.x());
            size_t binUtil =  binUtility.bin(testPosition, 0);
            if (binVec != binBin || binVec != binUtil )
               ATH_MSG_WARNING("Searches through binary (" << binBin << "), vector (" << binVec << ") or tutility  (" << binUtil << ") differ.");
         }
         
    }
    return StatusCode::SUCCESS;
}

void Trk::BinUtilityTest::prepareData(std::vector<float>& vec, std::map<float, size_t>& map, float& low, float& high) {

	// create the boundaries
        low  = 500.*Trk::TrkDetDescrUnitTestBase::m_flatDist->shoot();
        high = low + 1000.*Trk::TrkDetDescrUnitTestBase::m_flatDist->shoot();
        float range = high - low;
        map[low]  = 0;
        map[high] = 0;
        for (size_t is = 0; is < m_numberOfSegments-1; ++is){
           map[low + range*m_flatDist->shoot()]  = 0;
        }
        // now prepare the vector
        vec.reserve(m_numberOfSegments+1);
        std::map< float, size_t >::iterator sIter = map.begin();
        for (size_t cs = 0 ; sIter != map.end(); ++sIter, ++cs ){
		vec.push_back((*sIter).first);
                (*sIter).second = cs;
        }
}
