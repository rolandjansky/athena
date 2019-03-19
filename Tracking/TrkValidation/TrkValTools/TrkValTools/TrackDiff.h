/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrackDiff.h
//   Header file for TrackDiff
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRACKDIFF_H
#define TRK_TRACKDIFF_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkValInterfaces/ITrackDiff.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkValEvent/TrackStateData.h"
#include "AthContainers/DataVector.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

class AtlasDetectorID;
class TTree;

namespace Trk {

 class RIO_OnTrack;
 class Track;
 class Surface;
/** @class TrackDiff
    This validation tool writes the differences between two tracks into
    a ntuple. A detailed report of the differences found is written to
    the message stream in DEBUG mode.

    In the finalize function a summary of all tracks is given.
    
    Ntuple variables of Trk::TrackDiff
<table>
<tr><td>EventNumber</td> <td>Number of the event the tracks belong to</td> <td><tt>int</tt></td></tr>
<tr><td>RefTrackPhi0</td> <td>Phi0 of the reference track</td> <td><tt>float</tt></td></tr>
<tr><td>RefTrackEta0</td> <td>Eta0 of the reference track</td> <td><tt>float</tt></td></tr>
<tr><td>nRefStates</td> <td>number of reference TSoS for each detector type</td> <td><tt>int[detTypes]</tt></td></tr>
<tr><td>nCompareStates</td> <td>number of TSoS of the compared track (for each detector type)</td> <td><tt>int[detTypes]</tt></td></tr>
<tr><td>nFakeStates</td> <td>number of TSoS of the compared track which are not included in the reference track (does not include wrong types), (for each detector type)</td> <td><tt>int[detTypes]</tt></td></tr>
<tr><td>nMissedStates</td> <td>number of TSoS of the reference track which are not included in the compared track (does not include wrong types, (for each detector type))</td> <td><tt>int[detTypes]</tt></td></tr>
<tr><td>nWrongType</td> <td>number of TSoS which have different types in the reference and the compared track (for each detector type)</td> <td><tt>int[detTypes]</tt></td></tr>
<tr><td>nPRD_Mismatches</td> <td>number of TSoS on the same detector surface which have different Trk::PrepRawData pointers (for each detector type)</td> <td><tt>int[detTypes]</tt></td></tr>
<tr><td>nDriftCircleSignFlips</td> <td>number of Trk::RIO_OnTrack in the TRT which have flipped drift radii signs</td> <td><tt>int</tt></td></tr>
<tr><td>nRefTypes</td> <td>number of reference TSoS for each TSoS type</td> <td><tt>int[tsosTypes]</tt></td></tr>
<tr><td>nCompareTypes</td> <td>number of compared TSoS for each TSoS type</td> <td><tt>int[tsosTypes]</tt></td></tr>
<tr><td>nMissedTypes</td> <td>number of TSoS which are not on the compared track (for each TSoS type)</td> <td><tt>int[tsosTypes]</tt></td></tr>
<tr><td>nFakeTypes</td> <td>number of TSoS which are not on the reference track (for each TSoS type)</td> <td><tt>int[tsosTypes]</tt></td></tr>
<tr><td>nDiffs</td> <td>number of differences found</td> <td><tt>int</tt></td></tr>
<tr><td>MeasurementType</td> <td>detector type in which the difference was found</td> <td><tt>int[nDiffs]</tt></td></tr>
<tr><td>IsFake</td> <td>is the difference a fake state?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>IsMissing</td> <td>is the difference a missing state?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>IsPRD_Mismatch</td> <td>is the diff as PRD mismatch?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>IsFlippedSign</td> <td>has the diff flipped sign?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>IsNoDriftTime</td> <td>has the diff !NoDriftTime?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>RefIsMeasurement</td> <td>is reference state a measurement?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>RefIsOutlier</td> <td>is reference state an outlier?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>ComparedIsMeasurement</td> <td>is compared state a measurement?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>ComparedIsOutlier</td> <td>is compared state an outlier?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>MaxAssgnProb</td> <td>just if WriteCompetingROTdata=True: maximum assignment probability of the compared state</td> <td><tt>vector<float> [nDiffs]</tt></td></tr>
<tr><td>SumAssgnProb</td> <td>just if WriteCompetingROTdata=True: sum of assignment probabilities of the compared state</td> <td><tt>vector<float> [nDiffs]</tt></td></tr>
</table>
All numbers contain just the Trk::TrackStateOnSurface which have the types selected
in the jobOptions, i.e. in the standard configuration only Measurements and Outliers.
    @author Sebastian.Fleischmann@cern.ch
*/

class TrackDiff : virtual public Trk::ITrackDiff, public AthAlgTool {
public:

