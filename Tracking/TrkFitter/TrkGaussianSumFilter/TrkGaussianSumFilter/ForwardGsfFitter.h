/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* *******************************************************************************
      ForwardGsfFitter.h  -  description
      ----------------------------------
begin                : Wednesday 9th March 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Class definition for the forward GSF fitter
********************************************************************************** */

#ifndef TrkForwardGsfFitter_H
#define TrkForwardGsfFitter_H

#include "TrkGaussianSumFilter/IForwardGsfFitter.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkParameters/TrackParameters.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace Trk {

class IMultiStateMeasurementUpdator;
class IMultiComponentStateCombiner;
class IMultiStateExtrapolator;
class IRIO_OnTrackCreator;
class Surface;

class ForwardGsfFitter : public AthAlgTool, virtual public IForwardGsfFitter {

 public:

  /** Constructor with AlgTool parameters */
  ForwardGsfFitter(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~ForwardGsfFitter() {};

  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();

  /** Configure the forward GSF fitter
      - Configure the extrapolator
      - Configure the measurement updator
      - Configure the RIO_OnTrack creator */
  virtual StatusCode configureTools ( const ToolHandle<Trk::IMultiStateExtrapolator> &,
       const ToolHandle<Trk::IMultiStateMeasurementUpdator> &,
       const ToolHandle<Trk::IRIO_OnTrackCreator> &);

  /** Forward GSF fit using PrepRawData */
  virtual const ForwardTrajectory* fitPRD ( const PrepRawDataSet&,
               const TrackParameters&,
               const ParticleHypothesis particleHypothesis = nonInteracting ) const;

  /** Forward GSF fit using MeasurementSet */
  virtual const ForwardTrajectory* fitMeasurements ( const MeasurementSet&,
               const TrackParameters&,
               const ParticleHypothesis particleHypothesis = nonInteracting ) const;

  /** The interface will later be extended so that the initial state can be additionally a MultiComponentState object! */

 private:

  /** Progress one step along the fit */
  bool stepForwardFit ( ForwardTrajectory*,
      const PrepRawData*,
      const MeasurementBase*,
      const Surface&,
      const MultiComponentState*&,
      const ParticleHypothesis particleHypothesis = nonInteracting ) const;

 private:
  ToolHandle<IMultiStateExtrapolator>       m_extrapolator;
  ToolHandle<IMultiStateMeasurementUpdator> m_updator;
  ToolHandle<IRIO_OnTrackCreator>           m_rioOnTrackCreator;
  ToolHandle<IMultiComponentStateCombiner>  m_stateCombiner;
  double                                    m_cutChiSquaredPerNumberDOF;

  bool                                      m_overideMaterialEffectsSwitch;
  int                                       m_overideMaterialEffects;

  ParticleHypothesis                        m_overideParticleHypothesis;
  
};

} // end Trk namespace

#endif
