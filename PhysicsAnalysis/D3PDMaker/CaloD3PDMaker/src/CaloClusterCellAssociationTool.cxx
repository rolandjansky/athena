/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/CaloClusterCellAssociationTool.cxx
 * @author Hong Ma <hma@bnl.gov>
 * @date April, 2010
 * @brief Associate from a CaloCluster to CaloCell
 */


#include "CaloClusterCellAssociationTool.h"



namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */

CaloClusterCellAssociationTool::CaloClusterCellAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_icell(0)
{


}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
StatusCode CaloClusterCellAssociationTool::reset (const CaloCluster& p)
{
  m_icell = 0;
  m_cells.clear();

  // Protect against missing cells.
  if (p.isCellLinkValid())
    m_cells.assign (p.cell_begin(), p.cell_end());

  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
StatusCode CaloClusterCellAssociationTool::reset (const xAOD::CaloCluster& p)
{
  m_icell = 0;
  m_cells.clear();

  // Protect against missing cells.
  if (p.getCellLinks())
    m_cells.assign (p.cell_begin(), p.cell_end());

  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const CaloCell* CaloClusterCellAssociationTool::next()
{ 
  const CaloCell* out= 0;
  if (m_icell < m_cells.size())
    out = m_cells[m_icell++];
  return out;
} 

} // namespace D3PD
