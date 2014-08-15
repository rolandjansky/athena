/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/CaloCellRawFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2012
 * @brief Block filler tool for calo cell raw eta/phi.
 */


#include "CaloCellRawFillerTool.h"
#include "CaloEvent/CaloCell.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
CaloCellRawFillerTool::CaloCellRawFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : BlockFillerTool<CaloCell> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Book variables for this block.
 */
StatusCode CaloCellRawFillerTool::book()
{
  CHECK( addVariable ("raw_eta", m_raw_eta, "Cell raw eta.") );
  CHECK( addVariable ("raw_phi", m_raw_phi, "Cell raw phi.") );
 
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p Object from which to fill.
 */
StatusCode CaloCellRawFillerTool::fill (const CaloCell& p)
{
  *m_raw_eta = p.caloDDE()->eta_raw();
  *m_raw_phi = p.caloDDE()->phi_raw();
  return StatusCode::SUCCESS;
}


} // namespace D3PD
