/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ResidualPullCalculator.h
//   Header file for ResidualPullCalculator tool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_RESIDUALPULLCALCULATOR_H
#define TRK_RESIDUALPULLCALCULATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


#include "TrkToolInterfaces/IResidualPullCalculator.h"


class AtlasDetectorID;

namespace Trk {

  class MeasurementBase;
  class AlignmentEffectsOnTrack;

/** @brief AlgTool to calculate the residual and pull of a measurement and the
    related track state independently of the detector type.

    The Trk::ResidualPullCalculator is an AlgTool to calculate the residual
    and pull of a measurement and the related track state independent of the
    detector type. For sub-detectors like the SCT which need more attention
    a special tool in the detector-realm is called (to avoid dependencies on
    sub-detector code; only runtime-dependencies are created).
 
    The residuals and pulls are calculated according to the formulas given in
    the description of Trk::ResidualPull.
 */

class ResidualPullCalculator : virtual public Trk::IResidualPullCalculator, public AthAlgTool {
public:

    ResidualPullCalculator(const std::string& type, const std::string& name, const IInterface* parent);
    ~ResidualPullCalculator() {}
    virtual StatusCode initialize();
    virtual StatusCode finalize();


    /** This function returns (creates!) a Trk::ResidualPull object, which contains the values
     * of residual and pull for the given measurement and track state.
     * The track state can be an unbiased one (which can be retrieved by the Trk::IUpdator),
     * a biased one (which contains the measurement),
     * or a truth state.
     * The enum residualTyp must be set according to this, otherwise the pulls will be wrong.
     * Residuals differ in all three cases; please be aware of this.
     *
     * This function determines the sub-detector type itself by using the ID helper
     */
    virtual const Trk::ResidualPull* residualPull(
                        const Trk::MeasurementBase* measurement,
                        const Trk::TrackParameters* trkPar,
                        const Trk::ResidualPull::ResidualType resType,
                        const Trk::TrackState::MeasurementType) const;

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
                          const Trk::MeasurementBase* measurement,
                          const Trk::TrackParameters* trkPar,
                          const Trk::ResidualPull::ResidualType resType,
                          const Trk::TrackState::MeasurementType, 
                          const std::vector<const Trk::AlignmentEffectsOnTrack*>& ) const;

    /** This function is a light-weight version of the function above, designed for track fitters
     * where speed is critical. The user has to provide a std::vector of size 5, which gets
     * filled with the residuals.
     */
    virtual void residuals(
                        std::vector<double>&,
                        const Trk::MeasurementBase* measurement,
                        const Trk::TrackParameters* trkPar,
                        const Trk::ResidualPull::ResidualType resType,
                        const Trk::TrackState::MeasurementType) const;


private:
    ToolHandle<IResidualPullCalculator> m_SCTresidualTool; //!< the ResidualPullCalculator for the SCT
    ToolHandle<IResidualPullCalculator> m_RPCresidualTool; //!< the ResidualPullCalculator for the RPC
    ToolHandle<IResidualPullCalculator> m_TGCresidualTool; //!< the ResidualPullCalculator for the TGC
    const AtlasDetectorID*              m_idHelper;        //!< Used to know the sub-det from PRD->identify().

    double calcPull(
                const double residual,
                const double locMesCov,
                const double locTrkCov,
                const Trk::ResidualPull::ResidualType& resType ) const;
}; // end class
} // end namespace Trk

#endif //TRK_RESIDUALPULLCALCULATOR_H
