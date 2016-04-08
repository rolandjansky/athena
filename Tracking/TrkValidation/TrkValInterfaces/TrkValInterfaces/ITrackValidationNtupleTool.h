/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IValidationNtupleTool.h
//   Header file for interface of ValidationNtupleTools
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_ITRACKVALIDATIONNTUPLETOOL_H
#define TRK_ITRACKVALIDATIONNTUPLETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkFitterUtils/FitterStatusCode.h"

class TrackTruth;
class TTree;
class HepMcParticleLink;

namespace Trk {
static const InterfaceID IID_ITrackValidationNtupleTool("ITrackValidationNtupleTool",1,0);

class TrackParticleBase;
class MeasurementBase;     //!< measurement base
class Track;
class ProtoTrackStateOnSurface; //!< measurement base
class TrackStateOnSurface;

    /** trajectory of Trk::ProtoTrackStateOnSurface as used internally by the Trk::KalmanFitter and Trk::DeterministicAnnealingFilter */
    typedef std::vector<Trk::ProtoTrackStateOnSurface> ProtoTrajectory;

/** @class ITrackValidationNtupleTool
    provides the interface for validation tools which write special information
    about generated tracks into ntuples.
*/

class ITrackValidationNtupleTool : virtual public IAlgTool {
public:
     /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();

    /** @brief add branches to the tree
      Should be called once (per track collection and tree) dunring the initialisation phase by the calling algorithm
      (usually Trk::TrackValidationNtupleWriter) */
    virtual StatusCode addNtupleItems (
        TTree* tree ) const = 0;

    /** fill ntuple data of a given track without writing the record.
    - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackData (
        const Trk::Track&,
        const int iterationIndex,
        const Trk::FitterStatusCode fitStatCode = Trk::FitterStatusCode::Success ) const = 0;

    /** fill ntuple data of a given track particle without writing the record.
    - if this method is called twice without writing the ntuple inbetween the first data will be lost! */
    virtual StatusCode fillTrackParticleData (
        const Trk::TrackParticleBase&) const = 0;
        
    virtual StatusCode fillTrackTruthData ( const Trk::TrackParameters*&,
                                            const TrackTruth&, 
                                            const int truthIndex = -1 ) const = 0;
    
    /** fill ntuple data of a given proto-trajectory (function used for fitter validation) */
    virtual StatusCode fillProtoTrajectoryData (
        const Trk::ProtoTrajectory&,
        const int iterationIndex,
        const Trk::Perigee* = 0,
        const Trk::FitterStatusCode fitStatCode = Trk::FitterStatusCode::Success ) const = 0;

    /** reset the variables after writing the record to disk ntuple */
    virtual void resetVariables( ) const = 0;
};

inline const InterfaceID& Trk::ITrackValidationNtupleTool::interfaceID() {
    return IID_ITrackValidationNtupleTool;
}

} // end of namespace

#endif // TRK_ITRACKVALIDATIONNTUPLETOOL_H
