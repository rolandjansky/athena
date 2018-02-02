/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkHI/HIMBSkimmingTool.h"

// Constructor
DerivationFramework::HIMBSkimmingTool::HIMBSkimmingTool(const std::string& t,
							const std::string& n,
							const IInterface* p) :
  AthAlgTool(t, n, p),
  m_total(0),
  m_passed(0)
{

  declareInterface<DerivationFramework::ISkimmingTool>(this);
  declareProperty("HIEventSelectorTool",m_selection_tool);
}
  
// Destructor
DerivationFramework::HIMBSkimmingTool::~HIMBSkimmingTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::HIMBSkimmingTool::initialize()
{
  //CHECK(m_selection_tool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::HIMBSkimmingTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed " << m_total << " events, " << m_passed << " events passed filter ");  
  return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::HIMBSkimmingTool::eventPassesFilter() const
{

  m_total++;
  // if(m_selection_tool->pass())
  // {
  //   m_passed++;
  //   return true;
  // }
  // return false;
  ATH_MSG_INFO("checking event");
  auto a=m_selection_tool->accept();
  for(unsigned int i=0; i<a.getNCuts(); i++)ATH_MSG_INFO(a.getCutName(i) << "\t" << a.getCutDescription(i) << "\t" << a.getCutResult(i) );
  if(a)
  {
    m_passed++;
    ATH_MSG_INFO("checking event");
    return true;
  }
  ATH_MSG_INFO("event failed");
  return true;
}

