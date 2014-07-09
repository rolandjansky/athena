// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/CollectionGetterTool.h 
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Type-safe wrapper for collection getter tools.
 */


#ifndef D3PDMAKERUTILS_COLLECTIONGETTERTOOL_H
#define D3PDMAKERUTILS_COLLECTIONGETTERTOOL_H


#include "D3PDMakerUtils/CollectionGetterToolImpl.h"
#include "DataModel/ElementLink.h"
#include "DataModel/DataLink.h"


namespace D3PD {


/**
 * @brief Type-safe wrapper for @c ICollectionGetterTool.
 *
 * The template argument gives the type of the container that we're
 * returning.  This class implements the iteration over the container.
 * If your container obeys the standard iteration protocol (for a
 * @c forward_iterator), then you just need to derive from this class
 * and implement @c get(), and optionally @c releaseObject().
 */
template <class CONT>
class CollectionGetterTool
  : public CollectionGetterToolImpl
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CollectionGetterTool (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * Should be of the type given by @c typeinfo.
   * Return 0 on failure.
   *
   * This is implemented by calling @c get().
   */
  virtual const void* getUntyped (bool allowMissing = false);


  /**
   * @brief Return the type of the collection object retrieved by this tool.
   */
  virtual const std::type_info& typeinfo() const;


  /**
   * @brief Return the element type of the collection.
   *
   * I.e., @c nextUntyped returns a pointer to this type.
   */
  virtual const std::type_info& elementTypeinfo() const;


  /**
   * @brief Reset the iteration to the start of the collection.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * Return failure if the container cannot be retrieved.
   */
  virtual StatusCode reset (bool allowMissing = false);


  /**
   * @brief Return a pointer to the next element in the collection.
   *
   * Return 0 when the collection has been exhausted.
   */
  virtual const void* nextUntyped();


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
   * @brief Return the collection object.  Return 0 on failure.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   */
  virtual const CONT* get (bool allowMissing = false) = 0;


  /**
   * @brief Release an object retrieved from the getter.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c get().  The default implementation is a no-op,
   * but if the getter dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseObject (const CONT* p);


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


private:
  /// The current iterator.
  typename CONT::const_iterator m_it;

  /// The iterator at the end of the container.
  typename CONT::const_iterator m_end;
};


} // namespace D3PD


#include "D3PDMakerUtils/CollectionGetterTool.icc"


#endif // not D3PDMAKERUTILS_OBJGETTERTOOL_H
