/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IResidualPullCalculator.h
//   Header file for interface of ResidualPullCalculator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_IRESIDUALPULLCALCULATOR_H
#define TRK_IRESIDUALPULLCALCULATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h" // typedef
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkEventPrimitives/ResidualPull.h"

namespace Trk {
static const InterfaceID IID_IResidualPullCalculator("IResidualPullCalculator",1,0);

class MeasurementBase;
class AlignmentEffectsOnTrack;

/** @class IResidualPullCalculator
    @brief provides the interface for tools which calculate residuals and pulls.
    @author S. Fleischmann, W. Liebig
*/

class IResidualPullCalculator : virtual public IAlgTool {
public:
    /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();
    
    /** This function returns a Trk::ResidualPull object, which contains the values
     * of residual and pull for the given measurement and track state.
     * The pull calculation needs to have matching TrackParameters and ResidualType setting:
     * Unbiased track parameters (which can be calculated by the Trk::IUpdator) with 'Unbiased' type,
     * and biased track parameters (current hit included in fit) with 'Biased' or 'HitOnly' type.
     *
     * The function can determine the sub-detector type itself by using the ID helper,
     * or interpret the DetectorType enum from the method interface.
     */    
    virtual const Trk::ResidualPull* residualPull
      (const Trk::MeasurementBase*,
       const Trk::TrackParameters*,
       const Trk::ResidualPull::ResidualType residualType,
       const TrackState::MeasurementType = TrackState::unidentified) const = 0;
     
   /**This function returns (creates!) a Trk::ResidualPull object, which contains the values
   * of residual and pull for the given measurement and track state, and the Alignment effects.
   * The track state can be an unbiased one (which can be retrieved by the Trk::IUpdator),
   * a biased one (which contains the measurement),
   * or a truth state.
   * The enum residualTyp must be set according to this, otherwise the pulls will be wrong.
   * Residuals differ in all three cases; please be aware of this.
   *
   * This function determines the sub-detector type itself by using the ID helper*/
    virtual const Trk::ResidualPull* residualPull(
             const Trk::MeasurementBase* ,
             const Trk::TrackParameters* ,
             const Trk::ResidualPull::ResidualType ,
             const Trk::TrackState::MeasurementType, 
             const std::vector<const Trk::AlignmentEffectsOnTrack*>& ) const { return (Trk::ResidualPull*) 0; }

     /** This function is a light-weight version of the function above, designed for track fitters
      * where speed is critical. The user has to provide a std::vector of size 5, which gets 
      * filled with the residuals. If the size of the vector is not 5, the method will crash!
      */ 
    virtual void residuals
      (std::vector<double>&,
       const Trk::MeasurementBase*,
       const Trk::TrackParameters*,
       const Trk::ResidualPull::ResidualType residualType,
       const TrackState::MeasurementType = TrackState::unidentified) const = 0;
};

inline const InterfaceID& Trk::IResidualPullCalculator::interfaceID() {
    return IID_IResidualPullCalculator;
}

} // end of namespace

#endif // TRK_IRESIDUALPULLCALCULATOR_H
