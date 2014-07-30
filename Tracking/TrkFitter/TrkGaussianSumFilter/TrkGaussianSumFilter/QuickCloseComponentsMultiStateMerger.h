/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "TrkGaussianSumFilter/IMultiComponentStateMerger.h"
#include "TrkGaussianSumFilter/SortingClasses.h"


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "GaudiKernel/IChronoStatSvc.h"

#include <map>

namespace Trk{

class IMultiComponentStateCombiner;
class IComponentSeparationDistance;
class IMultiComponentStateAssembler;
class TrackStateOnSurface;

class QuickCloseComponentsMultiStateMerger : public AthAlgTool, virtual public IMultiComponentStateMerger {

 public:

  /** Constructor with parameters to be passed to AlgTool */
  QuickCloseComponentsMultiStateMerger(const std::string&, const std::string&, const IInterface*);

 /** Virtual destructor */
  virtual ~QuickCloseComponentsMultiStateMerger();

  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();

  virtual const MultiComponentState* merge(const MultiComponentState&) const;

 private:	
 
 
  typedef std::multimap<double, ComponentParameters, SortBySmallerWeight> MultiComponentStateMap;

	double calculateDistance(const Trk::ComponentParameters , const Trk::ComponentParameters) const;


  unsigned int m_maximumNumberOfComponents;

  ToolHandle<IComponentSeparationDistance>  m_distance;

  ToolHandle<Trk::IMultiComponentStateCombiner>   m_stateCombiner;
  ToolHandle<Trk::IMultiComponentStateAssembler>  m_stateAssembler;
  
  ServiceHandle<IChronoStatSvc>                  m_chronoSvc;           //!< Timing: The Gaudi time auditing service
};

}

#endif

