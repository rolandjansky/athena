/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "TrkEventUtils/TrkParametersComparisonFunction.h" // is a typedef
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

namespace Trk {

  class Track;
  class IExtrapolator;           // Extrapolation Tool
  typedef bool SortInputFlag;    //!< switch to toggle sorting
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
    TrackFitInputPreparator(const Amg::Vector3D&);

    //! destructor
    ~TrackFitInputPreparator() = default;

    /** @brief fill a new track object from track+measurements using flags
        for sorting and outliers. This method is a factory, that is the
        client needs to take care of deleting the track. */
    static Trk::Track*     copyToTrack (const Track&,  const MeasurementSet&,
                                 const SortInputFlag, const bool) ;

    /** @brief get the MeasurementSet out of a track+measurements combination.

        IMPORTANT: take this interface if you know there are no outliers on
        the track! Uses flags for sorting and outliers. */
    static MeasurementSet  stripMeasurements (const Track&,  const MeasurementSet&,
                                       const SortInputFlag, const bool) ;

    /** @brief create a vector of PrepRawData* from the mixed input of track
        and single PrepRawData. Switch for applying a sort function as well
        as re-integration of outliers onto the input track. */
    static PrepRawDataSet  stripPrepRawData (const Track&, const PrepRawDataSet&,
                                      const SortInputFlag, const bool) ;

  private:
    /** @brief the sorting reference point for getting reference track
        parameters in those cases when the first measurement does not have
        TPs stored with it. */
    const Amg::Vector3D           m_sortingRefPoint;
    

  };

}


#endif // TRK_TRACKFITINPUTPREPARATOR_H
