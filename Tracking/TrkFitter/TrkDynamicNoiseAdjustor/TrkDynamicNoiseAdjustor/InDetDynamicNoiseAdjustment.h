/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// InDetDynamicNoiseAdjustment.h
//   Header file for Kalman Dynamic Noise Adjustor
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_INDETDYNAMICNOISEADJUSTMENT_H
#define TRK_INDETDYNAMICNOISEADJUSTMENT_H

#include "TrkFitterInterfaces/IDynamicNoiseAdjustor.h"
#include "AthenaBaseComps/AthAlgTool.h"


namespace Trk {
  class CovarianceMatrix;
  class DNA_MaterialEffects;

  class InDetDynamicNoiseAdjustment : virtual public IDynamicNoiseAdjustor, public AthAlgTool {
public:

    enum DnaStrategy {
      unidentifiedStrategy   = 0,
      regularSuccessfulDNA   = 1000, //!< regular successful DNA activity, no special treatment
      backwardPixelTreatment = 1500, //!< special treatment for pixels, only in backward direction
      noGoodMinimum          = 2000, //!< no good minimum, possible special treatment
      geomCutsNotSatisfied   = 2003, //!< geometric cuts not satisfied. Mostly TRT, needs special treatment
      undevelopedTrack       = 2004 //!< low chi2, no canonical DNA needed, but may need some special treatment
    };
       

     // standard AlgTool methods
     InDetDynamicNoiseAdjustment(const std::string&,const std::string&,const IInterface*);
     virtual ~InDetDynamicNoiseAdjustment();

     // standard Athena methods
     StatusCode initialize();
     StatusCode finalize();

     //! see IDynamicNoiseAdjustor
     virtual StatusCode configureWithTools(IExtrapolator*, const IUpdator*);

     //! see IDynamicNoiseAdjustor
     virtual const DNA_MaterialEffects* DNA_Adjust(const TrackParameters*&,
                                                   const TrackParameters*&,
                                                   const MeasurementBase*,
                                                   const KalmanMatEffectsController&,
                                                   PropDirection,
                                                   const DNA_MaterialEffects* = 0) const;

private:

    //! numerical approximation for probability-mapping Bethe-Heitle to Gauss
    double calculateDna(double, double, double) const;

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
    double            m_yminmax_generic;   //!< max value of new chi2 at minimum (generic use)
    double            m_yminmax_electron;  //!< max value of new chi2 at minimum (electric use)

    double            m_signifmin_generic; //!< min allowed significance for brem (generic use)
    double            m_signifmin_electron;//!< min allowed significance for brem (electric use)

    double            m_lambdaxmin_generic;//!< scale of dx in smearing (generic use)
    double            m_lambdaxmin_electron;//!< scale of dx in smearing (electric use)

    double            m_lambdaqop_generic; //!< scale of sigma qOverP in smearing (generic use)
    double            m_lambdaqop_electron;//!< scale of sigma qOverP in smearing (electric use)

    mutable DnaStrategy m_currentDnaStrategy;//!< the chosen dna strategy depending on detector and chi2

    // the tool pointers
    IExtrapolator*                m_extrapolator;           //!< the tool pointer for extrapolation
    const IUpdator*               m_updator;                //!< the tool pointer for meas't updating


};

} // end of namespace

#endif // TRK_INDETDYNAMICNOISEADJUSTMENT_H
