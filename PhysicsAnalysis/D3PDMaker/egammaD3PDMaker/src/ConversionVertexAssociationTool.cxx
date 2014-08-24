/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ConversionVertexAssociationTool.cxx 605545 2014-07-09 04:51:09Z ssnyder $
/**
 * @file PhotonD3PDMaker/src/ConversionTrackParticleAssociationTool.cxx
 * @author Mike Hance <hance@hep.upenn.edu>, based on similar tools by scott snyder <snyder@bnl.gov>
 * @date August 2010
 * @brief Associate from a conversion to its vertex.
 */


#include "ConversionVertexAssociationTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ConversionVertexAssociationTool::ConversionVertexAssociationTool (const std::string& type,
								  const std::string& name,
								  const IInterface* parent)
  : Base (type, name, parent),
    m_vxCounter(0),
    m_ph(0)
{
}


/**
 * @brief Create any needed tuple variables.
 */
StatusCode ConversionVertexAssociationTool::reset (const xAOD::Photon& ph)
{
  m_vxCounter = 0;
  m_ph = &ph;
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const xAOD::Vertex*
ConversionVertexAssociationTool::next ()
{
  if(!m_ph || m_vxCounter >= m_ph->nVertices())
    return 0;

  return m_ph->vertex(m_vxCounter++);
}


} // namespace D3PD
