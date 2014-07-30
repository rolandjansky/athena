/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
			MultiComponentStateAssembler.h  -  description
			-----------------------------------------------
begin                : Monday 20th December 2004
author               : atkinson
email                : Tom.Atkinson@cern.ch
description          : This is a helper class to collect components of a
			multi-component state and put them all into a
			MultiComponentState. The addition of components can be
			one at a time through the addComponent method (taking a
			single ComponentParameters object) or many at a time,
			through the addComponents method (taking a
			MultiComponentState object). In addition this helper
			class also is used in weighting renormalisation
			calculations and the removal of components with
			insignificantly small weightings.
***************************************************************************/

#ifndef MultiComponentStateAssembler_H
#define MultiComponentStateAssembler_H

#include "TrkGaussianSumFilter/IMultiComponentStateAssembler.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "AthenaBaseComps/AthAlgTool.h"

namespace Trk {

class MultiComponentStateAssembler : public AthAlgTool, virtual public IMultiComponentStateAssembler{
  public:

  /** Constructor with AlgTool parameters */
  MultiComponentStateAssembler (const std::string&, const std::string&, const IInterface*);
  
  /** Virtual destructor */
  virtual ~MultiComponentStateAssembler ();
    
  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();

  /** Resets the AlgTool */
  virtual bool reset();

  /** Print the status of the assembler */
  virtual void status() const;

  /** Method to add a single set of Trk::ComponentParameters to the cashed Trk::MultiComponentState object under construction */
  virtual bool addComponent  (const ComponentParameters&);
  
  /** Method to add a new Trk::MultiComponentState to the cashed Trk::MultiComponentState onject under construction */
  virtual bool addMultiState (const MultiComponentState&);
  
  /** Method to include the weights of states that are invalid */
  virtual bool addInvalidComponentWeight (const double&);
  
  /** Method to return the cashed state object - it performs a reweighting before returning the object based on the valid and invaid weights */
  virtual const MultiComponentState* assembledState ();
  
  /** Method to return the cashed state object - it performs a reweighting based on the input parameter  */
  virtual const MultiComponentState* assembledState (const double&);
  
 private:
  
  /** Method to Check component entries before full assembly */
  bool prepareStateForAssembly ();
  
  /** Method to assemble state with correct weightings */
  const MultiComponentState* doStateAssembly (const double&);
  
  /** Method to check the validity of of the cashed state */
  bool isStateValid () const;
  
  /** Method to Remove components with insignificantly small weightings */
  void removeSmallWeights ();

  /** Method for the actual addition of a multi-component state to the cashed state */
  void addComponentsList (const MultiComponentState*);
  
  // Private data members
 private:
  int                                m_outputlevel;                      //!< to cache current output level  bool m_sortComponents;
  bool                               m_assemblyDone;
  double                             m_minimumFractionalWeight;
  double                             m_minimumValidFraction;
  double                             m_validWeightSum;
  double                             m_invalidWeightSum;
  MultiComponentState*               m_multiComponentState;

};
} // End Trk namepace

inline bool Trk::MultiComponentStateAssembler::isStateValid () const
{
  return !m_multiComponentState->empty();
}

#endif
