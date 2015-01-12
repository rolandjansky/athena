// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/ObjGetterToolImpl.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Non-template parts of @c ObjGetterTool.
 */


#ifndef D3PDMAKERUTILS_OBJGETTERTOOLIMPL_H
#define D3PDMAKERUTILS_OBJGETTERTOOLIMPL_H


#include "D3PDMakerInterfaces/IObjGetterTool.h"
#include "D3PDMakerUtils/TypeConverter.h"
#include "AthenaBaseComps/AthAlgTool.h"


namespace D3PD {


/**
 * @brief Non-template parts of @c ObjGetterTool.
 *
 * The class @c ObjGetterTool provides a type-safe wrapper for the
 * @c IObjGetterTool interface; it is templated on the type of
 * object on which it retrieves.  This class serves as a common base
 * class for the @c ObjGetterToolTool instantiations, and factors
 * out all the code that doesn't depend on the template argument.
 * It may also be useful for other getter classes that don't use
 * @c ObjGetterTool.
 */       
class ObjGetterToolImpl
  : public extends1<AthAlgTool, IObjGetterTool>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ObjGetterToolImpl (const std::string& type,
                     const std::string& name,
                     const IInterface* parent);


  /**
   * @brief Return the target object cast to a different pointer type.
   * @param ti The desired type.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * Return the object as a pointer to the @c ti type.
   * Return 0 if the get fails or if the pointer can't be converted.
   */
  virtual const void* getTypeinfo (const std::type_info& ti,
                                   bool allowMissing = false);


  /**
   * @brief Test type compatibility.
   * @param ti The desired type.
   *
   * Test to see if the object being returned by the tool can be converted
   * to a pointer to @c T.  This can be used to perform type checks during job
   * initialization.
   */
  virtual StatusCode configureTypeinfo (const std::type_info& ti);


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
  void releaseObjectTypeinfo (const void* p,
                              const std::type_info& ti);


private:
  /// Helper to convert pointers.
  TypeConverter m_converter;

  /// Helper to convert pointers back, for @c releaseObject.
  TypeConverter m_backConverter;
};


} // namespace D3PD


#endif // not D3PDMAKERUTILS_OBJGETTERTOOLIMPL_H
