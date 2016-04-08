/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BeamBackgroundClusterFillerTool.cxx 589851 2014-03-27 20:01:30Z ssnyder $
#include "BeamBackgroundClusterFillerTool.h"


namespace D3PD {


BeamBackgroundClusterFillerTool::BeamBackgroundClusterFillerTool (const std::string& type,
    const std::string& name,
    const IInterface* parent)
  : BlockFillerTool<xAOD::CaloCluster> (type, name, parent)
{
  declareProperty ("BeamBackgroundKey",
                   m_beamBackgroundKey = "BeamBackgroundData",
                   "StoreGate key for BeamBackground data object");

  book().ignore(); // Avoid coverity warnings.
}

StatusCode BeamBackgroundClusterFillerTool::book()
{
  CHECK( addVariable ("bbDecision", m_resultClus) );
  CHECK( addVariable ("drdz", m_drdzClus) );

  return StatusCode::SUCCESS;
}

StatusCode BeamBackgroundClusterFillerTool::fill(const xAOD::CaloCluster& p)
{
  CHECK( evtStore()->retrieve(m_beamBackgroundData, m_beamBackgroundKey) );

  int index = m_beamBackgroundData->GetIndexClus(&p);

  if( index != -1 ) {
    *m_resultClus = m_beamBackgroundData->GetResultClus(index);
    *m_drdzClus = m_beamBackgroundData->GetDrdzClus(index);
  } else {
    *m_resultClus = 0;
    *m_drdzClus = -1.;
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
