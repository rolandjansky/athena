/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MeasurementVectorNtupleTool.cxx
//   Source file for class MeasurementVectorNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann -at- cern.ch, Wolfgang.Liebig -at- cern.ch
///////////////////////////////////////////////////////////////////
#include "TTree.h"
// Trk
#include "MeasurementVectorNtupleTool.h"
#include "TrkValInterfaces/IValidationNtupleHelperTool.h"
#include "TrkTrack/Track.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkEventUtils/MeasurementTypeID.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include <cmath>

// constructor
Trk::MeasurementVectorNtupleTool::MeasurementVectorNtupleTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
        AthAlgTool(t,n,p),
        m_idHelper(nullptr),
        m_detTypeHelper(nullptr),
        m_updatorHandle("Trk::KalmanUpdator/TrkKalmanUpdator"),
        m_updator(nullptr),
        m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
        m_holeSearchTool("InDet::InDetTrackHoleSearchTool/InDetHoleSearchTool"),
        m_PixelNtupleHelperToolHandles(this), // retrieve as private tools
        m_SCTNtupleHelperToolHandles(this),
        m_TRTNtupleHelperToolHandles(this),
        m_MDTNtupleHelperToolHandles(this),
        m_CSCNtupleHelperToolHandles(this),
        m_RPCNtupleHelperToolHandles(this),
        m_TGCNtupleHelperToolHandles(this),
        m_GeneralNtupleHelperToolHandles(this),
        //m_useROTwithMaxAssgnProb
        //m_ignoreMissTrkCov
        //vectors could be initialised here
        m_pullWarning{},
        m_UpdatorWarning{},
        m_trkParametersWarning{},
        m_doTruth{},
        m_doHoleSearch(true),
        //map could be initialised here
        m_isUnbiased{},
        m_residualLocX(nullptr),
        m_residualLocY(nullptr),
        m_pullLocX(nullptr),
        m_pullLocY(nullptr),
        m_DetectorType(nullptr),
        m_isOutlier(nullptr),
        m_nPixelHits{},
        m_nSCTHits{},
        m_nTRTHits{},
        m_nMDTHits{},
        m_nCSCHits{},
        m_nRPCHits{},
        m_nTGCHits{},
        m_pixelHitIndex(nullptr),
        m_sctHitIndex(nullptr),
        m_trtHitIndex(nullptr),
        m_mdtHitIndex(nullptr),
        m_cscHitIndex(nullptr),
        m_rpcHitIndex(nullptr),
        m_tgcHitIndex(nullptr),
        m_nHoles{},
        m_nPixelHoles{},
        m_nSctHoles{},
        m_HoleDetectorType(nullptr)
{
  declareInterface<ITrackValidationNtupleTool>(this);

  declareProperty("UseROTwithMaxAssgnProb",       m_useROTwithMaxAssgnProb = true,        "In case of CompetingRIOsOnTrack: Use the ROT with maximum assignment probabilty to calculate residuals, etc or use mean measurement?");
  declareProperty("IgnoreMissingTrackCovarianceForPulls", m_ignoreMissTrkCov = false,   "Do not warn, if track states do not have covariance matries when calculating pulls");
  declareProperty("UpdatorTool",                  m_updatorHandle,                        "Measurement updator to calculate unbiased track states");
  declareProperty("ResidualPullCalculatorTool",   m_residualPullCalculator,               "Tool to calculate residuals and pulls");
  declareProperty("HoleSearchTool",               m_holeSearchTool,                       "Tool to search for holes on track");
                    // these are for detector-internal validation, unless an
                    // specific <-> general index hit mapper is there.
  declareProperty("PixelNtupleHelperTools",       m_PixelNtupleHelperToolHandles,
                  "List of Pixel validation tools");
  declareProperty("SCTNtupleHelperTools",         m_SCTNtupleHelperToolHandles,           "List of SCT validation tools");
  declareProperty("TRTNtupleHelperTools",         m_TRTNtupleHelperToolHandles,           "List of TRT validation tools");
  declareProperty("MDTNtupleHelperTools",         m_MDTNtupleHelperToolHandles,           "List of MDT validation tools");
  declareProperty("CSCNtupleHelperTools",         m_CSCNtupleHelperToolHandles,           "List of CSC validation tools");
  declareProperty("RPCNtupleHelperTools",         m_RPCNtupleHelperToolHandles,           "List of RPC validation tools");
  declareProperty("TGCNtupleHelperTools",         m_TGCNtupleHelperToolHandles,           "List of TGC validation tools");
  declareProperty("GeneralNtupleHelperTools",     m_GeneralNtupleHelperToolHandles,       "List of detector independent validation tools");
  declareProperty("DoTruth",                      m_doTruth,                              "Write truth data?");
  declareProperty("DoHoleSearch",                 m_doHoleSearch,                         "Write hole data?");
  m_UpdatorWarning = false;
  m_pullWarning = false;
  m_trkParametersWarning = false;

}

// destructor
Trk::MeasurementVectorNtupleTool::~MeasurementVectorNtupleTool() {}


