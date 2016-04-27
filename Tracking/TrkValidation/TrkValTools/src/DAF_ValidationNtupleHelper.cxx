/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DAF_ValidationNtupleHelper.cxx
//   Source file for class DAF_ValidationNtupleHelper
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
#include "TBranch.h"
// Trk
#include "TrkValTools/DAF_ValidationNtupleHelper.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include "TrkSurfaces/Surface.h"

#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"


// constructor
Trk::DAF_ValidationNtupleHelper::DAF_ValidationNtupleHelper(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
AthAlgTool(t,n,p),
        m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
        m_isUnbiased(0)
 {
    declareInterface<IValidationNtupleHelperTool>(this);
    declareProperty("IgnoreMissingTrackCovarianceForPulls", mjo_ignoreMissTrkCov = false,   "Do not warn, if track states do not have covariance matries when calculating pulls");
    declareProperty("WriteMeasurementPositionOfROTs",       mjo_writeHitPositions = true,   "Write measurement positions?");
    declareProperty("ResidualPullCalculatorTool",           m_residualPullCalculator,       "Tool to calculate residuals and pulls");
}

// destructor
Trk::DAF_ValidationNtupleHelper::~DAF_ValidationNtupleHelper() {}



//////////////
/// initialize
//////////////
StatusCode Trk::DAF_ValidationNtupleHelper::initialize() {

    StatusCode sc;

    // get the ResidualPullCalculator
    sc = m_residualPullCalculator.retrieve();
    if (sc.isFailure()) {
        ATH_MSG_FATAL ("Could not retrieve "<< m_residualPullCalculator <<" (to calculate residuals and pulls) " );
        return sc;
    }

    return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode Trk::DAF_ValidationNtupleHelper::finalize() {
    ATH_MSG_INFO ( "finalize() successful in " << name() );
    return StatusCode::SUCCESS;
}


///////////////////////////////////////
/// addNtupleItems
///////////////////////////////////////
StatusCode Trk::DAF_ValidationNtupleHelper::addNtupleItems (
    TTree* tree,
    //const std::bitset<8>& detectorType
    const int& ) const {

    // retrieve item (is the used track state unbiased?) from ntuple
    TBranch* trackStatesUnbiasedBranch = tree->GetBranch("TrackStatesUnbiased");
    if (!trackStatesUnbiasedBranch) {
        ATH_MSG_ERROR ("Unable to get Branch TrackStatesUnbiased in ntuple" );
        return StatusCode::FAILURE;
    }
    void* variableAdr = static_cast<void*>(trackStatesUnbiasedBranch->GetAddress());
    if (!variableAdr) {
      ATH_MSG_ERROR ( "Unable to get variable address of Branch TrackStatesUnbiased" );
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE ("Address of variable: " << variableAdr);
    m_isUnbiased = static_cast<int*>(variableAdr);

    // add items
    m_nContainedROTs        = new std::vector<int>();
    m_indexOfMaxAssgnProb   = new std::vector<int>();
    m_maxAssgnProb          = new std::vector<float>();
    tree->Branch("CompROTnContainedROTs",       &m_nContainedROTs);         // number of contained ROTs in the CompetingRIOsOnTrack (1-dim array: CompROTnContainedROTs[nHits])
    tree->Branch("CompROTindexOfMaxAssgnProb",  &m_indexOfMaxAssgnProb);    // index of the ROT with the maximum assignment probability
    tree->Branch("CompROTmaxAssgnProb",         &m_maxAssgnProb);           // maximum assignment probability

    ATH_MSG_VERBOSE ("added items to ntuple");
    return StatusCode::SUCCESS;
}

////////////////////////////////////////
/// fill CompetingRIOsOnTrack data
////////////////////////////////////////
StatusCode Trk::DAF_ValidationNtupleHelper::fillMeasurementData (
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters* trkParameters,
    //const NTuple::Item<long>& numberOfHits,
    //const std::bitset<8>& detectorType
    const int&,
    const bool& isOutlier) const {

    if (isOutlier) {
        // we do nothing for outliers, because competing ROTs are no outliers by definition
        return StatusCode::SUCCESS;
    }
    if (!trkParameters) {
      ATH_MSG_ERROR ( "no TrackParameters given" );
      return StatusCode::FAILURE;
    }

    const Trk::CompetingRIOsOnTrack* comprot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(measurement);
    if (comprot) {
        ATH_MSG_VERBOSE ( "measurement is CompetingRIOsOnTrack, fill ntuple variables" );
        //--------------------------
        // status info about compROT
        m_nContainedROTs->push_back(comprot->numberOfContainedROTs());
        unsigned int indexOfMaxAssgnProb =  comprot->indexOfMaxAssignProb();
        m_indexOfMaxAssgnProb->push_back(indexOfMaxAssgnProb);
        m_maxAssgnProb->push_back(comprot->assignmentProbability(indexOfMaxAssgnProb));

    } else {
      m_nContainedROTs->push_back(0);
      m_indexOfMaxAssgnProb->push_back(0);
      m_maxAssgnProb->push_back(0.);
      ATH_MSG_VERBOSE ( "measurement is not a CompetingRIOsOnTrack, ignore it");
    }
    return StatusCode::SUCCESS;
}

StatusCode Trk::DAF_ValidationNtupleHelper::resetVariables (
        const int&  ) const {
    m_nContainedROTs->clear();
    m_indexOfMaxAssgnProb->clear();
    m_maxAssgnProb->clear();
    return StatusCode::SUCCESS;
}

StatusCode Trk::DAF_ValidationNtupleHelper::fillHoleData (
        const Trk::TrackStateOnSurface&,
        const int&) const {
    // we do nothing with holes
    return StatusCode::SUCCESS;
}
