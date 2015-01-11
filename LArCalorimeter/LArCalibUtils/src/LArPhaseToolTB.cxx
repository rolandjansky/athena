/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LArPhaseToolTB.cxx,v 1.1 2006-06-27 18:32:09 ssnyder Exp $

/**
 * @file  LArCalibUtils/LArPhaseToolTB.cxx
 * @author scott snyder
 * @date Feb 2006
 * @brief Retrieve the phase for a LAr EM cell, for combined test beam data.
 */


#include "LArCalibUtils/LArPhaseToolTB.h"
#include "TBEvent/TBPhase.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IIncidentSvc.h"


/**
 * @brief Constructor.
 * @param type The type of the tool.
 * @param name The name of the tool.
 * @param parent The parent algorithm of the tool.
 */
LArPhaseToolTB::LArPhaseToolTB (const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : AthAlgTool (type, name, parent),
    m_tbphase (0),
    m_have_phase (false)
{
  declareInterface<ILArPhaseTool> (this);

  // Properties, with defaults.
  declareProperty ("PhaseKey", m_phasekey = "TBPhase");
}


/*
 * @brief Standard Gaudi initialize method.
 */
StatusCode LArPhaseToolTB::initialize()
{
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
void LArPhaseToolTB::handle (const Incident& /*incident*/)
{
  m_have_phase = 0;
}


/*
 * @brief Retrieve the phase for a specific LAr cell.
 * @param cell The cell for which we want the phase.
 * @param[out] phase The phase for the cell.
 *
 * This version retrieves the phase information from the
 * test beam scintillators.
 */
StatusCode LArPhaseToolTB::phase (const Identifier& /*cell*/, float& phase)
{
  // Cache the phase so we only have to do the storegate lookup
  // once per event, rather than once per cell.
  if (! m_have_phase) {
    TBPhase* tbphase;
    CHECK( evtStore()->retrieve (tbphase, m_phasekey) );
    m_tbphase = tbphase->getPhase();
    m_have_phase = true;
  }

  phase = m_tbphase;
  return StatusCode::SUCCESS;
}



