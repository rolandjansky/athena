/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* *******************************************************************************
			IForwardGsfFitter.h  -  description
			----------------------------------
created              : Thursday 8th January 2009
author               : amorley
email                : Anthony.Morley@cern.ch
decription           : Abstract interface for the forward GSF fitter
********************************************************************************** */

#ifndef TrkIForwardGsfFitter_H
#define TrkIForwardGsfFitter_H

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkParameters/TrackParameters.h"

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

class IMultiStateMeasurementUpdator;
class MultiComponentStateCombiner;
class IMultiStateExtrapolator;
class IRIO_OnTrackCreator;
class Surface;

static const InterfaceID InterfaceID_ForwardGsfFitter("ForwardGsfFitter", 1, 0);

class IForwardGsfFitter : virtual public IAlgTool {

 public:

  /** AlgTool interface method */
  static const InterfaceID& interfaceID()
    { return InterfaceID_ForwardGsfFitter; };

  /** Virtual destructor */
  virtual ~IForwardGsfFitter() {};


  /** Configure the forward GSF fitter
      - Configure the extrapolator
      - Configure the measurement updator
      - Configure the RIO_OnTrack creator */
  virtual StatusCode configureTools (const ToolHandle<Trk::IMultiStateExtrapolator> &,
			 const ToolHandle<Trk::IMultiStateMeasurementUpdator> &,
			 const ToolHandle<Trk::IRIO_OnTrackCreator> &) = 0;

  /** Forward GSF fit using PrepRawData */
  virtual const ForwardTrajectory* fitPRD ( const PrepRawDataSet&,
				    const TrackParameters&,
				    const ParticleHypothesis particleHypothesis = nonInteracting ) const = 0;

  /** Forward GSF fit using MeasurementSet */
  virtual const ForwardTrajectory* fitMeasurements ( const MeasurementSet&,
					     const TrackParameters&,
					     const ParticleHypothesis particleHypothesis = nonInteracting ) const = 0;

  /** The interface will later be extended so that the initial state can be additionally a MultiComponentState object! */
  
};

} // end Trk namespace

#endif
