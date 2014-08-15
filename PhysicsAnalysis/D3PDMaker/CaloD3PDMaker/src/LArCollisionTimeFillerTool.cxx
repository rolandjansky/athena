/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/LArCollisionTimeFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2010
 * @brief Block filler tool for collision time information.
 */


#include "LArCollisionTimeFillerTool.h"
#include "LArRecEvent/LArCollisionTime.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
LArCollisionTimeFillerTool::LArCollisionTimeFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : BlockFillerTool<LArCollisionTime> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Book variables for this block.
 */
StatusCode LArCollisionTimeFillerTool::book()
{
  CHECK( addVariable ("ncellA", m_ncellA,
                      "Number of A-side cells for time calculation.") );
  CHECK( addVariable ("ncellC", m_ncellC,
                      "Number of C-side cells for time calculation.") );
  CHECK( addVariable ("energyA", m_energyA,
                      "Total A-side energy.") );
  CHECK( addVariable ("energyC", m_energyC,
                      "Total C-side energy.") );
  CHECK( addVariable ("timeA", m_timeA,
                      "A-side time.") );
  CHECK( addVariable ("timeC", m_timeC,
                      "C-side time.") );
  CHECK( addVariable ("timeDiff", m_timeDiff,
                      "A-C LAr cell time difference.") );
 
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
StatusCode LArCollisionTimeFillerTool::fill (const LArCollisionTime& c)
{
  *m_ncellA   = c.ncellA();
  *m_ncellC   = c.ncellC();
  *m_energyA  = c.energyA();
  *m_energyC  = c.energyC();
  *m_timeA    = c.timeA();
  *m_timeC    = c.timeC();
  *m_timeDiff = *m_timeA - *m_timeC;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
