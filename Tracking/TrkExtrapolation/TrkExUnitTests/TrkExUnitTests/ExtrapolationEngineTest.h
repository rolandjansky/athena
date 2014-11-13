/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ExtrapolationEngineTest.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXUNITTESTS_EXTRAPOLATIONENGINETEST_H
#define TRKEXUNITTESTS_EXTRAPOLATIONENGINETEST_H

// Athena & Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
// Trk includes
#include "TrkExUnitTests/TrkExUnitTestBase.h"
#include "TrkExInterfaces/IExtrapolationEngine.h"
#include "TString.h"

class TTree;

namespace Trk {
    
    class IExtrapolationEngine;
             
    /** @class ExtrapolationEngineTest
               
        @author Andreas.Salzburger@cern.ch       
      */
      
    class ExtrapolationEngineTest : public TrkExUnitTestBase  {
     
     public:

       /** Standard Athena-Algorithm Constructor */
       ExtrapolationEngineTest(const std::string& name, ISvcLocator* pSvcLocator);
        
       /* finalize */
       StatusCode finalize();

       /* specify the test here */
       StatusCode runTest();
       
       /** initialize the test, i.e. retrieve the TrackingGeometry Svc */
       StatusCode initializeTest();
       
       /* book the TTree branches */
       StatusCode bookTree();
       
     private:
       template <class T, class P> StatusCode runTestT(); 
         
       template <class T, class P> StatusCode fillStepInformationT(ExtrapolationCell<T>& eCell, int fwbw);
         
       /** retrieve it */
       ToolHandle<IExtrapolationEngine>             m_extrapolationEngine;     
       
       bool                                         m_parametersMode; // 0 - neutral, 1 - charged, 2 - multi
       
     
       double                                       m_etaMin;
       double                                       m_etaMax;
       double                                       m_phiMin;
       double                                       m_phiMax; 
       
       double                                       m_ptMin;
       double                                       m_ptMax;
       
       double                                       m_pathLimit;
       
       bool                                         m_collectSensitive;
       bool                                         m_collectPassive;
       bool                                         m_collectBoundary;
       bool                                         m_collectMaterial;
       
       bool                                         m_backExtrapolation;
         
       bool                                         m_writeTTree;
       std::string                                  m_treeName;
       std::string                                  m_treeFolder;  
       std::string                                  m_treeDescription;
       TTree*                                       m_tree;
                                                    
       float                                        m_startPositionX;
       float                                        m_startPositionY;
       float                                        m_startPositionZ;
       float                                        m_startPositionR;
       float                                        m_startPhi;
       float                                        m_startTheta;                                                    
       float                                        m_startEta;                                                    
       float                                        m_startP;                                                    
       float                                        m_startPt;
       float                                        m_charge;                                                    
        
       int                                          m_endSuccessful;
       float                                        m_endPositionX;
       float                                        m_endPositionY;
       float                                        m_endPositionZ;
       float                                        m_endPositionR;
       float                                        m_endPhi;
       float                                        m_endTheta;                                                    
       float                                        m_endEta;                                                    
       float                                        m_endP;                                                    
       float                                        m_endPt; 
       
       int                                          m_backSuccessful;
       float                                        m_backPositionX;
       float                                        m_backPositionY;
       float                                        m_backPositionZ;
       float                                        m_backPositionR;
       float                                        m_backPhi;
       float                                        m_backTheta;                                                    
       float                                        m_backEta;                                                    
       float                                        m_backP;                                                    
       float                                        m_backPt; 
       
       std::vector<TString>                         m_parameterNames;                                                  
       std::vector< std::vector< float >* >         m_pPositionX;
       std::vector< std::vector< float >* >         m_pPositionY;
       std::vector< std::vector< float >* >         m_pPositionZ;
       std::vector< std::vector< float >* >         m_pPositionR;
       std::vector< std::vector< float >* >         m_pPhi;
       std::vector< std::vector< float >* >         m_pTheta;                                                
       std::vector< std::vector< float >* >         m_pEta;                                                    
       std::vector< std::vector< float >* >         m_pP;                                              
       std::vector< std::vector< float >* >         m_pPt;
       
       float                                        m_materialThicknessInX0;
       float                                        m_materialThicknessInL0;

       float                                        m_materialThicknessInX0Bwd;
       float                                        m_materialThicknessInL0Bwd;

       float                                        m_materialThicknessInX0Sensitive;
       float                                        m_materialThicknessInX0Passive;
       float                                        m_materialThicknessInX0Boundary;
       
       float                                        m_materialThicknessInX0Cylinder;
       float                                        m_materialThicknessInX0Disc;
       float                                        m_materialThicknessInX0Plane;
       
       std::vector< float >*                        m_materialThicknessInX0Steps;
       std::vector< float >*                        m_materialThicknessInL0Steps;
       std::vector< float >*                        m_materialPositionX;
       std::vector< float >*                        m_materialPositionY;
       std::vector< float >*                        m_materialPositionZ;
       std::vector< float >*                        m_materialPositionR;
       std::vector< float >*                        m_materialScaling;
       std::vector< int   >*                        m_stepDirection;

   };
}

// include the templated function
#include "ExtrapolationEngineTest.icc"

#endif
