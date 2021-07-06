/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// HitPositionNtupleHelper.cxx
//   Source file for class HitPositionNtupleHelper
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Anna.Franckowiak@cern.ch, Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////


//Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ITHistSvc.h"

#include "GaudiKernel/ISvcLocator.h"

// Trk
#include "TrkValTools/HitPositionNtupleHelper.h"
#include "TrkEventPrimitives/TrackStateDefs.h"

#include "TrkMeasurementBase/MeasurementBase.h"
//#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TTree.h"

// constructor
Trk::HitPositionNtupleHelper::HitPositionNtupleHelper(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
  AthAlgTool(t,n,p),

  m_hitX(nullptr), // do careful initialisation so we can issue a safe delete
  m_hitY(nullptr),
  m_hitZ(nullptr),
  m_hitRadius(nullptr),
  m_hitPhi(nullptr),
  m_hitLoc1(nullptr),
  m_hitLoc2(nullptr),
  m_hitSurfaceX(nullptr),
  m_hitSurfaceY(nullptr),
  m_hitSurfaceZ(nullptr),
  m_hitSurfaceRadius(nullptr),
  m_hitSurfacePhi(nullptr),
  m_hitSurfaceTheta(nullptr)

 {
    declareInterface<IValidationNtupleHelperTool>(this);
    //declareProperty("IgnoreMissingTrackCovarianceForPulls", mjo_ignoreMissTrkCov = false);
}

// destructor
Trk::HitPositionNtupleHelper::~HitPositionNtupleHelper() {}



//////////////
/// initialize
//////////////
StatusCode Trk::HitPositionNtupleHelper::initialize() {

    m_hitX              = new std::vector<float>();
    m_hitY              = new std::vector<float>();
    m_hitZ              = new std::vector<float>();
    m_hitRadius         = new std::vector<float>();
    m_hitPhi            = new std::vector<float>();
    m_hitLoc1           = new std::vector<float>();
    m_hitLoc2           = new std::vector<float>();
    m_hitSurfaceX       = new std::vector<float>();
    m_hitSurfaceY       = new std::vector<float>();
    m_hitSurfaceZ       = new std::vector<float>();
    m_hitSurfaceRadius  = new std::vector<float>();
    m_hitSurfacePhi     = new std::vector<float>();
    m_hitSurfaceTheta   = new std::vector<float>();

    return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode Trk::HitPositionNtupleHelper::finalize() {

  delete             m_hitX;
  delete             m_hitY;
  delete             m_hitZ;
  delete        m_hitRadius;
  delete           m_hitPhi;
  delete          m_hitLoc1;
  delete          m_hitLoc2;
  delete      m_hitSurfaceX;
  delete      m_hitSurfaceY;
  delete      m_hitSurfaceZ;
  delete m_hitSurfaceRadius;
  delete    m_hitSurfacePhi;
  delete  m_hitSurfaceTheta;

  ATH_MSG_INFO ("finalize() successful in " << name() );
  return StatusCode::SUCCESS;
}


///////////////////////////////////////
/// addNtupleItems
///////////////////////////////////////
StatusCode Trk::HitPositionNtupleHelper::addNtupleItems (
    TTree* tree,
    const int& ) const {

    
    // add items
    tree->Branch("HitX",                &m_hitX);
    tree->Branch("HitY",                &m_hitY);
    tree->Branch("HitZ",                &m_hitZ);
    tree->Branch("HitRadius",           &m_hitRadius);
    tree->Branch("HitPhi",              &m_hitPhi);
    tree->Branch("HitLocal1",           &m_hitLoc1);
    tree->Branch("HitLocal2",           &m_hitLoc2);
    tree->Branch("HitSurfaceX",         &m_hitSurfaceX);
    tree->Branch("HitSurfaceY",         &m_hitSurfaceY);
    tree->Branch("HitSurfaceZ",         &m_hitSurfaceZ);
    tree->Branch("HitSurfaceRadius",    &m_hitSurfaceRadius);
    tree->Branch("HitSurfacePhi",       &m_hitSurfacePhi);
    tree->Branch("HitSurfaceTheta",     &m_hitSurfaceTheta);


    ATH_MSG_VERBOSE ("added items to ntuple");
    return StatusCode::SUCCESS;
}

////////////////////////////////////////
/// fill hit position data
////////////////////////////////////////
StatusCode Trk::HitPositionNtupleHelper::fillMeasurementData (
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters*,
    const int& /*detectorType*/,
    const bool&) const {


    if (!measurement) {
        ATH_MSG_ERROR ("no MeasurementBase given");
        return StatusCode::FAILURE;
    }
    //--------------------------
    // hit position info

    // get surface, local and global parameters of the measurement
    const Trk::Surface& hitSurface  = measurement->associatedSurface();
    const Amg::Vector3D& sfc  = hitSurface.center();

    const Trk::LocalParameters& lp = measurement->localParameters();
    const Amg::Vector3D&  gp  = measurement->globalPosition();
    
    // fill measurement global position
    m_hitX->push_back( float(gp.x()) );
    m_hitY->push_back( float(gp.y()) );
    m_hitZ->push_back( float(gp.z()) );
    m_hitRadius->push_back( float(gp.perp()) );
    m_hitPhi->push_back( float(gp.phi()) );

    // fill surface global position
    m_hitSurfaceX->push_back( float(sfc.x()) );
    m_hitSurfaceY->push_back( float(sfc.y()) );
    m_hitSurfaceZ->push_back( float(sfc.z()) );
    m_hitSurfaceRadius->push_back( float(sfc.perp()) );
    m_hitSurfacePhi->push_back( float(sfc.phi()) );
    m_hitSurfaceTheta->push_back( float(sfc.theta()) );

    // fill measurement local positions - those which are measured
    if (lp.contains(Trk::loc1))
      m_hitLoc1->push_back( float(lp[Trk::loc1]) );
    else
      m_hitLoc1->push_back( s_errorEntry );

    if (lp.contains(Trk::loc2))
      m_hitLoc2->push_back( float(lp[Trk::loc2]) );
    else
      m_hitLoc2->push_back( s_errorEntry );

    return StatusCode::SUCCESS;
}

StatusCode Trk::HitPositionNtupleHelper::resetVariables (
        const int&  ) const {
    m_hitX->clear();
    m_hitY->clear();
    m_hitZ->clear();
    m_hitRadius->clear();
    m_hitPhi->clear();
    m_hitLoc1->clear();
    m_hitLoc2->clear();
    m_hitSurfaceX->clear();
    m_hitSurfaceY->clear();
    m_hitSurfaceZ->clear();
    m_hitSurfaceRadius->clear();
    m_hitSurfacePhi->clear();
    m_hitSurfaceTheta->clear();
    
    return StatusCode::SUCCESS;
}



StatusCode Trk::HitPositionNtupleHelper::fillHoleData (
        const Trk::TrackStateOnSurface&,
        const int&) const {
    // we do nothing with holes
    return StatusCode::SUCCESS;
}
