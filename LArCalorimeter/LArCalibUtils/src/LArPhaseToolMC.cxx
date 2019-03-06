/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LArPhaseToolMC.cxx,v 1.1 2006-06-27 18:32:08 ssnyder Exp $

/**
 * @file  LArCalibUtils/LArPhaseToolMC.cxx
 * @author scott snyder
 * @date Feb 2006
 * @brief Retrieve the phase for a LAr EM cell, for MC data.
 */


#include "LArCalibUtils/LArPhaseToolMC.h"
#include "LArSimEvent/LArHitContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/ITriggerTime.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IIncidentSvc.h"


/**
 * @brief Constructor.
 * @param type The type of the tool.
 * @param name The name of the tool.
 * @param parent The parent algorithm of the tool.
 */
LArPhaseToolMC::LArPhaseToolMC (const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : AthAlgTool (type, name, parent),
    m_idhelper (0),
    m_trigtime_tool (0)
{
  declareInterface<ILArPhaseTool> (this);

  // Declare properties with appropriate defaults.
  m_container_names.push_back ("LArHitEMB");
  m_container_names.push_back ("LArHitEMEC");
  declareProperty ("HitContainerNames", m_container_names);
  declareProperty ("TriggerTimeToolName", m_trigtime_tool_name = "");
}


/*
 * @brief Standard Gaudi initialize method.
 */
StatusCode LArPhaseToolMC::initialize()
{
  // Get the trigger time tool. 
  if (!m_trigtime_tool_name.empty()) {
    //CHECK( toolSvc()->retrieveTool (m_trigtime_tool_name,
    //                                m_trigtime_tool) );
    // The above doesn't work because ITriggerTime doesn't have an interfaceID
    // Need to do it this way instead.
    IAlgTool* tmp;
    CHECK( toolSvc()->retrieveTool (m_trigtime_tool_name, tmp) );
    m_trigtime_tool = dynamic_cast<ITriggerTime*> (tmp);
  }

  // Get the cell ID helper.
  m_idhelper = CaloDetDescrManager::instance()->getCaloCell_ID();

  // Get the incident service, and register to look at
  // end-of-event incidents.
  IIncidentSvc* incsvc = 0;
  CHECK( service("IncidentSvc", incsvc) );
  incsvc->addListener (this, "EndEvent");
  return StatusCode::SUCCESS;
}


/*
 * @brief Incident handler.
 * @param incident The incident to handle.
 * 
 * Used to clear cached data at the end of an event.
 */
void LArPhaseToolMC::handle (const Incident& /*incident*/)
{
  m_phases.clear();
}


/*
 * @brief Retrieve the phase for a specific LAr cell.
 * @param cell The cell for which we want the phase.
 * @param[out] phase The phase for the cell.
 *
 * This version looks at MC information to try to find the true
 * phase for MC events.  It tries to do the same thing that's
 * done during digitization.
 */
StatusCode LArPhaseToolMC::phase (const Identifier& cellid, float& phase)
{
  // Calculate the phases, if we haven't done it already
  // for this event.
  if (m_phases.empty())
    CHECK( fill_phases() );

  // Look up the phase for this cell.
  IdentifierHash hash = m_idhelper->calo_cell_hash (cellid);
  if (hash < m_phases.size())
    phase = m_phases[hash];
  else
    phase = 0;
  return StatusCode::SUCCESS;
}


/*
 * @brief Calculate the phases for one event.
 *
 * This is called the first time a phase is requested for a given event.
 * Finding the phases requires a scan over the hits.  So we cache the
 * phases for all cells, to avoid having to scan multiple times.
 * The cached values are cleared at the end of the event.
 */
StatusCode LArPhaseToolMC::fill_phases ()
{
  // Get the global trigger time.
  float trigtime = 0;
  if (m_trigtime_tool)
    trigtime = m_trigtime_tool->time();

  // Set the size of the phase container.
  m_phases.resize (m_idhelper->calo_cell_hash_max(), 0);

  // Scan over all hit containers.
  for (unsigned int icont=0; icont < m_container_names.size(); ++icont) {
    const LArHitContainer* hit_container = nullptr;
    if (evtStore()->retrieve (hit_container,
                              m_container_names[icont]) . isSuccess())
    {
      // Scan over all hits in the container.
      LArHitContainer::const_iterator end = hit_container->end();
      for (LArHitContainer::const_iterator ihit = hit_container->begin();
           ihit != end;
           ++ihit)
      {
        // Fill in the phase for each hit.
        const LArHit& hit = **ihit;
        IdentifierHash hash = m_idhelper->calo_cell_hash (hit.cellID());
        assert (hash < m_phases.size());
        m_phases[hash] = hit.time() - trigtime;
      }
    }
  }
  return StatusCode::SUCCESS;
}
