/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BeamBackgroundJetAssociationTool.cxx 589851 2014-03-27 20:01:30Z ssnyder $
#include "BeamBackgroundJetAssociationTool.h"

namespace D3PD {


BeamBackgroundJetAssociationTool::BeamBackgroundJetAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_jetCounter(0),
      m_beamBackgroundData(0)
{
}

/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode BeamBackgroundJetAssociationTool::reset (const BeamBackgroundData& p)
{
  m_jetCounter = 0;
  m_beamBackgroundData = &p;
   
  return StatusCode::SUCCESS;
}

/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const xAOD::Jet* BeamBackgroundJetAssociationTool::next()
{
  // reached the end
  if(m_jetCounter == m_beamBackgroundData->GetNumJet()) return 0;

  // else return a track and increment index
  const xAOD::Jet* jet = m_beamBackgroundData->GetIndexJet(m_jetCounter);
  m_jetCounter++;
  return jet;
}


} // namespace D3PD

