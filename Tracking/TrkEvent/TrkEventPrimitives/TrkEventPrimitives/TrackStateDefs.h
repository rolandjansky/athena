/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackStateDefs.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_TRACKSTATEDEFS_H
#define TRKEVENTPRIMITIVES_TRACKSTATEDEFS_H

namespace Trk {
  namespace TrackState {

    /** @enum MeasurementType
        @brief enum describing the flavour of MeasurementBase

        This enum is for use in interfaces, track fitting logic and
        caching of idHelper calls (like is_pixel).
        Note that it is not an official part of the MeasurementBase
        EDM, therefore the decisive identification remains with the
        Tracking EDM inheritance structure and the hit identifiers.

        @author S. Fleischmann, W. Liebig */

    enum MeasurementType {
        unidentified = 0,
        Pixel      = 1,
        SCT        = 2,
        TRT        = 3,
        MDT        = 4,
        CSC        = 5,
        RPC        = 6,
        TGC        = 7,
        Pseudo     = 8,
        Vertex     = 9,
        Segment    = 10,
        SpacePoint = 11,
        LArCal     = 12,
        TileCal    = 13,
        STGC       = 14,
        MM         = 15,
        NumberOfMeasurementTypes=16
    };

    /** @enum TrackStateType
        @brief enum describing the role of track states during cleaning
               and outlier removal.
        @author W. Liebig */
    
    enum TrackStateType {
      AnyState        = 0, //!< fittable or outlier
      Fittable        = 1, //!< normal MeasurementBase
      ExternalOutlier = 2, //!< input outlier from pattern or previous fit
      GeneralOutlier  = 3, //!< outlier for unspecified reason
      StateOutlier    = 4, //!< standard cut on pull or smoothed state Chi2
      AssProbOutlier  = 4, //!< cut on hit assignment probability (also state)
      TrackOutlier    = 5, //!< selected outlier to enhance track probability
      PredictedOutlier= 6, //!< determined from fit result without this state
      SensorOutlier   = 7, //!< track prediction is signif'ly outside sensor
      FilterOutlier   = 8, //!< progressive fit: removed to stabilise fit
      MissedOutlier   = 9, //!< progressive fit: missed by extrapolation
      DuplicateOutlier=10, //!< same hit is already on track, should not happen!
      Hole            =11, //!< a hole on track, or a generic surface on track
      Scatterer       =12, //!< a scatterer, i.e. a surface where the track direction changes
      Brem            =13, //!< a brem, i.e. a surface where the momentum changes
      NumberOfStateTypes=14
    };

  }
}
#endif
