/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
			CloseComponentsMultiStateMerger.h  -  description
			-------------------------------------------------
begin                : Wednesday 23rd February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Class for merging components of a multi-state based on 
                       combination of those which are "close" together as 
                       defined by some metric
*********************************************************************************/

#ifndef TrkCloseComponentsMultiStateMerger_H
#define TrkCloseComponentsMultiStateMerger_H

#include "TrkGaussianSumFilter/IMultiComponentStateMerger.h"
#include "TrkGaussianSumFilter/SortingClasses.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "GaudiKernel/IChronoStatSvc.h"

#include <map>
//class IChronoStatSvc;

namespace Trk{

class IMultiComponentStateCombiner;
class IComponentSeparationDistance;
class IMultiComponentStateAssembler;

class CloseComponentsMultiStateMerger : public AthAlgTool, virtual public IMultiComponentStateMerger {

 public:

  /** Constructor with parameters to be passed to AlgTool */
  CloseComponentsMultiStateMerger(const std::string&, const std::string&, const IInterface*);

 /** Virtual destructor */
  virtual ~CloseComponentsMultiStateMerger();

  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();

  virtual const MultiComponentState* merge(const MultiComponentState&) const;

 private:
  typedef std::multimap<double, ComponentParameters, SortByLargerWeight> MultiComponentStateMap;

  const std::pair<const ComponentParameters, MultiComponentStateMap::iterator>* pairWithMinimumDistance(MultiComponentStateMap&) const;

  unsigned int m_maximumNumberOfComponents;

  ToolHandle<IComponentSeparationDistance>  m_distance;

  ToolHandle<Trk::IMultiComponentStateCombiner>   m_stateCombiner;
  ToolHandle<Trk::IMultiComponentStateAssembler>  m_stateAssembler;
    
  ServiceHandle<IChronoStatSvc>                  m_chronoSvc;           //!< Timing: The Gaudi time auditing service
};

}

#endif