///////////////////////////////////////
// initialize
///////////////////////////////////////
StatusCode Trk::MeasurementVectorNtupleTool::initialize() {

  if (!m_residualPullCalculator.empty()) {
    m_pullLocX      = new std::vector<float>();
    m_pullLocY      = new std::vector<float>();
    m_residualLocX  = new std::vector<float>();
    m_residualLocY  = new std::vector<float>();
  }

  m_DetectorType  = new std::vector<int>();
  m_isOutlier     = new std::vector<int>();

  m_pixelHitIndex = new std::vector<int>();
  m_sctHitIndex   = new std::vector<int>();
  m_trtHitIndex   = new std::vector<int>();
  m_mdtHitIndex   = new std::vector<int>();
  m_cscHitIndex   = new std::vector<int>();
  m_rpcHitIndex   = new std::vector<int>();
  m_tgcHitIndex   = new std::vector<int>();
  if (m_doHoleSearch) {
    m_HoleDetectorType  = new std::vector<int>();
  }

  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    ATH_MSG_ERROR ("Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE;
  }
  m_detTypeHelper = new MeasurementTypeID(m_idHelper);

  StatusCode sc(StatusCode::SUCCESS);
  // ----------------------------------
  // use updator to get unbiased states
  if ( ! m_updatorHandle.empty() ) {
    sc = m_updatorHandle.retrieve();
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Could not retrieve measurement updator tool: "<< m_updatorHandle << endmsg;
      return sc;
    }
    m_updator = &(*m_updatorHandle);
  } else {
    ATH_MSG_DEBUG ("No Updator for unbiased track states given, use normal states!");
    m_updator = nullptr;
  }

  // need an Atlas id-helper to identify sub-detectors, take the one from detStore
  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    ATH_MSG_ERROR ("Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE;
  }    // ---------------------------------------

  // -------------------------------
  // get given helper tools
  ToolHandleArray< Trk::IValidationNtupleHelperTool >::iterator itTools;
  // get all the given ntuple helper tools for Pixel
  if ( m_PixelNtupleHelperToolHandles.retrieve().isFailure() ) {
    msg(MSG::ERROR) << "Failed to retreive " << m_PixelNtupleHelperToolHandles << endmsg;
  } else {
    msg(MSG::INFO) << "Retrieved " << m_PixelNtupleHelperToolHandles << endmsg;
  }
  itTools = m_PixelNtupleHelperToolHandles.begin();
  for (  ; itTools != m_PixelNtupleHelperToolHandles.end(); ++itTools ) {
    // add tool to list
    m_PixelHelperTools.push_back(&(*(*itTools)));
    m_helperToolWarning[&(*(*itTools))] = false;
  }
  // get all the given ntuple helper tools for SCT
  if ( m_SCTNtupleHelperToolHandles.retrieve().isFailure() ) {
    msg(MSG::ERROR) << "Failed to retreive " << m_SCTNtupleHelperToolHandles << endmsg;
  } else {
    msg(MSG::INFO) << "Retrieved " << m_SCTNtupleHelperToolHandles << endmsg;
  }
  itTools = m_SCTNtupleHelperToolHandles.begin();
  for (  ; itTools != m_SCTNtupleHelperToolHandles.end(); ++itTools ) {
    // add tool to list
    m_SCTHelperTools.push_back(&(*(*itTools)));
    m_helperToolWarning[&(*(*itTools))] = false;
  }
  // get all the given ntuple helper tools for TRT
  if ( m_TRTNtupleHelperToolHandles.retrieve().isFailure() ) {
    msg(MSG::ERROR) << "Failed to retreive " << m_TRTNtupleHelperToolHandles << endmsg;
  } else {
    msg(MSG::INFO) << "Retrieved " << m_TRTNtupleHelperToolHandles << endmsg;
  }
  itTools = m_TRTNtupleHelperToolHandles.begin();
  for (  ; itTools != m_TRTNtupleHelperToolHandles.end(); ++itTools ) {
    // add tool to list
    m_TRTHelperTools.push_back(&(*(*itTools)));
    m_helperToolWarning[&(*(*itTools))] = false;
  }
  // get all the given ntuple helper tools for MDT
  if ( m_MDTNtupleHelperToolHandles.retrieve().isFailure() ) {
    msg(MSG::ERROR) << "Failed to retreive " << m_MDTNtupleHelperToolHandles << endmsg;
  } else {
    msg(MSG::INFO) << "Retrieved " << m_MDTNtupleHelperToolHandles << endmsg;
  }
  itTools = m_MDTNtupleHelperToolHandles.begin();
  for (  ; itTools != m_MDTNtupleHelperToolHandles.end(); ++itTools ) {
    // add tool to list
    m_MDTHelperTools.push_back(&(*(*itTools)));
    m_helperToolWarning[&(*(*itTools))] = false;
  }
  // get all the given ntuple helper tools for CSC
  if ( m_CSCNtupleHelperToolHandles.retrieve().isFailure() ) {
    msg(MSG::ERROR) << "Failed to retreive " << m_CSCNtupleHelperToolHandles << endmsg;
  } else {
    msg(MSG::INFO) << "Retrieved " << m_CSCNtupleHelperToolHandles << endmsg;
  }
  itTools = m_CSCNtupleHelperToolHandles.begin();
  for (  ; itTools != m_CSCNtupleHelperToolHandles.end(); ++itTools ) {
    // add tool to list
    m_CSCHelperTools.push_back(&(*(*itTools)));
    m_helperToolWarning[&(*(*itTools))] = false;
  }
  // get all the given ntuple helper tools for RPC
  if ( m_RPCNtupleHelperToolHandles.retrieve().isFailure() ) {
    msg(MSG::ERROR) << "Failed to retreive " << m_RPCNtupleHelperToolHandles << endmsg;
  } else {
    msg(MSG::INFO) << "Retrieved " << m_RPCNtupleHelperToolHandles << endmsg;
  }
  itTools = m_RPCNtupleHelperToolHandles.begin();
  for (  ; itTools != m_RPCNtupleHelperToolHandles.end(); ++itTools ) {
    // add tool to list
    m_RPCHelperTools.push_back(&(*(*itTools)));
    m_helperToolWarning[&(*(*itTools))] = false;
  }
  // get all the given ntuple helper tools for TGC
  if ( m_TGCNtupleHelperToolHandles.retrieve().isFailure() ) {
    msg(MSG::ERROR) << "Failed to retreive " << m_TGCNtupleHelperToolHandles << endmsg;
  } else {
    msg(MSG::INFO) << "Retrieved " << m_TGCNtupleHelperToolHandles << endmsg;
  }
  itTools = m_TGCNtupleHelperToolHandles.begin();
  for (  ; itTools != m_TGCNtupleHelperToolHandles.end(); ++itTools ) {
    // add tool to list
    m_TGCHelperTools.push_back(&(*(*itTools)));
    m_helperToolWarning[&(*(*itTools))] = false;
  }
  // get all the given ntuple helper tools independent from detector tech
  if ( m_GeneralNtupleHelperToolHandles.retrieve().isFailure() ) {
    msg(MSG::ERROR) << "Failed to retreive " << m_GeneralNtupleHelperToolHandles << endmsg;
  } else {
    msg(MSG::INFO) << "Retrieved " << m_GeneralNtupleHelperToolHandles << endmsg;
  }
  itTools = m_GeneralNtupleHelperToolHandles.begin();
  for (  ; itTools != m_GeneralNtupleHelperToolHandles.end(); ++itTools ) {
        // add tool to list
    m_GeneralHelperTools.push_back(&(*(*itTools)));
    m_helperToolWarning[&(*(*itTools))] = false;
  }

  // get the ResidualPullCalculator
  if (m_residualPullCalculator.empty()) {
    msg(MSG::INFO) << "No residual/pull calculator for general hit residuals configured."
                   << endmsg;
    msg(MSG::INFO) << "It is recommended to give R/P calculators to the det-specific tool"
                   << " handle lists then." << endmsg;
  } else {
    sc = m_residualPullCalculator.retrieve();
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Could not retrieve "<< m_residualPullCalculator <<" (to calculate residuals and pulls) "<< endmsg;
      return sc;
    } else {
      msg(MSG::INFO) << "Generic hit residuals&pulls will be calculated in one or both "
                     << "available local coordinates" << endmsg;
    }
  }
  // get the hole search tool if needed
  if (m_doHoleSearch) {
    sc = m_holeSearchTool.retrieve();
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Could not retrieve "<< m_holeSearchTool <<" (needed for hole search) "<< endmsg;
      return sc;
    }
  }

  ATH_MSG_DEBUG ("successfully initialized in " << name());
  return StatusCode::SUCCESS;
}

