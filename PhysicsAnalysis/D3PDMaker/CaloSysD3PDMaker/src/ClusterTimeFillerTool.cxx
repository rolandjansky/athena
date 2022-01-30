/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloSysD3PDMaker/src/ClusterTimeFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Block filler tool for time information from a CaloCluster.
 */


#include "ClusterTimeFillerTool.h"
#include "CaloEvent/CaloCluster.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ClusterTimeFillerTool::ClusterTimeFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : BlockFillerTool<CaloCluster> (type, name, parent)
{
  ClusterTimeFillerTool::book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Book variables for this block.
 */
StatusCode ClusterTimeFillerTool::book()
{
  CHECK( addVariable ("time", m_time) );
 
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
StatusCode ClusterTimeFillerTool::fill (const CaloCluster& p)
{
  *m_time = p.getTime();

  return StatusCode::SUCCESS;
}


} // namespace D3PD
