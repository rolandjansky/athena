// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/ObjGetterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Type-safe wrapper for object getter tools.
 */


#ifndef D3PDMAKERUTILS_OBJGETTERTOOL_H
#define D3PDMAKERUTILS_OBJGETTERTOOL_H


#include "D3PDMakerUtils/ObjGetterToolImpl.h"


namespace D3PD {


/**
 * @brief This is a type-safe wrapper for the @c IObjGetterTool interface.
 *
 * The template argument gives the type of object being retrieved.
 * Derived classes should implement the @c get() method, to retrieve
 * an object, and optionally releaseObject() to dispose of an object.
 */
template <class T>
class ObjGetterTool
  : public ObjGetterToolImpl
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ObjGetterTool (const std::string& type,
                 const std::string& name,
                 const IInterface* parent);


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
   * @brief Return the type of object retrieved by this tool.
   */
  virtual const std::type_info& typeinfo() const;


  /**
   * @brief Return the target object.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * Return 0 on failure.
   */
  virtual const T* get (bool allowMissing = false) = 0;


  /**
   * @brief Release an object retrieved from the getter.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c get().  The default implementation is a no-op,
   * but if the getter dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseObject (const T* p);


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
};


} // namespace D3PD


#include "D3PDMakerUtils/ObjGetterTool.icc"


#endif // not D3PDMAKERUTILS_OBJGETTERTOOL_H
