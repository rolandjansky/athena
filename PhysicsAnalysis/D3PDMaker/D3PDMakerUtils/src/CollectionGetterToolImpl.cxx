/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/src/CollectionGetterToolImpl.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Non-template parts of @c CollectionGetterTool.
 */


#include "D3PDMakerUtils/CollectionGetterToolImpl.h"
#include "D3PDMakerInterfaces/ICollectionGetterRegistryTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
CollectionGetterToolImpl::CollectionGetterToolImpl (const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent)
  : base_class (type, name, parent)
{
  declareProperty ("Label", m_label,
                   "Label to assign to this getter, to be able to reference "
                   "it from an association tool.  Leave blank if no label "
                   "is needed.");
  declareProperty ("CollectionGetterRegistry", m_registry,
                   "Collection getter registry tool");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode CollectionGetterToolImpl::initialize()
{
  // If a label was provided, add ourselves to the registry.
  if (!m_label.empty()) {
    CHECK( m_registry.retrieve() );
    CHECK( m_registry->add (m_label, this) );
  }
  return ObjGetterToolImpl::initialize();
}


/**
 * @brief Return the next object cast to a different pointer type.
 *
 * Return the next element from the collection
 * as a pointer to the @c ti type.
 * Return 0 if the pointer can't be converted or at the end of the
 * iteration.
 */
const void*
CollectionGetterToolImpl::nextTypeinfo (const std::type_info& ti)
{
  // Configure the converter.
  if (!this->m_converter.isValid() ||
      this->m_converter.dstTypeinfo() != ti)
  {
    if (this->configureElementTypeinfo(ti).isFailure())
      return 0;
  }

  // Get the next object.  If the conversion fails, loop until we get
  // a good one.
  while (true) {
    const void* p = this->nextUntyped();
    if (!p) return 0;
    const void* pconv = this->m_converter.convertUntyped (p);
    if (pconv) return pconv;
    this->releaseElementUntyped (p);
    REPORT_MESSAGE (MSG::WARNING)
      << "Pointer conversion from " << m_converter.srcName() << " to "
      << m_converter.dstName() << "failed.";
  }
}


/**
 * @brief Release an element retrieved from the getter.
 * @param p The element to release.
 * @param ti The type of p.
 *
 * Call this when you are done with the element returned by
 * @c nextUntyped().  The default implementation is a no-op,
 * but if the getter dynamically allocated the object which
 * it returned, this gives it a chance to free it.
 */
void
CollectionGetterToolImpl::releaseElementTypeinfo (const void* p,
                                                  const std::type_info& ti)
{
  if (!p)
    return;

  // Configure the converter if needed.
  if (!this->m_backConverter.isValid() ||
      this->m_backConverter.srcTypeinfo() != ti)
  {
    if (this->m_backConverter.init (ti, this->typeinfo()).isFailure()) {
      REPORT_MESSAGE (MSG::WARNING)
        << "Can't configure pointer conversion from "
        << System::typeinfoName (ti) << " to "
        << System::typeinfoName (this->typeinfo());
      return;
    }
  }

  // Convert to the desired type.
  p = this->m_backConverter.convertUntyped (p);

  // Release.
  if (p)
    this->releaseElementUntyped (p);
  else {
    REPORT_MESSAGE (MSG::WARNING)
      << "Pointer conversion from " << m_backConverter.srcName() << " to "
      << m_backConverter.dstName() << "failed.";
  }
}


/**
 * @brief Test type compatibility.
 * @param ti The desired type.
 *
 * Test to see if the object being returned by the tool can be converted
 * to a pointer to @c T.  This can be used to perform type checks during job
 * initialization.
 */
StatusCode
CollectionGetterToolImpl::configureElementTypeinfo
   (const std::type_info& ti)
{
  return this->m_converter.init(this->elementTypeinfo(), ti);
}


} // namespace D3PD
