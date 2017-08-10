// This should only be compiled in RootCore
#ifdef ROOTCORE

// ROOT includes
#include "TError.h"

// Local includes
#include "AssociationUtils/ORToolBox.h"

static const std::string masterKey = "master";

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Initialize all tools in the toolbox
  //---------------------------------------------------------------------------
  StatusCode ToolBoxBase::initialize()
  {
    for(auto& keyTool : m_tools) {
      if(keyTool.second->initialize().isFailure())
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Set message levels
  //---------------------------------------------------------------------------
  void ToolBoxBase::setMsgLevel(MSG::Level lvl)
  {
    for(auto& keyTool : m_tools)
      keyTool.second->msg().setLevel(lvl);
  }

  //---------------------------------------------------------------------------
  // Retrieve a tool pointer
  //---------------------------------------------------------------------------
  asg::AsgTool* ToolBoxBase::getTool(const std::string& key, bool quiet)
  {
    auto itr = m_tools.find(key);
    if(itr == m_tools.end()) {
      if(!quiet){
        ::Warning("ToolBoxBase::getTool",
                  "Failed to retrieve tool with key %s", key.c_str());
      }
      return nullptr;
    }
    return itr->second.get();
  }

  //---------------------------------------------------------------------------
  // Add tool to the toolbox
  //---------------------------------------------------------------------------
  void ToolBoxBase::setTool(const std::string& key,
                        std::unique_ptr<asg::AsgTool> tool)
  { m_tools[key] = std::move(tool); }

  //---------------------------------------------------------------------------
  // Const-iterate over map elements
  //---------------------------------------------------------------------------
  ToolBoxBase::const_iterator ToolBoxBase::begin() const
  { return m_tools.begin(); }
  //---------------------------------------------------------------------------
  ToolBoxBase::const_iterator ToolBoxBase::end() const
  { return m_tools.end(); }

  //---------------------------------------------------------------------------
  // Get ptr to master tool
  //---------------------------------------------------------------------------
  asg::AsgTool* ORToolBox::getMasterTool()
  { return getTool(masterKey); }

  //---------------------------------------------------------------------------
  // Set ptr to master tool
  //---------------------------------------------------------------------------
  void ORToolBox::setMasterTool(std::unique_ptr<asg::AsgTool> tool)
  {
    setTool( masterKey, std::move(tool) );
  }

  //---------------------------------------------------------------------------
  // Add one overlap tool to the toolbox
  //---------------------------------------------------------------------------
  StatusCode ORToolBox::addOverlapTool(std::unique_ptr<asg::AsgTool> tool,
                                       std::string key)
  {
    if(key.empty()) key = tool->name();
    // Assign to master tool if available
    auto masterTool = getMasterTool();
    if(masterTool) {
      ToolHandle<IOverlapTool> handle( tool->name() );
      if(masterTool->setProperty(key, handle).isFailure()) {
        ::Error("ORToolBox::addOverlapTool",
                "Failed to assign tool to master with key %s", key.c_str());
        return StatusCode::FAILURE;
      }
    }
    // Consume the tool
    setTool( key, std::move(tool) );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Get a tool handle
  //---------------------------------------------------------------------------
  ToolHandle<IOverlapTool> ORToolBox::getHandle(const std::string& key)
  {
    auto tool = dynamic_cast<IOverlapTool*>( getTool(key) );
    if(!tool) {
      ::Error("ORToolBox::getHandle",
              "Failed to cast tool with key %s to IOverlapTool", key.c_str());
    }
    return ToolHandle<IOverlapTool>(tool);
  }

  //---------------------------------------------------------------------------
  // Get master tool handle
  //---------------------------------------------------------------------------
  ToolHandle<IOverlapRemovalTool> ORToolBox::getMasterHandle()
  {
    auto tool = dynamic_cast<IOverlapRemovalTool*>( getTool(masterKey) );
    if(!tool) {
      ::Error("ORToolBox::getMasterHandle",
              "Failed to cast master tool to IOverlapRemovalTool");
    }
    return ToolHandle<IOverlapRemovalTool>(tool);
  }

} // namespace ORUtils

#endif // ROOTCORE
