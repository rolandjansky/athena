/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
			PosteriorWeightsCalculator.h  -  description
			---------------------------------------------
created              : Wednesday 22nd December 2004
author               : atkinson
email                : Tom.Atkinson@cern.ch
description          : Class to calculate the weighting of state components
			after an update from a measurement. The information for
			the measurement is provided through the RIO_OnTrack
			object.
*********************************************************************************/

#ifndef PosteriorWeightsCalculator_H
#define PosteriorWeightsCalculator_H

#include "TrkGaussianSumFilter/IPosteriorWeightsCalculator.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "AthenaBaseComps/AthAlgTool.h"

namespace Trk {

class MeasurementBase;

class PosteriorWeightsCalculator : public AthAlgTool, virtual public IPosteriorWeightsCalculator {
 public:
  
  /** Constructor with AlgTool parameters */
  PosteriorWeightsCalculator (const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~PosteriorWeightsCalculator () {};

  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();

  /** Method to compute the state weights after measurement update */
  virtual const MultiComponentState* weights( const MultiComponentState&, const MeasurementBase& );
 
};

} // end Trk namespace

#endif
