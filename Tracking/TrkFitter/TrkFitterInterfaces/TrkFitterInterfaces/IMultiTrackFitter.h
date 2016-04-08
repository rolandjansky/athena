/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IMultiTrackFitter.h
//   Header file for interface of Track Fitters in the new EDM
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef IMULTITRACKFITTER_H
#define IMULTITRACKFITTER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkFitterUtils/FitterStatusCode.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
#include "DataModel/DataVector.h"
namespace Trk {

class Track;                 //!> ATLAS standard track class
class ProtoTrackStateOnSurface;




typedef std::vector<Trk::ProtoTrackStateOnSurface> Trajectory;
static const InterfaceID IID_IMultiTrackFitter("IMultiTrackFitter",1,0);


/** @class IMultiTrackFitter

    provides the abstract interface for multi track fitting.
    This interfaces carries different methods for the main use cases
    of fitting measurements and re-fitting a track.

    @author M. Neumann, S. Fleischmann <http://consult.cern.ch/xwho>
*/

class IMultiTrackFitter : virtual public IAlgTool {

public:
    /** other standard AlgTool methods */
    //    virtual ~IMultiTrackFitter() {};                //!< virtual destructor

    static const InterfaceID& interfaceID ()   //!< the Tool's interface
    {
        return IID_IMultiTrackFitter;
    }


    /** RE-FIT TRACKS.
        Since it is not our but the fitter model's decision if to
        re-fit on PRD or ROT level, it is made pure virtual.
        */
    virtual std::vector<Track*>* fit (  const DataVector<const Track >&,
                                        const ParticleHypothesis matEffects=Trk::nonInteracting) const = 0;



    /** FIT TRACKS TO SETS OF MEASUREMENTBASE.
        Main fit method. The TrackParameters vector is a first
        estimate for the tracks, represented close to the origin.
        The DataVector of MeasurementSet and TrackParameters have to have the same size.
    */
    virtual std::vector<Track*>* fit(   const DataVector < MeasurementSet > &,
                                        const DataVector <const TrackParameters >&,
                                        const ParticleHypothesis matEffects=Trk::nonInteracting) const = 0;


    /** provides way of getting more detailed information about failing
    fits than NULL track pointer */
    virtual FitterStatusCode statusCodeOfLastFit() const;

}; // end of class
} // end of namespace

// wait until all fitters have it implemented
inline Trk::FitterStatusCode
Trk::IMultiTrackFitter::statusCodeOfLastFit() const
{
    return Trk::FitterStatusCode::Success;
}


#endif  /* IMULTITRACKFITTER_H */
