/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// InDetDynamicNoiseAdjustment.h
//   Header file for Kalman Dynamic Noise Adjustor
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_INDETDYNAMICNOISEADJUSTMENT_H
#define TRK_INDETDYNAMICNOISEADJUSTMENT_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkFitterInterfaces/IDynamicNoiseAdjustor.h"

namespace Trk {
class CovarianceMatrix;
class DNA_MaterialEffects;

class InDetDynamicNoiseAdjustment
  : virtual public IDynamicNoiseAdjustor
  , public AthAlgTool
{
public:
  // standard AlgTool methods
  InDetDynamicNoiseAdjustment(const std::string&,
                              const std::string&,
                              const IInterface*);
  virtual ~InDetDynamicNoiseAdjustment();

  // standard Athena methods
  StatusCode initialize();
  StatusCode finalize();

  //! see IDynamicNoiseAdjustor
  virtual StatusCode configureWithTools(IExtrapolator*, const IUpdator*);

  //! see IDynamicNoiseAdjustor
  virtual const DNA_MaterialEffects* DNA_Adjust(
    Trk::IDynamicNoiseAdjustor::State& state,
    std::unique_ptr<const TrackParameters>& predPar,    // predicted into next layer
    std::unique_ptr<const TrackParameters>& updatedPar, // previously updated
    const MeasurementBase* fittableMeasurement,
    const KalmanMatEffectsController& matEff,
    PropDirection direction,
    const DNA_MaterialEffects* forwardMEF = nullptr) const;

private:
  //! numerical approximation for probability-mapping Bethe-Heitle to Gauss
  static double calculateDna(double xth, double c, double zest) ;

  //!< max value of new chi2 at minimum (generic use)
  double m_yminmax_generic; //!< max value of new chi2 at minimum (generic use)
  //!< max value of new chi2 at minimum (electric use)
  double m_yminmax_electron;
  //!< min allowed significance for brem (generic use)
  double m_signifmin_generic;
  //!< min allowed significance for brem (electron use)
  double m_signifmin_electron;
  //!< scale of dx in smearing (generic use)
  double m_lambdaxmin_generic;
  //!< scale of dx in smearing (electric use)
  double m_lambdaxmin_electron;
  //!< scale of sigma qOverP in smearing (generic use)
  double m_lambdaqop_generic;
  //!< scale of sigma qOverP in smearing (electric use)
  double m_lambdaqop_electron;

  // the tool pointers
  IExtrapolator* m_extrapolator; //!< the tool pointer for extrapolation
  const IUpdator* m_updator;     //!< the tool pointer for meas't updating
};

} // end of namespace

#endif // TRK_INDETDYNAMICNOISEADJUSTMENT_H
