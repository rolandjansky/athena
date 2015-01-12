// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/CollectionGetterToolImpl.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Non-template parts of @c CollectionGetterTool.
 */


#ifndef D3PDMAKERUTILS_COLLECTIONGETTERTOOLIMPL_H
#define D3PDMAKERUTILS_COLLECTIONGETTERTOOLIMPL_H


#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterRegistryTool.h"
#include "D3PDMakerUtils/ObjGetterToolImpl.h"
#include "D3PDMakerUtils/TypeConverter.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {



/**
 * @brief Non-template parts of @c CollectionGetterTool.
 *
 * @c CollectionGetterTool extends @c ObjGetterTool to allow iterating
 * over the objects within a container.  @c CollectionGetterTool
 * is templated on the collection type.  This class factors out
 * the pieces that don't depend on the template argument.
 */
class CollectionGetterToolImpl
  : public extends1<ObjGetterToolImpl, ICollectionGetterTool>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CollectionGetterToolImpl (const std::string& type,
                            const std::string& name,
                            const IInterface* parent);


  /// Standard Gaudi @c initialize method.
  StatusCode initialize();


  /**
   * @brief Return the next object cast to a different pointer type.
   *
   * Return the next element from the collection
   * as a pointer to the @c ti type.
   * Return 0 if the pointer can't be converted or at the end of the
   * iteration.
   */
  const void* nextTypeinfo (const std::type_info& ti);


  /**
   * @brief Test type compatibility.
   * @param ti The desired type.
   *
   * Test to see if the object being returned by the tool can be converted
   * to a pointer to @c T.  This can be used to perform type checks during job
   * initialization.
   */
  StatusCode configureElementTypeinfo (const std::type_info& ti);


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
  void releaseElementTypeinfo (const void* p,
                               const std::type_info& ti);


private:
  /// Property: label to assign to this getter (or null).
  std::string m_label;

  /// The collection getter registry tool.
  ToolHandle<ICollectionGetterRegistryTool> m_registry;

  /// Helper to convert pointers.
  TypeConverter m_converter;

  /// Helper to convert pointers back, for @c releaseElement.
  TypeConverter m_backConverter;
};


} // namespace D3PD


#endif // not D3PDMAKERUTILS_COLLECTIONGETTERTOOLIMPL_H
