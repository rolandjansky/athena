/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      QuickCloseComponentsMultiStateMerger.h  -  description
      -------------------------------------------------
begin                : Wednesday 3rd September 2008
author               : amorley
email                : Anthony.Morley@cern.ch
decription           : Class for merging components of a multi-state based on
                       combination of those which are "close" together as
                       defined by some metric ... but faster
*********************************************************************************/

#ifndef TrkQuickCloseComponentsMultiStateMerger_H
#define TrkQuickCloseComponentsMultiStateMerger_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkGaussianSumFilter/IMultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/IMultiComponentStateMerger.h"
#include "TrkGaussianSumFilter/SortingClasses.h"



namespace Trk {

class IMultiComponentStateCombiner;
class TrackStateOnSurface;

class QuickCloseComponentsMultiStateMerger
  : public AthAlgTool
  , virtual public IMultiComponentStateMerger
{

public:
  /** Constructor with parameters to be passed to AlgTool */
  QuickCloseComponentsMultiStateMerger(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~QuickCloseComponentsMultiStateMerger();

  /** AlgTool initialise method */
  StatusCode initialize() override final;

  /** AlgTool finalise method */
  StatusCode finalize() override final;

  virtual std::unique_ptr<MultiComponentState> merge(const MultiComponentState&) const override final;

    /** Method for merging components - ownership of objects is passed */
  virtual std::unique_ptr<MultiComponentState> merge(SimpleMultiComponentState&&) const override final;

private:
  Gaudi::Property<unsigned int> m_maximumNumberOfComponents{ this,
                                                             "MaximumNumberOfComponents",
                                                             12,
                                                             "Maximum number of components" };

  ToolHandle<Trk::IMultiComponentStateCombiner> m_stateCombiner{
    this,
    "CombinerTool",
    "Trk::MultiComponentStateCombiner/CloseComponentsCombiner",
    " Combonent combiner"
  };

  ToolHandle<Trk::IMultiComponentStateAssembler> m_stateAssembler{
    this,
    "MultiComponentStateAssembler",
    "Trk::MultiComponentStateAssembler/CloseComponentsStateAssembler",
    " "
  };

  ServiceHandle<IChronoStatSvc> m_chronoSvc; //!< Timing: The Gaudi time auditing service

  std::unique_ptr<MultiComponentState> mergeFullDistArray(IMultiComponentStateAssembler::Cache& cache,
                                                          SimpleMultiComponentState& ) const;


};

}

#endif
