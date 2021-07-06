/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ResidualValidationNtupleHelper.cxx
//   Source file for class ResidualValidationNtupleHelper
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////


#include "TBranch.h"
#include "TTree.h"
// Trk
#include "TrkValTools/ResidualValidationNtupleHelper.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkValEvent/TrackStateData.h"

#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
//#include "TrkEventPrimitives/CovarianceMatrix.h"
//#include "TrkParameters/MeasuredTrackParameters.h"

// constructor
Trk::ResidualValidationNtupleHelper::ResidualValidationNtupleHelper(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
  AthAlgTool(t,n,p),
  m_pullWarning(false),
  m_trkParametersWarning(false),
  m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
  m_isUnbiased(nullptr),
  m_PixPullPhi(nullptr), m_PixPullEta(nullptr), m_PixResPhi(nullptr), m_PixResEta(nullptr),
  m_SCTPull(nullptr),  m_SCTRes(nullptr),
  m_TRTPull(nullptr),  m_TRTRes(nullptr),
  m_MDTPull(nullptr),  m_MDTRes(nullptr),
  m_RPCPull(nullptr),  m_RPCRes(nullptr),
  m_TGCPull(nullptr),  m_TGCRes(nullptr),
  m_CSCPull(nullptr),  m_CSCRes(nullptr)
{
    declareInterface<IValidationNtupleHelperTool>(this);
    // Declare the properties
    declareProperty("IgnoreMissingTrackCovariance", m_ignoreMissTrkCov = false);
    declareProperty("ResidualPullCalculatorTool",   m_residualPullCalculator,               "Tool to calculate residuals and pulls");
    //    declareProperty("NtupleDirectoryName", m_ntupleDirName = "FitterValidation");
    //    declareProperty("NtupleTreeName", m_ntupleTreeName = "Validation");
    //    declareProperty("DoPixels", mjo_doPixels = true);
    //    declareProperty("DoTRT", mjo_doTRT = true);
}

// destructor
Trk::ResidualValidationNtupleHelper::~ResidualValidationNtupleHelper() {}

////////////////////////
/// initialize
////////////////////////
StatusCode Trk::ResidualValidationNtupleHelper::initialize() {


  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;

  // get the ResidualPullCalculator
  sc = m_residualPullCalculator.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR ("Could not retrieve "<< m_residualPullCalculator <<" (to calculate residuals,pulls) ");
    return sc;
  }
  m_PixPullPhi = new std::vector<float>();
  m_PixPullEta = new std::vector<float>();
  m_PixResPhi  = new std::vector<float>();
  m_PixResEta  = new std::vector<float>();
  m_SCTPull = new std::vector<float>();
  m_SCTRes  = new std::vector<float>();
  m_TRTPull = new std::vector<float>();
  m_TRTRes  = new std::vector<float>();
  m_MDTPull = new std::vector<float>();
  m_MDTRes  = new std::vector<float>();
  m_RPCPull = new std::vector<float>();
  m_RPCRes  = new std::vector<float>();
  m_TGCPull = new std::vector<float>();
  m_TGCRes  = new std::vector<float>();
  m_CSCPull = new std::vector<float>();
  m_CSCRes  = new std::vector<float>();

  return sc;
}

///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode Trk::ResidualValidationNtupleHelper::finalize() {

  delete m_PixPullPhi; delete m_PixPullEta;
  delete m_PixResPhi;  delete m_PixResEta;
  delete m_SCTPull;    delete m_SCTRes;
  delete m_TRTPull;    delete m_TRTRes;
  delete m_MDTPull;    delete m_MDTRes;
  delete m_RPCPull;    delete m_RPCRes;
  delete m_TGCPull;    delete m_TGCRes;
  delete m_CSCPull;    delete m_CSCRes;

  ATH_MSG_INFO ("finalize() successful in " << name() );
  return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// addNtupleItems
