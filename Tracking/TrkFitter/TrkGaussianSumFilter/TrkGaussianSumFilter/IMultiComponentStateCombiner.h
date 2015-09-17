/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************************************
      IMultiComponentStateCombiner.h  -  description
      -----------------------------------------------
created              : Thursday 8th January 2009
author               : amorley
email                : Anthony.Morley@cern.ch
decription           : Abstract interface for the Multi Component State Combiner 
*******************************************************************************/

#ifndef IMultiComponentStateCombiner_H
#define IMultiComponentStateCombiner_H

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "TrkGaussianSumFilter/IMultiComponentStateModeCalculator.h"

#include "GaudiKernel/IAlgTool.h"

namespace Trk {

static const InterfaceID IID_MultiComponentStateCombiner("MultiComponentStateCombiner", 1, 0);

class IMultiComponentStateCombiner : virtual public IAlgTool {

  public:
  
  /** Virtual destructor */
  virtual ~IMultiComponentStateCombiner () {};

  /** AlgTool interface methods */
  static const InterfaceID& interfaceID () { return IID_MultiComponentStateCombiner; };
  
  /** Calculate combined state of many components */
  virtual const TrackParameters* combine( const MultiComponentState&, bool useModeTemp = false) const = 0;

  /** Calculate combined state and weight of many components */
  virtual const ComponentParameters* combineWithWeight( const MultiComponentState&, bool useModeTemp = false ) const = 0;
  
  /** */
  virtual void useMode( bool ) = 0;
};

} // end Trk namespace

#endif
