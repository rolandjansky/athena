#ifndef ASSOCIATIONUTILS_OVERLAPREMOVALINIT_H
#define ASSOCIATIONUTILS_OVERLAPREMOVALINIT_H

/// @file OverlapRemovalInit.h
/// @brief Defines helper functions for initializing the OR tools in C++.
/// @author Steve Farrell <Steven.Farrell@cern.ch>
///

// System includes
#include <string>

// ASG includes
#include "AsgTools/StatusCode.h"

// Local includes
#include "AssociationUtils/ToolBox.h"
#include "AssociationUtils/ORToolBox.h"

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
    /// Activate boosted-lepton recommendations (sliding dR cones)
    bool boostedLeptons = false;
    /// Output value to assign passing objects.
    /// By default overlapping ('failing') objects are marked 'true'.
    bool outputPassValue = false;
    /// Enable overlap object ElementLinks.
    bool linkOverlapObjects = false;
    /// Do electron-electron overlap
    bool doEleEleOR = false;

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
  /// and initialize all the tools with a call to ORToolBox::initialize.
  ///
  /// This function uses the new ORFlags object to communicate common
  /// configuration options and the new ToolBox design based on AnaToolHandle.
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

#ifdef ROOTCORE

  /// @brief Pre-configured standard recommended OR tools.
  ///
  /// This function has the same default behavior as the one above, but is
  /// overloaded for the familiar interface which has been around for a while.
  ///
  /// DEPRECATED - please use the above function instead.
  ///
  /// @param[out] toolBox Container to be filled with tools.
  /// @param[in] name Name of the master tool and prefix for the sub-tools.
  /// @param[in] inputLabel Decoration name specifying input objects.
  /// @param[in] outputLabel Output decoration applied to objects.
  /// @param[in] bJetLabel Decoration name which specifies b-jets. Leave blank
  ///            to disable btag-aware overlap removal.
  /// @param[in] boostedLeptons activate sliding delta-R cones recommended for
  ///            analyses with boosted leptons.
  /// @param[in] outputPassValue Specifies the output flag logic. I.e., it
  ///            specifies the value assigned to objects that pass OR.
  /// @param[in] doTaus Toggles initialization of tau OR.
  /// @param[in] doPhotons Toggles initialization of photon OR.
  ///
  StatusCode recommendedTools(ORToolBox& toolBox,
                              const std::string& name,
                              const std::string& inputLabel = "selected",
                              const std::string& outputLabel = "overlaps",
                              const std::string& bJetLabel = "",
                              bool boostedLeptons = false,
                              bool outputPassValue = false,
                              bool doTaus=true, bool doPhotons=true)
  __attribute__((deprecated));


  /// @brief Pre-configured harmonization recommended OR tools.
  ///
  /// This function provides the tool configuration corresponding to the
  /// overlap removal recommendations in the harmonization document. The
  /// recommendations have been more recently updated. See the recommendedTools
  /// function above.
  ///
  /// DEPRECATED - these recommendations are no longer supported.
  ///
  /// Refer to the recommendedTools function documentation for more details,
  /// including explanation of the arguments.
  ///
  StatusCode harmonizedTools(ORToolBox& toolBox,
                             const std::string& name,
                             const std::string& inputLabel = "selected",
                             const std::string& outputLabel = "overlaps",
                             bool outputPassValue = false,
                             bool doTaus=true, bool doPhotons=true)
  __attribute__((deprecated));

#endif // ROOTCORE

} // namespace ORUtils

#endif
