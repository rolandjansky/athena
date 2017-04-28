/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__PHOTON_TOOL_H
#define QUICK_ANA__PHOTON_TOOL_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.

#include <QuickAna/Global.h>

#include <QuickAna/AnaToolCorrect.h>
#include <AsgTools/AnaToolHandle.h>
#include <QuickAna/AnaToolSelect.h>
#include <QuickAna/AnaToolWeight.h>
#include <QuickAna/SelectionCut.h>

/// Forward declarations
namespace CP{
  class IEgammaCalibrationAndSmearingTool;
  class IIsolationSelectionTool;
  class IIsolationCorrectionTool;
}
class IAsgPhotonEfficiencyCorrectionTool;
class IAsgPhotonIsEMSelector;
class IElectronPhotonShowerShapeFudgeTool;
class IIsolationSelectionTool;

namespace ana
{

  /// @brief Tool for the correction of photons
  ///
  /// Applies correction-related systematics as well
  ///
  class PhotonToolCorrect : public AnaToolCorrect<xAOD::PhotonContainer>
  {

    /// This macro generates a proper constructor for Athena
    ASG_TOOL_CLASS (PhotonToolCorrect, ana::IAnaTool)

  public:
    // Public interface methods

    /// @brief Standard constructor
    PhotonToolCorrect (const std::string& name);

    /// @brief Initialize this tool
    /// Returns failure on configuration error.
    StatusCode initialize() override;

    /// @brief Run the calibration on this event.
    /// The accept flag tells whether we accepted the object.
    virtual StatusCode
    correctObject (xAOD::Photon& photon) override;

  private:

    /// Configuration

    /// Is this AF2?
    bool m_isAF2;

    /// Is this data?
    bool m_isData;

    // CP tools

    /// The CP calibration tool
    asg::AnaToolHandle<CP::IEgammaCalibrationAndSmearingTool> m_calibration;
    /// The CP shower-shape fudge tool
    asg::AnaToolHandle<IElectronPhotonShowerShapeFudgeTool> m_fudgeMCTool;
    /// The CP isolation correction tool
    asg::AnaToolHandle<CP::IIsolationCorrectionTool> m_isoCorrTool;

    SelectionCut cut_fudge_tool;
    SelectionCut cut_calibration_tool;
    SelectionCut cut_isoCorr_tool;

  }; // class PhotonToolCorrect



  /// @brief Tool for selection of analysis photons
  ///
  /// Applies common photon selections
  ///
  class PhotonToolSelect : public AnaToolSelect<xAOD::PhotonContainer>
  {

    /// This macro generates a proper constructor for Athena
    ASG_TOOL_CLASS (PhotonToolSelect, ana::IAnaTool)

  public:

    // TODO: make this a private property
    unsigned int quality;

    // Public interface methods

    /// @brief Standard constructor
    PhotonToolSelect (const std::string& name);

    /// @brief Initialize this tool.
    /// Returns failure on configuration error.
    StatusCode initialize() override;

    /// @brief Apply selection on this object
    virtual StatusCode
    selectObject (xAOD::Photon& photon) override;

  private:
    /// The CP isEM selector.  No likelihood for photons yet.
    asg::AnaToolHandle<IAsgPhotonIsEMSelector> m_selection;
    asg::AnaToolHandle<CP::IIsolationSelectionTool> m_isolationTool;

    SelectionCut cut_OQ;
    SelectionCut cut_author;
    SelectionCut cut_cleaning;
    SelectionCut cut_selection;
    SelectionCut cut_isolationTool;

    /// Configuration

    /// Isolation working point
    std::string m_isolationWP;

    /// Using isolation
    bool m_isolationOn;

  }; // class PhotonToolSelect



  /// @brief Tool for calculating weights for analysis photons
  ///
  /// Applies the photon reco and ID efficiency scale factors
  ///
  class PhotonToolWeight : public AnaToolWeight<xAOD::PhotonContainer>
  {

    /// This macro generates a proper constructor for Athena
    ASG_TOOL_CLASS (PhotonToolWeight, ana::IAnaTool)

  public:
    // Public interface methods

    /// @brief Standard constructor
    PhotonToolWeight (const std::string& name);

    /// @brief Initialize this tool
    /// Returns failure on configuration error.
    StatusCode initialize() override;

    /// @brief Calculate the weight for this event
    virtual StatusCode
    objectWeight (const xAOD::Photon& photon, float& weight) override;

  private:
    // CP tools

    /// The efficiency correction tool
    asg::AnaToolHandle<IAsgPhotonEfficiencyCorrectionTool> m_efficiencyTool;
    asg::AnaToolHandle<IAsgPhotonEfficiencyCorrectionTool> m_isoSFTool;

    /// Configuration

    /// Is this AF2?
    bool m_isAF2;

    /// Isolation working point
    std::string m_isolationWP;
    /// Will isolation be used
    bool m_doIsolation;

  }; // class PhotonToolWeight

} // namespace ana

#endif

