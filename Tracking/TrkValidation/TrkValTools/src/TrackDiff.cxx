/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrackDiff.cxx
//   Source file for class TrackDiff
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////


//Gaudi
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TString.h"
#include "TTree.h"

// Trk
#include "TrkValTools/TrackDiff.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"

#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/Track.h"

#include "AtlasDetDescr/AtlasDetectorID.h"

// constructor
Trk::TrackDiff::TrackDiff(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
        AthAlgTool(t,n,p),
        m_nt(nullptr),
        m_idHelper(nullptr),
        //strings initialised in job options
        m_compareOutliers(true),
        m_compareAll(false),
        m_writeNtuple(true),
        m_writeCompetingROT(false),
        m_eventNumber{},
        m_nRefStates{},
        m_nCompStates{},
        m_fakes{},

        m_missed{},
        m_wrongType{},
        m_missingType{},
        m_fakeType{},
        m_refType{},
        m_compareType{},
        m_PRD_Mismatches{},
        m_driftCircleSignFlips{},
        m_noDriftTime{},
        m_nDiffs{},
         m_detectorType{},
         m_isFake{},   
         m_isMissing{},
         m_isPRD_Mismatch{},
         m_isFlippedSign{},
         m_isNoDriftTime{},
         m_refIsMeasurement{},
         m_refIsOutlier{},
         m_compIsMeasurement{},
         m_compIsOutlier{},
         m_maxAssignProb{},
         m_sumAssignProb{},
         m_surfX{},
         m_surfY{},
         m_surfZ{},
        m_trackEta{},
        m_trackPhi{},
        m_nRefStatesSum{},
        m_nCompStatesSum{},
        m_fakesSum{},
        m_missedSum{},
        m_wrongTypeSum{},
        m_PRD_MismatchesSum{},
        m_trackSum(0) {
    declareInterface<ITrackDiff>(this);
    // Declare the properties
    //declareProperty("BookNewNtuple", m_bookNewNtuple = true);
    declareProperty("NtupleFileName",       m_ntupleFileName = "/NTUPLES");
    declareProperty("NtupleDirectoryName",  m_ntupleDirName = "TrackValidation");
    declareProperty("NtupleTreeName",       m_ntupleTreeName = "TrackDiff");
    //declareProperty("CompareMeasurements",  m_compareMeasurements = true);
    declareProperty("CompareOutliers",      m_compareOutliers = true);
    declareProperty("CompareAllTSoS",       m_compareAll = false);
    declareProperty("WriteNtuple",          m_writeNtuple = true);
    declareProperty("WriteCompetingROTdata",m_writeCompetingROT = false);
}

// destructor
Trk::TrackDiff::~TrackDiff() {}



// initialize
StatusCode Trk::TrackDiff::initialize() {

  ATH_CHECK( m_evt.initialize() );

    StatusCode sc;
    if (m_writeNtuple) {
        // ---------------------------
        // retrive pointer to THistSvc
        ITHistSvc *tHistSvc;
        sc =  service("THistSvc", tHistSvc);
        if (sc.isFailure()) {
          ATH_MSG_ERROR ( "Unable to retrieve pointer to THistSvc" );
          return sc;
        }

        // ---------------------------
        // create tree and register it to THistSvc
        m_nt = new TTree(TString(m_ntupleTreeName), "Track diff output");
        // NB: we must not delete the tree, this is done by THistSvc
        std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleTreeName;
        sc = tHistSvc->regTree(fullNtupleName, m_nt);
        if (sc.isFailure()) {
          ATH_MSG_ERROR ("Unable to register TTree : " << fullNtupleName);
          return sc;
        }
    
        //-----------------
        // add items
        // event info:
        m_nt->Branch("EventNumber",             &m_eventNumber );
        m_nt->Branch("RefTrackPhi0",            &m_trackPhi    );
        m_nt->Branch("RefTrackEta0",            &m_trackEta    );
        
        m_nt->Branch("nRefStates",              &m_nRefStates, "nRefStates[8]/I");
        m_nt->Branch("nCompareStates",          &m_nCompStates, "nRefStates[8]/I");
        m_nt->Branch("nFakeStates",             &m_fakes, "nFakeStates[8]/I");
        m_nt->Branch("nMissedStates",           &m_missed, "nMissedStates[8]/I");
        m_nt->Branch("nWrongType",              &m_wrongType, "nWrongType[8]/I");
        m_nt->Branch("nPRD_Mismatches",         &m_PRD_Mismatches, "nPRD_Mismatches[8]/I");
        m_nt->Branch("nDriftCircleSignFlips",   &m_driftCircleSignFlips  );
        m_nt->Branch("nNoDriftTime",            &m_noDriftTime           );
        m_nt->Branch("nRefTypes",               &m_refType, "nRefTypes[8]/I");
        m_nt->Branch("nCompareTypes",           &m_compareType, "nCompareTypes[8]/I");
        m_nt->Branch("nMissedTypes",            &m_missingType, "nMissedTypes[8]/I");
        m_nt->Branch("nFakeTypes",              &m_fakeType, "nFakeTypes[8]/I");
        // entries for each difference
        m_nt->Branch("nDiffs",                  &m_nDiffs  );
        m_detectorType      = new std::vector<int>();
        m_isFake            = new std::vector<int>();
        m_isMissing         = new std::vector<int>();
        m_isPRD_Mismatch    = new std::vector<int>();
        m_isFlippedSign     = new std::vector<int>();
        m_isNoDriftTime     = new std::vector<int>();
        m_refIsMeasurement  = new std::vector<int>();
        m_refIsOutlier      = new std::vector<int>();
        m_compIsMeasurement = new std::vector<int>();
        m_compIsOutlier     = new std::vector<int>();
        m_surfX             = new std::vector<float>();
        m_surfY             = new std::vector<float>();
        m_surfZ             = new std::vector<float>();

        m_nt->Branch("DetectorType",            &m_detectorType);
        m_nt->Branch("IsFake",                  &m_isFake);
        m_nt->Branch("IsMissing",               &m_isMissing);
        m_nt->Branch("IsPRD_Mismatch",          &m_isPRD_Mismatch);
        m_nt->Branch("IsFlippedSign",           &m_isFlippedSign);
        m_nt->Branch("IsNoDriftTime",           &m_isNoDriftTime);
        m_nt->Branch("RefIsMeasurement",        &m_refIsMeasurement);
        m_nt->Branch("RefIsOutlier",            &m_refIsOutlier);
        m_nt->Branch("ComparedIsMeasurement",   &m_compIsMeasurement);
        m_nt->Branch("ComparedIsOutlier",       &m_compIsOutlier);
        m_nt->Branch("SurfaceX" ,               &m_surfX);
        m_nt->Branch("SurfaceY" ,               &m_surfY);
        m_nt->Branch("SurfaceZ" ,               &m_surfZ);

        if (m_writeCompetingROT) {
            m_maxAssignProb = new std::vector<float>();
            m_sumAssignProb = new std::vector<float>();
            m_nt->Branch("MaxAssgnProb",        &m_maxAssignProb);
            m_nt->Branch("SumAssgnProb",        &m_sumAssignProb);
        } // end if writeComepetingROT
    } // end if (m_writeNtuple)
    // ---------------------------------------
    // Get the detector helper to be able to identify sub-det technologies
    if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
      ATH_MSG_ERROR ("Could not get AtlasDetectorID helper" );
      return StatusCode::FAILURE;
    }

    for (int detType = 0; detType < Trk::TrackState::NumberOfMeasurementTypes; detType++) {
        m_fakesSum[detType] = 0;
        m_missedSum[detType] = 0;
        m_PRD_MismatchesSum[detType] = 0;
        m_nRefStatesSum[detType] = 0;
        m_nCompStatesSum[detType] = 0;
        m_wrongTypeSum[detType] = 0;
        m_fakes[detType] = 0;
        m_missed[detType] = 0;
        m_PRD_Mismatches[detType] = 0;
        m_nRefStates[detType] = 0;
        m_nCompStates[detType] = 0;
        m_wrongType[detType] = 0;
    }

    return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode Trk::TrackDiff::finalize() {
    ATH_MSG_INFO ( "sums for " << m_trackSum << " compared tracks" );
    ATH_MSG_INFO ( "reference  TSoS  (Pixel/SCT/TRT): " << m_nRefStatesSum[Trk::TrackState::Pixel] << "/" << m_nRefStatesSum[Trk::TrackState::SCT] << "/" << m_nRefStatesSum[Trk::TrackState::TRT] );
    ATH_MSG_INFO ( "compared   TSoS  (Pixel/SCT/TRT): " << m_nCompStatesSum[Trk::TrackState::Pixel] << "/" << m_nCompStatesSum[Trk::TrackState::SCT] << "/" << m_nCompStatesSum[Trk::TrackState::TRT] );
    ATH_MSG_INFO ( "missed     TSoS  (Pixel/SCT/TRT): " << m_missedSum[Trk::TrackState::Pixel] << "/" << m_missedSum[Trk::TrackState::SCT] << "/" << m_missedSum[Trk::TrackState::TRT] );
    ATH_MSG_INFO ( "faked      TSoS  (Pixel/SCT/TRT): " << m_fakesSum[Trk::TrackState::Pixel] << "/" << m_fakesSum[Trk::TrackState::SCT] << "/" << m_fakesSum[Trk::TrackState::TRT] );
    ATH_MSG_INFO ( "wrong type TSoS  (Pixel/SCT/TRT): " << m_wrongTypeSum[Trk::TrackState::Pixel] << "/" << m_wrongTypeSum[Trk::TrackState::SCT] << "/" << m_wrongTypeSum[Trk::TrackState::TRT] );
    ATH_MSG_INFO ( "PRD mismatches   (Pixel/SCT/TRT): " << m_PRD_MismatchesSum[Trk::TrackState::Pixel] << "/" << m_PRD_MismatchesSum[Trk::TrackState::SCT] << "/" << m_PRD_MismatchesSum[Trk::TrackState::TRT] );

    ATH_MSG_INFO ( "finalize() successful in " << name() );
    return StatusCode::SUCCESS;
}

//////////////////////////////////////
/// diff of two tracks
//////////////////////////////////////
StatusCode Trk::TrackDiff::diff (
        const Trk::Track& referenceTrack,
        const Trk::Track& comparedTrack ) const {

    ATH_MSG_VERBOSE ( "in diff(trk, trk)" );
    resetVariables();

    // ---------------------------------------
    // fill event data
    SG::ReadHandle<xAOD::EventInfo> evt(m_evt);
    if (!evt.isValid()) {
      ATH_MSG_ERROR ("Could not retrieve event info");
      return StatusCode::FAILURE;
    }

    m_eventNumber = evt->eventNumber();
    if (referenceTrack.perigeeParameters()) {
        m_trackPhi = referenceTrack.perigeeParameters()->parameters()[Trk::phi0];
        m_trackEta = referenceTrack.perigeeParameters()->eta();
    } else {
        m_trackPhi = -1000.;
        m_trackEta = -1000.;
    }
 

    // list of track state data of the reference track
    DataVector< const Trk::TrackStateData > *refTrackStateData;
    // list of track state data of the compared track
    DataVector< const Trk::TrackStateData > *compareTrackStateData;
    // get data from the track states to be able to compare the surfaces and PrepRawData
    refTrackStateData     = extractDataFromTrack(referenceTrack);
    compareTrackStateData = extractDataFromTrack(comparedTrack);

    ATH_MSG_VERBOSE ( "number of track states: ref=" << refTrackStateData->size() << ", compare=" << compareTrackStateData->size() );
    ATH_MSG_VERBOSE ( "reference track: eta_0 = " << m_trackEta << ", phi_0 = " << m_trackPhi );
    ATH_MSG_DEBUG ( "     Ref Comp other diffs     detector " );
    //----------------------------------------------
    // loop over the track states of the reference track
    //   and try to find the related measurements on the
    //   comparison track
    DataVector< const Trk::TrackStateData >::iterator refIter = refTrackStateData->begin();
    DataVector< const Trk::TrackStateData >::iterator compIter = compareTrackStateData->begin();
    for (; refIter != refTrackStateData->end(); refIter++) {
        // count the reference states by detector type
        //m_nRefStates[ refIter->detType() ]++;
        // loop over trackstates of the comparison track till we find
        //    a track state with the same surface or the same PRD
        bool foundMatchingState = false;
        //bool foundDiff = false;
        // cache the pointer to reference PRD, so we do not have get it again and again
        const Trk::PrepRawData* refPRD = 0;
        if ((*refIter)->rot()) {
            refPRD = (*refIter)->rot()->prepRawData();
        }
        compIter = compareTrackStateData->begin();
        for ( ; compIter != compareTrackStateData->end(); compIter++ ) {
            // compare the surfaces of the reference and the compared track state
            if ( (*compIter)->surface() == (*refIter)->surface() ) {
                // we found a track state on the same surface!
                foundMatchingState = true;
                //m_nCompStates[ compIter->detType() ]++;
                //if (diffStateInfo( refIter, compIter ) {
                //    foundDiff = true;
                //}
                // there should be at maximum only one track state
                // per surface, so we do not have to search further!
                break;
            } else if ( refPRD && (*compIter)->rot() ) {
                // try to match the PrepRawData, if we did not succeed with the surfaces
                // this is only needed in the case of dynamically created surfaces,
                // which contain track specific corrections (e.g. "wire sagging")
                if ( refPRD == ((*compIter)->rot()->prepRawData()) ) {
                    // we found a track state belonging to the same PrepRawData
                    foundMatchingState = true;
                   
                    // there should be only one track state
                    // belonging to the same PRD, so we can stop searching!
                    break;
                } // end if PRD match
            } // end else if( refPRD && compIter->rot() )
        } // end loop over compared states
        if (!foundMatchingState) {
            // we have a state in the reference, which is not contained in the
            // compared track!
            diffStateInfo((*refIter), 0);
        } else {
            diffStateInfo((*refIter), (*compIter));
            // drop the compared track state from our list:
            compareTrackStateData->erase(compIter);
        }

    } // end loop over reference states

    // loop over the remaining compared states: They are fakes
    compIter = compareTrackStateData->begin();
    for (; compIter != compareTrackStateData->end(); compIter++) {
        diffStateInfo(0, (*compIter));
    }
    // -----------------------
    // output some statistics:

    ATH_MSG_VERBOSE  ( "reference  TSoS  (Pixel/SCT/TRT): " << m_nRefStates[Trk::TrackState::Pixel] << "/" << m_nRefStates[Trk::TrackState::SCT] << "/" << m_nRefStates[Trk::TrackState::TRT] );
    ATH_MSG_VERBOSE  ( "compared   TSoS  (Pixel/SCT/TRT): " << m_nCompStates[Trk::TrackState::Pixel] << "/" << m_nCompStates[Trk::TrackState::SCT] << "/" << m_nCompStates[Trk::TrackState::TRT] );
    ATH_MSG_VERBOSE  ( "missed     TSoS  (Pixel/SCT/TRT): " << m_missed[Trk::TrackState::Pixel] << "/" << m_missed[Trk::TrackState::SCT] << "/" << m_missed[Trk::TrackState::TRT] );
    ATH_MSG_VERBOSE  ( "faked      TSoS  (Pixel/SCT/TRT): " << m_fakes[Trk::TrackState::Pixel] << "/" << m_fakes[Trk::TrackState::SCT] << "/" << m_fakes[Trk::TrackState::TRT] );
    ATH_MSG_VERBOSE  ( "wrong type TSoS  (Pixel/SCT/TRT): " << m_wrongType[Trk::TrackState::Pixel] << "/" << m_wrongType[Trk::TrackState::SCT] << "/" << m_wrongType[Trk::TrackState::TRT] );
    ATH_MSG_VERBOSE  ( "PRD mismatches   (Pixel/SCT/TRT): " << m_PRD_Mismatches[Trk::TrackState::Pixel] << "/" << m_PRD_Mismatches[Trk::TrackState::SCT] << "/" << m_PRD_Mismatches[Trk::TrackState::TRT] );
    ATH_MSG_VERBOSE  ( "missing   TSoS types: (Meas/Outl): " << m_missingType[Trk::TrackStateOnSurface::Measurement] << "/" << m_missingType[Trk::TrackStateOnSurface::Outlier] );
    ATH_MSG_VERBOSE  ( "fake      TSoS types: (Meas/Outl): " << m_fakeType[Trk::TrackStateOnSurface::Measurement] << "/" << m_fakeType[Trk::TrackStateOnSurface::Outlier] );
    if (m_writeNtuple) {
        //----------------------------------------------
        // write the ntuple record out (once per call)
        m_nt->Fill();
    }
    m_trackSum++;
    // clean up
    delete refTrackStateData;
    delete compareTrackStateData;
    return StatusCode::SUCCESS;
}

/// extract track state data from the track
DataVector< const Trk::TrackStateData >* Trk::TrackDiff::extractDataFromTrack( const Trk::Track& track ) const {

    // Get pointer to track state on surfaces
    const DataVector<const Trk::TrackStateOnSurface>* trackStates = track.trackStateOnSurfaces();
    if (!trackStates) {
      ATH_MSG_ERROR ( "track containes no track states, diff impossible" );
      return 0;
    }

    // create the vector of extracted data
    DataVector< const Trk::TrackStateData >* trackStateData = new DataVector< const Trk::TrackStateData >;

    // track state iterator
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iter = trackStates->begin();
    // Loop over all track states on surfaces
    //    to extract the measurements
    for (; iter != trackStates->end(); iter++) {
        if (!(*iter)) {
          ATH_MSG_WARNING ( "TrackStateOnSurface == Null" );
          continue;
        }
        if (!m_compareAll) {
            // just use TSoS of type "measurement" or "outlier"
            if (m_compareOutliers) {
                if (!( (*iter)->type(Trk::TrackStateOnSurface::Measurement) ||
                        (*iter)->type(Trk::TrackStateOnSurface::Outlier) ) ) {
                    continue;
                }
            } else {
                if (!( (*iter)->type(Trk::TrackStateOnSurface::Measurement) ) ) {
                    continue;
                }
            } // end if (compareOutliers)
        } // end if (!compareAll)

        // Get pointer to measurement on track
        const Trk::MeasurementBase *measurement = (*iter)->measurementOnTrack();
        if (!measurement && !m_compareAll) {
            ATH_MSG_ERROR ( "measurementOnTrack == Null for a TrackStateOnSurface "
            << "of type Measurement or Outlier" );
            continue;
        } // end if (!measurement)
        const Trk::Surface& surface = measurement ?
                                      (measurement->associatedSurface()) : 
                                      (*iter)->trackParameters()->associatedSurface();
        // we need the RIO_OnTrack to decide if we have the same PrepRawData
        //   on the track
        const Trk::RIO_OnTrack* ROT = dynamic_cast< const Trk::RIO_OnTrack* > (measurement);
        if (!ROT) {
            // try to cast to CompetingRIOsOnTrack
            const Trk::CompetingRIOsOnTrack* compROT = dynamic_cast< const Trk::CompetingRIOsOnTrack* > (measurement);
            if (compROT) {
                ROT = &compROT->rioOnTrack( compROT->indexOfMaxAssignProb() );
//                 // quick hack to identify DAF ouliers:
//                 if (!m_compareOutliers && (compROT->assignmentProbability(compROT->indexOfMaxAssignProb()) < 0.4)) {
//                     continue;
//                 }
                // use surface of the most prob. ROT:
                // FIXME put back in place: surface = compROT->rioOnTrack( compROT->indexOfMaxAssignProb() ).associatedSurface();
            } // end if (compROT)
        }
        // if we just compare Measurements and Outliers, we demand a ROT.
        if ( !ROT && !m_compareAll ) {
            continue;
        }
        // try to identify detector type by ROT
        std::string detTypeName = "unidentified";
        Trk::TrackState::MeasurementType detType = Trk::TrackState::unidentified;
        if ( ROT ) {
            detType = detectorType(*ROT, detTypeName);
        }
        // add the extracted data to the vector:
        const Trk::TrackStateData *tsData = new const Trk::TrackStateData(  (*iter),
                                                                            ROT,
                                                                            &surface,
                                                                            detTypeName,
                                                                            detType);
        trackStateData->push_back(tsData);
        //ATH_MSG_VERBOSE  ( detTypeName << " surf:  " << surface << " (" << surface->center().x() << ", " << surface->center().y() << "," << surface->center().z() << ")" 
        //                      << " ROT: " << ROT << " PRD: " << (ROT ? ROT->prepRawData() : 0) << " PRD detEl: " << (ROT ? ROT->prepRawData()->detectorElement() : 0) );
    } // end for loop over trackstates
    return trackStateData;
}


/// reset variables
void Trk::TrackDiff::resetVariables() const {
    // reset the counters
    //m_fakes = 0;
    //m_missed = 0;
    m_driftCircleSignFlips = 0;
    m_noDriftTime = 0;
    m_nDiffs = 0;
    for (int detType = 0; detType < Trk::TrackState::NumberOfMeasurementTypes; detType++) {
        m_fakesSum[detType] += m_fakes[detType];
        m_fakes[detType] = 0;
        m_missedSum[detType] += m_missed[detType];
        m_missed[detType] = 0;
        m_PRD_MismatchesSum[detType] += m_PRD_Mismatches[detType];
        m_PRD_Mismatches[detType] = 0;
        m_nRefStatesSum[detType] += m_nRefStates[detType];
        m_nRefStates[detType] = 0;
        m_nCompStatesSum[detType] += m_nCompStates[detType];
        m_nCompStates[detType] = 0;
        m_wrongType[detType] += m_wrongType[detType];
        m_wrongType[detType] = 0;
    }
    for (int TSoSType = 0; TSoSType < Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes; TSoSType++) {
        m_missingType[TSoSType] = 0;
        m_fakeType[TSoSType] = 0;
        m_refType[TSoSType] = 0;
        m_compareType[TSoSType] = 0;
    }
//     m_missingType->clear();
//     m_missingType->resize(Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes, 0);
//     m_fakeType->clear();
//     m_fakeType->resize(Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes, 0);
//     m_PRD_Mismatches->clear();
//     m_PRD_Mismatches->resize(8, 0);
    if (m_writeNtuple) {
        m_detectorType->clear();
        m_isFake->clear();
        m_isMissing->clear();
        m_isPRD_Mismatch->clear();
        m_isFlippedSign->clear();
        m_isNoDriftTime->clear();
        m_refIsMeasurement->clear();
        m_refIsOutlier->clear();
        m_compIsMeasurement->clear();
        m_compIsOutlier->clear();
        m_surfX->clear();
        m_surfY->clear();
        m_surfZ->clear();

        if (m_writeCompetingROT) {
            m_maxAssignProb->clear();
            m_sumAssignProb->clear();
        } // end if writeComepetingROT
    }

}

/// determine detector type
Trk::TrackState::MeasurementType Trk::TrackDiff::detectorType(const Trk::RIO_OnTrack& ROT,
                                                               std::string &detTypeName ) const {
    // identify by ROT of reference track:
    Identifier id = ROT.identify();

    // get dector type
    if ( id.is_valid()) {
        if (m_idHelper->is_pixel(id) ) {
            detTypeName = "Pixel";
            return Trk::TrackState::Pixel;
        } else if (m_idHelper->is_sct(id)) {
            detTypeName = "SCT";
            return Trk::TrackState::SCT;
        } else if (m_idHelper->is_trt(id)) {
            detTypeName = "TRT";
            return Trk::TrackState::TRT;
        } else if (m_idHelper->is_mdt(id)) {
            detTypeName = "MDT";
            return Trk::TrackState::MDT;
        } else if (m_idHelper->is_csc(id)) {
            detTypeName = "CSC";
            return Trk::TrackState::CSC;
        } else if (m_idHelper->is_rpc(id)) {
            detTypeName = "RPC";
            return Trk::TrackState::RPC;
        } else if (m_idHelper->is_tgc(id)) {
            detTypeName = "TGC";
            return Trk::TrackState::TGC;
        }
    }
    detTypeName = "unidentified";
    return Trk::TrackState::unidentified;
}

/// diff two TrackStateData
bool Trk::TrackDiff::diffStateInfo(const Trk::TrackStateData* refTrackState,
                                   const Trk::TrackStateData* compareTrackState) const {

    bool foundDiff = false;

    bool isFake = false;
    bool isMissing = false;
    bool isPRD_Mismatch = false;
    bool isFlippedSign = false;
    bool isNoDriftTime = false;
    bool refIsMeasurement = false;
    bool refIsOutlier = false;
    bool compIsMeasurement = false;
    bool compIsOutlier = false;
    bool wrongType = false;
    std::string detTypeName = "unidentified";
    Trk::TrackState::MeasurementType detType = Trk::TrackState::unidentified;
    //std::stringstream surfaceCenter;

    //ATH_MSG_VERBOSE ( "Writing info for states: " << (refTrackState ? "REF ": "") << (refTrackState ? m_refIndex : 0) << " " << (compareTrackState ?  "CMP " : "" ) << (compareTrackState ? m_compIndex : 0) );

    // info about the state types
    if (refTrackState){
        refIsMeasurement = refTrackState->trackStateOnSurface()->type(Trk::TrackStateOnSurface::Measurement);
        refIsOutlier = refTrackState->trackStateOnSurface()->type(Trk::TrackStateOnSurface::Outlier);
        if (!compareTrackState) {
            // we have a ref state without compared state
            isMissing = true;
            foundDiff = true;
            m_missed[ refTrackState->detType() ]++;
        }
        // count the reference states by detector type
        m_nRefStates[ refTrackState->detType() ]++;
        detType = refTrackState->detType();
        detTypeName = refTrackState->detTypeName();
        //surfaceCenter << "(" << refTrackState->surface()->center().x() << ", " << refTrackState->surface()->center().y() << "," <<refTrackState->surface()->center().z() << ")";
    } else {
        // we have a compared state without ref state
        isFake = true;
        foundDiff = true;
        m_fakes[ compareTrackState->detType() ]++;
        detType = compareTrackState->detType();
        detTypeName = compareTrackState->detTypeName();
    } // end if (refTrackState)
    if (compareTrackState){
        compIsMeasurement = compareTrackState->trackStateOnSurface()->type(Trk::TrackStateOnSurface::Measurement);
        compIsOutlier = compareTrackState->trackStateOnSurface()->type(Trk::TrackStateOnSurface::Outlier);
        // count the compared states by detector type
        m_nCompStates[ compareTrackState->detType() ]++;
    }//end if (compareTrackState)

    // find differences if both states exist
    if (refTrackState && compareTrackState) {

        // find differences in the TrackStateOnSurface types:
        if (m_compareAll) {

        } else {
            if (m_compareOutliers && (refIsOutlier != compIsOutlier)) {
                foundDiff = true;
                wrongType = true;
                if (refIsOutlier) {
                    m_missingType[Trk::TrackStateOnSurface::Outlier]++;
                } else {
                    m_fakeType[Trk::TrackStateOnSurface::Outlier]++;
                }
            } // end if differ in outlier type
            if (refIsMeasurement != compIsMeasurement ) {
                foundDiff = true;
                wrongType = true;
                if (refIsMeasurement) {
                    m_missingType[Trk::TrackStateOnSurface::Measurement]++;
                } else {
                    m_fakeType[Trk::TrackStateOnSurface::Measurement]++;
                }
            } // end if differ in measurement type
        } // end else (compare all types)

        // find differences in the ROTs:
        const Trk::RIO_OnTrack* refROT = refTrackState->rot();
        const Trk::RIO_OnTrack* compareROT = compareTrackState->rot();
        if (refROT && compareROT) {
            if (refROT->prepRawData() != compareROT->prepRawData()) {
                isPRD_Mismatch = true;
                (m_PRD_Mismatches[detType])++;
            }
            if (detType == Trk::TrackState::TRT) {
                isFlippedSign = ((refROT->localParameters()[Trk::locR]) * (compareROT->localParameters()[Trk::locR]) < 0. );
                if (isFlippedSign) m_driftCircleSignFlips++;
                isNoDriftTime = ((refROT->localParameters()[Trk::locR] != 0. ) && (compareROT->localParameters()[Trk::locR] == 0. ) );
                if (isNoDriftTime) m_noDriftTime++;
            }
            foundDiff = (foundDiff || isPRD_Mismatch || isFlippedSign || isNoDriftTime );
        } // end if (both states have ROTs)
    } // end if (both states exist)
    if (foundDiff) {
        double surfX = 0;
        double surfY = 0;
        double surfZ = 0;
        // give some info about the diff:
        msg(MSG::DEBUG) << "Diff:";
        if (isFake){ msg(MSG::DEBUG) << " -- ";}
        else {msg(MSG::DEBUG) << (refIsOutlier ? " O" : "  ") << (refIsMeasurement ? "M " : "  ");}
        if (isMissing){ msg(MSG::DEBUG) << " -- ";}
        else {msg(MSG::DEBUG) << (compIsOutlier ? " O" : "  ") << (compIsMeasurement ? "M " : "  ");}
        msg(MSG::DEBUG) << (isFake          ? " FakeState      " : "")
                        << (isMissing       ? " MissingState   " : "")
                        << (wrongType       ? " Wrong TSoS type" : "")
                        << (isPRD_Mismatch  ? " PRD mismatch   " : "")
                        << (isFlippedSign   ? " Flipped Sign   " : "")
                        << (isNoDriftTime   ? " NoDriftTime    " : "")
                        << " in " << detTypeName
                        << " detector at surface with center " << endmsg; //<< surfaceCenter
        if (refTrackState) {
            surfX = refTrackState->surface()->center().x();
            surfY = refTrackState->surface()->center().y();
            surfZ = refTrackState->surface()->center().z();
        } else {
            surfX = compareTrackState->surface()->center().x();
            surfY = compareTrackState->surface()->center().y();
            surfZ = compareTrackState->surface()->center().z();
        }
       ATH_MSG_DEBUG ( "(" << surfX << ", " << surfY << "," << surfZ << ")");
        if (m_writeNtuple)  {
            if (m_writeCompetingROT) {
                // write some data specific to TrkCompetingRIOsOnTrack
                float maxAssgnProb = -1.;
                float sumAssgnProb = -1.;
                if (compareTrackState) {
                    const Trk::CompetingRIOsOnTrack* competingROT = dynamic_cast< const Trk::CompetingRIOsOnTrack* > (compareTrackState->trackStateOnSurface()->measurementOnTrack());
                    if (competingROT) {
                        maxAssgnProb = competingROT->assignmentProbability(competingROT->indexOfMaxAssignProb());
                        // sum up all assignment probs:
                        sumAssgnProb = 0.;
                        for (unsigned int index = 0; index < competingROT->numberOfContainedROTs(); index++) {
                            sumAssgnProb += competingROT->assignmentProbability(index);
                        }
                    } // end if competingROT
                } // end if compareTrackState
                m_maxAssignProb->push_back(maxAssgnProb);
                m_sumAssignProb->push_back(sumAssgnProb);
            } // end if writeComepetingROT
        
            // push data into the vectors
            m_isFake->push_back(isFake?1:0);
            m_isMissing->push_back(isMissing?1:0);
            m_detectorType->push_back(detType);
            m_isPRD_Mismatch->push_back(isPRD_Mismatch?1:0);
            m_isFlippedSign->push_back(isFlippedSign?1:0);
            m_isNoDriftTime->push_back(isNoDriftTime?1:0);
            m_refIsOutlier->push_back(refIsOutlier?1:0);
            m_refIsMeasurement->push_back(refIsMeasurement?1:0);
            m_compIsOutlier->push_back(compIsOutlier?1:0);
            m_compIsMeasurement->push_back(compIsMeasurement?1:0);
            m_surfX->push_back(surfX);
            m_surfY->push_back(surfY);
            m_surfZ->push_back(surfZ);
        } // end if (writeNtuple)
    m_nDiffs++;
    } // end if (foundDiff) 
    return (foundDiff);

}
