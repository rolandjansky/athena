/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
			IComponentSeparationDistance.h  -  description
			----------------------------------------------
begin                : Thursday 24th February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Interface class for determining the distance between 
                       components. The actual metrics for determining this are
                       given by the derived classes.
*********************************************************************************/

#ifndef TrkIComponentSeparationDistance_H
#define TrkIComponentSeparationDistance_H

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "GaudiKernel/IAlgTool.h"

namespace Trk{

static const InterfaceID IID_IComponentSeparationDistance("IComponentSeparationDistance", 1, 0);

class IComponentSeparationDistance : virtual public IAlgTool {

 public:
  /** IAlgTool and AlgTool interface method */
  static const InterfaceID interfaceID() { return IID_IComponentSeparationDistance; };
  
  /** Virtual destructor */
  virtual ~IComponentSeparationDistance() {};

  /** Interface method for calculating the "distance" between 2 components */
  virtual double operator() (const ComponentParameters&, const ComponentParameters&) const = 0;
  
};

} // end Trk namespace

#endif
