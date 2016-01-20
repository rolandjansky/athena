/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DigitizationModuleTest.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg includes
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
// Trk includes
#include "TrkDigInterfaces/IModuleStepper.h"
#include "TrkDigUnitTests/DigitizationModuleTest.h"
#include "TrkDigEvent/RectangularSegmentation.h"
#include "TrkDigEvent/TrapezoidSegmentation.h"
#include "TrkDigEvent/DigitizationModule.h"
#include "TrkDigEvent/DigitizationCell.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/SurfaceCollection.h"
//Root includes
#include "TTree.h"
#include "TString.h"
// Gaudi
#include "GaudiKernel/ITHistSvc.h"

Trk::DigitizationModuleTest::DigitizationModuleTest(const std::string& name, ISvcLocator* pSvcLocator) :
 Trk::TrkDigUnitTestBase(name, pSvcLocator),
 m_halfThickness(0.250),
 m_readoutDirection(1),
 m_lorentzAngle(0.2),
 m_binsX(328),
 m_pitchX(0.05),
 m_pitchXmax(0.05),
 m_binsY(128),
 m_pitchY(0.4),
 m_testTrapezoidal(false),
 m_digitizationStepper(),
 m_digitizationModule(nullptr),
 m_recordSurfaces(true),
 m_recordSurfacesCollectionName("SurfaceCollection"),
 m_writeTTree(true),
 m_tree(nullptr),
 m_treeName("DigitizationModuleTest"),
 m_treeFolder("/val/"),
 m_treeDescription("GeometricDigitization test setup"), 
 m_incidentDeltaBinsRangeX(10),
 m_incidentDeltaBinsRangeY(10)
{
  // Module specification    
  declareProperty("ModuleHalfThickness",      m_halfThickness);
  declareProperty("ModuleReadoutDirection",   m_readoutDirection);
  declareProperty("LorentzAngle",             m_lorentzAngle);
  declareProperty("BinsX",                    m_binsX);
  declareProperty("PitchX",                   m_pitchX);
  declareProperty("PitchXmax",                m_pitchXmax);
  declareProperty("BinsY",                    m_binsY);
  declareProperty("PitchY",                   m_pitchY);
  declareProperty("TestTrapezoidal",          m_testTrapezoidal);
  // the stepper
  declareProperty("DigitizationStepper",      m_digitizationStepper);
  // display surfaces
  declareProperty("RecordSurfaces",           m_recordSurfaces);
  declareProperty("RecordSurfacesName",       m_recordSurfacesCollectionName);
  // wirte the tree of the test
  declareProperty("WriteTTree",               m_writeTTree);
  declareProperty("TreeName",                 m_treeName);
  declareProperty("TreeFolder",               m_treeFolder);
  declareProperty("TreeDescription",          m_treeDescription);   
  // incident emulation
  declareProperty("MaxClusterSizeX",          m_incidentDeltaBinsRangeX);
  declareProperty("MaxClusterSizeY",          m_incidentDeltaBinsRangeY);
}

StatusCode Trk::DigitizationModuleTest::finalize() {

    delete m_digitizationModule; m_digitizationModule = nullptr;
    return Trk::TrkDigUnitTestBase::finalize();   
}

StatusCode Trk::DigitizationModuleTest::initializeTest() 
{
    
    if (m_digitizationStepper.retrieve().isFailure()){
        ATH_MSG_ERROR("Could not retrieve digitization stepper. Aborting.");
        return StatusCode::FAILURE;
    }
    
    // success return
    return buildDetectorModule();    
}


