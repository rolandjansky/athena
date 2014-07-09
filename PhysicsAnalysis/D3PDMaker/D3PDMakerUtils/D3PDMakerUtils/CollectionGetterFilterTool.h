// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/CollectionGetterFilterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2009
 * @brief A collection getter that filters the results of another.
 */


#ifndef D3PDMAKERUTILS_COLLECTIONGETTERFILTERTOOL_H
#define D3PDMAKERUTILS_COLLECTIONGETTERFILTERTOOL_H


#include "D3PDMakerUtils/CollectionGetterFilterToolImpl.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


/**
 * @brief A collection getter that filters the results of another.
 *
 * This collection getter tool takes another collection getter tool
 * as a parameter and applies a filter to the results of that tool.
 *
 * The nested getter is given by the @c Getter parameter.
 *
 * The filter is defined by the @c filter virtual method.
 *
 * To use this, derive from it, and implement @c filter.
 */
template <class T>
class CollectionGetterFilterTool
  : public CollectionGetterFilterToolImpl
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CollectionGetterFilterTool (const std::string& type,
                              const std::string& name,
                              const IInterface* parent);


  /**
   * @brief Apply filtering to one element.
   *
   * Returns true if the filter passes; false otherwise.
   */
  virtual bool filter (const T* p) const = 0;


  /**
   * @brief Return the element type of the collection.
   *
   * I.e., @c nextUntyped returns a pointer to this type.
   */
  virtual const std::type_info& elementTypeinfo() const;


  /**
   * @brief Return a pointer to the next element in the collection.
   *
   * Return 0 when the collection has been exhausted.
   */
  virtual const void* nextUntyped();


  /**
   * @brief Release an object retrieved from the getter.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c nextUntyped().  The default implementation is a no-op,
   * but if the getter dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseElementUntyped (const void* p);
};


} // namespace D3PD


#include "D3PDMakerUtils/CollectionGetterFilterTool.icc"


#endif // not D3PDMAKERUTILS_COLLECTIONGETTERFILTERTOOL_H
