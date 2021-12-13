/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "TRandom.h"

class TTree;

class PixelID;
class SCT_ID;
class HGTD_ID;
class AtlasDetectorID;

namespace Trk {
  class IExtrapolationEngine;
  class IPositionMomentumWriter;

  /** @class ExtrapolationEngineTest

      Test Algorithm to run test extrapolations with the new IExtrapolationEngine

      @author Andreas.Salzburger@cern.ch, Noemi.Calace@cern.ch
   */

  class ExtrapolationEngineTest: public TrkExUnitTestBase  {
  public:
    /** Standard Athena-Algorithm Constructor */
    ExtrapolationEngineTest(const std::string& name, ISvcLocator* pSvcLocator);

    /* finalize */
    StatusCode finalize();

    /* specify the test here */
    StatusCode runTest();

    /* specify the scan here */
    StatusCode runScan();

    /** initialize the test, i.e. retrieve the TrackingGeometry Svc */
    StatusCode initializeTest();

    /* book the TTree branches */
    StatusCode bookTree();
  private:
       template <class T, class P> StatusCode runTestT(); 
         
       template <class T, class P> StatusCode fillStepInformationT(ExtrapolationCell<T>& eCell, int fwbw, std::vector<const Trk::Surface*>& stepSurfaces);
         
       /** retrieve it */
       ToolHandle<IExtrapolationEngine>             m_extrapolationEngine;     
       
       const AtlasDetectorID*                       m_idHelper;
       const PixelID*                               m_pixel_ID;       //!< Handle to the ID helper
       const SCT_ID*                                m_sct_ID;         //!< Handle to the ID helper
       const HGTD_ID*                               m_hgtd_ID;       //!< Handle to the ID helper
       bool                                         m_useHGTD;
              
       bool                                         m_parametersMode; // 0 - neutral, 1 - charged, 2 - multi
       int                                          m_particleHypothesis;
     
       bool                                         m_smearProductionVertex;
       bool                                         m_smearFlatOriginT;
       bool                                         m_smearFlatOriginZ;
       double                                       m_sigmaOriginT;
       double                                       m_sigmaOriginZ;
       double                                       m_d0Min;
       double                                       m_d0Max;
       double                                       m_z0Min;
       double                                       m_z0Max;
       std::vector<float>                           m_z0Values;
     
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
       bool                                         m_stepwiseExtrapolation;
       
       
       /** scanning parameters */
       int                                          m_stepsPhi;
       int                                          m_currentPhiStep;
       std::vector< float >                         m_etaScans;
       double                                       m_currentEta;
       std::vector< float >                         m_phiScans;
       double                                       m_currentPhi;
       bool                                         m_splitCharge;

       //!< the tree
         
       bool                                         m_writeTTree;
       ToolHandle<IPositionMomentumWriter>          m_posmomWriter;
       
       std::string                                  m_treeName;
       std::string                                  m_treeFolder;  
       std::string                                  m_treeDescription;
       TTree*                                       m_tree = nullptr;
       TRandom                                      m_tRandom;        
                                                    
                                                    
       float                                        m_startPositionX = 0.0F;
       float                                        m_startPositionY = 0.0F;
       float                                        m_startPositionZ = 0.0F;
       float                                        m_startPositionR = 0.0F;
       float                                        m_startPhi = 0.0F;
       float                                        m_startTheta = 0.0F;                                                    
       float                                        m_startEta = 0.0F;                                                    
       float                                        m_startP = 0.0F;                                                    
       float                                        m_startPt = 0.0F;
       float                                        m_charge;                                                    
        
       int                                          m_endSuccessful = 0;
       float                                        m_endPositionX = 0.0F;
       float                                        m_endPositionY = 0.0F;
       float                                        m_endPositionZ = 0.0F;
       float                                        m_endPositionR = 0.0F;
       float                                        m_endPhi = 0.0F;
       float                                        m_endTheta = 0.0F;                                                    
       float                                        m_endEta = 0.0F;                                                    
       float                                        m_endP = 0.0F;                                                    
       float                                        m_endPt = 0.0F;        
       float                                        m_endPathLength = 0.0F;
       
