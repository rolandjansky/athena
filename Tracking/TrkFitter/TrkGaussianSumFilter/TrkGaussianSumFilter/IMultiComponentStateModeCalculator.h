/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************************
      IMultiComponentStateModeCalculator.h  -  description
      ----------------------------------------------------
begin                : Thursday 6th July 2006
author               : atkinson
email                : Tom.Atkinson@cern.ch
description          : Interface class for AlgTools used to determine the mode (q/p) 
                       of a gaussian mixture
***************************************************************************************/

#ifndef Trk_IMultiComponentStateModeCalculator_H
#define Trk_IMultiComponentStateModeCalculator_H

//#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/IAlgTool.h"

namespace Trk{

class MultiComponentState;

//!< Interface ID for IMultiComponentStateModeCalculator
static const InterfaceID IID_IMultiComponentStateModeCalculator( "IMultiComponentStateModeCalculator", 1, 0 );

class IMultiComponentStateModeCalculator : virtual public IAlgTool{

 public:

  //!< Virtual destructor
  virtual ~IMultiComponentStateModeCalculator(){};

  //!< IAlgTool-AlgTool interface
  static const InterfaceID& interfaceID(){ return IID_IMultiComponentStateModeCalculator; };

  //!< Public method to calculate the mode of a multi-component state
  virtual Amg::VectorX calculateMode( const MultiComponentState& ) const = 0;

};

} // end Trk namespace

#endif
