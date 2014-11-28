/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRESOLUTION_JERSMEARINGTOOL_H
#define JETRESOLUTION_JERSMEARINGTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/CorrectionTool.h"

// EDM includes
#include "xAODJet/JetContainer.h"

// Local includes
#include "JetResolution/IJERSmearingTool.h"
#include "JetResolution/IJERTool.h"

// Other includes
#include "TRandom3.h"


/// Implementation of the dual-use JER smearing tool interface
///
/// This tool allows to smear the energy of jets either to correct
/// the jet energy resolution or to evaluate systematic uncertainties.
/// For information on the run-1 tool, see the Twiki:
/// https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/ApplyJetResolutionSmearing
/// The documentation for this specific xAOD-based tool is not yet completed.
///
/// Configuration
///
/// The user must provide an IJERTool via either python JO or in standlone via
/// the setJERTool method.
///
/// The tool will apply nominal smearing unless the property
/// 'ApplyNominalSmearing' is set to false, which can be done in standalone
/// via the setApplyNominalSmearing method.
///
/// Interface
///
/// Objects are smeared via either the applyCorrection or correctedCopy
/// methods.
///
/// Systematic behavior is set in the usual way via the
/// applySystematicVariation method.
///
/// @author Steve Farrell <steven.farrell@cern.ch>
///
class JERSmearingTool : public virtual IJERSmearingTool,
                        public virtual CP::ISystematicsTool,
                        public virtual CP::CorrectionTool<xAOD::JetContainer>,
                        public asg::AsgTool
{

    /// Proper constructor for Athena
    ASG_TOOL_CLASS2( JERSmearingTool, CP::ISystematicsTool,
                     IJERSmearingTool )

  public:

    /// Standard constructor for standalone usage
    JERSmearingTool(const std::string& name);
    /// Copy constructor for reflex in Athena
    JERSmearingTool(const JERSmearingTool& other);

    /// @name Methods implementing the asg::IAsgTool interface
    /// @{

    /// Initialize the tool
    virtual StatusCode initialize();

    /// @}

    /// @name Methods implementing the IJERSmearingTool interface
    /// @{

    /// Apply the JER correction on a modifiable jet
    virtual CP::CorrectionCode applyCorrection(xAOD::Jet& jet);

    /// Create a JER-corrected copy from a const jet
    virtual CP::CorrectionCode correctedCopy(const xAOD::Jet& input,
                                             xAOD::Jet*& output)
    {
      return CP::CorrectionTool<xAOD::JetContainer>::
        correctedCopy(input, output);
    }

    /// Optional method to set random seed. Note that the tool
    /// already has a deterministic way to do this using the jet phi.
    virtual void setRandomSeed(long int seed)
    { m_userSeed = seed; }

    /// @}

    /// @name Methods implementing the ISystematicsTool interface
    /// @{

    /// Specify whether tool is affected by provided systematic
    virtual bool isAffectedBySystematic
    (const CP::SystematicVariation& systematic) const;

    /// List of all systematics affecting this tool
    virtual CP::SystematicSet affectingSystematics() const;

    /// List of all systematics recommended for this tool
    virtual CP::SystematicSet recommendedSystematics() const;

    /// Configure tool to apply systematic variation
    virtual CP::SystematicCode applySystematicVariation
    (const CP::SystematicSet& systematics);

    /// @}

    /// @name Configuration methods
    /// @{

    /// Set the JER provider to use
    void setJERTool(IJERTool* jerTool) { m_jerTool = jerTool; }

    /// Toggle nominal smearing. If set to false, smearing
    /// will only be applied for the JER systematic evaluation.
    void setNominalSmearing(bool applyIt) { m_applyNominalSmearing = applyIt; }

    /// @}

  protected:

    /// Get nominal smearing factor
    double getSmearingFactorNom(const xAOD::Jet* jet);

    /// Get the systematic uncert smearing factor
    double getSmearingFactorSys(const xAOD::Jet* jet);

    /// Calculate the random gaussian smear factor for a requested sigma
    double getSmearingFactor(const xAOD::Jet* jet, double sigma);

    /// Handle to the associated JERTool
    ToolHandle<IJERTool> m_jerTool;

    /// Configuration flag for toggling nominal smearing
    bool m_applyNominalSmearing;

  private:

    /// Random number generator
    TRandom3 m_rand;
    /// Optional user seed
    long int m_userSeed;

    /// JER systematic variation
    const CP::SystematicVariation m_jerSys;

    /// Simple flag to specify application of systematic uncertainty
    bool m_applyJERSyst;

}; // class JERSmearingTool

#endif
