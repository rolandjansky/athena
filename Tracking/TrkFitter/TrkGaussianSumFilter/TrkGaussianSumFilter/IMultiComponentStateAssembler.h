/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                        IMultiComponentStateAssembler.h  -  description
                        -----------------------------------------------
created             : Thursday 8th January 2009
author               : amorley
email                : Anthony.Morley@cern.ch
decription           : Abstract interface for Multi Component State Assembler
***************************************************************************/

#ifndef IMultiComponentStateAssembler_H
#define IMultiComponentStateAssembler_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include <memory>
#include <vector>

namespace Trk {

class SortByLargerSimpleComponentWeight
{
public:
  SortByLargerSimpleComponentWeight(){};

  bool operator()(const SimpleComponentParameters& firstComponent,
                  const SimpleComponentParameters& secondComponent) const

  {
    return firstComponent.second > secondComponent.second;
  }
};

static const InterfaceID IID_MultiComponentStateAssembler("MultiComponentStateAssembler", 1, 0);
class IMultiComponentStateAssembler : virtual public IAlgTool
{
public:
  /** Virtual destructor */
  virtual ~IMultiComponentStateAssembler(){};

  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_MultiComponentStateAssembler; };

  struct Cache
  {
    Cache()
      : validWeightSum{ 0 }
      , invalidWeightSum{ 0 }
      , multiComponentState{}
      , assemblyDone{ false }
    {
      multiComponentState.reserve(72);
    }
    double validWeightSum;
    double invalidWeightSum;
    SimpleMultiComponentState multiComponentState;
    bool assemblyDone;
  };
  /** Resets the AlgTool */
  virtual void  reset(Cache& cache) const = 0;

  /** Print the status of the assembler */
  virtual void status(const Cache& cache) const = 0;

  /** Method to add a single set of Trk::ComponentParameters to the cashed Trk::MultiComponentState object under
   * construction*/
  virtual bool addComponent(Cache& cache, SimpleComponentParameters&&) const = 0;

  /** Method to add a new SimpleMultiComponentState to the cashed Trk::MultiComponentState onject under construction */
  virtual bool addMultiState(Cache& cache, SimpleMultiComponentState&&) const = 0;

  /** Method to include the weights of states that are invalid */
  virtual bool addInvalidComponentWeight(Cache& cache, const double) const = 0;

  /** Method to return the cashed state object - it performs a reweighting before returning the object based on the
   * valid and invaid weights */
  virtual std::unique_ptr<MultiComponentState> assembledState(Cache& cache) const = 0;

  /** Method to return the cashed state object - it performs a reweighting based on the input parameter  */
  virtual std::unique_ptr<MultiComponentState> assembledState(Cache& cache, const double) const = 0;
};
} // End Trk namepace

#endif
