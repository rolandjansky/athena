/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventCommonD3PDMaker/src/ChargeFillerTool.cxx
 * @author Serhan Mete, scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for @c IParticle::charge().
 */


#include "ChargeFillerTool.h"
#include "EventKernel/IParticle.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ChargeFillerTool::ChargeFillerTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
  : BlockFillerTool<IParticle> (type, name, parent)
{
  ChargeFillerTool::book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode ChargeFillerTool::book()
{
  CHECK( addVariable ("charge",  m_charge)  );
 
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
StatusCode ChargeFillerTool::fill (const IParticle& p)
{
  if (p.hasCharge())
    *m_charge  = static_cast<float> (p.charge());
  
  return StatusCode::SUCCESS;
}


} // namespace D3PD
