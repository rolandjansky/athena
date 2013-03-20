/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $
/**
 * @file TrigTauD3PDMaker/TrigL2TauTracksInfoAssTool.cxx
 * @author e ptacek <eptacek@uoregon.edu>
 * @date Feb, 2010
 * @brief Associate to a trigger object using TrigReadNavTool.
 */

#include "TrigL2TauTracksInfoAssTool.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrigL2TauTracksInfoAssTool::TrigL2TauTracksInfoAssTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : Base (type, name, parent),
        m_matchTool ("TrigTauTool::TrigReadNavTool/TrigReadNavTool"),
        m_incSvc ("IncidentSvc", name)
{
  this->declareProperty ("MatchTool", m_matchTool,
                         "Trigger object matching tool.");
  this->declareProperty ("IncidentService", m_incSvc,
                         "Gaudi incident service.");
  this->declareProperty ("ChainPattern", m_chainPattern = "L2.*tau.*, L2.*trk.*",
                         "Pattern to use for matching chains.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TrigL2TauTracksInfoAssTool::initialize()
{
  CHECK( m_matchTool.retrieve() );
  CHECK( m_incSvc.retrieve() );

  m_incSvc->addListener (this, "StoreCleared");

  return StatusCode::SUCCESS;
}


/**
 * @brief Create any needed tuple variables.
 */
StatusCode TrigL2TauTracksInfoAssTool::book()
{
  //CHECK( this->addVariable ("dr", m_dr) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const TrigTauTracksInfo*
TrigL2TauTracksInfoAssTool::get (const TrigTau& p)
{
  const TrigTauTracksInfo* tautrks=0;
  std::vector<const TrigTauTracksInfo*> tautrksptrs = m_matchTool->getRelations<TrigTauTracksInfo, TrigTau>(m_chainPattern, &p);
  if(tautrksptrs.size()>0) tautrks = tautrksptrs[0];
  return tautrks;
}

/**
 * @brief Handle Gaudi incidents.
 * @param inc The incident to handle.
 */
void TrigL2TauTracksInfoAssTool::handle
(const Incident& inc)
{
  if (inc.type() == "StoreCleared") {
    ;
  }
}


} // namespace D3PD
