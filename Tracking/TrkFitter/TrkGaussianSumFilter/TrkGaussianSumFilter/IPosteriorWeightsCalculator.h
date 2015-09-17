/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
			IPosteriorWeightsCalculator.h  -  description
			---------------------------------------------
created              : Thursday 8th January 2009
author               : amorley
email                : Anthony.Morley@cern.ch
decription           : Abstract interface for the posterior weights calculator
*********************************************************************************/

#ifndef IPosteriorWeightsCalculator_H
#define IPosteriorWeightsCalculator_H

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "GaudiKernel/IAlgTool.h"

namespace Trk {

class MeasurementBase;

static const InterfaceID IID_PosteriorWeightsCalculator("PosteriorWeightsCalculator", 1, 0);

class IPosteriorWeightsCalculator : virtual public IAlgTool {
 public:
  
  /** Virtual destructor */
  virtual ~IPosteriorWeightsCalculator () {};

  /** AlgTool interface methods */
  static const InterfaceID& interfaceID () { return IID_PosteriorWeightsCalculator; };

  /** Method to compute the state weights after measurement update */
  virtual const MultiComponentState* weights( const MultiComponentState&, const MeasurementBase& ) = 0;
 
};

} // end Trk namespace

#endif
