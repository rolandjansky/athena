/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



// #include "EventShapeUtils/StaticHelpers.h"

#include "EventShapeRec/EventEtDensityCalculator.h"

#include <iostream>
#include <sstream>
#include <iomanip>

EventEtDensityCalculator::EventEtDensityCalculator(const std::string& name,
						   ISvcLocator* pSvcLocator)
  : EventFeatureAlgorithmBase(name,pSvcLocator)
						  // , m_densityTools(0)
{
  // declareProperty("EventEtDensityTools",m_densityTools);
}

EventEtDensityCalculator::~EventEtDensityCalculator()
{ }

StatusCode EventEtDensityCalculator::appInitialize()
{
  // if ( !m_densityTools.empty() && m_densityTools.retrieve().isFailure() )
  // {
  // ATH_MSG_ERROR("Cannot allocate event Et density calculator tools");
  // return StatusCode::FAILURE;
  // }
  // ToolHandleArray<IEventEtDensityTool>::iterator fTool(m_densityTools.begin());
  // ToolHandleArray<IEventEtDensityTool>::iterator lTool(m_densityTools.end());
  // for ( ; fTool != lTool; ++fTool )
  // {
  /* std::ostringstream ostr;
     ostr << "Allocated tool <" 
     << EventShapeHelpers::stringFormatter(fTool->type(),32)
     << "/" << EventShapeHelpers::stringFormatter(fTool->name(),32)
     << ">";
     ATH_MSG_INFO(ostr.str()); */
  // }
  return StatusCode::SUCCESS;
}

StatusCode 
EventEtDensityCalculator::appExecute(EventFeatureStore* /* pStore */,
				     const INavigable4MomentumCollection* /* pColl */ )
{
  // ToolHandleArray<IEventEtDensityTool>::iterator fTool(m_densityTools.begin());
  // ToolHandleArray<IEventEtDensityTool>::iterator lTool(m_densityTools.end());
  // for ( ; fTool != lTool; ++fTool )
  // {
  //      this->executeMonitor(fTool->name(),fTool->type(),
  //		   (*fTool)->execute(pStore,pColl));
  // }
  return StatusCode::SUCCESS;
}

StatusCode
EventEtDensityCalculator::appExecute(EventFeatureStore* /* pStore */ )
{
  // ToolHandleArray<IEventEtDensityTool>::iterator fTool(m_densityTools.begin());
  // ToolHandleArray<IEventEtDensityTool>::iterator lTool(m_densityTools.end());
  // for ( ; fTool != lTool; ++fTool )
  // {
  // this->executeMonitor(fTool->name(),fTool->type(),
  //	   (*fTool)->execute(pStore));
  // }
  return StatusCode::SUCCESS;
}

