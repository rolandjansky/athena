// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerInterfaces/ISingleAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2009
 * @brief Abstract interface to form a single association.
 */

#ifndef D3PDMAKERINTERFACES_ISINGLEASSOCIATIONTOOL_H
#define D3PDMAKERINTERFACES_ISINGLEASSOCIATIONTOOL_H


#include "D3PDMakerInterfaces/IAddVariable.h"
#include "GaudiKernel/IAlgTool.h"
#include <typeinfo>


namespace D3PD {


/**
 * @brief Abstract interface to form a single association.
 *
 * This tool forms a single association; that is, it associates an object
 * with at most one other object.  The interface is similar
 * to @c IObjGetterTool, except that @c get takes a pointer to the source
 * object for the association.
 *
 * Once you're done with the object, you should call releaseObjectUntyped().
 * This will usually be a no-op, but it allows the getter to return
 * a dynamically-allocated object.
 */
class ISingleAssociationTool
  : virtual public IAlgTool
{
public:
  /// Gaudi interface definition.
  DeclareInterfaceID(ISingleAssociationTool, 1, 0);


  /**
   * @brief Configure during initialization: book variables and type-check.
   * @param tree Our parent for tuple making.
   * @param ti Gives the type of the object being passed to @c resetUntyped.
   *
   * @c configureD3PD should check that the type of the object coming as input
   * (to @c getUntyped)
   * is compatible with what it expects, and raise an error otherwise.
   */
  virtual StatusCode configureD3PD (IAddVariable* tree,
                                    const std::type_info& ti) = 0;


  /**
   * @brief Declare tuple variables.
   *
   * This is called at the start of the first event.
   */
  virtual StatusCode book() = 0;


  /**
   * @brief Return the type of object retrieved by this tool.
   */
  virtual const std::type_info& typeinfo() const = 0;


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   * Should be of the type given by @c typeinfo.
   */
  virtual const void* getUntyped (const void* p) = 0;


  /**
   * @brief Release an object retrieved from the association.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c getUntyped().  The default implementation is a no-op,
   * but if the association dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseObjectUntyped (const void* p);
};


} // namespace D3PD


#include "D3PDMakerInterfaces/ISingleAssociationTool.icc"


#endif // not D3PDMAKERINTERFACES_ISINGLEASSOCIATIONTOOL_H
