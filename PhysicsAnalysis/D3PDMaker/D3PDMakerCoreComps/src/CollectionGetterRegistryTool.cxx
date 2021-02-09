/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMakerCoreComps/src/CollectionGetterRegistryTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Tool to keep a registry of collection getter tools.
 */


#include "CollectionGetterRegistryTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IToolSvc.h"

#include <boost/algorithm/string/predicate.hpp>

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
CollectionGetterRegistryTool::CollectionGetterRegistryTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : base_class (type, name, parent),
      m_jos       ("JobOptionsSvc", name),
      m_toolsvc   ("ToolSvc",       name)
{
  declareProperty ("JobOptionsSvc", m_jos,
                   "The JobOptionsSvc instance.");
  declareProperty ("ToolSvc",       m_toolsvc,
                   "The ToolSvc instance.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode
CollectionGetterRegistryTool::initialize()
{
  CHECK( AthAlgTool::initialize() );
  CHECK( m_jos.retrieve() );
  CHECK( m_toolsvc.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Register a new collection getter tool.
 * @param label Label for the tool.
 * @param tool The tool instance.
 */
StatusCode CollectionGetterRegistryTool::add (const std::string& label,
                                              ICollectionGetterTool* tool)
{
  std::pair<map_t::iterator, bool> res =
    m_collection_map.insert (std::make_pair (label, tool));
  if (!res.second) {
    REPORT_MESSAGE_WITH_CONTEXT (MSG::ERROR,
                                 "D3PD::CollectionGetterRegistryTool")
      << "Duplicate collection label " << label
      << " for tools " << res.first->second->name()
      << " and " << tool->name();
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Get a copy of a registered collection getter tool.
 * @param label Label for the tool.
 * @param parent Parent for the new tool.
 * @param tool The new tool instance.
 *
 * The new tool will be a private tool, owned by @c parent.
 */
StatusCode
CollectionGetterRegistryTool::get (const std::string& label,
                                   const INamedInterface* parent,
                                   ICollectionGetterTool*& tool)
{
  // Loop up the tool in the map.
  tool = 0;
  map_t::iterator i = m_collection_map.find (label);
  if (i == m_collection_map.end()) {
    REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING,
                                 "D3PD::CollectionGetterRegistryTool")
      << "Can't find collection label " << label;
    return StatusCode::SUCCESS;
  }

  // Get the properties for the source tool.
  const auto& props = m_jos->items([&i](const auto& p) {
    return boost::algorithm::starts_with(std::get<0>(p), i->second->name()+".");
  });

  // Copy them to the destination tool (except for Label).
  std::string fullname = parent->name() + "." + label;
  for (const auto& p : props) {
    const std::string& oldname = std::get<0>(p);
    std::string::size_type ipos = oldname.rfind ('.');
    if (ipos != std::string::npos) {
      std::string pname = oldname.substr (ipos, std::string::npos);
      if (pname != ".Label") {
        m_jos->set(fullname + pname, std::get<1>(p));
      }
    }
  }

  // Create the new tool.
  return m_toolsvc->retrieveTool (i->second->type(), label, tool, parent);
}


} // namespace D3PD
