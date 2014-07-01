/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SelectedParticlesFilterTool.cxx 486128 2012-03-02 08:54:20Z krasznaa $
/**
 * @file D3PDMakerCoreComps/srcSelectedParticlesFilterTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2010
 * @brief A collection getter that filters the results of another
 *        based on the contents of a @c SelectedParticles object.
 */


#include "SelectedParticlesFilterTool.h"
#include "ParticleEvent/SelectedParticles.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
SelectedParticlesFilterTool::SelectedParticlesFilterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : CollectionGetterFilterToolImpl (type, name, parent),
      m_selgetter (this),
      m_selection (0),
      m_index (0)
{
  declareProperty ("SelectionGetter", m_selgetter,
                   "Getter for the SelectedParticles object.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode SelectedParticlesFilterTool::initialize()
{
  CHECK( CollectionGetterFilterToolImpl::initialize() );
  CHECK( m_selgetter.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the element type of the collection.
 *
 * I.e., @c nextUntyped returns a pointer to this type.
 */
const std::type_info& SelectedParticlesFilterTool::elementTypeinfo() const
{
  return m_getter->elementTypeinfo();
}


/**
 * @brief Reset the iteration to the start of the collection.
 * @param allowMissing If true, then we should not generate errors
 *        if the requested object is missing.
 *
 * Return failure if the container cannot be retrieved.
 */
StatusCode SelectedParticlesFilterTool::reset (bool allowMissing /*= false*/)
{
  m_index = 0;
  if (m_selection) {
    m_selgetter->releaseObject (m_selection);
    m_selection = 0;
  }
  CHECK( CollectionGetterFilterToolImpl::reset (allowMissing) );
  m_selection = m_selgetter->get<SelectedParticles> (allowMissing);
  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the collection.
 *
 * Return 0 when the collection has been exhausted.
 */
const void* SelectedParticlesFilterTool::nextUntyped()
{
  // Loop until the filter passes.
  while (const void* p = m_getter->nextUntyped()) {
    ++m_index;
    if (!m_selection || m_selection->isGood (m_index-1))
      return p;
    else 
      m_getter->releaseElementUntyped (p);
  }
  if (m_selection) {
    m_selgetter->releaseObject (m_selection);
    m_selection = 0;
  }
  return 0;
}


/**
 * @brief Release an object retrieved from the getter.
 * @param p The object to release.
 *
 * Call this when you are done with the object returned by
 * @c nextUntyped().  The default implementation is a no-op,
 * but if the getter dynamically allocated the object which
 * it returned, this gives it a chance to free it.
 */
void SelectedParticlesFilterTool::releaseElementUntyped (const void* p)
{
  return m_getter->releaseElementUntyped (p);
}


} // namespace D3PD
