/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $
/**
 * @file TrigTauD3PDMaker/TrigL2InDetTrackAssTool.cxx
 * @author e ptacek <eptacek@uoregon.edu>
 * @date June, 2010
 * @brief Associate to a trigger object using TrigReadNavTool.
 */

#include "TrigL2InDetTrackAssTool.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrigL2InDetTrackAssTool::TrigL2InDetTrackAssTool
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
  this->declareProperty ("ChainPattern", m_chainPattern = "L2.*tau.*,L2.*trk.*",
                         "Pattern to use for matching chains.");

  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TrigL2InDetTrackAssTool::initialize()
{
  CHECK( m_matchTool.retrieve() );
  CHECK( m_incSvc.retrieve() );

  m_incSvc->addListener (this, "StoreCleared");

  return StatusCode::SUCCESS;
}


/**
 * @brief Create any needed tuple variables.
 */
StatusCode TrigL2InDetTrackAssTool::book()
{
  CHECK( this->addVariable ("tracks_algorithmId", m_algId) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const TrigInDetTrack*
TrigL2InDetTrackAssTool::get (const TrigTau& p)
{
  const TrigInDetTrack* tautrks=0;
  *m_algId=-1;
  std::vector<const TrigInDetTrackCollection*> trackcollections = m_matchTool->getRelations<TrigInDetTrackCollection, TrigTau>(m_chainPattern, &p);
  for(size_t k=0; k<trackcollections.size(); k++){
    const TrigInDetTrackCollection* trackcollection = trackcollections[k];
    if(trackcollection->size()>0){
      tautrks = (*trackcollection)[0];
      *m_algId=tautrks->algorithmId();
    }
  }
  return tautrks;
}

/**
 * @brief Handle Gaudi incidents.
 * @param inc The incident to handle.
 */
void TrigL2InDetTrackAssTool::handle
(const Incident& inc)
{
  if (inc.type() == "StoreCleared") {
    ;
  }
}


} // namespace D3PD
