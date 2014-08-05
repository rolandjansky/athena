/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/TruthParticleBarcodesFillerTool.cxx
 * @author sss
 * @date Mar, 2012
 * @brief Block filler tool for TruthParticle barcode lists (parent/child)
 */

#include "TruthParticleBarcodesFillerTool.h"
#include "McParticleEvent/TruthParticle.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthParticleBarcodesFillerTool::TruthParticleBarcodesFillerTool
 (const std::string& type,
  const std::string& name,
  const IInterface* parent)
  : BlockFillerTool<TruthParticle> (type, name, parent)
{
  book().ignore();  // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode TruthParticleBarcodesFillerTool::book()
{
  CHECK( addVariable("parents",   m_parents) );
  CHECK( addVariable("children",  m_children) );
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
StatusCode TruthParticleBarcodesFillerTool::fill (const TruthParticle& p)
{
  m_parents->reserve (p.nParents());
  for(unsigned int i=0; i < p.nParents(); i++)
  {
    m_parents->push_back(p.mother(i)->barcode());
  }
  std::sort (m_parents->begin(),  m_parents->end());
  m_children->reserve (p.nDecay());
  for(unsigned int i=0; i < p.nDecay(); i++)
  {
    m_children->push_back(p.child(i)->barcode());
  }
  std::sort (m_children->begin(),  m_children->end());
  return StatusCode::SUCCESS;
}


} // namespace D3PD
