// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerInterfaces/IObjFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2009
 * @brief Abstract interface for tool to fill an object in the D3PD.
 */

#ifndef D3PDMAKERINTERFACES_IOBJFILLERTOOL_H
#define D3PDMAKERINTERFACES_IOBJFILLERTOOL_H


#include "GaudiKernel/IAlgTool.h"


namespace D3PD {


class IAddVariable;


/**
 * @brief Abstract interface for tool to fill an object in the D3PD.
 *
 * Each tree consists of a set of @e objects.  Each of these will generally
 * correspond to one C++ object, or set of objects in a container.
 * Each object is filled by a tool of type @c IObjFillerTool.
 * (These tools are intended to be generic; the object-specific code
 * is in block filling).  For the case of objects in a container,
 * all the filled tuple leaves should start with a common prefix
 * (e.g., @c `el_'), and a count leaf will be automatically added (@c `el_n').
 * For single objects not in a container, a prefix is optional; but if
 * present should be common to all the leaves of the object.
 *
 * Inputs to object filler tools are provided by `getter' tools.
 */
class IObjFillerTool
  : virtual public IAlgTool
{
public:
  /// Gaudi interface definition.
  DeclareInterfaceID(IObjFillerTool, 1, 0);


  /**
   * @brief Configure the tool.
   * @param tree The parent D3PD tree.
   *
   * This is called during initialization.
   */
  virtual StatusCode configureD3PD (IAddVariable* tree) = 0;


  /**
   * @brief Declare tuple variables.
   *
   * This is called at the start of the first event.
   */
  virtual StatusCode book() = 0;


  /**
   * @brief Fill one object.
   *
   * This is called once per event.  It should:
   *   - Fetch the input.
   *   - If it's a container, loop over the contents.
   *   - Loop over the contained block filler tools.
   */ 
  virtual StatusCode fill() = 0;


  /**
   * @brief Find out if the filler handles containers or single objects
   *
   * This is used by the D3PDReader code generator to know how to
   * create the generated source files exactly.
   */
  virtual bool isContainerFiller() const = 0;


};


} // namespace D3PD


#endif // not D3PDMAKERINTERFACES_IOBJFILLERTOOL_H
