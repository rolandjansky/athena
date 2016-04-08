/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BeamBackgroundJetFillerTool.cxx 589851 2014-03-27 20:01:30Z ssnyder $
#include "BeamBackgroundJetFillerTool.h"


namespace D3PD {


BeamBackgroundJetFillerTool::BeamBackgroundJetFillerTool (const std::string& type,
    const std::string& name,
    const IInterface* parent)
  : BlockFillerTool<xAOD::Jet> (type, name, parent)
{
  declareProperty ("BeamBackgroundKey",
                   m_beamBackgroundKey = "BeamBackgroundData",
                   "StoreGate key for BeamBackground data object");

  book().ignore(); // Avoid coverity warnings.
}

StatusCode BeamBackgroundJetFillerTool::book()
{
  CHECK( addVariable ("bbDecision", m_bbDecision) );

  return StatusCode::SUCCESS;
}

StatusCode BeamBackgroundJetFillerTool::fill(const xAOD::Jet& p)
{
  CHECK( evtStore()->retrieve(m_beamBackgroundData, m_beamBackgroundKey) );

  int index = m_beamBackgroundData->GetIndexJet(&p);

  if( index != -1 ) {
    *m_bbDecision = m_beamBackgroundData->GetResultJet(index);
  } else {
    *m_bbDecision = 0;
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
