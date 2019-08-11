/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

namespace Trk {

class MultiComponentStateAssembler
  : public AthAlgTool
  , virtual public IMultiComponentStateAssembler
{
public:
  /** Constructor with AlgTool parameters */
  MultiComponentStateAssembler(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~MultiComponentStateAssembler();

  /** AlgTool initialise method */
  virtual StatusCode initialize() override;

  /** AlgTool finalise method */
  virtual StatusCode finalize() override;

  typedef IMultiComponentStateAssembler::Cache Cache;

  /** Resets the AlgTool */
  virtual bool reset(Cache& cache) const override;

  /** Print the status of the assembler */
  virtual void status(const Cache& cache) const override;

  /** Method to add a single set of Trk::ComponentParameters to the cashed Trk::MultiComponentState object under
   * construction */
  virtual bool addComponent(Cache& cache, const ComponentParameters&) const override;

  /** Method to add a new Trk::MultiComponentState to the cashed Trk::MultiComponentState onject under construction */
  virtual bool addMultiState(Cache& cache, const MultiComponentState&) const override;

  /** Method to include the weights of states that are invalid */
  virtual bool addInvalidComponentWeight(Cache& cache, const double) const override;

  /** Method to return the cashed state object - it performs a reweighting before returning the object based on the
   * valid and invaid weights */
  virtual const MultiComponentState* assembledState(Cache& cache) const override;

  /** Method to return the cashed state object - it performs a reweighting based on the input parameter  */
  virtual const MultiComponentState* assembledState(Cache& cache, const double) const override;

private:
  /** Method to Check component entries before full assembly */
  bool prepareStateForAssembly(Cache& cache) const;

  /** Method to assemble state with correct weightings */
  const MultiComponentState* doStateAssembly(Cache& cache, const double) const;

  /** Method to check the validity of of the cashed state */
  bool isStateValid(Cache& cache) const;

  /** Method for addition of a multi-component state to the cashed state */
  void addComponentsList(Cache& cache, SimpleMultiComponentState&) const;

  // Private data members
private:
  Gaudi::Property<double> m_minimumFractionalWeight{ this,
                                                     "minimumFractionalWeight",
                                                     1.e-9,
                                                     " Minimum Fractional Weight" };
  Gaudi::Property<double> m_minimumValidFraction{ this, "minimumValidFraction", 0.01, " Minimum Valid Fraction" };
};
} // End Trk namepace

inline bool
Trk::MultiComponentStateAssembler::isStateValid(Cache& cache) const
{
  return !cache.multiComponentState.empty();
}

#endif
