/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MappingTest.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk includes
#include "TrkDetDescrUnitTests/MappingTest.h"
#include "TrkDetDescrInterfaces/IGeometryProcessor.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkNeutralParameters/NeutralParameters.h"
//Root includes
#include "TTree.h"
#include "TString.h"
// Gaudi
#include "GaudiKernel/ITHistSvc.h"


Trk::MappingTest::MappingTest(const std::string& name, ISvcLocator* pSvcLocator) :
 Trk::TrkDetDescrUnitTestBase(name, pSvcLocator),
   m_executed(false),
   m_trackingGeometrySvc("TrackingGeometrySvc","AtlasTrackingGeometrySvc"),
   m_trackingGeometry(nullptr),
   m_trackingGeometryName("AtlasTrackingGeometry"),
   m_etaCutOff(6.0),
   m_mappingVolumeName("InDet::Detectors::Pixel::Barrel"), 
   m_mappingTreeName("LayerMappingTest"),
   m_mappingTreeDescription("Test Algorithm for Layer - 3D point association"),
   m_mappingTree(nullptr),
   m_mappingPositionX(0.),
   m_mappingPositionY(0.),
   m_mappingPositionZ(0.),
   m_mappingPositionR(0.),
   m_assignedPositionX(0.),
   m_assignedPositionY(0.),
   m_assignedPositionZ(0.),
   m_assignedPositionR(0.),
   m_assignedCorrection(0.),
   m_assignedLayerIndex(0),
   m_assignmentDistance(0.),
   m_unmappedTree(nullptr),
   m_unmappedPositionX(0.),
   m_unmappedPositionY(0.),
   m_unmappedPositionZ(0.),
   m_unmappedPositionR(0.)
 {
     // get the service handle for the TrackingGeometry
     declareProperty("TrackingGeometrySvc"         , m_trackingGeometrySvc);
     // Eta cut off
     declareProperty("EtaCutOff"                   , m_etaCutOff);
     declareProperty("HighestVolume"               , m_mappingVolumeName);
     //
     declareProperty("MappingTreeName"             , m_mappingTreeName);
     declareProperty("MappingTreeDescription"      , m_mappingTreeDescription);
 }

StatusCode Trk::MappingTest::initializeTest() 
{

    // Get the TrackingGeometry from StoreGate
    // initialize the TrackingGeometrySvc
    if (m_trackingGeometrySvc.retrieve().isFailure()) {
        ATH_MSG_FATAL( "Cannot retrieve TrackingGeometrySvc. Abort job. " );
        return StatusCode::FAILURE;
    }
    m_trackingGeometryName = m_trackingGeometrySvc->trackingGeometryName();
        
    return StatusCode::SUCCESS;
}

StatusCode Trk::MappingTest::bookTree()
{
    ATH_MSG_VERBOSE("Booking the Material Tree.");
    
    // ------------------------------> OUTPUT NTUPLE (geometry validation)
    
    m_mappingTree = new TTree(m_mappingTreeName.c_str(), m_mappingTreeDescription.c_str());
    // add the Branches
    m_mappingTree->Branch("ValPosX",  &m_mappingPositionX, "valpx/F");
    m_mappingTree->Branch("ValPosY",  &m_mappingPositionY, "valpy/F");
    m_mappingTree->Branch("ValPosZ",  &m_mappingPositionZ, "valpz/F");
    m_mappingTree->Branch("ValPosR",  &m_mappingPositionR, "valpr/F");
    
    // add the Branches for the association objects
    m_mappingTree->Branch("AssocPosX",     &m_assignedPositionX,  "asspx/F");
    m_mappingTree->Branch("AssocPosY",     &m_assignedPositionY,  "asspy/F");
    m_mappingTree->Branch("AssocPosZ",     &m_assignedPositionZ,  "asspz/F");
    m_mappingTree->Branch("AssocPosR",     &m_assignedPositionR,  "asspr/F");
    m_mappingTree->Branch("AssocCorr",     &m_assignedCorrection, "asscor/F");   
    m_mappingTree->Branch("AssocLayer",    &m_assignedLayerIndex, "assidx/I");
    m_mappingTree->Branch("AssocDistance", &m_assignmentDistance, "asignd/F");
    
    // ------------------------------> OUTPUT NTUPLE (unmapped hits)
    m_unmappedTree = new TTree("UnmappedHits", "Unmapped Hits of the MappingTest Algorithm");
    // add the Branches
    m_unmappedTree->Branch("UnmappedHitX", &m_unmappedPositionX, "unmappedx/F");
    m_unmappedTree->Branch("UnmappedHitY", &m_unmappedPositionY, "unmappedy/F");
    m_unmappedTree->Branch("UnmappedHitZ", &m_unmappedPositionZ, "unmappedz/F");
    m_unmappedTree->Branch("UnmappedHitR", &m_unmappedPositionR, "unmappedr/F");
    
    // now register the Tree
    ITHistSvc* tHistSvc = nullptr;
    if (service("THistSvc",tHistSvc).isFailure()) {
        ATH_MSG_ERROR( "initialize() Could not find Hist Service  -> Switching Tree output off !" );
        delete m_unmappedTree; m_unmappedTree = nullptr;
        delete m_mappingTree; m_mappingTree = nullptr;
    }
    if (tHistSvc && ((tHistSvc->regTree("/val/UnmappedAssociations", m_unmappedTree)).isFailure()
            ||  (tHistSvc->regTree("/val/MappingTest", m_mappingTree)).isFailure()) ) {
        ATH_MSG_ERROR( "initialize() Could not register the validation Tree -> Switching Tree output off !" );
        delete m_unmappedTree; m_unmappedTree = nullptr;
        delete m_mappingTree; m_mappingTree = nullptr;
    }
    
    return StatusCode::SUCCESS;
    
}

