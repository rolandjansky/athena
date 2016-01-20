/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DigitizationModuleTest.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDIGUNITTEST_RECTANGULARMODULETEST_H
#define TRKDIGUNITTEST_RECTANGULARMODULETEST_H

// Athena & Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
// GeoPrimitives
#include "GeoPrimitives/GeoPrimitives.h"
// Trk includes
#include "TrkDigUnitTests/TrkDigUnitTestBase.h"
// 
#include <algorithm>

class TTree;

namespace Trk {

    class IModuleStepper;
    class DigitizationModule;
             
    /** @class DigitizationModuleTest
       
        Test calling the internal surface intersection methods 
        
        @author Andreas.Salzburger@cern.ch       
      */
      
    class DigitizationModuleTest : public TrkDigUnitTestBase  {
     public:

       /** Standard Athena-Algorithm Constructor */
       DigitizationModuleTest(const std::string& name, ISvcLocator* pSvcLocator);
        
       /* finalize */
       StatusCode finalize();

       /* specify the test here */
       StatusCode runTest() override;
       
       /** initialize the test, i.e. retrieve the TrackingGeometry Svc */
       StatusCode initializeTest() override;
       
       /** book the validation tree */
       StatusCode bookTree() override;
       
     private:
       /** Build the Detector Module */
       StatusCode buildDetectorModule();
       
       double                                               m_halfThickness;
       int                                                  m_readoutDirection;
       double                                               m_lorentzAngle;
       int                                                  m_binsX;
       double                                               m_pitchX;
       double                                               m_pitchXmax;
       int                                                  m_binsY;
       double                                               m_pitchY;
       bool                                                 m_testTrapezoidal;
       
       ToolHandle<IModuleStepper>                           m_digitizationStepper;
       
       DigitizationModule*                                  m_digitizationModule;
       
       bool                                                 m_recordSurfaces;  
       std::string                                          m_recordSurfacesCollectionName;
       
       bool                                                 m_writeTTree;
       TTree*                                               m_tree;
       std::string                                          m_treeName;
       std::string                                          m_treeFolder;
       std::string                                          m_treeDescription;
       
       float                                                m_startPositionX;
       float                                                m_startPositionY;
       float                                                m_startPositionZ;
       float                                                m_endPositionX;
       float                                                m_endPositionY;
       float                                                m_endPositionZ;
              
       float                                                m_clusterTrueX;
       float                                                m_clusterTrueY;
       int                                                  m_clusterTrueIdsX;
       int                                                  m_clusterTrueIdsY;

       float                                                m_nominalPathLength;
       float                                                m_accumulatedPathLength;

       float                                                m_clusterRecoX;
       float                                                m_clusterRecoY;
       int                                                  m_clusterRecoIdsX;
       int                                                  m_clusterRecoIdsY;

       std::vector< int >*                                  m_cellIdsX;
       std::vector< int >*                                  m_cellIdsY;
       std::vector< float >*                                m_cellPositionX;
       std::vector< float >*                                m_cellPositionY;
       std::vector< float >*                                m_cellStep;
       std::vector< float >*                                m_cellDriftLength;
         
       std::vector< float >*                                m_stepStartPositionX;
       std::vector< float >*                                m_stepStartPositionY;
       std::vector< float >*                                m_stepStartPositionZ;
       std::vector< float >*                                m_stepEndPositionX;
       std::vector< float >*                                m_stepEndPositionY;
       std::vector< float >*                                m_stepEndPositionZ;
       
       // run parameters
       double                                               m_incidentDeltaBinsRangeX;
       double                                               m_incidentDeltaBinsRangeY;
       
   };
}

#endif
