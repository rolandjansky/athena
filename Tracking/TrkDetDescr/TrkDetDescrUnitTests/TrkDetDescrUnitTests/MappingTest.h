/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MappingTest.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUNITTESTS_MAPPINGTEST_H
#define TRKDETDESCRUNITTESTS_MAPPINGTEST_H

// Athena & Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
// Trk includes
#include "TrkDetDescrUnitTests/TrkDetDescrUnitTestBase.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"

class TTree;

namespace Trk {
     
    class TrackingGeometry;

        
    /** @class MappingTest
       
        Test to build the TrackingGeometry and try to map all points to layers.
        
        @author Andreas.Salzburger@cern.ch       
      */
      
    class MappingTest : public TrkDetDescrUnitTestBase  {
     public:

       /** Standard Athena-Algorithm Constructor */
       MappingTest(const std::string& name, ISvcLocator* pSvcLocator);
       
       /* specify the test here */
       StatusCode runTest();
       
       /* initialize the test, i.e. retrieve the TrackingGeometry Svc */
       StatusCode initializeTest();

       /* book the TTree branches */
       StatusCode bookTree();
                                                  
     private:          
       bool                                         m_executed;                   //!< Make sure it only runs once 
                                                        
       ServiceHandle<Trk::ITrackingGeometrySvc>     m_trackingGeometrySvc;        //!< Service handle for retrieving the TrackingGeometry
       mutable const TrackingGeometry*              m_trackingGeometry;           //!< The TrackingGeometry to be retrieved
       std::string                                  m_trackingGeometryName;       //!< The Name of the TrackingGeometry
       
       double                                       m_etaCutOff;                  //!< do not map beyond this point
       std::string                                  m_mappingVolumeName;          //!< only map within this volume
       
       std::string                                  m_mappingTreeName;
       std::string                                  m_mappingTreeDescription;
       TTree*                                       m_mappingTree;
                                                    
       float                                        m_mappingPositionX;
       float                                        m_mappingPositionY;
       float                                        m_mappingPositionZ;
       float                                        m_mappingPositionR;
                                                    
       float                                        m_assignedPositionX;
       float                                        m_assignedPositionY;
       float                                        m_assignedPositionZ;
       float                                        m_assignedPositionR;
       float                                        m_assignedCorrection;
       int                                          m_assignedLayerIndex;
       float                                        m_assignmentDistance;
                                                    
       TTree*                                       m_unmappedTree;
       float                                        m_unmappedPositionX;
       float                                        m_unmappedPositionY;
       float                                        m_unmappedPositionZ;
       float                                        m_unmappedPositionR;
       
                                    
   };
}

#endif