StatusCode Trk::MappingTest::runTest()
{
    ATH_MSG_VERBOSE("Running the MappingTest Test");
    
    // ------------------------------- get the trackingGeometry at first place
    if (!m_trackingGeometry) {
        if ((detStore()->retrieve(m_trackingGeometry, m_trackingGeometryName)).isFailure())
            ATH_MSG_FATAL( "Could not retrieve TrackingGeometry '" << m_trackingGeometryName << "' from DetectorStore." );
        else
            ATH_MSG_INFO( "TrackingGeometry '" << m_trackingGeometryName << "' successfully retrieved from DetectorStore." );
    }
    // only run if it didn't already run before
    if (!m_executed && m_trackingGeometry){
        // get the volume and the overal boundaries
        const TrackingVolume* tVolume = m_trackingGeometry->trackingVolume(m_mappingVolumeName);
        if (tVolume){
            ATH_MSG_INFO("Retrieved TrackingVolume '" << tVolume->volumeName() << "'. Start the mapping validation.");
            const Trk::CylinderVolumeBounds* cvb = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(tVolume->volumeBounds()));
            if (cvb){
                double mRad  = cvb->innerRadius();
                double dRad  = cvb->deltaRadius();
                double zHalf = cvb->halflengthZ();
                // run the mapping
                for ( size_t it = 0; it < TrkDetDescrUnitTestBase::m_numTests; ++it ){ 
                    // create the mapping position
                    double mapRad = mRad + TrkDetDescrUnitTestBase::m_flatDist->shoot()*dRad;
                    double mapPhi = M_PI * (2*TrkDetDescrUnitTestBase::m_flatDist->shoot() - 1.);
                    double mapZ   = zHalf * (2*TrkDetDescrUnitTestBase::m_flatDist->shoot() - 1.);
                    double mapX   = mapRad*cos(mapPhi);
                    double mapY   = mapRad*sin(mapPhi);
                    // the mapping position & unit is the same
                    Amg::Vector3D position(mapX, mapY, mapZ);
                    Amg::Vector3D direction = position.normalized();
                    // create curvilinear parameters
                    Trk::NeutralCurvilinearParameters ncp(position,direction,0.);
                    // get the closest material layer
                    const Trk::LayerIntersection<Amg::Vector3D> lIntersect 
                        = m_trackingGeometry->closestMaterialLayer<Trk::NeutralCurvilinearParameters>(ncp, Trk::mappingMode, true);
                    if (lIntersect.intersection.valid){
                        // the mapping positions
                        m_mappingPositionX = mapX;
                        m_mappingPositionY = mapY;
                        m_mappingPositionZ = mapZ;
                        m_mappingPositionR = mapRad;
                        // the resulting assigned position
                        m_assignedPositionX  = lIntersect.intersection.position.x();
                        m_assignedPositionY  = lIntersect.intersection.position.y();
                        m_assignedPositionZ  = lIntersect.intersection.position.z();
                        m_assignedPositionR  = lIntersect.intersection.position.perp();
                        m_assignedCorrection = 0.;
                        m_assignedLayerIndex = lIntersect.object->layerIndex().value();
                        m_assignmentDistance = lIntersect.intersection.pathLength;
                        // and now write the tree
                        m_mappingTree->Fill();
                    } else {
                        // the unmapped positions
                        // and now fill the tree
                        m_unmappedPositionX = mapY;
                        m_unmappedPositionY = mapY;
                        m_unmappedPositionZ = mapZ;
                        m_unmappedPositionR = mapRad;
                        // and now fill the tree
                        m_unmappedTree->Fill();
                    }
                }
            }
        } else 
            ATH_MSG_INFO("Could not retrieve the TrackingVolume '" << m_mappingVolumeName << "'." );
        
    }
    m_executed = true;
    return StatusCode::SUCCESS;
       
}


