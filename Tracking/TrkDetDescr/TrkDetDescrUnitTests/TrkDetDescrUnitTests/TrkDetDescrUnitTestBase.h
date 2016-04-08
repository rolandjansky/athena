/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkDetDescrUnitTestBase.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUNITTESTS_TRKDETDESCRUNITTESTBASE_H
#define TRKDETDESCRUNITTESTS_TRKDETDESCRUNITTESTBASE_H

// Athena & Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

namespace Trk {
 
        
    /** @class TrkDetDescrUnitTestBase
       
        Base class for all unit tests in the TrkDetDescr package,
        gives access to gaussian and flat random numbers
        
        @author Andreas.Salzburger@cern.ch       
      */
      
    class TrkDetDescrUnitTestBase : public AthAlgorithm  {

     public:

       /** Standard Athena-Algorithm Constructor */
       TrkDetDescrUnitTestBase(const std::string& name, ISvcLocator* pSvcLocator);

       /** Default Destructor */
       virtual ~TrkDetDescrUnitTestBase();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();

       /** standard Athena-Algorithm method */
       StatusCode          execute();
       
       /** standard Athena-Algorithm method */
       StatusCode          finalize();
       
       /* specify the test here */
       virtual StatusCode runTest() = 0;
       
       /* book the TTree branches */
       virtual StatusCode bookTree();
       
       /* initalizeTest, this includes loading of tools */
       virtual StatusCode initializeTest();

    protected:
      /** Random Number setup */
      Rndm::Numbers*            m_gaussDist;
      Rndm::Numbers*            m_flatDist;      
      
      /** number of tests */
      size_t                    m_numTests; 
      
                             
   };
      
   
}

#endif 

