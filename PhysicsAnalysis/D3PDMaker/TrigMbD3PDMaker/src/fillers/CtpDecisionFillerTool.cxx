/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMbD3PDMaker/fillers/CtpDecisionFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "AnalysisTriggerEvent/CTP_Decision.h"

namespace TrigMbD3PD {

CtpDecisionFillerTool::CtpDecisionFillerTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
  : CtpDecisionFillerBase (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode CtpDecisionFillerTool::initialize()
{
  CHECK( CtpDecisionFillerBase::initialize() );

  return StatusCode::SUCCESS;
}

StatusCode CtpDecisionFillerTool::book()
{

  CHECK( addVariable ("decisionTypeWord", m_ctpDecisionTypeWord) );
  CHECK( addVariable ("decisionItems", m_ctpDecisionItems) );
  CHECK( addVariable ("decisionWords", m_ctpDecisionWords) );
  CHECK( addVariable ("nDecisionItems", m_ctpNDecisionItems) );

  return StatusCode::SUCCESS;
}

StatusCode CtpDecisionFillerTool::fill (const CtpDecisionFillerObjectType& ctp)
{

  this->clearData();

  *m_ctpDecisionItems = ctp.getItems();
  *m_ctpDecisionWords = ctp.getWords();
  *m_ctpDecisionTypeWord = ctp.getTriggerTypeWord();
  *m_ctpNDecisionItems = m_ctpDecisionItems->size();

  return StatusCode::SUCCESS;
}

void CtpDecisionFillerTool::clearData(){

  *m_ctpDecisionTypeWord = 999;
  *m_ctpNDecisionItems = 0;

  m_ctpDecisionItems->clear();
  m_ctpDecisionWords->clear();
}


} // namespace TrigMbD3PD