StatusCode Trk::DigitizationModuleTest::buildDetectorModule() {
  
  if (!m_testTrapezoidal) {    
    // rectangle bounds 
    std::shared_ptr<const Trk::RectangleBounds> rectangleBounds(new Trk::RectangleBounds(0.5*m_binsX*m_pitchX,0.5*m_binsY*m_pitchY));    
    // create the segmentation
    std::shared_ptr<const Trk::Segmentation> rectangleSegmentation(new Trk::RectangularSegmentation(rectangleBounds,(size_t)m_binsX,(size_t)m_binsY));
    // build the module
    m_digitizationModule = new Trk::DigitizationModule(rectangleSegmentation, 
                                                       m_halfThickness,
                                                       m_readoutDirection,
                                                       m_lorentzAngle);
    ATH_MSG_INFO("Building Rectangle Segmentation with dimensions (halfX, halfY) = (" << 0.5*m_binsX*m_pitchX << ", " << 0.5*m_binsY*m_pitchY << ")");
  } else {
    // trapezoidal bounds 
    std::shared_ptr<const Trk::TrapezoidBounds> trapezoidBounds(new Trk::TrapezoidBounds(0.5*m_binsX*m_pitchX,0.5*m_binsX*m_pitchXmax,0.5*m_binsY*m_pitchY));    
    // create the segmentation
    std::shared_ptr<const Trk::Segmentation> trapezoidSegmentation(new Trk::TrapezoidSegmentation(trapezoidBounds,(size_t)m_binsX,(size_t)m_binsY));
    // build the module
    m_digitizationModule = new Trk::DigitizationModule(trapezoidSegmentation, 
                                                       m_halfThickness,
                                                       m_readoutDirection,
                                                       m_lorentzAngle);
    ATH_MSG_INFO("Building Trapezoid Segmentation with dimensions (halfXmin, halfXmax, halfY) = (" << 0.5*m_binsX*m_pitchX << ", " << 0.5*m_binsX*m_pitchXmax << ", " << 0.5*m_binsY*m_pitchY << ")");
  }                                                       
                                                        
  // record the surfaces to StoreGate if you want
  if (m_recordSurfaces){
      // the surface collection
      SurfaceCollection* sCollection = new SurfaceCollection(SG::VIEW_ELEMENTS);
      // reserve the right amount of memory
      sCollection->reserve(m_binsX+m_binsY+6);
      // fill the boudnaries
      for (auto& surfacePtr : m_digitizationModule->boundarySurfaces())
          sCollection->push_back(surfacePtr.get());
      // fill the X bins
      for (auto& surfacePtr : m_digitizationModule->segmentationSurfacesX())
          sCollection->push_back(surfacePtr.get());
      // fill the Y bins
      //for (auto& surfacePtr : m_digitizationModule->segmentationSurfacesY())
      //    sCollection->push_back(surfacePtr.get());
      // and now let's record them to StoreGate
      if (evtStore()->record(sCollection,m_recordSurfacesCollectionName).isFailure())
          ATH_MSG_WARNING("Could not register SurfaceCollection in StoreGate");
      else
          ATH_MSG_INFO("SurfaceCollection successfully registered.");
  }
                                                        
  // success return
  return StatusCode::SUCCESS;   
    
}

