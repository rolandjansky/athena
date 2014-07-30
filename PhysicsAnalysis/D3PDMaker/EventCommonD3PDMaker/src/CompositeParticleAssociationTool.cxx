// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/*
 * @file EventCommonD3PDMaker/src/CompositeParticleAssociationTool.cxx>
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2011
 * @brief Associate from a CompositeParticle to its constituents.
 */


#include "CompositeParticleAssociationTool.h"
#include "ParticleEvent/CompositeParticle.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
CompositeParticleAssociationTool::CompositeParticleAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_it(),
      m_end()
{
}


/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode CompositeParticleAssociationTool::reset (const CompositeParticle& p)
{
  m_it = p.navigableBase().begin();
  m_end = p.navigableBase().end();
  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const INavigable4Momentum* CompositeParticleAssociationTool::next()
{
  if (m_it >= m_end)
    return 0;
  return *m_it++;
}


} // namespace D3PD
