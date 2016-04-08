// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerInterfaces/ICollectionGetterRegistryTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2009
 * @brief Abstract interface to keep a registry of collection getter tools.
 */

#ifndef D3PDMAKERINTERFACES_ICOLLECTIONGETTERREGISTRYTOOL_H
#define D3PDMAKERINTERFACES_ICOLLECTIONGETTERREGISTRYTOOL_H


#include "GaudiKernel/IAlgTool.h"


namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Abstract interface to keep a registry of collection getter tools.
 *
 * In order to represent associations by an index, we need to be able
 * to get access to the collection within which we're indexing.
 * We do this by optionally associating a label with a collection
 * getter tool (this will typically be the object prefix).  We can then
 * look up the label here and make a copy of the matching getter tool.
 * (We make a copy rather than using the original instance since the
 * collection getters are stateful.)
 */
class ICollectionGetterRegistryTool
  : virtual public IAlgTool
{
public:
  /// Gaudi interface definition.
  DeclareInterfaceID(ICollectionGetterRegistryTool, 1, 0);


  /**
   * @brief Register a new collection getter tool.
   * @param label Label for the tool.
   * @param tool The tool instance.
   */
  virtual StatusCode add (const std::string& label,
                          ICollectionGetterTool* tool) = 0;


  /**
   * @brief Get a copy of a registered collection getter tool.
   * @param label Label for the tool.
   * @param parent Parent for the new tool.
   * @param tool The new tool instance.
   *
   * The new tool will be a private tool, owned by @c parent.
   */
  virtual StatusCode get (const std::string& label,
                          const INamedInterface* parent,
                          ICollectionGetterTool*& tool) = 0;
};


} // namespace D3PD


#endif // not D3PDMAKERINTERFACES_ICOLLECTIONGETTERREGISTRYTOOL_H
