/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_TOOLBOX_H
#define ASSOCIATIONUTILS_TOOLBOX_H

// Framework includes
#include "AsgMessaging/AsgMessaging.h"
#include "AsgTools/AnaToolHandle.h"

// Local includes
#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/IOverlapRemovalTool.h"

// STL includes
#include <string>

// Forward declarations
class INamedInterface;
namespace asg {
  class AsgTool;
}

namespace ORUtils
{

  /// @class ToolBox
  /// @brief A container and helper class for overlap removal tools.
  ///
  /// This implementation uses AnaToolHandles to setup, manage, and
  /// configure the overlap tools and a master overlap removal tool.
  ///
  /// @todo Add some more usage details.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class ToolBox : public asg::AsgMessaging
  {

    public:

      /// Aliases
      using parentType_t = INamedInterface;
      using MasterHandle_t = asg::AnaToolHandle<IOverlapRemovalTool>;
      using OverlapHandle_t = asg::AnaToolHandle<IOverlapTool>;


      /// Basic constructor with name and parent.
      ToolBox(const std::string& name = "ORToolBox",
              parentType_t* theParent = nullptr);

      /// Attach and initialize all tools
      StatusCode initialize();

      /// Get a list of all handles to loop over
      std::vector<OverlapHandle_t*> getOverlapTools();

      /// Apply a global property to all tools
      template<class T> StatusCode
      setGlobalProperty(const std::string& property, const T& value);

      /// Master overlap removal tool handle
      MasterHandle_t masterTool;

      /// @name overlap tool handles
      /// @{
      OverlapHandle_t muPFJetORT;
      OverlapHandle_t eleEleORT;
      OverlapHandle_t eleMuORT;
      OverlapHandle_t eleJetORT;
      OverlapHandle_t muJetORT;
      OverlapHandle_t tauEleORT;
      OverlapHandle_t tauMuORT;
      OverlapHandle_t tauJetORT;
      OverlapHandle_t phoEleORT;
      OverlapHandle_t phoMuORT;
      OverlapHandle_t phoJetORT;
      OverlapHandle_t eleFatJetORT;
      OverlapHandle_t jetFatJetORT;
      /// @}

      /// Pointer to the parent component for the tool handles
      parentType_t* parent() { return m_parent; }

    private:

      /// Initialize and attach one overlap tool
      StatusCode initTool(OverlapHandle_t& handle, const std::string& key);

      /// Initialize master tool
      StatusCode initMaster();

      /// Pointer to the parent component for the tool handles
      parentType_t* m_parent;

  };


  //---------------------------------------------------------------------------
  // Implementation method to set a property on all tools.
  //---------------------------------------------------------------------------
  template<class T> StatusCode
  ToolBox::setGlobalProperty(const std::string& property, const T& value)
  {
    // Handle the overlap tools
    for(auto handlePtr : getOverlapTools()) {
      ATH_CHECK( handlePtr->setProperty(property, value) );
    }
    // Handle the master tool
    if(!masterTool.empty()) ATH_CHECK( masterTool.setProperty(property, value) );
    return StatusCode::SUCCESS;
  }

} // namespace ORUtils

#endif
