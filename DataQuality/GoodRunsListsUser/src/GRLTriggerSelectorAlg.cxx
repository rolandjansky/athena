/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GoodRunsListsUser/GRLTriggerSelectorAlg.h"
#include "GoodRunsLists/IGoodRunsListSelectorTool.h"
#include "GoodRunsLists/ITriggerRegistryTool.h"

#include "GoodRunsLists/TMsgLogger.h"
#include "TrigDecisionTool/TrigDecisionTool.h" // rel 15
#include "TrigDecisionEvent/TrigDecision.h" // rel 15
#include "TROOT.h"

GRLTriggerSelectorAlg::GRLTriggerSelectorAlg(const std::string& name, ISvcLocator* pSvcLocator) 
 : AthAlgorithm(name, pSvcLocator)
 , m_GoodRunsListSelectorTool("GoodRunsListSelectorTool")
 , m_TriggerRegistryTool("TriggerRegistryTool")
 , m_tdTool("Trig::TrigDecisionTool")
 , m_gcounter(0)
 , m_gaccept(0)
 , m_tcounter(0)
 , m_taccept(0)
 , m_trigpar(0)
 , m_applyTriggerSelection(false)
{
  declareProperty( "GoodRunsListArray", m_grlnameVec );
  declareProperty( "BlackRunsListArray", m_brlnameVec );
  declareProperty( "TriggerSelection", m_regexpr );
  declareProperty( "TriggerSelectionRegistration", m_regexprRegistrationOnly );
  declareProperty( "CounterPrescale", m_prescale = 1000 );
}


GRLTriggerSelectorAlg::~GRLTriggerSelectorAlg()
{ 
  // cleanup
  if (m_trigpar!=0) { delete[] m_trigpar; m_trigpar=0; }
} 


StatusCode GRLTriggerSelectorAlg::initialize() 
{
  ATH_MSG_DEBUG ("initialize()");

  Root::TMsgLogger::SetMinLevel(static_cast<Root::TMsgLevel>(msgLevel()));

  /// construct registration name
  {
    std::ostringstream tmp;
    tmp << "Sequence" << m_GoodRunsListSelectorTool->getGRLSelectorRegistry().size();
    m_myname = tmp.str();
  };

  /////////////////////////////////////

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

  /// register the GRL selector for later use in lumiblockmetadata tool. 
  ATH_MSG_INFO ("initialize() :: Now registering this GRL Selector with unique name <" << m_myname << ">.");
  bool regOk = m_GoodRunsListSelectorTool->registerGRLSelector(m_myname,m_grlnameVec,m_brlnameVec);
  if (!regOk) {
    ATH_MSG_ERROR ("initialize() :: Failure to register this GRL Selector with name <" << m_myname << ">. Return failure.");
    return StatusCode::FAILURE;
  }

  /////////////////////////////////////

  /// determine correct trigger selection expression
  if (!m_regexprRegistrationOnly.empty() && m_regexpr.empty()) {
    m_regexpr = m_regexprRegistrationOnly;
    m_applyTriggerSelection = false;
  } else if (m_regexprRegistrationOnly.empty() && !m_regexpr.empty()) {
    m_applyTriggerSelection = true;
  } else if (!m_regexprRegistrationOnly.empty() && !m_regexpr.empty()) {
    ATH_MSG_WARNING ("initialize() :: Both TriggerSelectionRegistration and TriggerSelection are set. Ignoring TriggerSelectionRegistration.");
    m_regexprRegistrationOnly = "";
    m_applyTriggerSelection = true;
  } else { // default in case nothing is set, pass through mode
    m_regexpr = "1";
    m_applyTriggerSelection = false;
  }

  if (!m_applyTriggerSelection)
    ATH_MSG_INFO ("initialize() :: Trigger selection set to pass-through mode.");

  /// Retrieve the TriggerRegistry tool using the ToolHandles
  if ( m_TriggerRegistryTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL
      (m_TriggerRegistryTool.propertyName() << ": Failed to retrieve tool "
       << m_TriggerRegistryTool.type());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG
      (m_TriggerRegistryTool.propertyName() << ": Retrieved tool " 
       << m_TriggerRegistryTool.type());
  }

  int formOk = m_formula.setFormula(m_regexpr.c_str()) ;

  if ( 0!=formOk ) {
    ATH_MSG_ERROR ("Cannot parse regular expression : " << m_regexpr << " . Return Failure.");
    return StatusCode::FAILURE;
  }

  // get trigger names from formula 
  m_par = m_formula.getParNames();
  m_trigpar = new double[m_formula.getNPars()];

  /// register the trigger selector for later use in lumicalcsvc. 
  ATH_MSG_INFO ("initialize() :: Now registering this Trigger Selector with unique name <" << m_myname << ">.");
  regOk = m_TriggerRegistryTool->registerTriggerSelector(m_myname,m_regexpr,m_par);
  if (!regOk) {
    ATH_MSG_ERROR ("initialize() :: Failure to register this Trigger Selector with name <" << m_myname << ">. Return failure.");
    return StatusCode::FAILURE;
  }

  /////////////////////////////////////

  ATH_MSG_DEBUG ("initialize() successful");

  return StatusCode::SUCCESS;
}


