/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkExUnitTestBase.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUNITTESTS_TrkExUnitTestBase_H
#define TRKDETDESCRUNITTESTS_TrkExUnitTestBase_H

// Athena & Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

namespace Trk {
 
        
    /** @class TrkExUnitTestBase
       
        Base class for all unit tests in the TrkEx package,
        gives access to gaussian and flat random numbers
        
        @author Andreas.Salzburger@cern.ch       
      */
      
    class TrkExUnitTestBase : public AthAlgorithm  {

     public:

       /** Standard Athena-Algorithm Constructor */
       TrkExUnitTestBase(const std::string& name, ISvcLocator* pSvcLocator);

       /** Default Destructor */
       virtual ~TrkExUnitTestBase();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();

       /** standard Athena-Algorithm method */
       StatusCode          execute();
       
       /** standard Athena-Algorithm method */
       StatusCode          finalize();
       
       /* specify the test here */
       virtual StatusCode runTest() = 0;

       /* specify the scan here */
       virtual StatusCode runScan() = 0;

       /* book the TTree branches */
       virtual StatusCode bookTree();
       
       /* initalizeTest, this includes loading of tools */
       virtual StatusCode initializeTest();

    protected:
      /** Random Number setup */
      Rndm::Numbers*            m_gaussDist;
      Rndm::Numbers*            m_flatDist;
      Rndm::Numbers*            m_landauDist;
      
      /** number of tests */
      size_t                    m_numTests; 
      
      /** enable scan mode */
      bool                      m_scanMode;
  };
      
   
}

#endif 

