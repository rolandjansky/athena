/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LArPhaseToolConst.cxx,v 1.1 2006-06-27 18:32:07 ssnyder Exp $

/**
 * @file  LArCalibUtils/LArPhaseToolConst.cxx
 * @author scott snyder
 * @date Feb 2006
 * @brief Return a constant LAr phase.
 */


#include "LArCalibUtils/LArPhaseToolConst.h"


/**
 * @brief Constructor.
 * @param type The type of the tool.
 * @param name The name of the tool.
 * @param parent The parent algorithm of the tool.
 */
LArPhaseToolConst::LArPhaseToolConst (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : AthAlgTool (type, name, parent)
{
  declareInterface<ILArPhaseTool> (this);
  declareProperty ("Phase", m_phase = 0);
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode LArPhaseToolConst::initialize()
{
  return StatusCode::SUCCESS;
}


/*
 * @brief Retrieve the phase for a specific LAr cell.
 * @param cell The cell for which we want the phase.
 * @param[out] phase The phase for the cell.
 *
 * This version just returns a constant phase,
 * set from job options.
 */
StatusCode LArPhaseToolConst::phase (const Identifier& /*cell*/,
                                     float& phase)
{
  phase = m_phase;
  return StatusCode::SUCCESS;
}



