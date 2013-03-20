/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $
/**
 * @file TrigTauD3PDMaker/TrigL2TauClusterAssTool.cxx
 * @author e ptacek <eptacek@uoregon.edu>
 * @date Feb, 2010
 * @brief Associate to a trigger object using TrigReadNavTool.
 */

#include "TrigL2TauClusterAssTool.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrigL2TauClusterAssTool::TrigL2TauClusterAssTool
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
StatusCode TrigL2TauClusterAssTool::initialize()
{
  CHECK( m_matchTool.retrieve() );
  CHECK( m_incSvc.retrieve() );

  m_incSvc->addListener (this, "StoreCleared");

  return StatusCode::SUCCESS;
}


/**
 * @brief Create any needed tuple variables.
 */
StatusCode TrigL2TauClusterAssTool::book()
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
const TrigTauCluster*
TrigL2TauClusterAssTool::get (const TrigTau& p)
{
  const TrigTauCluster* tauclus=0;
  std::vector<const TrigTauCluster*> tauclusptrs = m_matchTool->getRelations<TrigTauCluster, TrigTau>(m_chainPattern, &p);
  if(tauclusptrs.size()>0) tauclus = tauclusptrs[0];
  return tauclus;
}

/**
 * @brief Handle Gaudi incidents.
 * @param inc The incident to handle.
 */
void TrigL2TauClusterAssTool::handle
(const Incident& inc)
{
  if (inc.type() == "StoreCleared") {
    ;
  }
}


} // namespace D3PD
