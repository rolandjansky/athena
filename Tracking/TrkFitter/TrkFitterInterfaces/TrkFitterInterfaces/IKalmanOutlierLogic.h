/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanOutlierLogic.h
//   Interface header file for Kalman Outlier Logic
//////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig at cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_IKALMANOUTLIERLOGIC_H
#define TRK_IKALMANOUTLIERLOGIC_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"

namespace Trk {
	
  class IExtrapolator;            // Extrapolation Tool
  class IUpdator;                 // Interface for Estimator updating
  class IMeasurementRecalibrator; // interface to re-do ROT creation
  class FitQuality;               // track class FitQuality
  typedef std::vector<Trk::ProtoTrackStateOnSurface> Trajectory;

  static const InterfaceID InterfaceID_IKalmanOutlierLogic ("IKalmanOutlierLogic", 1, 0);

  /** @brief Interface for AlgTools performing a quality check of the track
      fit, and clean it by detecting and flaggin outliers.

      It performs an evaluation on the input Proto-TrackState vector,
      but would not execute by itself the subsequent refit on a reduced
      set of hits though, but would leave that to the calling client.

      @author Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
  */
  class IKalmanOutlierLogic : virtual public IAlgTool { 
  public:	
		
    virtual StatusCode configureWithTools(IExtrapolator*, const IUpdator*,
                                          const IMeasurementRecalibrator* ) = 0;

    /** applies logics to find outliers, if any it flags them on the trajectory and
        returns if outliers have been found and a refit is needed. */
    virtual bool flagNewOutliers(Trajectory&, const FitQuality&, int&,
                                       const int) const = 0; 

    /** determines if a track is to be rejected because of a bad fit,
        such that the outlier logic should be run. */
    virtual bool reject(const FitQuality&) const = 0;
		
  };
	
} // end of namespace

#endif // TRK_IKALMANOUTLIERLOGIC_H
