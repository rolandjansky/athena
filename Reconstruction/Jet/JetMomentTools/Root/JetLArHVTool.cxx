/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AsgDataHandles/WriteDecorHandle.h"
#include "JetUtils/JetDistances.h"

#include "JetMomentTools/JetLArHVTool.h"
#include "xAODCaloEvent/CaloCluster.h"

JetLArHVTool::JetLArHVTool(const std::string& name)
  : asg::AsgTool(name)
{
}


StatusCode JetLArHVTool::initialize()
{
  if(m_fracKey.key().at(0) == '.' || m_nCellKey.key().at(0) == '.'){
    ATH_MSG_ERROR("JetLArHVTool needs to have its input jet container name configured!");
    return StatusCode::FAILURE;
  }

  // Prepend jet container name
  m_fracKey = m_jetContainerName + "." + m_fracKey.key();
  m_nCellKey = m_jetContainerName + "." + m_nCellKey.key();

  ATH_CHECK(m_fracKey.initialize());
  ATH_CHECK(m_nCellKey.initialize());
  return StatusCode::SUCCESS;
}


StatusCode JetLArHVTool::decorate(const xAOD::JetContainer& jets) const
{

  SG::WriteDecorHandle<xAOD::JetContainer, float> fracHandle(m_fracKey);
  SG::WriteDecorHandle<xAOD::JetContainer, int> nCellHandle(m_nCellKey);

  for(const xAOD::Jet* jet : jets){

    double energyHVaff=0;
    int numCellsHVaff=0;

    // loop over raw constituents. Look for clusters
    size_t num = jet->numConstituents();
    for(size_t i=0;i<num;i++) {
      const xAOD::CaloCluster * cl = dynamic_cast<const xAOD::CaloCluster *>(jet->rawConstituent(i));
      if( !cl) continue;

      double m;
      if(cl->retrieveMoment(xAOD::CaloCluster::ENG_BAD_HV_CELLS,m) ) energyHVaff  += m;
      if(cl->retrieveMoment(xAOD::CaloCluster::N_BAD_HV_CELLS, m) ) numCellsHVaff += m;
    }

    double emE = jet->jetP4(xAOD::JetEMScaleMomentum).E();
    // set the attributes
    fracHandle(*jet) = energyHVaff / emE;
    nCellHandle(*jet) = numCellsHVaff;
  }
  return StatusCode::SUCCESS;
}


