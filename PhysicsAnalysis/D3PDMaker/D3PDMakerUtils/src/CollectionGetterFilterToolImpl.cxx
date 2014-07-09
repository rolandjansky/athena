/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/src/CollectionGetterFilterToolImpl.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2009
 * @brief Non-template parts of @c CollectionGetterFilterTool.
 */


#include "D3PDMakerUtils/CollectionGetterFilterToolImpl.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
CollectionGetterFilterToolImpl::CollectionGetterFilterToolImpl
 (const std::string& type,
  const std::string& name,
  const IInterface* parent)
   : CollectionGetterToolImpl (type, name, parent),
     m_getter (this)
{
  declareProperty ("Getter", m_getter,
                   "Getter defining the sequence to filter.");
}



/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode CollectionGetterFilterToolImpl::initialize()
{
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureElementTypeinfo (elementTypeinfo()) );
  return CollectionGetterToolImpl::initialize();
}


/**
 * @brief Reset the iteration to the start of the collection.
 * @param allowMissing If true, then we should not generate errors
 *        if the requested object is missing.
 *
 * Return failure if the container cannot be retrieved.
 */
StatusCode CollectionGetterFilterToolImpl::reset (bool allowMissing/*= false*/)
{
  return m_getter->reset (allowMissing);
}


/**
 * @brief Return an estimate of the number of elements in the iteration.
 * @param allowMissing If true, then we should not generate errors
 *        if the requested object is missing.
 *
 * This can be used to pre-allocate memory.
 * (It's possible that this isn't known in advance of
 * iterating over the entire collection, for example
 * if a selection is being applied, so this is only a hint.)
 */
size_t CollectionGetterFilterToolImpl::sizeHint (bool allowMissing /*= false*/)
{
  return m_getter->sizeHint (allowMissing);
}


/**
 * @brief Return the type of object retrieved by this tool.
 */
const std::type_info& CollectionGetterFilterToolImpl::typeinfo() const
{
  return m_getter->typeinfo();
}


/**
 * @brief Return the target object.
 * @param allowMissing If true, then we should not generate errors
 *        if the requested object is missing.
 *
 * Should be of the type given by @c typeinfo.
 * Return 0 on failure.
 */
const void*
CollectionGetterFilterToolImpl::getUntyped (bool allowMissing /*= false*/)
{
  return m_getter->getUntyped (allowMissing);
}


/**
 * @brief Release an object retrieved from the getter.
 * @param p The object to release.
 *
 * Call this when you are done with the object returned by
 * @c getUntyped().  The default implementation is a no-op,
 * but if the getter dynamically allocated the object which
 * it returned, this gives it a chance to free it.
 */
void
CollectionGetterFilterToolImpl::releaseObjectUntyped (const void* p)
{
  return m_getter->releaseObjectUntyped (p);
}


} // namespace D3PD
