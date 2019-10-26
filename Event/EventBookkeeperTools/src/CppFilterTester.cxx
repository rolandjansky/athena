/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Creation: Joao Costa July 2010 <joao.firmino.da.costa@desy.de> and inspired from alg from David Cote, September 2008. <david.cote@cern.ch>
///////////////////////////////////////////////////////////////////

#include "CppFilterTester.h"

CppFilterTester::CppFilterTester(const std::string& name, ISvcLocator* pSvcLocator)
  : AthFilterAlgorithm(name, pSvcLocator)
{
}


StatusCode CppFilterTester::initialize()
{
  ATH_MSG_DEBUG("initialize()");

  // Get filter EB and only then register the cuts
  m_filterCutID = cutFlowSvc()->registerFilter(this->name(), "Algorithm cut");
  m_cut1ID      = cutFlowSvc()->registerCut("cut1", "First cut", m_filterCutID);
  m_cut2ID      = cutFlowSvc()->registerCut("cut2", "Second cut", m_filterCutID);

  return StatusCode::SUCCESS;
}


StatusCode CppFilterTester::execute()
{
  cutFlowSvc()->addEvent(m_cut1ID, m_cut1);
  cutFlowSvc()->addEvent(m_cut2ID, m_cut2);
  cutFlowSvc()->addEvent(m_filterCutID, 2);

  return StatusCode::SUCCESS;
}
