/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_OVERLAPREMOVALINIT_H
#define ASSOCIATIONUTILS_OVERLAPREMOVALINIT_H

/// @file OverlapRemovalInit.h
/// @brief Defines helper functions for initializing the OR tools in C++.
/// @author Steve Farrell <Steven.Farrell@cern.ch>
///

// System includes
#include <string>

// ASG includes
#include "AsgMessaging/StatusCode.h"

// Local includes
#include "AssociationUtils/ToolBox.h"

namespace ORUtils
{

  /// @brief A struct of global config options used to simplify the config
  /// helper interface.
  ///
  struct ORFlags
  {
    /// Use constructor for convenience to initialize a few options
    ORFlags(const std::string& masterToolName = "OverlapRemovalTool",
            const std::string& theInputLabel = "selected",
            const std::string& theOutputLabel = "overlaps");

    /// Master tool name
    std::string masterName;
    /// Input object decoration
    std::string inputLabel;
    /// Output decoration
    std::string outputLabel;

    /// B-jet decoration
    std::string bJetLabel = "";
    /// Set the maximum electron pT for which b-tag aware overlap removal
    /// is done. Set to negative value to use for all electrons.
    double maxElePtForBJetAwareOR = 100. * 1000;
    /// Activate boosted-lepton recommendations (sliding dR cones)
    bool boostedLeptons = false;
    /// Output value to assign passing objects.
    /// By default overlapping ('failing') objects are marked 'true'.
    bool outputPassValue = false;
    /// Enable overlap object ElementLinks.
    bool linkOverlapObjects = false;
    /// Do electron-electron overlap
    bool doEleEleOR = false;
    /// Do PFlow muon-jet removal
    bool doMuPFJetOR = false;

    /// @name Objects to configure tools for
    /// @{
    bool doElectrons = true;
    bool doMuons = true;
    bool doJets = true;
    bool doTaus = true;
    bool doPhotons = true;
    bool doFatJets = false;
    /// @}
  };


  /// @brief Pre-configured standard recommended OR tools.
  ///
  /// This function provides the tool configuration corresponding to the
  /// updated overlap removal recommendations as summarized in this ASG meeting
  /// by Will Buttinger: https://indico.cern.ch/event/457238/
  ///
  /// The tools are returned uninitialized; the user can override properties
  /// and initialize all the tools with a call to ToolBox::initialize.
  ///
  /// This function uses the ORFlags object to communicate common configuration
  /// options and the ToolBox design based on AnaToolHandle.
  ///
  /// Users can replace overlap tools with their own configured versions by
  /// setting ToolBox handles prior to calling this method. Right now the way
  /// to do that would be with something like this:
  /// @code
  ///     ORUtils::ToolBox toolBox;
  ///     toolBox.tauEleORT =
  ///       ORUtils::ToolBox::OverlapHandle_t("ORUtils::DeltaROverlapTool/TauEleORT");
  ///     CHECK( toolBox.tauEleORT.setProperty(...) );
  ///     CHECK( ORUtils::recommendedTools(flags, toolBox) );
  /// @endcode
  /// Note that the config function will still set the 'global' properties on
  /// your custom tool (e.g. input label). I'm assuming this is the behavior
  /// users would want.
  ///
  /// @param[in] flags An ORFlags object containing config options.
  /// @param[out] toolBox Container to be filled with tools.
  ///
  StatusCode recommendedTools(const ORFlags& flags, ToolBox& toolBox);

} // namespace ORUtils

#endif
