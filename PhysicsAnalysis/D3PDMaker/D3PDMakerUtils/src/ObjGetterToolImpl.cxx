/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/src/ObjGetterToolImpl.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Non-template parts of @c ObjGetterTool.
 */


#include "D3PDMakerUtils/ObjGetterToolImpl.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/System.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ObjGetterToolImpl::ObjGetterToolImpl (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : base_class (type, name, parent)
{
}


/**
 * @brief Return the target object cast to a different pointer type.
 * @param ti The desired type.
 * @param allowMissing If true, then we should not generate errors
 *        if the requested object is missing.
 *
 * Return the object as a pointer to the @c ti type.
 * Return 0 if the get fails or if the pointer can't be converted.
 */
const void* ObjGetterToolImpl::getTypeinfo (const std::type_info& ti,
                                            bool allowMissing /*= false*/)
{
  // Configure the converter if needed.
  if (!this->m_converter.isValid() ||
      this->m_converter.dstTypeinfo() != ti)
  {
    if (this->configureTypeinfo(ti).isFailure())
      return 0;
  }

  // Get the object.
  const void* p = this->getUntyped (allowMissing);

  // Convert to the desired type.
  if (p) {
    p = this->m_converter.convertUntyped (p);
    if (!p) {
      REPORT_MESSAGE (MSG::WARNING)
        << "Pointer conversion from " << m_converter.srcName() << " to "
        << m_converter.dstName() << "failed.";
    }
  }
  return p;
}


/**
 * @brief Release an object retrieved from the getter.
 * @param p The object to release.
 * @param ti The type of p.
 *
 * Call this when you are done with the object returned by
 * @c getUntyped().  The default implementation is a no-op,
 * but if the getter dynamically allocated the object which
 * it returned, this gives it a chance to free it.
 */
void ObjGetterToolImpl::releaseObjectTypeinfo (const void* p,
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
    this->releaseObjectUntyped (p);
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
ObjGetterToolImpl::configureTypeinfo (const std::type_info& ti)
{
  return this->m_converter.init(this->typeinfo(), ti);
}


} // namespace D3PD
