/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDMaker/src/TruthParticleChildAssociationTool.cxx
 * @author Ryan Reece  <ryan.reece@cern.ch>
 * @date Mar 2010
 * @brief Associate all the children of a TruthParticle.
 */


#include "TruthParticleChildAssociationTool.h"
#include "xAODTruth/TruthVertex.h"
#include "barcodeOrder.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthParticleChildAssociationTool::TruthParticleChildAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_i (0)
{}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TruthParticleChildAssociationTool::initialize()
{
  return Base::initialize();
}


/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode
TruthParticleChildAssociationTool::reset (const xAOD::TruthParticle& p)
{
  m_i = 0;

  const xAOD::TruthVertex* vx = p.decayVtx();
  if (!vx) {
    m_xaod_children.clear();
    return StatusCode::SUCCESS;
  }

  size_t sz = vx->nOutgoingParticles();
  m_xaod_children.reserve (sz);
  for (unsigned int i = 0; i < sz; i++) {
    const xAOD::TruthParticle* p = vx->outgoingParticle (i);
    if (p)
      m_xaod_children[i] =  p;
  }
  std::sort (m_xaod_children.begin(), m_xaod_children.end(), barcodeOrder());
  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const xAOD::TruthParticle* TruthParticleChildAssociationTool::next()
{
  if (m_i < m_xaod_children.size())
    return m_xaod_children[m_i++];
  return 0;
}


} // namespace D3PD