///////////////////////////////////////
// finalize
///////////////////////////////////////
StatusCode Trk::MeasurementVectorNtupleTool::finalize() {

  ATH_MSG_DEBUG ("start finalize() in " << name());

  if (!m_residualPullCalculator.empty()) {
    delete m_pullLocX;
    delete m_pullLocY;
    delete m_residualLocX;
    delete m_residualLocY;
  }
  delete m_DetectorType;
  delete m_isOutlier;
  delete m_pixelHitIndex;
  delete m_sctHitIndex;
  delete m_trtHitIndex;
  delete m_mdtHitIndex;
  delete m_cscHitIndex;
  delete m_rpcHitIndex;
  delete m_tgcHitIndex;
  if (m_doHoleSearch) delete m_HoleDetectorType;
  delete m_detTypeHelper;

  return StatusCode::SUCCESS;
}

StatusCode Trk::MeasurementVectorNtupleTool::addNtupleItems( TTree* tree ) const {
    if (!tree) return StatusCode::FAILURE;
    ATH_MSG_DEBUG ("added branches to ntuple");
    //-----------------
    // add items  *** Note: Documentation is in the header file, doxygen and wikis! ***
    //
    tree->Branch("TrackStatesUnbiased", &m_isUnbiased        );
    m_isUnbiased=999;

    if (!m_residualPullCalculator.empty()) {
      tree->Branch("pullLocX",            &m_pullLocX            );
      tree->Branch("pullLocY",            &m_pullLocY            );
      tree->Branch("residualLocX",        &m_residualLocX        );
      tree->Branch("residualLocY",        &m_residualLocY        );
    }

    tree->Branch("DetectorType",        &m_DetectorType      );
    tree->Branch("outlierFlag",         &m_isOutlier         );

    tree->Branch("nPixelHits",          &m_nPixelHits        );
    tree->Branch("nSCTHits",            &m_nSCTHits          );
    tree->Branch("nTRTHits",            &m_nTRTHits          );
    tree->Branch("nMDTHits",            &m_nMDTHits          );
    tree->Branch("nCSCHits",            &m_nCSCHits          );
    tree->Branch("nRPCHits",            &m_nRPCHits          );
    tree->Branch("nTGCHits",            &m_nTGCHits          );

    tree->Branch("pixelHitIndex",       &m_pixelHitIndex     );
    tree->Branch("sctHitIndex",         &m_sctHitIndex       );
    tree->Branch("trtHitIndex",         &m_trtHitIndex       );
    tree->Branch("mdtHitIndex",         &m_mdtHitIndex       );
    tree->Branch("cscHitIndex",         &m_cscHitIndex       );
    tree->Branch("rpcHitIndex",         &m_rpcHitIndex       );
    tree->Branch("tgcHitIndex",         &m_tgcHitIndex       );

    ATH_MSG_VERBOSE ("added own branches to ntuple");

    StatusCode sc(StatusCode::SUCCESS);
    ToolHandleArray< IValidationNtupleHelperTool >::const_iterator itTools;
    // get all the given ntuple helper tools for Pixel
    itTools = m_PixelNtupleHelperToolHandles.begin();
    for (  ; itTools != m_PixelNtupleHelperToolHandles.end(); ++itTools ) {
      // let tool add its items
      sc = (*itTools)->addNtupleItems (tree, TrackState::Pixel);
      if(sc.isFailure())   {
        msg(MSG::ERROR)<<"The helper " << (*itTools) << " could not add its items" << endmsg;
        return sc;
      }
    }
    // SCT helper tools
    itTools = m_SCTNtupleHelperToolHandles.begin();
    for (  ; itTools != m_SCTNtupleHelperToolHandles.end(); ++itTools ) {
      // let tool add its items
      sc = (*itTools)->addNtupleItems (tree, TrackState::SCT);
      if(sc.isFailure())   {
        msg(MSG::ERROR)<<"The helper " << (*itTools) << " could not add its items" << endmsg;
        return sc;
      }
    }
    // get all the given ntuple helper tools for TRT
    itTools = m_TRTNtupleHelperToolHandles.begin();
    for (  ; itTools != m_TRTNtupleHelperToolHandles.end(); ++itTools ) {
      // let tool add its items
      sc = (*itTools)->addNtupleItems (tree, TrackState::TRT);
      if(sc.isFailure())   {
        msg(MSG::ERROR)<<"The helper " << (*itTools) << " could not add its items" << endmsg;
        return sc;
      }
    }
    // get all the given ntuple helper tools for MDT
    itTools = m_MDTNtupleHelperToolHandles.begin();
    for (  ; itTools != m_MDTNtupleHelperToolHandles.end(); ++itTools ) {
      // let tool add its items
      sc = (*itTools)->addNtupleItems (tree, TrackState::MDT);
      if(sc.isFailure())   {
        msg(MSG::ERROR)<<"The helper " << (*itTools) << " could not add its items" << endmsg;
        return sc;
      }
    }
    // get all the given ntuple helper tools for CSC
    itTools = m_CSCNtupleHelperToolHandles.begin();
    for (  ; itTools != m_CSCNtupleHelperToolHandles.end(); ++itTools ) {
      // let tool add its items
      sc = (*itTools)->addNtupleItems (tree, TrackState::CSC);
      if(sc.isFailure())   {
        msg(MSG::ERROR)<<"The helper " << (*itTools) << " could not add its items" << endmsg;
        return sc;
      }
    }
    // get all the given ntuple helper tools for RPC
    itTools = m_RPCNtupleHelperToolHandles.begin();
    for (  ; itTools != m_RPCNtupleHelperToolHandles.end(); ++itTools ) {
      // let tool add its items
      sc = (*itTools)->addNtupleItems (tree, TrackState::RPC);
      if(sc.isFailure())   {
        msg(MSG::ERROR)<<"The helper " << (*itTools) << " could not add its items" << endmsg;
        return sc;
      }
    }
    // get all the given ntuple helper tools for TGC
    itTools = m_TGCNtupleHelperToolHandles.begin();
    for (  ; itTools != m_TGCNtupleHelperToolHandles.end(); ++itTools ) {
      // let tool add its items
      sc = (*itTools)->addNtupleItems (tree, TrackState::TGC);
      if(sc.isFailure())   {
        msg(MSG::ERROR)<<"The helper " << (*itTools) << " could not add its items" << endmsg;
        return sc;
      }
    }
    itTools = m_GeneralNtupleHelperToolHandles.begin();
    for (  ; itTools != m_GeneralNtupleHelperToolHandles.end(); ++itTools ) {
      // let tool add its items
      sc = (*itTools)->addNtupleItems (tree, TrackState::unidentified);
      if(sc.isFailure())   {
        msg(MSG::ERROR)<<"The helper " << (*itTools) << " could not add its items" << endmsg;
        return sc;
      }
    }
    return StatusCode::SUCCESS;
}

