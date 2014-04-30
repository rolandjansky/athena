/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IWeightCalculator.h
//   Header file for interface of WeightCalculator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_IWEIGHTCALCULATOR_H
#define TRK_IWEIGHTCALCULATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h" // typedef

// just needed for assignProb typedef till now:
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
// may result in circular dependency :-(
#include <vector>

namespace Trk {
static const InterfaceID IID_IWeightCalculator("IWeightCalculator",1,0);

class RIO_OnTrack;         //!< RIO_OnTrack base
//typedef AssignmentProb;    //!< assignment probabilities


/** @class IWeightCalculator
    provides the interface to encapsulate the calculations of assignmentProbabilities of 
    competing measurements to a track.
    These calculations are needed in fitters such as the
    Deterministic Annealing Filter.
*/

class IWeightCalculator : virtual public IAlgTool {
public:
    /** Type def of annealing factor: factor to increase the variances of measurements, to be lowered in the annealing process */
    typedef double AnnealingFactor; 

    /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();

//    /** calculate the assignment probabilities (assignment weights) for a collection of measurements:
//    if doNormalization==true the AssignmentProbs will be normalized using the given cutValue*/
//    virtual const std::vector< AssignmentProb >* calculateWeights (
//        const TrackParameters&,
//        const std::vector< const RIO_OnTrack* >*,
//        const double cutValue,
//        const bool doNormalization=true ) const = 0;
    /** calculate the assignment probabilities (assignment weights) for a collection of measurements:
    if doNormalization==true the AssignmentProbs will be normalized using the given cutValue
    use vector of TrackParameters to avoid transformations to local frame (TrackParameters must
    be given on the associated surface of the particular ROT)*/
    virtual const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >* calculateWeights (
        const std::vector< const TrackParameters* >*,
        const std::vector< const RIO_OnTrack* >*,
        const AnnealingFactor,
        const double cutValue,
        const bool doNormalization=true ) const = 0;

    /** calculate the assignment probability (assignment weight) for a single measurement (no normalization can be done in this case)*/
    virtual Trk::CompetingRIOsOnTrack::AssignmentProb calculateWeight (
        const TrackParameters&,
        const RIO_OnTrack&,
        const AnnealingFactor ) const = 0;

    /** normalize given assignment probabilities (assignment weights) using a given cutValue*/
    virtual const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >* normalize (
        const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >*,
        const std::vector< const RIO_OnTrack* >*,
        const AnnealingFactor,
        const double cutValue ) const = 0;
    /** normalize given assignment probabilities (assignment weights) using a given cutValue*/
    virtual void normalize (
        std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >&,
        const std::vector< const RIO_OnTrack* >*,
        const AnnealingFactor,
        const double cutValue ) const = 0;

};

inline const InterfaceID& Trk::IWeightCalculator::interfaceID() {
    return IID_IWeightCalculator;
}

} // end of namespace

#endif // TRK_IWEIGHTCALCULATOR_H