StatusCode Trk::DigitizationModuleTest::bookTree()
{
     
     m_cellIdsX              = new std::vector<int>;
     m_cellIdsY              = new std::vector<int>;
     m_cellPositionX         = new std::vector<float>;
     m_cellPositionY         = new std::vector<float>;
     m_cellStep              = new std::vector<float>;
     m_cellDriftLength       = new std::vector<float>;
     m_stepStartPositionX    = new std::vector<float>;
     m_stepStartPositionY    = new std::vector<float>;
     m_stepStartPositionZ    = new std::vector<float>;
     m_stepEndPositionX      = new std::vector<float>;
     m_stepEndPositionY      = new std::vector<float>;
     m_stepEndPositionZ      = new std::vector<float>;
          
     // ------------------------------> OUTPUT NTUPLE (geometry validation)
     m_tree = new TTree(m_treeName.c_str(), m_treeDescription.c_str());
     // add the Branches
     m_tree->Branch("StartPosX",             &m_startPositionX);
     m_tree->Branch("StartPosY",             &m_startPositionY);
     m_tree->Branch("StartPosZ",             &m_startPositionZ);
     m_tree->Branch("EndPosX",               &m_endPositionX);
     m_tree->Branch("EndPosY",               &m_endPositionY);
     m_tree->Branch("EndPosZ",               &m_endPositionZ);
     m_tree->Branch("ClusterTrueX",          &m_clusterTrueX);
     m_tree->Branch("ClusterTrueY",          &m_clusterTrueY);
     m_tree->Branch("ClusterTrueCellIdX",    &m_clusterTrueIdsX);
     m_tree->Branch("ClusterTrueCellIdY",    &m_clusterTrueIdsY);
     m_tree->Branch("ClusterRecoX",          &m_clusterRecoX);
     m_tree->Branch("ClusterRecoY",          &m_clusterRecoY);
     m_tree->Branch("ClusterRecoCellIdX",    &m_clusterRecoIdsX);
     m_tree->Branch("ClusterRecoCellIdY",    &m_clusterRecoIdsY);
     m_tree->Branch("PathLengthNominal",     &m_nominalPathLength);
     m_tree->Branch("PathLengthAccumulated", &m_accumulatedPathLength);
     m_tree->Branch("StepCellIdX",           &m_cellIdsX);       
     m_tree->Branch("StepCellIdY",           &m_cellIdsY);       
     m_tree->Branch("StepCellPositionX",     &m_cellPositionX);  
     m_tree->Branch("StepCellPositionY",     &m_cellPositionY);  
     m_tree->Branch("StepLength",            &m_cellStep);
     m_tree->Branch("StepDriftLength",       &m_cellDriftLength);       
     m_tree->Branch("StepStartPositionX",    &m_stepStartPositionX);  
     m_tree->Branch("StepStartPositionY",    &m_stepStartPositionY);       
     m_tree->Branch("StepStartPositionZ",    &m_stepStartPositionZ);  
     m_tree->Branch("StepEndPositionX",      &m_stepEndPositionX);  
     m_tree->Branch("StepEndPositionY",      &m_stepEndPositionY);       
     m_tree->Branch("StepEndPositionZ",      &m_stepEndPositionZ);  
     
     // now register the Tree
     ITHistSvc* tHistSvc = 0;
     if (service("THistSvc",tHistSvc).isFailure()) {
       ATH_MSG_ERROR( "initialize() Could not find Hist Service  -> Switching Tree output off !" );
         delete m_tree; m_tree = 0;
     } else
         ATH_MSG_INFO( "THistSvc has been retrieved successfully." );
     
     std::string fullTreeName = m_treeFolder+m_treeName;
     if (tHistSvc && ((tHistSvc->regTree(fullTreeName.c_str(), m_tree)).isFailure()) ) {
         ATH_MSG_ERROR( "initialize() Could not register the validation Tree -> Switching Tree output off !" );
         delete m_tree; m_tree = 0;
     } else
         ATH_MSG_INFO("Tree '" << fullTreeName.c_str() << "' successfully registered");
     
     return StatusCode::SUCCESS;     
}

