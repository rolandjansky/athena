// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMakerCoreComps/src/CollectionGetterRegistryTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Tool to keep a registry of collection getter tools.
 */


#ifndef D3PDMAKERCORECOMPS_COLLECTIONGETTERREGISTRYTOOL_H
#define D3PDMAKERCORECOMPS_COLLECTIONGETTERREGISTRYTOOL_H


#include "D3PDMakerInterfaces/ICollectionGetterRegistryTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Interfaces/IOptionsSvc.h"

#include <unordered_map>


class IToolSvc;


namespace D3PD {


/**
 * @brief Tool to keep a registry of collection getter tools.
 *
 * In order to represent associations by an index, we need to be able
 * to get access to the collection within which we're indexing.
 * We do this by optionally associating a label with a collection
 * getter tool (this will typically be the object prefix).  We can then
 * look up the label here and make a copy of the matching getter tool.
 * (We make a copy rather than using the original instance since the
 * collection getters are stateful.)
 */
class CollectionGetterRegistryTool
  : public extends1<AthAlgTool, ICollectionGetterRegistryTool>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CollectionGetterRegistryTool (const std::string& type,
                                const std::string& name,
                                const IInterface* parent);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Register a new collection getter tool.
   * @param label Label for the tool.
   * @param tool The tool instance.
   */
  virtual StatusCode add (const std::string& label,
                          ICollectionGetterTool* tool);


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
                          ICollectionGetterTool*& tool);



private:
  /// Property: The job options service.
  ServiceHandle<Gaudi::Interfaces::IOptionsSvc> m_jos;

  /// Property: The tool service.
  ServiceHandle<IToolSvc> m_toolsvc;

  /// Map from getter labels to instances.
  typedef std::unordered_map<std::string, D3PD::ICollectionGetterTool*> map_t;
  map_t m_collection_map;
};


} // namespace D3PD



#endif // not D3PDMAKERCORECOMPS_COLLECTIONGETTERREGISTRYTOOL_H
