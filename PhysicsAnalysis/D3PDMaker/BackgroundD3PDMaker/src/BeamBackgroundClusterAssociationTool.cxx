/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BeamBackgroundClusterAssociationTool.cxx 589851 2014-03-27 20:01:30Z ssnyder $
#include "BeamBackgroundClusterAssociationTool.h"

namespace D3PD {


BeamBackgroundClusterAssociationTool::BeamBackgroundClusterAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_clusCounter(0),
      m_beamBackgroundData(0)
{
}

/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode BeamBackgroundClusterAssociationTool::reset (const BeamBackgroundData& p)
{
  m_clusCounter = 0;
  m_beamBackgroundData = &p;
   
  return StatusCode::SUCCESS;
}

/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const xAOD::CaloCluster* BeamBackgroundClusterAssociationTool::next()
{
  // reached the end
  if(m_clusCounter == m_beamBackgroundData->GetNumMatched()) return 0;

  // else return a track and increment index
  const xAOD::CaloCluster* clus = m_beamBackgroundData->GetIndexClus(m_clusCounter);
  m_clusCounter++;
  return clus;
}


} // namespace D3PD

