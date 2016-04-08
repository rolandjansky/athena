/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkDetDescrTPCnvTest.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUNITTESTS_TRKDETDESCRTPCNVTEST_H
#define TRKDETDESCRUNITTESTS_TRKDETDESCRTPCNVTEST_H

// Trk
#include "TrkDetDescrUnitTests/TrkDetDescrUnitTestBase.h"
// Athena & Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {
             
    /** @class TrkDetDescrTPCnvTest
       
        Test the new TP Converter for writing and reading
        
        @author Andreas.Salzburger@cern.ch       
      */
      
    class TrkDetDescrTPCnvTest : public TrkDetDescrUnitTestBase  {
     public:

       /** Standard Athena-Algorithm Constructor */
       TrkDetDescrTPCnvTest(const std::string& name, ISvcLocator* pSvcLocator);
       
       /* specify the test here */
       StatusCode runTest();
       
     private:
         bool        m_writeMode;
         
         std::string m_materialStepCollectionName;
         std::string m_layerMaterialCollectionName;
         std::string m_elementTableName;
                                           
   };
}

#endif