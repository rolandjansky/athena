/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrackingGeometryTest.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk includes
#include "TrkDetDescrUnitTests/TrackingGeometryTest.h"
#include "TrkDetDescrInterfaces/IGeometryProcessor.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"

// monitor memory usage
#ifdef TRKDETDESCR_MEMUSAGE   
#include <unistd.h>
#endif


Trk::TrackingGeometryTest::TrackingGeometryTest(const std::string& name, ISvcLocator* pSvcLocator) :
 Trk::TrkDetDescrUnitTestBase(name, pSvcLocator),
#ifdef TRKDETDESCR_MEMUSAGE   
   m_memoryLogger(),
#endif
   m_executed(false),
   m_trackingGeometrySvc("TrackingGeometrySvc","AtlasTrackingGeometrySvc"),
   m_trackingGeometry(nullptr),
   m_trackingGeometryName("AtlasTrackingGeometry"),
   m_trackingGeometryProcessors()
 {
     // get the service handle for the TrackingGeometry
     declareProperty("TrackingGeometrySvc"          , m_trackingGeometrySvc);
     // get the tools for display and recording
     declareProperty("TrackingGeometryProcessors"   , m_trackingGeometryProcessors);     
 }

StatusCode Trk::TrackingGeometryTest::initializeTest() 
{
    #ifdef TRKDETDESCR_MEMUSAGE   
        m_memoryLogger.refresh(getpid());
        ATH_MSG_INFO( "[ memory usage ] Memory monitoring activated through TRKDETDESCR_MEMUSAGE " );
        ATH_MSG_INFO( "[ memory usage ] initialize (start) : "                                     ); 
        ATH_MSG_INFO( m_memoryLogger );
    #endif   

    // Get the TrackingGeometry from StoreGate
    // initialize the TrackingGeometrySvc
    if (m_trackingGeometrySvc.retrieve().isFailure()) {
        ATH_MSG_FATAL( "Cannot retrieve TrackingGeometrySvc. Abort job. " );
        return StatusCode::FAILURE;
    }
    m_trackingGeometryName = m_trackingGeometrySvc->trackingGeometryName();
    
    // The Processors -------------------------------------------------------------
    if ( m_trackingGeometryProcessors.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool(s) in  " << m_trackingGeometryProcessors );
        return StatusCode::FAILURE;
    } else
        ATH_MSG_INFO( "Retrieved tools : " << m_trackingGeometryProcessors );
    
    #ifdef TRKDETDESCR_MEMUSAGE   
        m_memoryLogger.refresh(getpid());
        ATH_MSG_INFO( "[ memory usage ] initialize (end) : " ); 
        ATH_MSG_INFO( m_memoryLogger );
    #endif  
    
    return StatusCode::SUCCESS;
}


StatusCode Trk::TrackingGeometryTest::runTest()
{
    ATH_MSG_VERBOSE("Running the TrackingGeometryTest Test");
    
    // ------------------------------- get the trackingGeometry at first place
    if (!m_trackingGeometry) {
        if ((detStore()->retrieve(m_trackingGeometry, m_trackingGeometryName)).isFailure())
            ATH_MSG_FATAL( "Could not retrieve TrackingGeometry '" << m_trackingGeometryName << "' from DetectorStore." );
        else
            ATH_MSG_INFO( "TrackingGeometry '" << m_trackingGeometryName << "' successfully retrieved from DetectorStore." );
    }
    // only run if it didn't already run before
    if (!m_executed && m_trackingGeometry){
        // push the geometry through the different processors
        for (ToolHandle<Trk::IGeometryProcessor> proc : m_trackingGeometryProcessors) {
            ATH_MSG_INFO("Parse geometry with processor " << proc->name() );
            if ((proc->process(*m_trackingGeometry)).isFailure()){
                ATH_MSG_FATAL("Could not process the TrackingGeometry with '" << proc->name() <<"'. Aborting test.");
                return StatusCode::FAILURE;
            }
        }
        m_executed = true;    
    }
    return StatusCode::SUCCESS;
       
}