    // standard AlgToolmethods
    TrackDiff(const std::string&,const std::string&,const IInterface*);
    ~TrackDiff();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();


    /** diff of two given tracks */
    virtual StatusCode diff (
        const Trk::Track& referenceTrack,
        const Trk::Track& comparedTrack) const;


private:

    static const int s_maxHits  = 300;   // maximal number of hits per track

    TTree* m_nt; //!< Pointer to the NTuple tree

    AtlasDetectorID* m_idHelper;        //!< Used to find out the sub-det from PRD->identify().

    // jobOptions
    std::string m_ntupleFileName;       //!< jobOption: Ntuple file name
    std::string m_ntupleDirName;        //!< jobOption: Ntuple directory name
    std::string m_ntupleTreeName;       //!< jobOption: Ntuple tree name
    //bool m_compareMeasurements;         //!< jobOption: compare track states of type "measurement"
    bool m_compareOutliers;             //!< jobOption: compare track states of type "outlier"
    bool m_compareAll;                  //!< jobOption: compare track states of all types
    bool m_writeNtuple;                 //!< jobOption: write data to ntuple?
    bool m_writeCompetingROT;           //!< jobOption: write data about TrkCompetingRIOsOnTrack?

    mutable int m_eventNumber;
   
    mutable int m_nRefStates[Trk::TrackState::NumberOfMeasurementTypes];
    mutable int m_nCompStates[Trk::TrackState::NumberOfMeasurementTypes];
    mutable int m_fakes[Trk::TrackState::NumberOfMeasurementTypes];

    mutable int m_missed[Trk::TrackState::NumberOfMeasurementTypes];
    mutable int m_wrongType[Trk::TrackState::NumberOfMeasurementTypes];
    mutable int m_missingType[Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes];
    mutable int m_fakeType[Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes];
    mutable int m_refType[Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes];
    mutable int m_compareType[Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes];
    mutable int m_PRD_Mismatches[Trk::TrackState::NumberOfMeasurementTypes];
    mutable int m_driftCircleSignFlips;
    mutable int m_noDriftTime;

    mutable int m_nDiffs;
    std::vector<int>*   m_detectorType;
    std::vector<int>*   m_isFake;   //!< vector<bool> would be more efficient, but vector<bool> is not a normal vector<> and seems to make trouble
    std::vector<int>*   m_isMissing;
    std::vector<int>*   m_isPRD_Mismatch;
    std::vector<int>*   m_isFlippedSign;
    std::vector<int>*   m_isNoDriftTime;
    std::vector<int>*   m_refIsMeasurement;
    std::vector<int>*   m_refIsOutlier;
    std::vector<int>*   m_compIsMeasurement;
    std::vector<int>*   m_compIsOutlier;
    std::vector<float>* m_maxAssignProb;
    std::vector<float>* m_sumAssignProb;
    std::vector<float>* m_surfX;
    std::vector<float>* m_surfY;
    std::vector<float>* m_surfZ;

    mutable float m_trackEta;
    mutable float m_trackPhi;


    mutable int m_nRefStatesSum[Trk::TrackState::NumberOfMeasurementTypes];
    mutable int m_nCompStatesSum[Trk::TrackState::NumberOfMeasurementTypes];
    mutable int m_fakesSum[Trk::TrackState::NumberOfMeasurementTypes];
    mutable int m_missedSum[Trk::TrackState::NumberOfMeasurementTypes];
    mutable int m_wrongTypeSum[Trk::TrackState::NumberOfMeasurementTypes];
    mutable int m_PRD_MismatchesSum[Trk::TrackState::NumberOfMeasurementTypes];
    mutable int m_trackSum;

    SG::ReadHandleKey<xAOD::EventInfo>    m_evt  {this, "EvtInfo", "EventInfo", "EventInfo name"};

    /** extract data from a Trk::Track into a list of Trk::TrackStateData */
    DataVector< const Trk::TrackStateData >* extractDataFromTrack( const Trk::Track& ) const;

    /** reset the ntuple variables */
    void resetVariables() const;

    /** get detector type and name by identifier of Trk::RIO_OnTrack */
    Trk::TrackState::MeasurementType detectorType(const Trk::RIO_OnTrack& ROT,
                                                   std::string &detTypeName ) const;

    /** diff of two given track state data */
    bool diffStateInfo(const Trk::TrackStateData* refTrackStateData,
                       const Trk::TrackStateData* compareTrackStateData) const;

};


} // end of namespace

#endif // TRK_TRACKDIFF_H
