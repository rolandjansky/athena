/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// BremPositionNtupleHelper.cxx
//   Source file for class BremPositionNtupleHelper
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////


//Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ITHistSvc.h"

#include "GaudiKernel/ISvcLocator.h"
#include "TTree.h"

// Trk
#include "TrkValTools/BremPositionNtupleHelper.h"

// added SUE
#include "TrkMeasurementBase/MeasurementBase.h" 
#include "TrkSurfaces/Surface.h" 
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkParameters/TrackParameters.h" 
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkTrack/TrackStateOnSurface.h"

// constructor
Trk::BremPositionNtupleHelper::BremPositionNtupleHelper(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
  AthAlgTool(t,n,p),
  //m_trkParametersWarning(false),
  m_bremX(nullptr),
  m_bremY(nullptr),
  m_bremZ(nullptr),
  m_trkQoverP(nullptr),
  m_thicknessInX0(nullptr),
  m_dna(nullptr),
  m_dnaDirection(nullptr),
  m_trackStateIndices(nullptr)

 {
    declareInterface<IValidationNtupleHelperTool>(this);
    //declareProperty("IgnoreMissingTrackCovarianceForPulls", mjo_ignoreMissTrkCov = false);
}

// destructor
Trk::BremPositionNtupleHelper::~BremPositionNtupleHelper() {}


//////////////
/// initialize
//////////////
StatusCode Trk::BremPositionNtupleHelper::initialize() {

    m_bremX          = new std::vector<float>();
    m_bremY          = new std::vector<float>();
    m_bremZ          = new std::vector<float>();
    m_trkQoverP      = new std::vector<float>();
    m_thicknessInX0  = new std::vector<float>();
    m_dna            = new std::vector<float>();
    m_dnaDirection   = new std::vector<int>();
    m_trackStateIndices = new std::vector<int>();

    ATH_MSG_INFO ("initialize() successful in " << name());
    return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode Trk::BremPositionNtupleHelper::finalize() {

  delete m_bremX;
  delete m_bremY;
  delete m_bremZ;
  delete m_trkQoverP;
  delete m_thicknessInX0;
  delete m_dna;
  delete m_dnaDirection;
  delete m_trackStateIndices;
    
  ATH_MSG_INFO ("finalize() successful in " << name());
  return StatusCode::SUCCESS;
}


///////////////////////////////////////
/// addNtupleItems
///////////////////////////////////////
StatusCode Trk::BremPositionNtupleHelper::addNtupleItems (
    TTree* tree,
    const int& ) const {

    // add items
    tree->Branch("BremEffectsX",    &m_bremX);
    tree->Branch("BremEffectsY",    &m_bremY);
    tree->Branch("BremEffectsZ",    &m_bremZ);
    tree->Branch("BremCurrentQoverP", &m_trkQoverP);
    tree->Branch("BremThicknessInX0", &m_thicknessInX0);
    tree->Branch("BremDnaValue",    &m_dna);
    tree->Branch("BremDnaDirection",&m_dnaDirection);
    tree->Branch("BremToTrackStateIndex",&m_trackStateIndices);

    ATH_MSG_VERBOSE ("added items to ntuple.");
    return StatusCode::SUCCESS;
}

////////////////////////////////////////
/// fill hit position data
////////////////////////////////////////
StatusCode Trk::BremPositionNtupleHelper::fillMeasurementData (
    const Trk::MeasurementBase*,
    const Trk::TrackParameters* /*trkPar*/,
    const int&,
    const bool& /*isOutlier*/) const {

    return StatusCode::SUCCESS;
}

StatusCode Trk::BremPositionNtupleHelper::resetVariables (
        const int&  ) const {
    m_bremX->clear();
    m_bremY->clear();
    m_bremZ->clear();
    m_trkQoverP->clear();
    m_thicknessInX0->clear();
    m_dna->clear();
    m_dnaDirection->clear();
    m_trackStateIndices->clear();
    return StatusCode::SUCCESS;

}


StatusCode Trk::BremPositionNtupleHelper::fillHoleData (
        const Trk::TrackStateOnSurface& tsos,
        const int& bremToTrackIndex ) const { // FIXME this abuses the interface even more

  // check if state has brem to analyse
  const Trk::MaterialEffectsBase* mEffect = tsos.materialEffectsOnTrack();
  const Trk::EstimatedBremOnTrack*  ebrot = dynamic_cast<const Trk::EstimatedBremOnTrack*>(mEffect);
  const Trk::TrackParameters*        tParameter = (tsos).trackParameters();
  if (ebrot == nullptr || tParameter == nullptr) {
    ATH_MSG_DEBUG ("Insufficient data to analyse brem. &TP=" 
                   << tParameter << " and &EB=" << ebrot);
      return StatusCode::SUCCESS;
  }

  const Amg::Vector3D& gmom = tParameter->momentum();
  const Amg::Vector3D& gpos = tParameter->position();
  m_bremX->push_back( float(gpos.x()) );
  m_bremY->push_back( float(gpos.y()) );
  m_bremZ->push_back( float(gpos.z()) );
  m_trkQoverP->push_back( (float) tParameter->parameters()[Trk::qOverP]);
  m_thicknessInX0->push_back((float) mEffect->thicknessInX0());
  float dna                  = gmom.mag() * sqrt(ebrot->sigmaQoverPsquared());
  m_dna->push_back(dna);
  m_dnaDirection->push_back(static_cast<int>(ebrot->searchDirection()));
  m_trackStateIndices->push_back(bremToTrackIndex);

  return StatusCode::SUCCESS;
}