//////////////////////////////////////
/// fill track data into variables without actually writing the record
//////////////////////////////////////
StatusCode Trk::MeasurementVectorNtupleTool::fillTrackData (
    const Trk::Track& track,
    const int  /*iterationIndex*/,
    const unsigned int /*fitStatCode*/ )  const {
   // const Trk::FitterStatusCode /*fitStatCode*/ ) const {

  ATH_MSG_VERBOSE ("in fillTrackData(trk, indx) filling info about track states");

  resetVariables();
  if (m_updator) {
    m_isUnbiased = 1;
  } else {
    m_isUnbiased = 0;
  }

  //----------------------------------------------
  // fill info about trackstates in ntuple
  const DataVector<const Trk::TrackStateOnSurface>* trackStates=track.trackStateOnSurfaces();
  if (trackStates == nullptr) {
    msg(MSG::WARNING) << "current track does not have any TrackStateOnSurface vector, no data will be written for this track" << endmsg;
    return StatusCode::FAILURE;
  }

  // Loop over all track states on surfaces
  int stateIndexCounter=0;
  for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it=trackStates->
         begin();
       it!=trackStates->end();
       ++it) {


    if (!(*it)) {
      msg(MSG::WARNING) << "TrackStateOnSurface == Null" << endmsg;
      continue;
    }
    if ((*it)->type(Trk::TrackStateOnSurface::Measurement) ||
        (*it)->type(Trk::TrackStateOnSurface::Outlier)     ) {

      ++stateIndexCounter;
      ATH_MSG_VERBOSE ("try to get measurement for track state");
      // Get pointer to measurement on track
      const Trk::MeasurementBase *measurement = (*it)->measurementOnTrack();
      if (!measurement) {
        msg(MSG::ERROR) << "measurementOnTrack == Null for a TrackStateOnSurface "
                        << "of type Measurement or Outlier" << endmsg;
        return StatusCode::FAILURE;
      } // end if (!measurement)
      TrackState::MeasurementType detectorType = m_detTypeHelper->defineType(measurement);
      const Trk::TrackParameters* theParameters = (*it)->trackParameters();
      const Trk::TrackParameters* unbiasedParameters = nullptr;

      // -----------------------------------------
      // use unbiased track states or normal ones?
      // unbiased track parameters are tried to retrieve if the updator tool
      //    is available and if unbiased track states could be produced before
      //    for the current track (ie. if one trial to get unbiased track states
      //    fails, for all following track states of the current track the biased state
      //    will be used).
      if (theParameters && m_updator && (m_isUnbiased==1)
          && (detectorType!=TrackState::Pseudo)
	  && (! (*it)->type(Trk::TrackStateOnSurface::Outlier)) ) {
        if ( theParameters->covariance() ) {
            // Get unbiased state
          unbiasedParameters = m_updator->removeFromState( *theParameters,
                               measurement->localParameters(),
                               measurement->localCovariance()).release();
          if (unbiasedParameters) {
            theParameters = unbiasedParameters;
            ATH_MSG_VERBOSE ("successfully calculated unbiased state");
          } else {
            if (measurement->localParameters().contains(Trk::locX)) {
              double covTrk = (*theParameters->covariance())(Trk::locX,Trk::locX);
              double covRot = measurement->localCovariance()(Trk::locX,Trk::locX);
              if (std::abs(covTrk-covRot)<0.001*covRot) {
                ATH_MSG_DEBUG("Track not overconstrained in local X (track_cov_x = "<<covTrk<<" vs. meas_cov_x="<<covRot<<" => do not calculate unbiased residual.");
              } else ATH_MSG_INFO ("Could not get unbiased track parameters, use normal parameters");
            } else ATH_MSG_INFO ("Could not get unbiased track parameters, use normal parameters");
            m_isUnbiased = 0;
          } // end if no unbiased track parameters
        } else if(!m_UpdatorWarning) {
          // warn only once!
          msg(MSG::WARNING) << "TrackParameters contain no covariance: Unbiased track states can not be calculated (ie. pulls and residuals will be too small)" << endmsg;
          m_UpdatorWarning = true;
          m_isUnbiased = 0;
        } else {
          m_isUnbiased = 0;
        } // end if no measured track parameters
      } // end if m_updator

      m_DetectorType->push_back((int)detectorType);
      ATH_MSG_VERBOSE ("meas #" << stateIndexCounter <<
                       ": detector technology identified as " << detectorType);

      if ((fillMeasurementData(measurement,
                               theParameters,
                               ((*it)->type(Trk::TrackStateOnSurface::Outlier)),
                               detectorType)).isFailure())
        msg(MSG::WARNING) << "info about TrackState could not be written to ntuple" << endmsg;
      if ((callHelperTools(measurement,
                           theParameters,
                           ((*it)->type(Trk::TrackStateOnSurface::Outlier)),
                           detectorType,
                           stateIndexCounter)).isFailure())
        msg(MSG::WARNING) << "Could not call helper Tool! " << endmsg;

      delete unbiasedParameters;
    } // end if(TSoS is measurement)
  } // end for loop on trackstates


  //----------------------------------------------
  // do hole search if selected
  if (m_doHoleSearch) {
    std::unique_ptr<const Trk::TrackStates> holesOnTrack (m_holeSearchTool->getHolesOnTrack(track, track.info().particleHypothesis()));
    // loop over holes
    if (!holesOnTrack) {
      msg(MSG::WARNING) << "Got no holes on track" << endmsg;
      return StatusCode::SUCCESS;
    }
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it=holesOnTrack->begin();
         it!=holesOnTrack->end();
         ++it) {
      if (!(*it)) {
        msg(MSG::WARNING) << "TrackStateOnSurface from hole search tool == Null" << endmsg;
        continue;
      }
      if (fillHoleData(*(*it)).isFailure()) {
        msg(MSG::WARNING) << "info about TrackState (hole) could not be written to ntuple" << endmsg;
      }
    } // end loop on holes
  }

  return StatusCode::SUCCESS;
}

