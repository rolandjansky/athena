/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

namespace Trk {

static const InterfaceID IID_MultiComponentStateAssembler("MultiComponentStateAssembler", 1, 0);
	
class IMultiComponentStateAssembler : virtual public IAlgTool {
  public:
  
  /** Virtual destructor */
  virtual ~IMultiComponentStateAssembler (){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID () { return IID_MultiComponentStateAssembler; };
  
  /** Resets the AlgTool */
  virtual bool reset() = 0 ;

  /** Print the status of the assembler */
  virtual void status() const = 0;

  /** Method to add a single set of Trk::ComponentParameters to the cashed Trk::MultiComponentState object under construction */
  virtual bool addComponent  (const ComponentParameters&) = 0;
  
  /** Method to add a new Trk::MultiComponentState to the cashed Trk::MultiComponentState onject under construction */
  virtual bool addMultiState (const MultiComponentState&) = 0;
  
  /** Method to include the weights of states that are invalid */
  virtual bool addInvalidComponentWeight (const double&) = 0;
  
  /** Method to return the cashed state object - it performs a reweighting before returning the object based on the valid and invaid weights */
  virtual const MultiComponentState* assembledState () = 0;
  
  /** Method to return the cashed state object - it performs a reweighting based on the input parameter  */
  virtual const MultiComponentState* assembledState (const double&) = 0;

};
} // End Trk namepace


#endif
