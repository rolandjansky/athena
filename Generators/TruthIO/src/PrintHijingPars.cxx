/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthIO/PrintHijingPars.h"

#include "GeneratorObjects/HijingEventParams.h"

PrintHijingPars::PrintHijingPars(const std::string& name, ISvcLocator* pSvcLocator) :
  GenBase(name, pSvcLocator)
{
  // Set users' request
  declareProperty("HijingParamsKey",     m_key="Hijing_event_params");
  declareProperty("VerboseOutput",  m_VerboseOutput=true);
  declareProperty("FirstEvent",   m_firstEvt=1);
  declareProperty("LastEvent",   m_lastEvt=100000000);
}

StatusCode PrintHijingPars::initialize(){

  ATH_CHECK( GenBase::initialize() );

  ATH_MSG_INFO(">>> PrintHijingPars from Initialize");
  std::cout << "----- PrintHijingPars From initialize" << std::endl;

  if (m_lastEvt<m_firstEvt)  { m_lastEvt = m_firstEvt; }

  // Initialization terminated
  return StatusCode::SUCCESS;
}

StatusCode PrintHijingPars::execute() 
{
  
  const HijingEventParams *hijing_pars;
  const StatusCode sc =  evtStore()->retrieve(hijing_pars, m_key);
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Could not retrieve Hijing_event_params");
    return StatusCode::FAILURE;
  }
  
  // printout hijing event params
  // ----------------------------
  std::cout << "----- PrintHijingPars -----" << std::endl;
  std::cout << "np:   " << hijing_pars->get_np() << std::endl;
  std::cout << "nt:   " << hijing_pars->get_nt() << std::endl;
  std::cout << "n0:   " << hijing_pars->get_n0() << std::endl;
  std::cout << "n01:  " << hijing_pars->get_n01() << std::endl;
  std::cout << "n10:  " << hijing_pars->get_n10() << std::endl;
  std::cout << "n11:  " << hijing_pars->get_n11() << std::endl;
  std::cout << "natt:  " << hijing_pars->get_natt() << std::endl;
  std::cout << "jatt:  " << hijing_pars->get_jatt() << std::endl;
  std::cout << "b:    " << hijing_pars->get_b() << std::endl;
  std::cout << "bphi: " << hijing_pars->get_bphi() << std::endl;
  std::cout << "---------------------------" << std::endl;
  
  // End of execution for each event
  return StatusCode::SUCCESS;
}

StatusCode PrintHijingPars::finalize() {

  ATH_MSG_INFO(">>> PrintHijingPars from finalize");
  std::cout << "----- PrintHijingPars From finalize" << std::endl;

  // End of finalization step
  return StatusCode::SUCCESS;
}
