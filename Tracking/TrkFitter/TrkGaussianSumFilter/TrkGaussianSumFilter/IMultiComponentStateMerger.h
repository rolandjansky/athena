/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
                        IMultiComponentStateMerger.h  -  description
                        -------------------------------------------
begin                : Wednesday 23rd February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Abstract base class for component reduction tools for the
                       gaussian-sum filter
*********************************************************************************/

#ifndef TrkIMultiComponentStateMerger_H
#define TrkIMultiComponentStateMerger_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

namespace Trk {

static const InterfaceID IID_IMultiComponentStateMerger("IMultiComponentStateMerger", 1, 0);

class IMultiComponentStateMerger : virtual public IAlgTool
{

public:
  /** AlgTool interface method */
  static const InterfaceID& interfaceID() { return IID_IMultiComponentStateMerger; };

  /** Abstract base class virtual destructor */
  virtual ~IMultiComponentStateMerger() = default;

  /** Method for merging components - ownership of objects is passed */
  virtual MultiComponentState merge(Trk::MultiComponentState) const = 0;
};

} // end Trk namespace

#endif
