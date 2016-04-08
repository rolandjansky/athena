/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DAF_WeightCalculator.h
//   Header file for DAF WeightCalculator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_DAF_WEIGHTCALCULATOR_H
#define TRK_DAF_WEIGHTCALCULATOR_H

#include "TrkToolInterfaces/IWeightCalculator.h"
#include "AthenaBaseComps/AthAlgTool.h"
//#include "CLHEP/Matrix/Vector.h"
//#include "CLHEP/Matrix/Matrix.h"
#include "TrkParameters/TrackParameters.h"
// just needed for assignProb typedef till now:
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
namespace Trk {
  class RIO_OnTrack;

/**  Deterministic Annealing Filter implementation to calculate the assignmentProbabilities of
   competing measurements to a track.
 
   The following formulas are used to calculate the assignment probabilities, which are
   proportional to a multivariate Gaussian distribution:
   
   @f$ \phi_i = \frac{1}{(2\pi)^{\frac{n}{2}} \sqrt{\beta det V_i}} \exp  \left(  \frac{1}{2 \beta} (m_i - Hx)^T G_i (m_i - Hx)  \right) @f$
   (@f$ G = V^{-1} @f$)
   
   for the non-normalized probabilities (@f$ \beta @f$ is the so-called annealing factor)
   and
   
   @f$ p_i = \frac{\phi_i}{ \sum_j (\phi_j + \phi_j^{cut}) } @f$
   
   for the normalization where
   @f$ \phi_j^{cut} = \frac{1}{(2\pi)^{\frac{n}{2}} \sqrt{\beta det V_j}} \cdot \exp( \frac{1}{2} \frac{c}{\beta} )   @f$.
   
   @f$ n @f$ is the dimension of the measurement vector (localParameters of the ROT) and
   @f$ c @f$ is a cut-off parameter which might be detector dependend.
   
   The annealing factor @f$ \beta @f$ is needed to increase the variances of the measurements
   in the beginning of the annealing process and is lowered to 1 in this process.
 
 */

class DAF_WeightCalculator : virtual public IWeightCalculator, public AthAlgTool {
public:
    // standard AlgToolmethods
    DAF_WeightCalculator(const std::string&,const std::string&,const IInterface*);
    ~DAF_WeightCalculator();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    //    /** calculate the assignment probabilities (assignment weights) for a collection of measurements:
    //    if doNormalization==true the AssignmentProbs will be normalized using the given cutValue*/
    //    virtual const std::vector< AssignmentProb >* calculateWeights (
    //        const TrackParameters&,
    //        const std::vector< const RIO_OnTrack* >*,
    //        const double cutValue,
    //        const bool doNormalization=true ) const;
    /** calculate the assignment probabilities (assignment weights) for a collection of measurements:
    if doNormalization==true the AssignmentProbs will be normalized using the given cutValue
    use vector of TrackParameters to avoid transformations to local frame
    (TrackParameters must be given on the associated surface of the particular ROT)*/
    virtual const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >* calculateWeights (
        const std::vector< const TrackParameters* >*,
        const std::vector< const RIO_OnTrack* >*,
        const AnnealingFactor,
        const double cutValue,
        const bool doNormalization=true ) const;

    /** calculate the assignment probability (assignment weight) for a single measurement (no normalization can be done in this case)*/
    virtual Trk::CompetingRIOsOnTrack::AssignmentProb calculateWeight (
        const TrackParameters&,
        const RIO_OnTrack&,
        const AnnealingFactor ) const;

    /** normalize given assignment probabilities (assignment weights) using a given cutValue and annealing factor*/
    virtual const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >* normalize (
        const std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >*,
        const std::vector< const RIO_OnTrack* >*,
        const AnnealingFactor,
        const double cutValue ) const;
    /** normalize given assignment probabilities (assignment weights) using a given cutValue and annealing factor*/
    virtual void normalize (
        std::vector< Trk::CompetingRIOsOnTrack::AssignmentProb >&,
        const std::vector< const RIO_OnTrack* >*,
        const AnnealingFactor,
        const double cutValue ) const;
private:

};

} // end of namespace

#endif // TRK_DAF_WEIGHTCALCULATOR_H
