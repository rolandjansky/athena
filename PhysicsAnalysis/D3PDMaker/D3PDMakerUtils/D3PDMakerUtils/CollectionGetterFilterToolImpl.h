// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/CollectionGetterFilterToolImpl.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2009
 * @brief Non-template parts of @c CollectionGetterFilterTool.
 */


#ifndef D3PDMAKERUTILS_COLLECTIONGETTERFILTERTOOLIMPL_H
#define D3PDMAKERUTILS_COLLECTIONGETTERFILTERTOOLIMPL_H


#include "D3PDMakerUtils/CollectionGetterToolImpl.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


/**
 * @brief Non-template parts of @c CollectionGetterFilterTool.
 *
 * @c CollectionGetterFilterTool is a getter that filters another
 * collection getter.  @c CollectionGetterFilterTool is templated
 * on the object type.  This class factors out the pieces
 * that don't depend on the template argument.
 */
class CollectionGetterFilterToolImpl
  : public CollectionGetterToolImpl
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CollectionGetterFilterToolImpl (const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent);


  /**
   * @brief Standard Gaudi initialize method.
   */
  StatusCode initialize();


  /**
   * @brief Reset the iteration to the start of the collection.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * Return failure if the container cannot be retrieved.
   */
  virtual StatusCode reset (bool allowMissing = false);


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
  virtual size_t sizeHint (bool allowMissing = false);


  /**
   * @brief Return the type of object retrieved by this tool.
   */
  virtual const std::type_info& typeinfo() const;


  /**
   * @brief Return the target object.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * Should be of the type given by @c typeinfo.
   * Return 0 on failure.
   */
  virtual const void* getUntyped (bool allowMissing = false);


  /**
   * @brief Release an object retrieved from the getter.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c getUntyped().  The default implementation is a no-op,
   * but if the getter dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseObjectUntyped (const void* p);


protected:
  /// Property: The getter defining the sequence to filter.
  ToolHandle<ICollectionGetterTool> m_getter;
};


} // namespace D3PD


#endif // not D3PDMAKERUTILS_COLLECTIONGETTERFILTERTOOLIMPL_H
