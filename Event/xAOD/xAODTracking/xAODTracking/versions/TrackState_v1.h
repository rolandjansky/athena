/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_TRACKSTATE_V1_H
#define XAODTRACKING_VERSIONS_TRACKSTATE_V1_H
#include <cstdint>
#include "AthContainers/AuxElement.h"
namespace xAOD {
    using TrackStateIndexType=uint32_t; // TODO move this declaration to one location
    /**
     * @brief class describing TrackState - backend of navigation object of Acts::MultiTrajectory
     */
    class TrackState_v1 : public SG::AuxElement {
        /**
         * @brief get chi2 of this track state
         */
        double chi2() const;

        /**
         * @brief Set the Chi2 value of this track state
         */
        void setChi2(double);

        /**
         * @brief get path length of this track state
         */
        double pathLength() const;
        /**
         * @brief set path length of this track state
         */
        void setPathLength(double);


        /**
         * @brief set the index of preceding TrackState 
         * TrackStates in the collection from multitude of short one-directional linked lists pointing from state to an earlier state.
         * This index allows to traverse backwards a list to which this TrackState belongs.
         * Maximum possible value indicates that this is the inital TrackState (terminal node of linked-list)
         * @return index, invalid if identical to max possible value of this type
         */
        TrackStateIndexType previous() const;

        /**
         * @brief Set the Previous index
         * @see previous() for explanation
         */
        void setPrevious(TrackStateIndexType);

        /**
         * @brief index in the TrackParametersContainer corresponding to this TrackStates container
         * It points to the parameter predicted parameters value (by KF) 
         * @return index, invalid if identical to max possible value of this type
         */
        TrackStateIndexType predicted() const; 

        /**
         * @brief Set the Predicted index
         * @see predicted() method for explanation
         */
        void setPredicted(TrackStateIndexType);

        /**
         * @brief index in the TrackParametersContainer corresponding to this TrackStates container
         * Points to filtered parameters
         * @return index, invalid if identical to max possible value of this type
         */
        TrackStateIndexType filtered() const; 

        /**
         * @brief Set the Filtered index
         * @see filtered() method for explanation
         */
        void setFiltered(TrackStateIndexType);

        /**
         * @brief index in the TrackParametersContainer corresponding to this TrackStates container
         * Points to the smoothed parameters
         * @return index, invalid if identical to max possible value of this type
         */
        TrackStateIndexType smoothed() const; 

        /**
         * @brief Set the Smoothed index
         * @see smoothed() method for explanation
         */
        void setSmoothed(TrackStateIndexType);

        /**
         * @brief index in the TrackJacobianContainer corresponding to this TrackStates container
         * 
         * @return index, invalid if identical to max possible value of this type
         */
        TrackStateIndexType jacobian() const; 

        /**
         * @brief Set the Jacobian index
         * @see jacobian() method for explanation
         */
        void setJacobian(TrackStateIndexType);

        /**
         * @brief index in TrackMeasurementContainer corresponding to this TracksTate
         * 
         * @return index, invalid if identical to max possible value of this type
         */
        TrackStateIndexType projector() const; 
        /**
         * @brief Set the Projector index
         * @see projector()
         */        
        void setProjector(TrackStateIndexType);

        /**
         * @brief index in TrackMeasurementContainer corresponding to this TracksTate
         * Points to uncalibrated measurement
         * @return index, invalid if identical to max possible value of this type
         */
        TrackStateIndexType uncalibrated() const;
        /**
         * @brief Set the Uncalibrated index
         * @see uncalibrated() method
         */
        void setUncalibrated(TrackStateIndexType);

        /**
         * @brief index in TrackMeasurementContainer corresponding to this TracksTate
         * Points to calibrated measurement
         * @return index, invalid if identical to max possible value of this type
         */
        TrackStateIndexType calibrated() const; 
        /**
         * @brief Set the Calibrated index
         * @see calibrated() method
         */
        void setCalibrated(TrackStateIndexType);

        /**
         * @brief measurement dimensions
         * 
         * @return TrackStateIndexType 
         */
        TrackStateIndexType measDim() const; 
        /**
         * @brief Set the measurement dimensions
         */
        void setMeasDim(TrackStateIndexType);


    };
}


#endif