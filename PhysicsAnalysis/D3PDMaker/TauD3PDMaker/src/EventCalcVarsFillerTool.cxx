/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/EventCalcVarsFillerTool.cxx
 * @author Michel Trottier-McDonald <mtm@cern.ch>
 * @date December, 2010
 * @brief Block filler tool for event-based calculated variables/
 */


#include "EventCalcVarsFillerTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
EventCalcVarsFillerTool::EventCalcVarsFillerTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : BlockFillerTool<void> (type, name, parent),
    m_detailsManager(0)
{
  book().ignore(); // Avoid coverity warnings
}

StatusCode EventCalcVarsFillerTool::initialize()
{
  // Getting access to StoreGate
  StoreGateSvc* storeGate = 0;
  StatusCode sc = service("StoreGateSvc", storeGate);
  
  if ( sc == StatusCode::FAILURE ) 
    {
      REPORT_MESSAGE(MSG::ERROR) << "   could not access StoreGate ";
      return StatusCode::FAILURE;
    }

  m_detailsManager = new TauDetailsManager(storeGate);

  return StatusCode::SUCCESS;  
}

/**
 * @brief Book variables for this block.
 */
StatusCode EventCalcVarsFillerTool::book()
{
    CHECK( addVariable ("numGoodVertices", m_numGoodVertices)  );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode EventCalcVarsFillerTool::fill ()
{
  m_detailsManager->updateEvent();

  *m_numGoodVertices = m_detailsManager->getIntDetailValue(Details::NUM_PILEUP_AND_PRIMARY_VERTICES);

  return StatusCode::SUCCESS;
}


} // namespace D3PD
