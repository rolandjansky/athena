/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDMaker/src/TruthParticleParentAssociationTool.cxx
 * @author Ryan Reece  <ryan.reece@cern.ch>
 * @date Mar 2010
 * @brief Associate all the parents of a TruthParticle.
 */


#include "TruthParticleParentAssociationTool.h"
#include "barcodeOrder.h"
#include "xAODTruth/TruthVertex.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthParticleParentAssociationTool::TruthParticleParentAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_i (0)
{}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TruthParticleParentAssociationTool::initialize()
{
  return Base::initialize();
}


/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode
TruthParticleParentAssociationTool::reset (const xAOD::TruthParticle& p)
{
  const xAOD::TruthVertex* vx = p.prodVtx();
  if (!vx)
    m_parents.clear();
  else {
    size_t sz = vx->nIncomingParticles();
    m_parents.resize (sz);
    for (unsigned int i = 0; i < sz; i++)
      m_parents[i] = vx->incomingParticle(i);
    std::sort (m_parents.begin(), m_parents.end(), barcodeOrder());
    m_i = 0;
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const xAOD::TruthParticle* TruthParticleParentAssociationTool::next()
{
  if (m_i < m_parents.size())
    return m_parents[m_i++];
  return 0;
}


} // namespace D3PD