StatusCode 
GRLTriggerSelectorAlg::execute() 
{
  ATH_MSG_DEBUG ("execute()");

  /////////////////////////////////////

  if (m_gcounter % m_prescale == 0) 
    ATH_MSG_INFO ("Now processing event passing GRL selection : " << m_gcounter);
  m_gcounter++ ;

  bool statusOk = m_GoodRunsListSelectorTool->passThisRunLB(m_grlnameVec,m_brlnameVec);
  this->setFilterPassed (statusOk); // This skips the execution of following algs for this event
  if (statusOk) ++m_gaccept;
  else { return StatusCode::SUCCESS; }

  /////////////////////////////////////

  if (m_applyTriggerSelection) { 
    if( m_tdTool.retrieve().isFailure()) {
      ATH_MSG_ERROR ("Could not retrieve TrigDecisionTool. Return Failure.");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG ("Successfully retrieved TrigDecisionTool.");
    }
  }

  if (m_tcounter % m_prescale == 0)
    ATH_MSG_INFO ("Now processing event passing Trigger selection : " << m_tcounter);
  m_tcounter++ ;

  // Filling trigger map
  std::list<TString>::iterator tnItr = m_par.begin() ;
  for (unsigned int i=0; tnItr!=m_par.end(); ++tnItr, ++i) {
    bool isPassed(true);
    if (m_applyTriggerSelection) { isPassed = m_tdTool->isPassed(tnItr->Data()); }
    m_trigpar[i] = static_cast<double>(isPassed);
    ATH_MSG_DEBUG ("Trigger name : " << *tnItr << " = " << m_trigpar[i]);
  }

  // evaluate trigger regular expression
  double dummy(0);
  bool pass(true);
  if (m_applyTriggerSelection) pass = static_cast<bool>(m_formula.EvalPar(&dummy,m_trigpar));
  this->setFilterPassed (pass);  // skip execution of following algs for this event
  if (pass) ++m_taccept;

  /////////////////////////////////////

  ATH_MSG_DEBUG ("Event accepted? " << pass);

  ATH_MSG_DEBUG ("execute() successful");

  return StatusCode::SUCCESS;
}


StatusCode GRLTriggerSelectorAlg::finalize() 
{
  ATH_MSG_DEBUG ("finalize()");

  ATH_MSG_INFO ("GRL     : Processed " << m_gcounter << " events, accepted " << m_gaccept << " events.");
  ATH_MSG_INFO ("Trigger : Processed " << m_tcounter << " events, accepted " << m_taccept << " events.");

  ATH_MSG_DEBUG ("finalize() successful");

  return StatusCode::SUCCESS;
}

