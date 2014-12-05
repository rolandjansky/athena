/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUtils/JetDistances.h"

#include "JetMomentTools/JetLArHVTool.h"
#include "xAODCaloEvent/CaloCluster.h"

JetLArHVTool::JetLArHVTool(const std::string& name)
  : JetModifierBase(name)
{
}


StatusCode JetLArHVTool::initialize()
{
  return StatusCode::SUCCESS;
}


int JetLArHVTool::modifyJet( xAOD::Jet& jet ) const 
{
  double energyHVaff=0;
  int numCellsHVaff=0;

  // loop over raw constituents. Look for clusters
  size_t num = jet.numConstituents();
  for(size_t i=0;i<num;i++) {
    const xAOD::CaloCluster * cl = dynamic_cast<const xAOD::CaloCluster *>(jet.rawConstituent(i));
    if( !cl) continue;
    
    double m;
    if(cl->retrieveMoment(xAOD::CaloCluster::ENG_BAD_HV_CELLS,m) ) energyHVaff  += m;
    if(cl->retrieveMoment(xAOD::CaloCluster::N_BAD_HV_CELLS, m) ) numCellsHVaff += m;
  }

  double emE = jet.jetP4(xAOD::JetEMScaleMomentum).E();
  // set the attributes
  jet.setAttribute<float>("LArBadHVEnergyFrac", energyHVaff / emE);
  jet.setAttribute<int>("LArBadHVNCell", numCellsHVaff);
    
  return 0;
}


