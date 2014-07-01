// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerInterfaces/ID3PD.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2009
 * @brief Abstract interface for a D3PD tree.
 */

#ifndef D3PDMAKERINTERFACES_ID3PD_H
#define D3PDMAKERINTERFACES_ID3PD_H

#include "D3PDMakerInterfaces/IAddVariable.h"
#include "GaudiKernel/StatusCode.h"
#include <typeinfo>
#include <string>


namespace D3PD {


/**
 * @brief Define an abstract interface for building a D3PD tree.
 *
 * Variables are added via the @c IAddVariable interface.
 *
 * A D3PD also allows storing arbitrary metadata objects, identified
 * by a string key.
 */
class ID3PD
  : public IAddVariable
{
public:
  /**
   * @brief Capture the current state of all variables and write to the tuple.
   */
  virtual StatusCode capture () = 0;


  /**
   * @brief Clear all the tuple variables.
   */
  virtual StatusCode clear () = 0;


  /// Currently unimplemented --- see design note.
  typedef unsigned int Dim_t;
  virtual StatusCode redim (const Dim_t* ptr) = 0;


  /**
   * @brief Add a new piece of metadata to the tuple.
   * @param key - The key for this object.
   *              Any existing object will be overwritten.
   * @param obj - Pointer to the object to write.
   * @param ti - Type of the object to write.
   *
   * The interpretation of the @c key parameter is up to the concrete
   * D3PD implementation.  However, a key name with a trailing slash
   * NAME/ indicates that all metadata items with this name should
   * be grouped together in a collection called NAME (for example,
   * in a Root directory with that name).
   */
  virtual StatusCode addMetadata (const std::string& key,
                                  const void* obj,
                                  const std::type_info& ti) = 0;


  /**
   * @brief Add a new piece of metadata to the tuple.
   * @param key - The key for this object.
   *              Any existing object will be overwritten.
   * @param obj - Pointer to the object to write.
   *
   * The interpretation of the @c key parameter is up to the concrete
   * D3PD implementation.  However, a key name with a trailing slash
   * NAME/ indicates that all metadata items with this name should
   * be grouped together in a collection called NAME (for example,
   * in a Root directory with that name).
   */
   template <class T>
   StatusCode addMetadata (const std::string& key, const T* obj);
};


} // namespace D3PD


#include "D3PDMakerInterfaces/ID3PD.icc"


#endif // not D3PDMAKERINTERFACES_ID3PD_H
