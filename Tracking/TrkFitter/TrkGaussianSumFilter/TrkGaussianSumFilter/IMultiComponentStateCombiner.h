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

#include "GaudiKernel/IAlgTool.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include <memory>
namespace Trk {

static const InterfaceID IID_MultiComponentStateCombiner("MultiComponentStateCombiner", 1, 0);

class IMultiComponentStateCombiner : virtual public IAlgTool
{

public:
  /** Virtual destructor */
  virtual ~IMultiComponentStateCombiner(){};

  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_MultiComponentStateCombiner; };

  /** Calculate combined state of many components */
  virtual std::unique_ptr<Trk::TrackParameters> combine(const MultiComponentState&, bool useModeTemp = false) const = 0;

  /** Calculate combined state and weight of many components */
  virtual void combineWithWeight(Trk::ComponentParameters& mergeTo, const Trk::ComponentParameters& addThis) const = 0;

  /** Calculate combined state and weight of many components */
  virtual std::unique_ptr<Trk::ComponentParameters> combineWithWeight(const MultiComponentState&,
                                                                      bool useModeTemp = false) const = 0;
};

} // end Trk namespace

#endif
