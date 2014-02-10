/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// BinUtilityTest.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUNITTESTS_BinUtilityTEST_H
#define TRKDETDESCRUNITTESTS_BinUtilityTEST_H

// Athena & Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
// Trk includes
#include "TrkDetDescrUnitTests/TrkDetDescrUnitTestBase.h"
#include "TrkDetDescrInterfaces/ISurfaceBuilder.h"

#include <vector>
#include <map>

namespace Trk {
             
    /** @class BinUtilityTest
       
        Test calling the internal surface intersection methods 
        
        @author Andreas.Salzburger@cern.ch       
      */
      
    class BinUtilityTest : public TrkDetDescrUnitTestBase  {
     public:

       /** Standard Athena-Algorithm Constructor */
       BinUtilityTest(const std::string& name, ISvcLocator* pSvcLocator);
       
       /* specify the test here */
       StatusCode runTest();
              
      private:
        /** preparation of std::vector and std::map for comparison */
        void prepareData(std::vector<float>& vec, std::map<float, size_t>& map, float& low, float& high);

        /** A binary search with a map - superior in O(10) searches*/
        size_t searchInVectorWithBoundary(std::vector<float>& array, float value)
        {
            if ( value < array[0] ) return 0;
            if ( value > array[array.size()-1] ) return (array.size()-1);
            std::vector<float>::iterator vIter = array.begin();
            size_t bin = 0; 
            for ( ; vIter !=  array.end() ;  ++vIter, ++bin )
                if ((*vIter) > value) break; 
            return (bin-1);            
        }

        /** A binary search with underflow/overflow */
        size_t binarySearchWithBoundary(std::vector<float>& array, float value)
        {
            // Binary search in an array of n values to locate value
            // underflow
            if ( value < array[0] ) return 0;
            if ( value > array[array.size()-1] ) return (array.size()-1);
            // prepare and run
            size_t nabove, nbelow, middle;
            // overflow
            nabove = array.size()+1;
            if ( value > array[nabove-2]) return nabove-2;
            // binary search
            nbelow = 0;
            while (nabove-nbelow > 1) {
              middle = (nabove+nbelow)/2;
              if (value == array[middle-1]) return middle-1;
              if (value  < array[middle-1]) nabove = middle;
              else                          nbelow = middle;
            }
            return nbelow-1;
        }

        size_t     m_numberOfSegments;   
        size_t     m_numberOfTestsPerSet;

   };
}

#endif
