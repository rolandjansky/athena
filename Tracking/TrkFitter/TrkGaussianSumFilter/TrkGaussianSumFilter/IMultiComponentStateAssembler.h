/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "GaudiKernel/IAlgTool.h"
#include <memory>
#include <vector>



namespace Trk {

typedef std::pair<std::unique_ptr<Trk::TrackParameters>,double> SimpleComponentParameters;
typedef std::vector<SimpleComponentParameters>   SimpleMultiComponentState;


class SortByLargerSimpleComponentWeight {
 public:
  SortByLargerSimpleComponentWeight () {};

  bool operator () ( const SimpleComponentParameters& firstComponent,
          const SimpleComponentParameters& secondComponent ) const

    { return firstComponent.second > secondComponent.second; }

};

static const InterfaceID IID_MultiComponentStateAssembler("MultiComponentStateAssembler", 1, 0);
class IMultiComponentStateAssembler : virtual public IAlgTool {
  public:

  /** Virtual destructor */
  virtual ~IMultiComponentStateAssembler (){};

  /** AlgTool interface methods */
  static const InterfaceID& interfaceID () { return IID_MultiComponentStateAssembler; };

  struct Cache{
    Cache():validWeightSum{0},
    invalidWeightSum{0},
    multiComponentState{},
    assemblyDone{false}{
    }
    double                               validWeightSum;
    double                               invalidWeightSum;
    SimpleMultiComponentState            multiComponentState;
    bool                                 assemblyDone;
  };
  /** Resets the AlgTool */
  virtual bool reset(Cache& cache) const = 0 ;

  /** Print the status of the assembler */
  virtual void status(const Cache& cache) const = 0;

  /** Method to add a single set of Trk::ComponentParameters to the cashed Trk::MultiComponentState object under construction */
  virtual bool addComponent  (Cache& cache, const ComponentParameters&) const = 0;

  /** Method to add a new Trk::MultiComponentState to the cashed Trk::MultiComponentState onject under construction */
  virtual bool addMultiState (Cache& cache,const MultiComponentState&) const = 0;

  /** Method to include the weights of states that are invalid */
  virtual bool addInvalidComponentWeight (Cache& cache,const double) const = 0;

  /** Method to return the cashed state object - it performs a reweighting before returning the object based on the valid and invaid weights */
  virtual const MultiComponentState* assembledState (Cache& cache) const = 0;

  /** Method to return the cashed state object - it performs a reweighting based on the input parameter  */
  virtual const MultiComponentState* assembledState (Cache& cache, const double) const = 0;

};
} // End Trk namepace


#endif
