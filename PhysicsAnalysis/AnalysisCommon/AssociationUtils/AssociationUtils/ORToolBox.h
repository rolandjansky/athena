#ifndef ASSOCIATIONUTILS_ORTOOLBOX_H
#define ASSOCIATIONUTILS_ORTOOLBOX_H

// This code should only be used in RootCore
#ifdef ROOTCORE

// Framework includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

// Local includes
#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/IOverlapRemovalTool.h"


namespace ORUtils
{

  /// @class ToolBoxBase
  /// @brief A generic helper class for holding tools.
  ///
  /// This is being replaced by the new ToolBox class.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class ToolBoxBase
  {

    public:

      // Tool-map typedefs
      using MapType_t = std::map< std::string, std::unique_ptr<asg::AsgTool> >;
      using const_iterator = typename MapType_t::const_iterator;

      /// Initializes all tools in the toolbox
      StatusCode initialize();

      /// Set message level of all tools in one go
      void setMsgLevel(MSG::Level lvl);

      /// @brief Retrieve an AsgTool pointer
      /// Returns NULL in case the tool isn't found.
      asg::AsgTool* getTool(const std::string& key, bool quiet=false);

      /// @brief Add a tool to the box.
      /// Overwrites a pre-existing tool.
      void setTool(const std::string& key, std::unique_ptr<asg::AsgTool> tool);

      /// Read-only iteration over the map elements
      const_iterator begin() const;
      /// Read-only iteration over the map elements
      const_iterator end() const;

    private:

      /// @brief Map of key to tool pointer.
      /// Key does not necessarily need to be the tool name, though using the
      /// tool name might make the most sense in most situations.
      MapType_t m_tools;

  }; // class ToolBoxBase



  /// @class ORToolBox
  /// @brief Specialized toolbox for overlap tools and a master tool.
  ///
  /// This is being replaced by the new ToolBox class.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class ORToolBox : public ToolBoxBase
  {

    public:

      /// Get AsgTool ptr to the master tool
      asg::AsgTool* getMasterTool();

      /// Set the master tool (uses key="master")
      void setMasterTool(std::unique_ptr<asg::AsgTool> tool);

      /// @brief Add one overlap tool.
      /// If master tool is already added, then the tool will also be
      /// assigned to the master tool using the key. The tool name will
      /// be used in case the key is empty (default).
      StatusCode addOverlapTool(std::unique_ptr<asg::AsgTool> tool,
                                std::string key = "");

      /// Get a ToolHandle to an IOverlapTool
      ToolHandle<IOverlapTool> getHandle(const std::string& key);

      /// Get a ToolHandle to the master IOverlapRemovalTool
      ToolHandle<IOverlapRemovalTool> getMasterHandle();

  }; // class ORToolBox

} // namespace ORUtils

#endif // ROOTCORE

#endif // ASSOCIATIONUTILS_ORTOOLBOX_H