//////////////////////////////////////
/// fill trackparticle data into variables without actually writing the record
//////////////////////////////////////
StatusCode Trk::MeasurementVectorNtupleTool::fillTrackParticleData
( const Trk::TrackParticleBase&) const
{

  ATH_MSG_WARNING ("MeasurementVectorNtupleTool not meant to be used with TrackParticles.");

  return StatusCode::SUCCESS;
}

//////////////////////////////////////
// fill ntuple data of a given proto-trajectory (function used for fitter validation)
//////////////////////////////////////
StatusCode Trk::MeasurementVectorNtupleTool::fillProtoTrajectoryData
(  const Trk::ProtoTrajectory&,
   const int,
   const Trk::Perigee*,
   const unsigned int ) const
   //const Trk::FitterStatusCode) const
{
  ATH_MSG_WARNING ("MeasurementVectorNtupleTool not meant to be used with TrackParticles.");
  return StatusCode::SUCCESS;
}

//////////////////////////////////////
// reset variables
//////////////////////////////////////
void Trk::MeasurementVectorNtupleTool::resetVariables() const {

  // reset the counters
  m_nPixelHits = 0;
  m_nSCTHits = 0;
  m_nTRTHits = 0;
  m_nMDTHits = 0;
  m_nCSCHits = 0;
  m_nRPCHits = 0;
  m_nTGCHits = 0;

  m_pixelHitIndex->clear();
  m_sctHitIndex->clear();
  m_trtHitIndex->clear();
  m_mdtHitIndex->clear();
  m_cscHitIndex->clear();
  m_rpcHitIndex->clear();
  m_tgcHitIndex->clear();

  if (m_doHoleSearch) {
    m_nHoles      = 0;
    m_nPixelHoles = 0;
    m_nSctHoles   = 0;
    m_HoleDetectorType->clear();
  }

  // clear data vectors
  if (!m_residualPullCalculator.empty()) {
    m_pullLocX->clear();
    m_pullLocY->clear();
    m_residualLocX->clear();
    m_residualLocY->clear();
  }
  m_DetectorType->clear();
  m_isOutlier->clear();

  std::vector< const Trk::IValidationNtupleHelperTool* >::const_iterator toolIter;
  toolIter = m_PixelHelperTools.begin();
  for (  ; toolIter != m_PixelHelperTools.end(); ++toolIter ) {
    // let tool reset its variables
    StatusCode sc = (*toolIter)->resetVariables (TrackState::Pixel);
    if(sc.isFailure())   {
      msg(MSG::ERROR)<<"The helper could not reset its ntuple variables" << endmsg;
      return;
    }
  }
  toolIter = m_SCTHelperTools.begin();
  for (  ; toolIter != m_SCTHelperTools.end(); ++toolIter ) {
    // let tool reset its variables
    StatusCode sc = (*toolIter)->resetVariables (TrackState::SCT);
    if(sc.isFailure())   {
      msg(MSG::ERROR)<<"The helper could not reset its ntuple variables" << endmsg;
      return;
    }
  }
  toolIter = m_TRTHelperTools.begin();
  for (  ; toolIter != m_TRTHelperTools.end(); ++toolIter ) {
    // let tool reset its variables
    StatusCode sc = (*toolIter)->resetVariables (TrackState::TRT);
    if(sc.isFailure())   {
      msg(MSG::ERROR)<<"The helper could not reset its ntuple variables" << endmsg;
      return;
    }
  }
  toolIter = m_MDTHelperTools.begin();
  for (  ; toolIter != m_MDTHelperTools.end(); ++toolIter ) {
    // let tool reset its variables
    StatusCode sc = (*toolIter)->resetVariables (TrackState::MDT);
    if(sc.isFailure())   {
      msg(MSG::ERROR)<<"The helper could not reset its ntuple variables" << endmsg;
      return;
    }
  }
  toolIter = m_CSCHelperTools.begin();
  for (  ; toolIter != m_CSCHelperTools.end(); ++toolIter ) {
    // let tool reset its variables
    StatusCode sc = (*toolIter)->resetVariables (TrackState::CSC);
    if(sc.isFailure())   {
      msg(MSG::ERROR)<<"The helper could not reset its ntuple variables" << endmsg;
      return;
    }
  }
  toolIter = m_RPCHelperTools.begin();
  for (  ; toolIter != m_RPCHelperTools.end(); ++toolIter ) {
    // let tool reset its variables
    StatusCode sc = (*toolIter)->resetVariables (TrackState::RPC);
    if(sc.isFailure())   {
      msg(MSG::ERROR)<<"The helper could not reset its ntuple variables" << endmsg;
      return;
    }
  }
  toolIter = m_TGCHelperTools.begin();
  for (  ; toolIter != m_TGCHelperTools.end(); ++toolIter ) {
    // let tool reset its variables
    StatusCode sc = (*toolIter)->resetVariables (TrackState::TGC);
    if(sc.isFailure())   {
      msg(MSG::ERROR)<<"The helper could not reset its ntuple variables" << endmsg;
      return;
    }
  }
  toolIter = m_GeneralHelperTools.begin();
  for (  ; toolIter != m_GeneralHelperTools.end(); ++toolIter ) {
    // let tool reset its variables
    StatusCode sc = (*toolIter)->resetVariables (TrackState::unidentified);
    if(sc.isFailure())   {
      msg(MSG::ERROR)<<"The helper could not reset its ntuple variables" << endmsg;
            return;
    }
  }
}

