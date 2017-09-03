/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_OVERLAPREMOVALTOOL_H
#define ASSOCIATIONUTILS_OVERLAPREMOVALTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// Local includes
#include "AssociationUtils/IOverlapRemovalTool.h"
#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/OverlapRemovalDefs.h"
#include "AssociationUtils/OverlapDecorationHelper.h"


namespace ORUtils
{

  /// @class OverlapRemovalTool
  /// @brief top-level tool for performing overlap removal.
  ///
  /// This tool will apply the overlap removals specified by the user in the
  /// form of IOverlapTools in the recommended order.
  ///
  /// @author Steve Farrell <steven.farrell@cern.ch>
  ///
  class OverlapRemovalTool : public virtual IOverlapRemovalTool,
                             public asg::AsgTool
  {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS(OverlapRemovalTool, IOverlapRemovalTool)

    public:

      /// Constructor for standalone usage
      OverlapRemovalTool(const std::string& name);

      /// Initialize the tool
      virtual StatusCode initialize() override;

      /// Top-level method for performing full overlap-removal.
      /// The individual OR methods will be called in the recommended order,
      /// and the considered objects will be decorated with the output result.
      virtual StatusCode
      removeOverlaps(const xAOD::ElectronContainer* electrons,
                     const xAOD::MuonContainer* muons,
                     const xAOD::JetContainer* jets,
                     const xAOD::TauJetContainer* taus = 0,
                     const xAOD::PhotonContainer* photons = 0,
                     const xAOD::JetContainer* fatJets = 0) const override;

    private:

      /// Use one tool to remove overlaps
      StatusCode removeOverlap(const ToolHandle<IOverlapTool>& tool,
                               const xAOD::IParticleContainer* cont1,
                               const xAOD::IParticleContainer* cont2) const;

    private:

      //
      // Configurable properties
      //

      /// Input object decoration which specifies which objects to look at
      std::string m_inputLabel;
      /// Output object decoration which specifies overlapping objects
      std::string m_outputLabel;

      /// Toggle the output flag logic. If true, then non-overlapping
      /// objects will be assigned "true".
      bool m_outputPassValue;

      /// Require non-null container pointers when expected; i.e., when
      /// corresponding overlap tools are configured. On by default.
      bool m_requireExpectedPointers;

      //
      // Helper utilities
      //

      /// Helper used to reset decorations
      std::unique_ptr<OverlapDecorationHelper> m_decHelper;

      //
      // Overlap tool handles
      //

      /// Ele-ele overlap handle
      ToolHandle<IOverlapTool> m_eleEleORT;

      /// Ele-mu overlap handle
      ToolHandle<IOverlapTool> m_eleMuORT;

      /// Ele-jet overlap handle
      ToolHandle<IOverlapTool> m_eleJetORT;

      /// Mu-jet overlap handle
      ToolHandle<IOverlapTool> m_muJetORT;

      /// Tau-ele overlap handle
      ToolHandle<IOverlapTool> m_tauEleORT;

      /// Tau-mu overlap handle
      ToolHandle<IOverlapTool> m_tauMuORT;

      /// Tau-jet overlap handle
      ToolHandle<IOverlapTool> m_tauJetORT;

      /// Pho-ele overlap handle
      ToolHandle<IOverlapTool> m_phoEleORT;

      /// Pho-mu overlap handle
      ToolHandle<IOverlapTool> m_phoMuORT;

      /// Pho-jet overlap handle
      ToolHandle<IOverlapTool> m_phoJetORT;

      /// ele-fatjet overlap handle
      ToolHandle<IOverlapTool> m_eleFatJetORT;

      /// jet-fatjet overlap handle
      ToolHandle<IOverlapTool> m_jetFatJetORT;

  }; // class OverlapRemovalTool

} // namespace ORUtils

#endif
