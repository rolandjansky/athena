/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrackingGeometryTest.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUNITTESTS_TRACKINGGEOMETRYTEST_H
#define TRKDETDESCRUNITTESTS_TRACKINGGEOMETRYTEST_H

// Athena & Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
// Trk includes
#include "TrkDetDescrUnitTests/TrkDetDescrUnitTestBase.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"

#ifdef TRKDETDESCR_MEMUSAGE
#include "TrkDetDescrUtils/MemoryLogger.h"
#endif

namespace Trk {
     
    class IGeometryProcessor;
    class TrackingGeometry;
    class TrackingVolume;
        
    /** @class TrackingGeometryTest
       
        Test to build the TrackingGeometry and process it with analizers,
        thosw are of type IGeometryProcessor
        
        @author Andreas.Salzburger@cern.ch       
      */
      
    class TrackingGeometryTest : public TrkDetDescrUnitTestBase  {
     public:

       /** Standard Athena-Algorithm Constructor */
       TrackingGeometryTest(const std::string& name, ISvcLocator* pSvcLocator);
       
       /* specify the test here */
       StatusCode runTest();
              
       /* initialize the test, i.e. retrieve the TrackingGeometry Svc */
       StatusCode initializeTest();
       
       
     private:
#ifdef TRKDETDESCR_MEMUSAGE
       Trk::MemoryLogger                            m_memoryLogger;               //!< little memory logger to check the used memory
#endif                                              
       bool                                         m_executed;                   //!< Make sure it only runs once 
                                                    
       ServiceHandle<Trk::ITrackingGeometrySvc>     m_trackingGeometrySvc;        //!< Service handle for retrieving the TrackingGeometry
       mutable const TrackingGeometry*              m_trackingGeometry;           //!< The TrackingGeometry to be retrieved
       std::string                                  m_trackingGeometryName;       //!< The Name of the TrackingGeometry
       ToolHandleArray<Trk::IGeometryProcessor>     m_trackingGeometryProcessors; //!< Tool to write out a Display format for external viewers
                                    
   };
}

#endif