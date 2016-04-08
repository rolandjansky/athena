// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerInterfaces/IObjGetterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2009
 * @brief Abstract interface to get an object to put in the tuple.
 */

#ifndef D3PDMAKERINTERFACES_IOBJGETTERTOOL_H
#define D3PDMAKERINTERFACES_IOBJGETTERTOOL_H


#include "GaudiKernel/IAlgTool.h"


namespace D3PD {


/**
 * @brief Abstract interface to get an object to put in the tuple.
 *
 * The input to an object filler tool is provided by a tool of type
 * @c IObjGetterTool.  This will return an object or an object container
 * of a given type.  It is expected that the usual case for this will
 * be to fetch an object from StoreGate; generic code will be provided
 * for this case and for the case of retrieving the results of a particle
 * selection algorithm.  Other tools can be defined for specific
 * special cases, such as using selections stored in a top inputs object,
 * or for performing additional selections.
 *
 * User-defined getters generally shouldn't implement this interface
 * directly, but instead use the type-safe wrappers
 * provided by @c ObjGetterTool.
 *
 * Once you're done with the object, you should call releaseObject().
 * This will usually be a no-op, but it allows the getter to return
 * a dynamically-allocated object.
 */
class IObjGetterTool
  : virtual public IAlgTool
{
public:
  /// Gaudi interface definition.
  DeclareInterfaceID(IObjGetterTool, 1, 0);


  /**
   * @brief Return the type of object retrieved by this tool.
   */
  virtual const std::type_info& typeinfo() const = 0;


  /**
   * @brief Return the target object.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * Should be of the type given by @c typeinfo.
   * Return 0 on failure.
   */
  virtual const void* getUntyped (bool allowMissing = false) = 0;


  /**
   * @brief Type-safe wrapper for @c get.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * Return the object as a pointer to @c T.
   * Return 0 if the get fails or if the pointer can't be converted.
   *
   * This is implemented in terms of @c getTypeinfo().
   */
  template <class T>
  const T* get (bool allowMissing = false);


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


  /**
   * @brief Type-safe wrapper for @c releaseObjectUntyped.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c get().  The default implementation is a no-op,
   * but if the getter dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   *
   * This is implemented in terms of @c releaseObjectTypeinfo().
   */
  template <class T>
  void releaseObject (const T* p);


  /**
   * @brief Test type compatibility.
   *
   * Test to see if the object being returned by the tool can be converted
   * to a pointer to @c T.  This can be used to perform type checks during job
   * initialization.
   *
   * This is implemented in terms of @c configureTypeinfo.
   */
  template <class T>
  StatusCode configureD3PD();


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
                                   bool allowMissing = false) = 0;


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


  /**
   * @brief Test type compatibility.
   * @param ti The desired type.
   *
   * Test to see if the object being returned by the tool can be converted
   * to a pointer to @c T.  This can be used to perform type checks during job
   * initialization.
   */
  virtual StatusCode configureTypeinfo (const std::type_info& ti) = 0;
};


} // namespace D3PD


#include "D3PDMakerInterfaces/IObjGetterTool.icc"


#endif // not D3PDMAKERINTERFACES_IOBJGETTERTOOL_H
