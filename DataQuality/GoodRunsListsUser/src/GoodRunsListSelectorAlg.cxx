/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GoodRunsListsUser/GoodRunsListSelectorAlg.h"
#include "GoodRunsLists/IGoodRunsListSelectorTool.h"
#include "GoodRunsLists/TMsgLogger.h"
#include "TROOT.h"

GoodRunsListSelectorAlg::GoodRunsListSelectorAlg(const std::string& name, ISvcLocator* pSvcLocator) 
 : AthAlgorithm(name, pSvcLocator)
 , m_GoodRunsListSelectorTool("GoodRunsListSelectorTool")
 , m_counter(0)
 , m_accept(0)
{
  declareProperty( "GoodRunsListArray", m_grlnameVec );
  declareProperty( "BlackRunsListArray", m_brlnameVec );
  declareProperty( "CounterPrescale", m_prescale = 1000);
}


GoodRunsListSelectorAlg::~GoodRunsListSelectorAlg()
{ 
} 


StatusCode GoodRunsListSelectorAlg::initialize() 
{
  ATH_MSG_DEBUG ("initialize()");

  Root::TMsgLogger::SetMinLevel(static_cast<Root::TMsgLevel>(msgLevel()));

  /// Retrieve the GoodRunsListSelector tool using the ToolHandles
  if ( m_GoodRunsListSelectorTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL 
      (m_GoodRunsListSelectorTool.propertyName() << ": Failed to retrieve tool "
       << m_GoodRunsListSelectorTool.type());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG
      (m_GoodRunsListSelectorTool.propertyName() << ": Retrieved tool " 
       << m_GoodRunsListSelectorTool.type());
  }

  /// construct registration name
  {
    std::ostringstream tmp;
    tmp << "Sequence" << m_GoodRunsListSelectorTool->getGRLSelectorRegistry().size();
    m_myname = tmp.str();
  };

  /// register the GRL selector for later use in lumiblockmetadata tool. 
  ATH_MSG_INFO ("initialize() :: Now registering this GRL Selector with unique name <" << m_myname << ">.");
  bool regOk = m_GoodRunsListSelectorTool->registerGRLSelector(m_myname,m_grlnameVec,m_brlnameVec);
  if (!regOk) {
    ATH_MSG_ERROR ("initialize() :: Failure to register this GRL Selector with name <" << m_myname << ">. Return failure.");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG ("initialize() successful");

  return StatusCode::SUCCESS;
}


StatusCode 
GoodRunsListSelectorAlg::execute() 
{
  ATH_MSG_DEBUG ("execute()");

  if (m_counter % m_prescale == 0) 
    ATH_MSG_INFO ("Now processing event : " << m_counter);
  m_counter++ ;

  bool statusOk = m_GoodRunsListSelectorTool->passThisRunLB(m_grlnameVec,m_brlnameVec);
  this->setFilterPassed (statusOk); // This skips the execution of following algs for this event
  if (statusOk) ++m_accept;

  ATH_MSG_DEBUG ("execute() successful");

  return StatusCode::SUCCESS;
}


StatusCode GoodRunsListSelectorAlg::finalize() 
{
  ATH_MSG_DEBUG ("finalize()");

  ATH_MSG_INFO ("Processed " << m_counter << " events, accepted " << m_accept << " events.");

  ATH_MSG_DEBUG ("finalize() successful");

  return StatusCode::SUCCESS;
}