StatusCode Trk::DigitizationModuleTest::runTest()
{
    ATH_MSG_VERBOSE("Running the DigitizationModuleTest Test");
    
    // ----------------- creation of the surfaces & track parameters -------------
    for (size_t it = 0; it < TrkDigUnitTestBase::m_numTests; ++it ){
        
        // true cluster information
        m_clusterTrueX      = 0.;
        m_clusterTrueY      = 0.;
        m_clusterTrueIdsX   = 0.;
        m_clusterTrueIdsY   = 0.;
        

        
        // step information 
        m_cellIdsX->clear();        
        m_cellIdsY->clear();        
        m_cellPositionX->clear();   
        m_cellPositionY->clear();   
        m_cellStep->clear();
        m_cellDriftLength->clear();        
        m_stepStartPositionX->clear();   
        m_stepStartPositionY->clear();   
        m_stepStartPositionZ->clear();           
        m_stepEndPositionX->clear();   
        m_stepEndPositionY->clear();   
        m_stepEndPositionZ->clear();  
        m_nominalPathLength = 0.;
        m_accumulatedPathLength = 0.;
        
        // create a random intersection
        double intersectY       = 0.5*m_binsY*m_pitchY * (2*TrkDigUnitTestBase::m_flatDist->shoot()-1);
	double localPicthX      = m_pitchX;
 	if (m_testTrapezoidal) {
     	  double tanPhi         = 0.5*m_binsX*(m_pitchXmax-m_pitchX)/(0.5*m_binsY*m_pitchY);
     	  double lengthXatHit   = (0.5*m_binsY*m_pitchY*0.5*m_binsX*(m_pitchXmax+m_pitchX)/(0.5*m_binsX*(m_pitchXmax-m_pitchX)) + intersectY)* tanPhi;
 	  localPicthX           = lengthXatHit/float(m_binsX);
 	}
	double intersectX      = 0.5*m_binsX*localPicthX*(2*TrkDigUnitTestBase::m_flatDist->shoot()-1);
	
	Trk::DigitizationCell dCell = m_digitizationModule->segmentation().cell(Amg::Vector3D(intersectX,intersectY,0.));
        
        ATH_MSG_VERBOSE("Created a hit at [intersectX/intersectY] = " << intersectX << "/" << intersectY << " with cell ID = " << dCell.first << "/" << dCell.second);
        
        double intersectDeltaY     = 0.5*m_incidentDeltaBinsRangeY*m_pitchY * (2*TrkDigUnitTestBase::m_flatDist->shoot()-1);
	double intersectDeltaMinX  = 0.5*m_incidentDeltaBinsRangeX*localPicthX * (2*TrkDigUnitTestBase::m_flatDist->shoot()-1);
	double intersectDeltaMaxX  = 0.5*m_incidentDeltaBinsRangeX*localPicthX * (2*TrkDigUnitTestBase::m_flatDist->shoot()-1); 
	
	double clusterPicthX = m_pitchX;
	if (m_testTrapezoidal) {
    	  double tanPhi         = 0.5*m_binsX*(m_pitchXmax-m_pitchX)/(0.5*m_binsY*m_pitchY);
	  // evaluating the intersectDeltaMinX (X variation at position intersectY-intersectDeltaY)
	  double localMinY      = intersectY-intersectDeltaY;
    	  double lengthXatHit   = (0.5*m_binsY*m_pitchY*0.5*m_binsX*(m_pitchXmax+m_pitchX)/(0.5*m_binsX*(m_pitchXmax-m_pitchX)) + localMinY)* tanPhi;
	  double localPicthMinX = lengthXatHit/float(m_binsX);
	  intersectDeltaMinX    = 0.5*m_incidentDeltaBinsRangeX*localPicthMinX * (2*TrkDigUnitTestBase::m_flatDist->shoot()-1);
	  // evaluating the intersectDeltaMinX (X variation at position intersectY+intersectDeltaY)
	  double localMaxY      = intersectY+intersectDeltaY;
    	  lengthXatHit          = (0.5*m_binsY*m_pitchY*0.5*m_binsX*(m_pitchXmax+m_pitchX)/(0.5*m_binsX*(m_pitchXmax-m_pitchX)) + localMaxY)* tanPhi;
	  double localPicthmaxX = lengthXatHit/float(m_binsX);
	  intersectDeltaMaxX    = 0.5*m_incidentDeltaBinsRangeX*localPicthmaxX * (2*TrkDigUnitTestBase::m_flatDist->shoot()-1);
	  clusterPicthX         = 0.5*(localPicthMinX+localPicthmaxX);
	}	
        // screen output
        
        ATH_MSG_VERBOSE("Created a clusters with average size [binsX/binsY] = " << int(2*(0.5*(fabs(intersectDeltaMaxX)+fabs(intersectDeltaMinX)))/clusterPicthX)+1  << "/" << int(2*fabs(intersectDeltaY)/m_pitchY)+1);
        // create the vector position
        Amg::Vector3D entryPoint(intersectX-intersectDeltaMinX,intersectY-intersectDeltaY,-m_halfThickness);
        Amg::Vector3D exitPoint(intersectX+intersectDeltaMinX,intersectY+intersectDeltaY,m_halfThickness);
	
        ATH_MSG_VERBOSE("entryPoint = " << entryPoint.x() << ", " << entryPoint.y() << ", " << entryPoint.z());
	ATH_MSG_VERBOSE("exitPoint  = " << exitPoint.x() << ", " << exitPoint.y() << ", " << exitPoint.z());
        
        // write it to the TTree
        // - input paramters
        m_clusterTrueX    = intersectX;
        m_clusterTrueY    = intersectY;
        m_clusterTrueIdsX = dCell.first;
        m_clusterTrueIdsY = dCell.second;                
        m_startPositionX  = entryPoint.x();
        m_startPositionY  = entryPoint.y();
        m_startPositionZ  = entryPoint.z();
        m_endPositionX    = exitPoint.x();
        m_endPositionY    = exitPoint.y();
        m_endPositionZ    = exitPoint.z();

        // - reco'd parameters
        m_clusterRecoX      = 0.;
        m_clusterRecoY      = 0.;
        m_clusterRecoIdsX   = 0;
        m_clusterRecoIdsY   = 0;

        // now get the steps
        std::vector<Trk::DigitizationStep> digitizationSteps = m_digitizationStepper->cellSteps(*m_digitizationModule,entryPoint,exitPoint);
        // the nominal path lengt
        m_nominalPathLength = (entryPoint-exitPoint).mag();
        
        ATH_MSG_VERBOSE("Stepped through " << digitizationSteps.size() << " individual cells.");


        for (auto& dStep : digitizationSteps){
            ATH_MSG_VERBOSE("Step in cell " << dStep.stepCell.first << "/" << dStep.stepCell.second);
            m_accumulatedPathLength += dStep.stepLength;
            // record the step information
            m_cellIdsX->push_back(dStep.stepCell.first);
            m_cellIdsY->push_back(dStep.stepCell.second);
            m_cellStep->push_back(dStep.stepLength);
            m_cellDriftLength->push_back(dStep.driftLength); 
            m_cellPositionX->push_back(dStep.stepCellCenter.x());  
            m_cellPositionY->push_back(dStep.stepCellCenter.y());  
            m_stepStartPositionX->push_back(dStep.stepEntry.x());  
            m_stepStartPositionY->push_back(dStep.stepEntry.y());       
            m_stepStartPositionZ->push_back(dStep.stepEntry.z());   
            m_stepEndPositionX->push_back(dStep.stepExit.x());  
            m_stepEndPositionY->push_back(dStep.stepExit.y());       
            m_stepEndPositionZ->push_back(dStep.stepExit.z());
            // @TODO implement smearing
            m_clusterRecoX += dStep.stepLength * dStep.stepCellCenter.x(); 
            m_clusterRecoY += dStep.stepLength * dStep.stepCellCenter.y(); 
            
        }
        // reweight
        m_clusterRecoX /= m_accumulatedPathLength;
        m_clusterRecoY /= m_accumulatedPathLength;
        
        Trk::DigitizationCell rCell = m_digitizationModule->segmentation().cell(Amg::Vector2D(m_clusterRecoX,m_clusterRecoY));
        m_clusterRecoIdsX = rCell.first;
        m_clusterRecoIdsY = rCell.second;
        
        // fill the tree
        m_tree->Fill();

    }
    
    return StatusCode::SUCCESS;
}
