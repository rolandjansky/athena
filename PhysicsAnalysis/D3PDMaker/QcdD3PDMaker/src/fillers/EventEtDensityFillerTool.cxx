// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventEtDensityFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief 
 */


#include "QcdD3PDMaker/fillers/EventEtDensityFillerTool.h"
#include "EventShapeInterface/IEventEtDensityProvider.h"
#include "AthenaKernel/errorcheck.h"


namespace QcdD3PD {


EventEtDensityFillerTool::EventEtDensityFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : D3PD::BlockFillerTool<void> (type, name, parent),
      m_tool ("")
{
  declareProperty("EventEtDensityTool", m_tool,
                  "Event Et density tool");
  declareProperty ("VariableSuffix", m_variableSuffix,
                   "Suffix for variable names");

  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode EventEtDensityFillerTool::initialize()   
{
  CHECK( m_tool.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode EventEtDensityFillerTool::book()   
{
  CHECK( addVariable ("rho" + m_variableSuffix,
                      m_rho, "Event Et density") );
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
StatusCode EventEtDensityFillerTool::fill()
{
  *m_rho = m_tool->rho();
  return StatusCode::SUCCESS;
}


} // namespace QcdD3PD
