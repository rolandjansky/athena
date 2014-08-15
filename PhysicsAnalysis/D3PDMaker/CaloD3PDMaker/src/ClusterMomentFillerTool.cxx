/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/ClusterMomentFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for CaloCluster moments.
 */


#include "ClusterMomentFillerTool.h"
#include "CaloEvent/CaloCluster.h"
#include "AthenaKernel/errorcheck.h"
#include <sstream>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ClusterMomentFillerTool::ClusterMomentFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : BlockFillerTool<CaloCluster> (type, name, parent)
{
  declareProperty ("Moments", m_moments);
}


/**
 * @brief Book variables for this block.
 */
StatusCode ClusterMomentFillerTool::book()
{
  m_vars.resize (m_moments.size() / 2);
  for (size_t i = 0; i < m_vars.size(); ++i) {
    m_vars[i].key =
      static_cast<CaloClusterMoment::MomentType>
      (std::atoi (m_moments[2*i].c_str()));
    CHECK( addVariable (m_moments[2*i+1], m_vars[i].x) );
  }

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
StatusCode ClusterMomentFillerTool::fill (const CaloCluster& p)
{
  for (size_t i = 0; i < m_vars.size(); i++) {
    Var& var = m_vars[i];
    *var.x = static_cast<float> (p.getMomentValue (var.key));
  }
  
  return StatusCode::SUCCESS;
}


} // namespace D3PD
