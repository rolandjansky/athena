/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// #include "EventShapeEvent/EventFeatureStore.h"

// #include "EventShapeUtils/StaticHelpers.h"

// #include "EventShapeInterface/StaticConstants.h"

#include "EventShapeRec/EventFeatureAlgorithmBase.h"

#include <iostream>
#include <iomanip>
#include <sstream>

EventFeatureAlgorithmBase::EventFeatureAlgorithmBase(const std::string& name,
						     ISvcLocator* pSvcLocator)
  : AthAlgorithm(name,pSvcLocator)
						    // , m_inputCollectionKey(EventShapeConstants::invalidKey())
  , m_haveInputCollection(false)
						    // , m_eventFeatureStoreKey(EventShapeConstants::invalidKey())
  , m_createFeatureStore(false)
  , m_eventFeatureTools(0)
  , m_haveFeatureTools(false)
{
  declareProperty("InputCollectionKey",  m_inputCollectionKey);
  declareProperty("EventFeatureStoreKey",m_eventFeatureStoreKey);
  declareProperty("EventFeatureTools",   m_eventFeatureTools);
}

EventFeatureAlgorithmBase::~EventFeatureAlgorithmBase()
{ }

StatusCode EventFeatureAlgorithmBase::initialize()
{
  // retrieve tool
  m_haveFeatureTools = !m_eventFeatureTools.empty();
  if ( m_haveFeatureTools )
    {
      if ( m_eventFeatureTools.retrieve().isFailure() )
	{
	  ATH_MSG_ERROR("Could not retrieve EventFeatureTools");
	  return StatusCode::FAILURE;
	}
    }
  // process control
  /*  m_haveInputCollection = 
      m_inputCollectionKey != EventShapeConstants::invalidKey();
      m_createFeatureStore  = 
      m_eventFeatureStoreKey != EventShapeConstants::invalidKey(); */

  // execute sub-class initialization
  return this->appInitialize();
}

StatusCode EventFeatureAlgorithmBase::appInitialize()
{ return StatusCode::SUCCESS; }

StatusCode EventFeatureAlgorithmBase::execute()
{
  // create new feature store
  EventFeatureStore* pStore = new EventFeatureStore();
  // retrieve input
  if ( m_haveInputCollection ) 
    {
      const INavigable4MomentumCollection* pColl = 0;
      if ( evtStore()->retrieve(pColl,m_inputCollectionKey).isFailure() )
	{
	  ATH_MSG_ERROR("Cannot retrieve INavigable4MomentumCollection with "
			<< " key <" << m_inputCollectionKey << ">");
	  return StatusCode::FAILURE; 
	}
      // loop tools if requested
      if ( m_haveFeatureTools )
	{
	  tool_iterator_t fTool(m_eventFeatureTools.begin());
	  tool_iterator_t lTool(m_eventFeatureTools.end());
	  for ( ; fTool != lTool; ++fTool )
	    {
	      this->executeMonitor(fTool->name(),fTool->type(),
				   (*fTool)->execute(pStore,pColl));
	    }
	}
      // check client application
      if ( m_createFeatureStore ) 
	{
	  return this->recordStore(pStore,m_eventFeatureStoreKey).isSuccess() 
	    ? this->appExecute(pStore,pColl)
	    : StatusCode::FAILURE;
	}
      else
	{
	  StatusCode checkOut(this->appExecute(pStore,pColl));
	  if ( pStore != 0 ) delete pStore;
	  return checkOut.isSuccess() 
	    ? StatusCode::SUCCESS : StatusCode::SUCCESS;
	}
    }
  // no input
  else
    {
      if ( m_haveFeatureTools )
	{
	  tool_iterator_t fTool(m_eventFeatureTools.begin());
	  tool_iterator_t lTool(m_eventFeatureTools.end());
	  for ( ; fTool != lTool; ++fTool )
	    {
	      this->executeMonitor(fTool->name(),fTool->type(),
				   (*fTool)->execute(pStore));
	    }
	}
      if ( m_createFeatureStore )
	{
	  return this->recordStore(pStore,m_eventFeatureStoreKey).isSuccess()
	    ? this->appExecute(pStore)
	    : StatusCode::FAILURE;
	}
      else
	{
	  StatusCode checkOut(this->appExecute(pStore));
	  if ( pStore != 0 ) delete pStore;
	  return checkOut.isSuccess() 
	    ? StatusCode::SUCCESS : StatusCode::SUCCESS;
	}
    }
}

StatusCode 
EventFeatureAlgorithmBase::appExecute(EventFeatureStore* /*pStore*/,
				      const INavigable4MomentumCollection* 
				      /*pColl*/)
{ return StatusCode::SUCCESS; }

StatusCode 
EventFeatureAlgorithmBase::appExecute(EventFeatureStore* /*pStore*/)
{ return StatusCode::SUCCESS; }

StatusCode EventFeatureAlgorithmBase::finalize()
{ this->executeSummary(); return this->appFinalize(); }

StatusCode EventFeatureAlgorithmBase::appFinalize()
{ return StatusCode::SUCCESS; }

void EventFeatureAlgorithmBase::executeMonitor(const std::string& name,
					       const std::string& type,
					       StatusCode         checkOut)
{
  tag_t tag(name,type);
  map_t::iterator fMap(m_executeStats.find(tag));
  if ( fMap == m_executeStats.end() ) 
    { 
      m_executeStats[tag] = data_t(0,0); 
      fMap = m_executeStats.find(tag);
    }
  //
  if ( checkOut.isSuccess() )
    { ((*fMap).second).first++; }
  else
    { ((*fMap).second).second++; }
}

void EventFeatureAlgorithmBase::executeSummary(bool cleanUp)
{
  // static size_t _mstrlen(32);
  ATH_MSG_INFO("Tool execution summary: ");
  map_t::iterator fMap(m_executeStats.begin());
  map_t::iterator lMap(m_executeStats.end());
  for ( ; fMap != lMap; ++fMap )
    {
      /* std::ostringstream ostr;
	 ostr << "Tool <name/type> success/failures: <"
	 << std::setw(_mstrlen) << std::right
	 << EventShapeHelpers::stringFormatter(fMap->first.first,_mstrlen) 
	 << "/" 
	 << std::setw(_mstrlen) << std::left
	 << EventShapeHelpers::stringFormatter(fMap->first.second,_mstrlen)
	 << "> " << std::setw(8) << std::right
	 << fMap->second.first << "/"
	 << std::setw(8) << std::left
	 << fMap->second.second;
	 ATH_MSG_INFO(ostr.str()); */
    }
  if ( cleanUp ) m_executeStats.clear();
}

const std::string& EventFeatureAlgorithmBase::getFeatureStoreKey() const
{ return this->featureStoreKey(); }
