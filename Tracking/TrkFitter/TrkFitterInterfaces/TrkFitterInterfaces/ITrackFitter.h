/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ITrackFitter.h
//   Header file for interface of Track Fitters in the new EDM
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_ITRACKFITTER_H
#define TRK_ITRACKFITTER_H

#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkFitterUtils/FitterStatusCode.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
namespace Trk {


static const InterfaceID IID_ITrackFitter("ITrackFitter", 1, 0);

/** @class ITrackFitter

    provides the abstract interface for track fitting in the common
    ATLAS Tracking EDM.
    This interfaces carries different methods for the main use cases
    of fitting, extending and re-fitting a track. Each implementation
    can decide how they bundle these use cases onto a common maths core.
    The interface is held general so that it is suitable for global
    and progressive fitter algorithms.
    Regarding athena, it is implemented using the IAlgTool inheritance,
    so fitter implementations should be a component_library.


    @author M. Elsing, W. Liebig <http://consult.cern.ch/xwho>
    @author C. Anastopoulos (Athena MT)
*/

class ITrackFitter : virtual public IAlgTool
{

public:

  static const InterfaceID& interfaceID() //!< the Tool's interface
  {
    return IID_ITrackFitter;
  }

  /*
   * Event context aware  methods
   * returning unique_ptr
   */

  /** Event context aware (Athena MT) RE-FIT A TRACK. */
  virtual std::unique_ptr<Track> fit(
    const EventContext& ctx,
    const Track& track,
    const RunOutlierRemoval runOutlier = false,
    const ParticleHypothesis matEffects = Trk::nonInteracting) const = 0;
  /** Event context aware (Athena MT) RE-FIT A TRACK, ADDING A PRD SET.
      this method will disintegrate the track back to PRD
      and call the fit(PRDset) method.
  */
  virtual std::unique_ptr<Track> fit(
    const EventContext& ctx,
    const Track& track,
    const PrepRawDataSet& prepRawSet,
    const RunOutlierRemoval runOutlier = false,
    const ParticleHypothesis matEffects = Trk::nonInteracting) const = 0;
  /** Event context aware  FIT A TRACK TO A SET OF PrepRawData.
     Main fit method. The TrackParameters is a first
     estimate for the track, represented close to the origin.
     Use-cases can be thought of that make it necessary
     to control toggling outlier removal and material effects not
     via job options (once-per-job) but at each call (toggle
     within event, large set of fast fit followed by one final full fit).
 */
  virtual std::unique_ptr<Track> fit(
    const EventContext& ctx,
    const PrepRawDataSet& prepRawSet,
    const TrackParameters& params,
    const RunOutlierRemoval runOutlier = false,
    const ParticleHypothesis matEffects = Trk::nonInteracting) const = 0;

  /** Event context aware RE-FIT A TRACK, ADDING A FITTABLE MEASUREMENT SET.
      this method will use the vector of measurements from the
      existing track and refit adding the new measurements. The code
      is in this class, but can be overwritten by inheriting classes.
  */
  virtual std::unique_ptr<Track> fit(
    const EventContext& ctx,
    const Track& track,
    const MeasurementSet& measSet,
    const RunOutlierRemoval runOutlier = false,
    const ParticleHypothesis matEffects = Trk::nonInteracting) const = 0;

  /** Event context aware FIT A TRACK TO A SET OF MEASUREMENTBASE.
      Main fit method. The TrackParameters is a first
      estimate for the track, represented close to the origin.
  */
  virtual std::unique_ptr<Track> fit(
    const EventContext& ctx,
    const MeasurementSet& measSet,
    const TrackParameters& params,
    const RunOutlierRemoval runOutlier = false,
    const ParticleHypothesis matEffects = Trk::nonInteracting) const = 0;

  /** Event context aware COMBINE TWO TRACKS BY REFITTING.
      Specifically designed for combined muon fits, allowing to extract
      extra informations (ID-exit & MS-entrance parameters, layers, Mefos)
      from already fitted tracks.
  */
  virtual std::unique_ptr<Track> fit(
    const EventContext& ctx,
    const Track& track1,
    const Track& track2,
    const RunOutlierRemoval runOutlier = false,
    const ParticleHypothesis matEffects = Trk::nonInteracting) const = 0;

  /** provides way of getting more detailed information about failing fits
   * than NULL track pointer */
  virtual FitterStatusCode statusCodeOfLastFit() const{
     return Trk::FitterStatusCode::Success;
  }

}; // end of class
} // end of namespace


#endif // TRK_ITRACKFITTER_H
