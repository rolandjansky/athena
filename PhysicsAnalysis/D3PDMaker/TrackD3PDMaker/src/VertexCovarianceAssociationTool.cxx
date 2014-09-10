/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VertexCovarianceAssociationTool.cxx 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/VertexCovarianceAssociationTool.cxx
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a VxCandidate to its position ErrorMatrix.
 */

#include "VertexCovarianceAssociationTool.h"
#include "VxVertex/VxCandidate.h"

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
VertexCovarianceAssociationTool::VertexCovarianceAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
  const AmgSymMatrix(3) *VertexCovarianceAssociationTool::get (const Trk::VxCandidate& p)
{
  return &(p.recVertex().covariancePosition());
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const AmgSymMatrix(3)*
VertexCovarianceAssociationTool::get (const xAOD::Vertex& p)
{
  return &p.covariancePosition();
}


} // namespace D3PD