///////////////////////////////////////
StatusCode Trk::ResidualValidationNtupleHelper::addNtupleItems (
    TTree* tree,
    const int& detectorType ) const {

    TBranch* trackStatesUnbiasedBranch = tree->GetBranch("TrackStatesUnbiased");
    if (!trackStatesUnbiasedBranch) {
      ATH_MSG_ERROR ( "Unable to get Branch TrackStatesUnbiased in ntuple" );
      return StatusCode::FAILURE;
    }
    void* variableAdr = static_cast<void*>(trackStatesUnbiasedBranch->GetAddress());
    if (!variableAdr) {
      ATH_MSG_ERROR ("Unable to get variable address of Branch TrackStatesUnbiased");
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE ("Address of unbiasedState tree variable: " << variableAdr );
    m_isUnbiased = static_cast<int*>(variableAdr);

//     TBranch* nHitsBranch = m_nt->GetBranch("nHits");
//     variableAdr = static_cast<void*>(nHitsBranch->GetAddress());
//     m_hits = static_cast<int*>(variableAdr);

    // -------------------------------------------
    // retrieve and add items from/to the ntuple
    //    if (detectorType.test(Pixel)) {
    if (detectorType==Trk::TrackState::Pixel) {
      ATH_MSG_INFO ( "adding Pixel residual/pull variables to ntuple ");
      // add items for pixel
      tree->Branch("PixPullPhi",      &m_PixPullPhi);
      tree->Branch("PixPullEta",      &m_PixPullEta);
      tree->Branch("PixResPhi",       &m_PixResPhi);
      tree->Branch("PixResEta",       &m_PixResEta);
    }

    //if (detectorType.test(TRT)) {
    if (detectorType==Trk::TrackState::TRT) {
      ATH_MSG_INFO ("adding TRT residual/pull variables to ntuple ");
      tree->Branch("TRTPull",     &m_TRTPull);
      tree->Branch("TRTRes",      &m_TRTRes);
    }
    if (detectorType==Trk::TrackState::SCT) {
      ATH_MSG_INFO ( "adding SCT residual/pull variables to ntuple ");
      tree->Branch("SCTPull",     &m_SCTPull);
      tree->Branch("SCTRes",      &m_SCTRes);
    }

    if (detectorType==Trk::TrackState::MDT) {
      ATH_MSG_INFO ( "adding MDT residual/pull variables to ntuple ");
      // add items for MDT
      tree->Branch("MDTPull",     &m_MDTPull);
      tree->Branch("MDTRes",      &m_MDTRes);
    }

    if (detectorType==Trk::TrackState::RPC) {
      ATH_MSG_INFO ("adding RPC residual/pull variables to ntuple ");
      ATH_MSG_INFO ("Reminder: these variables do not distinguish RPC phi from eta hits.");
      // add items for RPC
      tree->Branch("RPCPull",     &m_RPCPull);
      tree->Branch("RPCRes",      &m_RPCRes);
    }

    if (detectorType==Trk::TrackState::TGC) {
      ATH_MSG_INFO ( "adding TGC residual/pull variables to ntuple ");
      ATH_MSG_INFO ( "Reminder: these variables do not distinguish TGC phi from eta hits.");
      // add items for TGC
      tree->Branch("TGCPull",     &m_TGCPull);
      tree->Branch("TGCRes",      &m_TGCRes);
    }
    
    if (detectorType==Trk::TrackState::CSC) {
      ATH_MSG_INFO ( "adding CSC residual/pull variables to ntuple ");
      ATH_MSG_INFO ( "Reminder: these variables do not distinguish CSC phi from eta hits.");
      // add items for CSC
      tree->Branch("CSCPull",     &m_CSCPull);
      tree->Branch("CSCRes",      &m_CSCRes);
    }

    ATH_MSG_VERBOSE ( "added items to ntuple") ;
    return StatusCode::SUCCESS;
}

////////////////////////////////////////
/// fill residual data
////////////////////////////////////////
StatusCode Trk::ResidualValidationNtupleHelper::fillMeasurementData (
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters* trkParameters,
    //const NTuple::Item<long>& numberOfHits,
    //const std::bitset<8>& detectorType
    const int& detectorType,
    const bool& isOutlier ) const {

    //const Trk::MeasuredTrackParameters *measuredTrkParameters = dynamic_cast<const Trk::MeasuredTrackParameters*>(trkParameters);
    if (!trkParameters) {
        if (!isOutlier) {
            // outliers usually do have not track parameters, so do not warn for them!
            if(!m_trkParametersWarning) {
                // warn once only!
              ATH_MSG_WARNING ("No TrackParameters: Residuals cannot be filled (filling dummy values).");
              ATH_MSG_WARNING ("   This is possible for slimmed tracks; if residuals are needed choose another track collection");
              ATH_MSG_WARNING ("   (further warnings will be suppressed)");
              m_trkParametersWarning = true;
            }
        }
        fillValues(detectorType);
        return StatusCode::SUCCESS;
    } // end if (!trkPar)
    /* open up to more measurement flavours
    const Trk::RIO_OnTrack *ROT = dynamic_cast<const Trk::RIO_OnTrack*>(measurement);
    if (!ROT) {
        msg(MSG::WARNING) << "No RIO_OnTrack: Residuals cannot be calculated (filling dummy values)." << endmsg;
        fillValues(detectorType);
        return StatusCode::SUCCESS;
    } */
    const Trk::ResidualPull* residualPull=nullptr;
    if (detectorType!=TrackState::unidentified) {
        residualPull = m_residualPullCalculator->residualPull(measurement, trkParameters,
          (*m_isUnbiased==1) ? Trk::ResidualPull::Unbiased : Trk::ResidualPull::Biased);
    } else {
      ATH_MSG_WARNING ("Detector type could not be identified, can not calculate residuals!");
      fillValues(detectorType);
      return StatusCode::FAILURE;
    }
    if (!residualPull) {
      ATH_MSG_WARNING ("Residual Pull Calculator did not succeed!");
      fillValues(detectorType);
      return StatusCode::FAILURE;
    }

    if (detectorType==TrackState::Pixel) {
        if ((residualPull->isPullValid()) || m_ignoreMissTrkCov ) {
            fillValues( detectorType,
                        residualPull->residual()[Trk::distPhi],
                        residualPull->pull()[Trk::distPhi],
                        residualPull->residual()[Trk::distEta],
                        residualPull->pull()[Trk::distEta] );
        } else {
            givePullWarning();
            fillValues( detectorType,
                        residualPull->residual()[Trk::distPhi],
                        -1000.,
                        residualPull->residual()[Trk::distEta],
                        -1000. );
        }
    } else {
        if ((residualPull->isPullValid()) || m_ignoreMissTrkCov ) {
            fillValues( detectorType,
                        residualPull->residual()[Trk::loc1],
                        residualPull->pull()[Trk::loc1] );
        } else {
            givePullWarning();
            fillValues( detectorType,
                        residualPull->residual()[Trk::loc1],
                        -1000. );
        }
    }
    delete residualPull;

    return StatusCode::SUCCESS;
}

StatusCode Trk::ResidualValidationNtupleHelper::resetVariables (
        const int& detectorType ) const {
    if (detectorType==TrackState::Pixel) {
        m_PixPullPhi->clear();
        m_PixPullEta->clear();
        m_PixResPhi->clear();
        m_PixResEta->clear();
    } else if (detectorType==TrackState::SCT) {
        m_SCTPull->clear();
        m_SCTRes->clear();
    } else if (detectorType==TrackState::TRT) {
        m_TRTRes->clear();
        m_TRTPull->clear();
    } else if (detectorType==TrackState::MDT) {
        m_MDTRes->clear();
        m_MDTPull->clear();
    } else if (detectorType==TrackState::RPC) {
        m_RPCRes->clear();
        m_RPCPull->clear();
    } else if (detectorType==TrackState::TGC) {
        m_TGCRes->clear();
        m_TGCPull->clear();
    } else if (detectorType==TrackState::CSC) {
        m_CSCRes->clear();
        m_CSCPull->clear();
    }
    return StatusCode::SUCCESS;
}


StatusCode Trk::ResidualValidationNtupleHelper::fillHoleData (
        const Trk::TrackStateOnSurface&,
        const int&) const {
    // we do nothing with holes
    return StatusCode::SUCCESS;
}
