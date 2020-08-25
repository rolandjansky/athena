/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ProtoTrajectoryUtility, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_PROTOTRAJECTORYUTILITY_H
#define TRK_PROTOTRAJECTORYUTILITY_H

//Trk
#include "TrkParameters/TrackParameters.h"
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"
#include "CxxUtils/checker_macros.h"

class AtlasDetectorID;            // to identify measurements
namespace Trk {

  typedef std::vector<Trk::ProtoTrackStateOnSurface> Trajectory;

  /** @brief class to provide operations on vectors of ProtoTrackStateOnSurface

      @author Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
    */

  class ATLAS_NOT_THREAD_SAFE ProtoTrajectoryUtility {
      
  public:
    //! Default constructor
    ProtoTrajectoryUtility();
      
    //! constructor with id-helper. Expects that helper remains with client
    ProtoTrajectoryUtility(const AtlasDetectorID*&);

    //! Virtual destructor
    ~ProtoTrajectoryUtility(){}

    /** @brief method returning the first measurement (non-outlier) element
        from a trajectory. Gives end() if none found. */
    Trajectory::iterator firstFittableState  (Trajectory&) const;

    /** @brief method returning the last measurement (non-outlier) element
        from a trajectory. Gives end() if none found. */
    Trajectory::iterator lastFittableState   (Trajectory&) const;

    /** @brief method returning the previous measurement (non-outlier)
        element from a trajectory. Gives end() if not found. */
    Trajectory::iterator         previousFittableState(const Trajectory&,
                                                       Trajectory::iterator) const;

    /** @brief method returning the next measurement (non-outlier) element
        from a trajectory as the previous element of the reversely iterated
        trajectory. Gives rend() if not found. */
    Trajectory::reverse_iterator previousFittableState(const Trajectory&,
                                                       Trajectory::reverse_iterator) const;

    /** @brief method returning the next measurement (non-outlier) element
        from a trajectory. Gives end() if not found. */
    Trajectory::iterator         nextFittableState    (const Trajectory&,
                                                       Trajectory::iterator) const;

    /** @brief method returning the previous measurement (non-outlier)
        element from a trajectory as the next element of the reversely
        iterated trajectory. Gives rend() if not found. */
    Trajectory::reverse_iterator nextFittableState    (const Trajectory&,
                                                       Trajectory::reverse_iterator) const;

    //! method returning the number of measurements up to element number i.
    int            numberOfMeasurements(const Trajectory&, int i=0) const;
    

    //! method returning the number of measurements x their rank.
    int            rankedNumberOfMeasurements(const Trajectory&) const;

    //! method returning the number of outliers up to element number i.
    int            numberOfOutliers(const Trajectory&, int i=0) const;

    //! method returning the number of new outliers up to element number i.
    int            numberOfNewOutliers(const Trajectory&, int i=0) const;

    //! method returning the number of new outliers for detector, type and iteration
    int            numberOfSpecificStates
      (const Trajectory&,
       const TrackState::MeasurementType& mt=TrackState::unidentified,
       const TrackState::TrackStateType&  tt=TrackState::AnyState,
       const int&             it=0) const;

    //! returns true if a DNA_Mat'Eff' object is contained in the actual trajectory 
    bool           trajectoryHasMefot(const Trajectory&) const;

    /** @brief method to remove all fit results but keep all measurements, material
        layers, reference parameters and jacobians.
        The states 1, ... firststate-1 keep their forwardParameters so that the
        can resume the filtering procedure from there. */
    void           clearFitResultsAfterOutlier(Trajectory&, FitQuality*&,
                                               int firststate=1) const;

    /** @brief method to remove all fit results, the reference track with jacobians
         but keep measurements+material as they define the trajectory. The full
         removal can be done with a simple .clear() instead. Again the states
         1...(firstate-1) keep the reference and forward trajectories. */
    void           clearFitResultsAndReference(Trajectory&, FitQuality*&,
                                               int firststate=1) const;

    /** @brief put all 'new outliers' back to measurements.

        if argument outlierState is >0, then apart from the state number N
    */
    void                 defineMeasurementsExceptThis(Trajectory&, int outlierState=0) const;

    /** @brief sum up FQoS values from forward filter */
    FitQuality           forwardFilterQuality(const Trajectory&,
      Trk::TrackState::MeasurementType=Trk::TrackState::unidentified) const;

    /** @brief make a screen output of the trajectory contents. Uses the
        AtlasIdHelper to print the hit type in readable form. */
    void                 dumpTrajectory             (const Trajectory&, const std::string&) const;

    /** @brief set the detector/measurement types of each state.
        Needs an AtlasIdHelper */
    void                 identifyMeasurement        (ProtoTrackStateOnSurface&) const;

    /** @brief set the detector/measurement types of each state.
        Needs an AtlasIdHelper */
    void                 identifyMeasurements       (Trajectory&) const;

  private:
    const AtlasDetectorID*           m_idHelper;

  };

}


#endif // TRK_PROTOTRAJECTORYUTILITY_H
