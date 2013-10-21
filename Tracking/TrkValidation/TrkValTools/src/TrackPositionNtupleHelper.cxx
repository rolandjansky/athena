/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrackPositionNtupleHelper.cxx
//   Source file for class TrackPositionNtupleHelper
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "TrkValTools/TrackPositionNtupleHelper.h"
#include "TTree.h"

// constructor
Trk::TrackPositionNtupleHelper::TrackPositionNtupleHelper(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
  AthAlgTool(t,n,p),
  m_trkParametersWarning(false),
  m_trkX(0),
  m_trkY(0),
  m_trkZ(0),
  m_trkMomentumX(0),
  m_trkMomentumY(0),
  m_trkMomentumZ(0),
  m_trkLoc1(0),
  m_trkLoc2(0)

 {
    declareInterface<IValidationNtupleHelperTool>(this);
    //declareProperty("IgnoreMissingTrackCovarianceForPulls", mjo_ignoreMissTrkCov = false);
}

// destructor
Trk::TrackPositionNtupleHelper::~TrackPositionNtupleHelper() {}


//////////////
/// initialize
//////////////
StatusCode Trk::TrackPositionNtupleHelper::initialize() {

    m_trkX          = new std::vector<float>();
    m_trkY          = new std::vector<float>();
    m_trkZ          = new std::vector<float>();
    m_trkMomentumX  = new std::vector<float>();
    m_trkMomentumY  = new std::vector<float>();
    m_trkMomentumZ  = new std::vector<float>();
    m_trkLoc1       = new std::vector<float>();
    m_trkLoc2       = new std::vector<float>();

    return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode Trk::TrackPositionNtupleHelper::finalize() {

  delete         m_trkX;
  delete         m_trkY;
  delete         m_trkZ;
  delete m_trkMomentumX;
  delete m_trkMomentumY;
  delete m_trkMomentumZ;
  delete      m_trkLoc1;
  delete      m_trkLoc2;

  ATH_MSG_INFO ("finalize() successful in " << name() );
  return StatusCode::SUCCESS;
}


///////////////////////////////////////
/// addNtupleItems
///////////////////////////////////////
StatusCode Trk::TrackPositionNtupleHelper::addNtupleItems (
    TTree* tree,
    const int& ) const {

    // add items
    tree->Branch("TrackX",          &m_trkX);
    tree->Branch("TrackY",          &m_trkY);
    tree->Branch("TrackZ",          &m_trkZ);
    tree->Branch("TrackMomentumX",  &m_trkMomentumX);
    tree->Branch("TrackMomentumY",  &m_trkMomentumY);
    tree->Branch("TrackMomentumZ",  &m_trkMomentumZ);
    tree->Branch("TrackLocal1",     &m_trkLoc1);
    tree->Branch("TrackLocal2",     &m_trkLoc2);

    ATH_MSG_VERBOSE ("added items to ntuple.");
    return StatusCode::SUCCESS;
}

////////////////////////////////////////
/// fill hit position data
////////////////////////////////////////
StatusCode Trk::TrackPositionNtupleHelper::fillMeasurementData (
    const Trk::MeasurementBase*,
    const Trk::TrackParameters* trkPar,
    //const NTuple::Item<long>& numberOfHits,
    //const std::bitset<8>& detectorType
    const int&,
    const bool& isOutlier) const {

    if (!trkPar) {
        m_trkX->push_back( s_errorEntry );
        m_trkY->push_back( s_errorEntry );
        m_trkZ->push_back( s_errorEntry );
        m_trkMomentumX->push_back( s_errorEntry );
        m_trkMomentumY->push_back( s_errorEntry );
        m_trkMomentumZ->push_back( s_errorEntry );
        m_trkLoc1->push_back( s_errorEntry );
        m_trkLoc2->push_back( s_errorEntry );
        if (!isOutlier) {
            // outliers usually have no track parameters, so do not warn for them!
            if(!m_trkParametersWarning) {
                // warn once only!
                ATH_MSG_WARNING ("No TrackParameters: Track positions cannot be filled (filling dummy values).");
                ATH_MSG_WARNING ("   This is possible for slimmed tracks; if track positions are needed choose another track collection");
                ATH_MSG_WARNING ("   (further warnings will be suppressed)");
                m_trkParametersWarning = true;
            }
        }
        return StatusCode::SUCCESS;
    } // end if (!trkPar)

    //--------------------------
    // track position info

    // get local and global parameters of the track state
    Amg::Vector2D lp  = trkPar->localPosition();

    const Amg::Vector3D& gmom = trkPar->momentum();
    const Amg::Vector3D& gpos = trkPar->position();
    
    m_trkX->push_back( float(gpos.x()) );
    m_trkY->push_back( float(gpos.y()) );
    m_trkZ->push_back( float(gpos.z()) );
    m_trkMomentumX->push_back( float(gmom.x()) );
    m_trkMomentumY->push_back( float(gmom.y()) );
    m_trkMomentumZ->push_back( float(gmom.z()) );

    m_trkLoc1->push_back( float(lp[Trk::locX]) );
    m_trkLoc2->push_back( float(lp[Trk::locY]) );

    return StatusCode::SUCCESS;
}

StatusCode Trk::TrackPositionNtupleHelper::resetVariables (
        const int&  ) const {
    m_trkX->clear();
    m_trkY->clear();
    m_trkZ->clear();
    m_trkMomentumX->clear();
    m_trkMomentumY->clear();
    m_trkMomentumZ->clear();
    m_trkLoc1->clear();
    m_trkLoc2->clear();

    return StatusCode::SUCCESS;
}


StatusCode Trk::TrackPositionNtupleHelper::fillHoleData (
        const Trk::TrackStateOnSurface&,
        const int&) const {
    // we do nothing with holes
    return StatusCode::SUCCESS;
}


