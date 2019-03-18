/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// BasicValidationNtupleTool.h
//   Header file for BasicValidationNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann -at- cern.ch, Wolfgang.Liebig -at- cern.ch
///////////////////////////////////////////////////////////////////
#ifndef TRK_TRACKINFONTUPLETOOL_H
#define TRK_TRACKINFONTUPLETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkValInterfaces/ITrackValidationNtupleTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

namespace Trk {

class TrackInfo;

/** @class BasicValidationNtupleTool
    @brief This validation tool writes basic information about Trk::Track
    into an ntuple; it is also the steering tool for all Trk::IValidationNtupleHelperTool.

    Every entry (track) contains the event number, a track id (numbering the tracks within
    one event) and an optional iteration index, which can be used if different
    intermediate results of a track are produced in the refit procedure. The track id is the
    same for all tracks generated within one iteration process and reset to 0 every
    time a new event is started.

**/

class TrackInformationNtupleTool : virtual public Trk::ITrackValidationNtupleTool, public AthAlgTool {
public:

    // standard AlgToolmethods
    TrackInformationNtupleTool(const std::string&,const std::string&,const IInterface*);
    ~TrackInformationNtupleTool();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    /** @brief add branches to the tree
      Should be called once (per track collection and tree) dunring the initialisation phase by the
      calling algorithm (usually Trk::TrackValidationNtupleWriter) */
    virtual StatusCode addNtupleItems ( TTree* tree ) const;

    /** fill ntuple data of a given track without writing the record.
    - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackData (
        const Trk::Track&,
        const int iterationIndex,
        const unsigned int fitStatCode ) const;

    /** fill ntuple data of a given TrackParticle without writing the record.
        - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackParticleData ( const Trk::TrackParticleBase& ) const;

    //! fill data about the truth match (score, parameter-pulls etc)
    virtual StatusCode fillTrackTruthData ( const TrackParameters*&,
                                            const TrackTruth&,
                                            const int) const;

    /** fill ntuple data of a given proto-trajectory (function used for fitter validation) */
    virtual StatusCode fillProtoTrajectoryData (
        const Trk::ProtoTrajectory&,
        const int iterationIndex,
        const Trk::Perigee*,
        const unsigned int fitStatCode ) const;

    /** reset the variables after writing the record to disk ntuple */
    virtual void resetVariables( ) const;

private:

    // attempts to assign a clear seed based on the hierarchy of pattern algs
    int  getSeed(const Trk::TrackInfo& info) const;

    mutable int             m_TrackIDcounter;
    mutable unsigned int    m_lastEventNumber;

    // --- ntuple variables, doxygen comments are also basis for wiki texts --- 
    mutable int         m_runNumber;   //!< run number the track belongs to
    mutable int         m_eventNumber; //!< event number the track belongs to 
    mutable int         m_TrackID;     //!< number of the track within the current event
    mutable int         m_iterIndex;   //!< iteration index of the track (for DAF & internal call, EDM tracks always 0)
    mutable int         m_fitStatusCode;   //!< return status code of the track fitter (for fit debugging)
    mutable int         m_trackFitAuthor;  //!< author (fitter) of the current track
    mutable int         m_trackSeedAuthor; //!< author (main seed finder) of the current track.
    mutable int         m_particleHypothesis; //!< particle hypothesis with which track was fitted
    mutable float       m_Rec_chi2overNdof; //!< chi2 / n.d.o.f of reconstructed track
    mutable int         m_ndof;        //!< number of degrees of freedom of track fit
    mutable int         m_nHits;       //!< number of measurements on the track (including outliers)

    SG::ReadHandleKey<xAOD::EventInfo>    m_evt  {this, "EvtInfo", "EventInfo", "EventInfo name"};
};


} // end of namespace

inline StatusCode Trk::TrackInformationNtupleTool::fillTrackTruthData (const TrackParameters*&,
                                                                       const TrackTruth&,
                                                                       const int ) const
{return StatusCode::SUCCESS;}


#endif // TRK_TRACKINFONTUPLETOOL_H