StatusCode Trk::MeasurementVectorNtupleTool::fillTrackTruthData ( const TrackParameters*& /*truePerigee*/, const TrackTruth& /*trackTruth*/, const int /*indexInTruthTree*/ ) const
{

  // implement code here (truth trajectory?)
  return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////////////////
/// fill the per-state vector variables
/////////////////////////////////////////////////////////////////////////////
StatusCode Trk::MeasurementVectorNtupleTool::fillMeasurementData(
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters* trkParameters,
    const bool& isOutlier,
    const int&  detectorType ) const {

    ATH_MSG_VERBOSE ("in fillMeasurementData");

    if (!isOutlier && trkParameters==nullptr) {
      ATH_MSG_VERBOSE ("Given TrackParameters == NULL");
      if(!m_trkParametersWarning) {
        // warn once only!
        msg(MSG::WARNING) << "TSoS (type: measurement) contains no TrackParameters: Residuals, etc. cannot be calculated" << endmsg;
        msg(MSG::WARNING) << "   This is possible for slimmed tracks; if residuals are needed choose another track collection" << endmsg;
        msg(MSG::WARNING) << "   (further warnings will be suppressed)" << endmsg;
        m_trkParametersWarning = true;
      }
    }

    m_isOutlier->push_back(isOutlier? 1 : 0);
    if (!m_residualPullCalculator.empty()) {
      // --------------------------------------
      // fill general residuals and pulls
      // for the residuals and pulls always use the ROT.
      // --------------------------------------
      float residualLocX = s_errorEntry;
      float residualLocY = s_errorEntry;
      float pullLocX     = s_errorEntry;
      float pullLocY     = s_errorEntry;
      if (trkParameters) {
        const Trk::ResidualPull* residualPull
          = m_residualPullCalculator->residualPull(measurement, trkParameters,
                                                   (m_isUnbiased==1) ? Trk::ResidualPull::Unbiased :
                                                    Trk::ResidualPull::Biased);
        if (residualPull) {
          residualLocX = residualPull->residual()[Trk::loc1];
          if (residualPull->dimension() >= 2)
            residualLocY = residualPull->residual()[Trk::loc2];
          if ((residualPull->isPullValid()) || m_ignoreMissTrkCov ) {
            pullLocX = residualPull->pull()[Trk::loc1];
            if (residualPull->dimension() >= 2)
              pullLocY = residualPull->pull()[Trk::loc2];
          } else {
            if (!m_pullWarning && !isOutlier) {  // warn only once!!!
              m_pullWarning = true;
              msg(MSG::WARNING) << "no covariance of the track parameters given, can not compute pull!" << endmsg;
              msg(MSG::INFO) << "Detector type "<< detectorType
			     << (isOutlier? " (flagged as outlier)" : "(not an outlier)") << endmsg;
              msg(MSG::INFO) << "you may want to use the jobOption 'IgnoreMissingTrackCovarianceForPulls' to calculate it anyhow." << endmsg;
              msg(MSG::INFO) << "No further warnings will be given for this type of situation." << endmsg;
            } else {
              ATH_MSG_DEBUG ("invalid pull due to missing COV at detector type " << detectorType
                             << (isOutlier? " (flagged as outlier)." : "."));
            }
          }
          delete residualPull;
        } else {
          msg(MSG::WARNING) << "ResidualPullCalculator failed!" << endmsg;
        }
      }
      m_residualLocX->push_back(residualLocX);
      m_residualLocY->push_back(residualLocY);
      m_pullLocX->push_back(pullLocX);
      m_pullLocY->push_back(pullLocY);
    } // end if (!m_residualPullCalculator.empty())

    return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
/// call the helper tools
/////////////////////////////////////////////////////////////////////////////
StatusCode Trk::MeasurementVectorNtupleTool::callHelperTools(
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters* trkPar,
    const bool& isOutlier,
    const int& detectorType,
    const int& nCurrentHit) const {

    // ------------------------------------
    // try if measurement is a competingROT and check if joboption
    //   is set to use ROT with highest assgn. prob. in the case of a competingROT.
    // for general helper tools measurement is always used (the jobOption
    //   has no meaning for them).
    const Trk::MeasurementBase* measurementBaseOrROT = measurement;
    if (m_useROTwithMaxAssgnProb) {
      // use ROT with max assgn prob for calculations
      const Trk::CompetingRIOsOnTrack* comprot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(measurement);
      if (comprot){
        measurementBaseOrROT = &comprot->rioOnTrack(comprot->indexOfMaxAssignProb());
      }
    }

    // -------------------------------------------
    // write detector specific data (for used measurements only, i.e. not for outliers)
    if ( !isOutlier && ( detectorType==TrackState::Pixel ||
                         detectorType==TrackState::SCT   ||
                         detectorType==TrackState::TRT   ||
                         detectorType==TrackState::MDT   ||
                         detectorType==TrackState::CSC   ||
                         detectorType==TrackState::RPC   ||
                         detectorType==TrackState::TGC)   ) {
      std::vector< const Trk::IValidationNtupleHelperTool* >::const_iterator toolIter;
      std::vector< const Trk::IValidationNtupleHelperTool* >::const_iterator toolIterEnd;
      switch (detectorType) {
      case TrackState::Pixel:
        toolIter = m_PixelHelperTools.begin();
        toolIterEnd = m_PixelHelperTools.end();
        m_pixelHitIndex->push_back(nCurrentHit-1);
        m_nPixelHits++;
        break;
      case TrackState::SCT:
        toolIter = m_SCTHelperTools.begin();
        toolIterEnd = m_SCTHelperTools.end();
        m_sctHitIndex->push_back(nCurrentHit-1);
        m_nSCTHits++;
        break;
      case TrackState::TRT:
        toolIter = m_TRTHelperTools.begin();
        toolIterEnd = m_TRTHelperTools.end();
        m_trtHitIndex->push_back(nCurrentHit-1);
        m_nTRTHits++;
        break;
      case TrackState::MDT:
        toolIter = m_MDTHelperTools.begin();
        toolIterEnd = m_MDTHelperTools.end();
        m_mdtHitIndex->push_back(nCurrentHit-1);
        m_nMDTHits++;
        break;
      case TrackState::CSC:
        toolIter = m_CSCHelperTools.begin();
        toolIterEnd = m_CSCHelperTools.end();
        m_cscHitIndex->push_back(nCurrentHit-1);
        m_nCSCHits++;
        break;
      case TrackState::RPC:
        toolIter = m_RPCHelperTools.begin();
        toolIterEnd = m_RPCHelperTools.end();
        m_rpcHitIndex->push_back(nCurrentHit-1);
        m_nRPCHits++;
        break;
      case TrackState::TGC:
        toolIter = m_TGCHelperTools.begin();
        toolIterEnd = m_TGCHelperTools.end();
        m_tgcHitIndex->push_back(nCurrentHit-1);
        m_nTGCHits++;
        break;
      }
      // now run the detector specific helper tools
      for (; toolIter!=toolIterEnd; ++toolIter) {
        if (((*toolIter)->fillMeasurementData(measurementBaseOrROT, trkPar, detectorType, isOutlier)).isFailure()) {
          if (!m_helperToolWarning[(*toolIter)]) {
            msg(MSG::WARNING) << "sub-det helper tool did not succeed to fill data (further warnings for this tool will be suppressed)"  << endmsg;
            m_helperToolWarning[(*toolIter)] = true;
          }
        }
      }
    } //end if (detectorType!=TrackState::unidentified && !isOutlier)

    // call the general tools
    if (m_GeneralHelperTools.size() > 0) {
      std::vector< const Trk::IValidationNtupleHelperTool* >::const_iterator toolIter = m_GeneralHelperTools.begin();
      for (; toolIter!=m_GeneralHelperTools.end(); ++toolIter) {
        if (((*toolIter)->fillMeasurementData(measurement, trkPar, detectorType, isOutlier)).isFailure()) {
          if (!m_helperToolWarning[(*toolIter)]) {
            msg(MSG::WARNING) << "general helper tool did not succeed to fill general data"  << endmsg;
            m_helperToolWarning[(*toolIter)] = true;
          }
        }
      }
    }

    return StatusCode::SUCCESS;
}

StatusCode Trk::MeasurementVectorNtupleTool::fillHoleData (
        const Trk::TrackStateOnSurface& tsos) const {
    if (!m_doHoleSearch) return StatusCode::SUCCESS;
    // check if we really have a hole
    if (!tsos.type(Trk::TrackStateOnSurface::Hole)) return StatusCode::FAILURE;
    // hole without track pars is not allowed
    if (!tsos.trackParameters()) return StatusCode::FAILURE;
    // identify the detector type
    int detectorType = TrackState::unidentified;
    const Identifier id = tsos.trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
    if ( id.is_valid() ) {
        if (m_idHelper->is_pixel(id) ) {
            detectorType = TrackState::Pixel;
        } else if (m_idHelper->is_sct(id)) {
            detectorType = TrackState::SCT;
        } else if (m_idHelper->is_trt(id)) {
            detectorType = TrackState::TRT;
        } else if (m_idHelper->is_mdt(id)) {
            detectorType = TrackState::MDT;
        } else if (m_idHelper->is_csc(id)) {
            detectorType = TrackState::CSC;
        } else if (m_idHelper->is_rpc(id)) {
            detectorType = TrackState::RPC;
        } else if (m_idHelper->is_tgc(id)) {
            detectorType = TrackState::TGC;
        } else {
            detectorType = TrackState::unidentified;
        }
    } // end if(isValid)
    // count the holes
    m_nHoles++;
    m_HoleDetectorType->push_back(detectorType);
    ATH_MSG_VERBOSE ("hole #" << m_nHoles-1 << ": detector technology identified as " << detectorType);
    if (detectorType == TrackState::Pixel) m_nPixelHoles++;
    if (detectorType == TrackState::SCT) m_nSctHoles++;

    // ----------------------------
    // write detector specific data
    if ( detectorType!=TrackState::unidentified ) {
        std::vector< const Trk::IValidationNtupleHelperTool* >::const_iterator toolIter;
        std::vector< const Trk::IValidationNtupleHelperTool* >::const_iterator toolIterEnd;
        switch (detectorType) {
        case TrackState::Pixel:
            toolIter = m_PixelHelperTools.begin();
            toolIterEnd = m_PixelHelperTools.end();
            break;
        case TrackState::SCT:
            toolIter = m_SCTHelperTools.begin();
            toolIterEnd = m_SCTHelperTools.end();
            break;
        case TrackState::TRT:
            toolIter = m_TRTHelperTools.begin();
            toolIterEnd = m_TRTHelperTools.end();
            break;
        case TrackState::MDT:
            toolIter = m_MDTHelperTools.begin();
            toolIterEnd = m_MDTHelperTools.end();
            break;
        case TrackState::CSC:
            toolIter = m_CSCHelperTools.begin();
            toolIterEnd = m_CSCHelperTools.end();
            break;
        case TrackState::RPC:
            toolIter = m_RPCHelperTools.begin();
            toolIterEnd = m_RPCHelperTools.end();
            break;
        case TrackState::TGC:
            toolIter = m_TGCHelperTools.begin();
            toolIterEnd = m_TGCHelperTools.end();
            break;
        }
        // now run the detector specific helper tools
        for (; toolIter!=toolIterEnd; ++toolIter) {
            if (((*toolIter)->fillHoleData(tsos, detectorType)).isFailure()) {
                msg(MSG::WARNING) << "sub-det helper tool did not succeed to fill hole data"  << endmsg;
            }
        }
    } //end if (detectorType!=TrackState::unidentified)

    // call the general tools
    if (m_GeneralHelperTools.size() > 0) {
        std::vector< const Trk::IValidationNtupleHelperTool* >::const_iterator toolIter = m_GeneralHelperTools.begin();
        for (; toolIter!=m_GeneralHelperTools.end(); ++toolIter) {
            if (((*toolIter)->fillHoleData(tsos, detectorType)).isFailure()) {
                msg(MSG::WARNING) << "general helper tool did not succeed to fill general hole data"  << endmsg;
            }
        }
    }
    return StatusCode::SUCCESS;
}