       int                                          m_backSuccessful = 0;
       float                                        m_backPositionX = 0.0F;
       float                                        m_backPositionY = 0.0F;
       float                                        m_backPositionZ = 0.0F;
       float                                        m_backPositionR = 0.0F;
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

       std::vector< int >*                          m_sensitiveSurfaceType = nullptr;
       std::vector< int >*                          m_sensitiveLayerIndex;
       std::vector< float >*                        m_sensitiveLocalPosX;
       std::vector< float >*                        m_sensitiveLocalPosY;
       std::vector< float >*                        m_sensitiveCenterPosX;
       std::vector< float >*                        m_sensitiveCenterPosY;
       std::vector< float >*                        m_sensitiveCenterPosZ;
       std::vector< float >*                        m_sensitiveCenterPosR;
       std::vector< float >*                        m_sensitiveLocalPosR = nullptr;
       std::vector< float >*                        m_sensitiveLocalPosPhi = nullptr;
       std::vector< int >*                          m_sensitiveDetector = nullptr;
       std::vector< int >*                          m_sensitiveIsInnermost = nullptr;
       std::vector< int >*                          m_sensitiveIsNextToInnermost = nullptr;
       std::vector< int >*                          m_sensitiveBarrelEndcap = nullptr;
       std::vector< int >*                          m_sensitiveLayerDisc = nullptr;
       std::vector< int >*                          m_sensitiveEtaModule = nullptr;
       std::vector< int >*                          m_sensitivePhiModule = nullptr;
       std::vector< int >*                          m_sensitiveSide = nullptr;
       std::vector< int >*                          m_sensitiveNearBondGap = nullptr;
       std::vector< int >*                          m_sensitiveisInside = nullptr;
       std::vector< int >*                          m_sensitiveisInsideBound = nullptr;
       std::vector< float >*                        m_materialtInX0AccumulatedUpTo = nullptr;
       
       float                                        m_materialThicknessInX0;
       float                                        m_materialThicknessInL0;
       float                                        m_materialThicknessZARho;
       float                                        m_materialEmulatedIonizationLoss;

       float                                        m_materialThicknessInX0Bwd;
       float                                        m_materialThicknessInL0Bwd;

       float                                        m_materialThicknessInX0Sensitive;
       float                                        m_materialThicknessInX0Passive;
       float                                        m_materialThicknessInX0Boundary;
       
       float                                        m_materialThicknessInX0Cylinder;
       float                                        m_materialThicknessInX0Disc;
       float                                        m_materialThicknessInX0Plane;
       
       std::vector< float >*                        m_materialThicknessInX0Accumulated;
       std::vector< float >*                        m_materialThicknessInX0Steps;
       std::vector< float >*                        m_materialThicknessInL0Steps;
       std::vector< float >*                        m_materialPositionX;
       std::vector< float >*                        m_materialPositionY;
       std::vector< float >*                        m_materialPositionZ;
       std::vector< float >*                        m_materialPositionR;
       std::vector< float >*                        m_materialPositionP;
       std::vector< float >*                        m_materialPositionPt;
       std::vector< float >*                        m_materialScaling;
       std::vector< int   >*                        m_stepDirection;
       
       int                                          m_endStepSuccessful;
       float                                        m_endStepPositionX;
       float                                        m_endStepPositionY;
       float                                        m_endStepPositionZ;
       float                                        m_endStepPositionR;
       float                                        m_endStepPhi;
       float                                        m_endStepTheta;                                                    
       float                                        m_endStepEta;                                                    
       float                                        m_endStepP;                                                    
       float                                        m_endStepPt;        
       float                                        m_endStepPathLength;
       float                                        m_endStepThicknessInX0;

   };
}

// include the templated function
#include "ExtrapolationEngineTest.icc"

#endif
