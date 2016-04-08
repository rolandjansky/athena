// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerInterfaces/IMultiAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2009
 * @brief Abstract interface to form a multi-way association.
 */

#ifndef D3PDMAKERINTERFACES_IMULTIASSOCIATIONTOOL_H
#define D3PDMAKERINTERFACES_IMULTIASSOCIATIONTOOL_H


#include "GaudiKernel/IAlgTool.h"
#include "D3PDMakerInterfaces/IIteration.h"
#include <typeinfo>


namespace D3PD {


class IAddVariable;


/**
 * @brief Abstract interface to form a multi-way association.
 *
 * This tool forms a multiple association.  That is, it associates
 * from a single object to a collection of objects.  The interface
 * is similar to @c ICollectionGetterTool, except that the @c reset
 * method takes a pointer to the source object for the association.
 * @c next then iterates over the association results.
 */
class IMultiAssociationTool
  : virtual public IAlgTool,
    public IIteration
{
public:
  /// Gaudi interface definition.
  DeclareInterfaceID(IMultiAssociationTool, 1, 0);


  /**
   * @brief Configure during initialization: type-check.
   * @param tree Our parent for tuple making.
   * @param ti Gives the type of the object being passed to @c resetUntyped.
   *
   * @c configureD3PD should check that the type of the object coming as input
   * (to @c resetUntyped)
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
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode resetUntyped (const void* p) = 0;
};


} // namespace D3PD


#endif // not D3PDMAKERINTERFACES_IMULTIASSOCIATIONTOOL_H
