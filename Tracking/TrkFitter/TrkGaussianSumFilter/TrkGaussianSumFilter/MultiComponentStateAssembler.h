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
  virtual bool reset(Cache& cache) const override final;

  /** Print the status of the assembler */
  virtual void status(const Cache& cache) const override final;

  /** Method to add a single set of Trk::ComponentParameters to the cached Trk::MultiComponentState object under
   * construction */
  virtual bool addComponent(Cache& cache, SimpleComponentParameters&&) const override final;

  /** Method to add a new Trk::MultiComponentState to the cached Trk::MultiComponentState onject under construction */
  virtual bool addMultiState(Cache& cache, SimpleMultiComponentState&&) const override final;

  /** Method to include the weights of states that are invalid */
  virtual bool addInvalidComponentWeight(Cache& cache, const double) const override final;

  /** Method to return the cached state object - it performs a reweighting before returning the object based on the
   * valid and invaid weights */
  virtual std::unique_ptr<MultiComponentState> assembledState(Cache& cache) const override final;

  /** Method to return the cached state object - it performs a reweighting based on the input parameter  */
  virtual std::unique_ptr<MultiComponentState> assembledState(Cache& cache, const double) const override final;

private:
  /** Method to Check component entries before full assembly */
  bool prepareStateForAssembly(Cache& cache) const;

  /** Method to assemble state with correct weightings */
  std::unique_ptr<MultiComponentState> doStateAssembly(Cache& cache, const double) const;

  /** Method to check the validity of of the cached state */
  bool isStateValid(const Cache& cache) const;

  // Private data members
  Gaudi::Property<double> m_minimumFractionalWeight{ this,
                                                     "minimumFractionalWeight",
                                                     1.e-9,
                                                     "Minimum Fractional Weight" };
  Gaudi::Property<double> m_minimumValidFraction{ this, "minimumValidFraction", 0.01, " Minimum Valid Fraction" };
};
} // End Trk namepace

inline bool
Trk::MultiComponentStateAssembler::isStateValid(const Cache& cache) const
{
  return !cache.multiComponentState.empty();
}

#endif
