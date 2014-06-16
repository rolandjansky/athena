// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPEREC_EVENTETDENSITYCALCULATOR_H
#define EVENTSHAPEREC_EVENTETDENSITYCALCULATOR_H

#include "GaudiKernel/ToolHandle.h"

#include "EventShapeInterface/IEventEtDensityTool.h"

#include "EventShapeRec/EventFeatureAlgorithmBase.h"

class EventEtDensityCalculator : public EventFeatureAlgorithmBase
{
public:
  EventEtDensityCalculator(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~EventEtDensityCalculator();

protected:

  virtual StatusCode appInitialize();
  virtual StatusCode appExecute(EventFeatureStore* pStore,
				const INavigable4MomentumCollection* pColl);
  virtual StatusCode appExecute(EventFeatureStore* pStore);

  //  ToolHandleArray<IEventEtDensityTool> m_densityTools;
}; 
#endif
