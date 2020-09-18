/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackFitInputPreparator, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRACKFITINPUTPREPARATOR_H
#define TRK_TRACKFITINPUTPREPARATOR_H

//Trk
#include "GaudiKernel/StatusCode.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"
#include "TrkEventUtils/TrkParametersComparisonFunction.h" // is a typedef
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

namespace Trk {

  class Track;
  class IExtrapolator;           // Extrapolation Tool
  typedef bool SortInputFlag;    //!< switch to toggle sorting
  typedef std::vector<Trk::ProtoTrackStateOnSurface> Trajectory;
  typedef DataVector<const TrackStateOnSurface>::const_iterator TS_iterator;
  typedef std::vector<std::pair<const Trk::MeasurementBase*, int> > MB_IndexVector;

  /** @brief Helper class to transform combinations of tracking input
      (for example, existing tracks plus additional measurements) into
      a digestable fitter input: either tracks, measurement-sets or
      proto-state vectors.

      @author Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
    */

  class TrackFitInputPreparator {
    // forbid copying    
    TrackFitInputPreparator(const TrackFitInputPreparator &a) = delete ;
    TrackFitInputPreparator &operator=(const TrackFitInputPreparator &a) = delete ;
      
  public:
    //! Default constructor
    TrackFitInputPreparator();
      
    //! constructor with non-zero sorting reference point.
    TrackFitInputPreparator(const Amg::Vector3D&, const IExtrapolator* extrapolator=0);

    //! destructor
    ~TrackFitInputPreparator();

    /** @brief fill KF-internal trajectory from input Track and determine
        the reference parameters. Optionally sort and reset the outlier
        flags. */
    StatusCode copyToTrajectory (Trajectory&, const TrackParameters*&,
                                 const Track&, const SortInputFlag,
                                 const bool,
                                 const ParticleHypothesis&  partHypo ) const;

    /** @brief fill KF-internal trajectory from input Track and an additional
        vector of measurements. 

        Also determines the reference parameters. Optionally sort and reset
        the outlier flags. */
    StatusCode copyToTrajectory (Trajectory&, const TrackParameters*&,
                                 const Track&, const MeasurementSet&,
                                 const SortInputFlag, const bool,
                                 const ParticleHypothesis&  partHypo) const;

    /** @brief fill KF-internal trajectory from two input Tracks and determine
        the reference parameters. Optionally sort and reset the outlier flags. */
    StatusCode copyToTrajectory (Trajectory&, const TrackParameters*&,
                                 const Track&, const Track&,
                                 const SortInputFlag, const bool,
                                 const ParticleHypothesis&  partHypo
                                 ) const;

    /** @brief fill a new track object from track+measurements using flags
        for sorting and outliers. This method is a factory, that is the
        client needs to take care of deleting the track. */
    Trk::Track*     copyToTrack (const Track&,  const MeasurementSet&,
                                 const SortInputFlag, const bool) const;

    /** @brief get the MeasurementSet out of a track+measurements combination.

        IMPORTANT: take this interface if you know there are no outliers on
        the track! Uses flags for sorting and outliers. */
    MeasurementSet  stripMeasurements (const Track&,  const MeasurementSet&,
                                       const SortInputFlag, const bool) const;

    /** @brief create a vector of PrepRawData* from the mixed input of track
        and single PrepRawData. Switch for applying a sort function as well
        as re-integration of outliers onto the input track. */
    PrepRawDataSet  stripPrepRawData (const Track&, const PrepRawDataSet&,
                                      const SortInputFlag, const bool) const;

  private:
    /** @brief the sorting reference point for getting reference track
        parameters in those cases when the first measurement does not have
        TPs stored with it. */
    const Amg::Vector3D           m_sortingRefPoint;
    TrkParametersComparisonFunction*    m_TP_ComparisonFunction;
    
    const IExtrapolator*          m_extrapolator;

    void insertStateIntoTrajectory(Trajectory&, const MeasurementBase*,
                             bool, bool, const TrackParameters*,
                             const ParticleHypothesis&  partHypo) const;
  };

}


#endif // TRK_TRACKFITINPUTPREPARATOR_H
