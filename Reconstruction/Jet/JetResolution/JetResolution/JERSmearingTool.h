/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRESOLUTION_JERSMEARINGTOOL_H
#define JETRESOLUTION_JERSMEARINGTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
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
/// For information, see the Twiki:
/// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetEnergyResolutionXAODTools
///
/// @author Steve Farrell <steven.farrell@cern.ch>
///
class JERSmearingTool : public asg::AsgTool,
                        public IJERSmearingTool,
                        public CP::CorrectionTool<xAOD::JetContainer>
{

    /// Proper constructor for Athena
    ASG_TOOL_CLASS( JERSmearingTool, IJERSmearingTool )

  public:

    /// Standard constructor for standalone usage
    JERSmearingTool(const std::string& name);
    /// Copy constructor for reflex in Athena
    JERSmearingTool(const JERSmearingTool& other);

    /// We shouldn't need assignment; regardless, it's not implemented
    JERSmearingTool& operator=(const JERSmearingTool&) = delete;

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

  protected:

    /// Get nominal smearing factor
    double getSmearingFactorNom(const xAOD::Jet* jet);

    /// Get the systematic uncert smearing factor
    double getSmearingFactorSys(const xAOD::Jet* jet);

    /// Calculate the random gaussian smear factor for a requested sigma
    double getSmearingFactor(const xAOD::Jet* jet, double sigma);

  private:

    /// Handle to the associated JERTool
    ToolHandle<IJERTool> m_jerTool;
    /// Workaround to set the JERTool tool handle in PyROOT
    std::string m_jerToolName;

    /// Configuration flag for toggling nominal smearing
    bool m_applyNominalSmearing;

    /// isMC flag
    bool m_isMC;

    /// Systematic breakdown configuration: "Simple" or "Full"
    std::string m_sysMode;

    /// Random number generator
    TRandom3 m_rand;
    /// Optional user seed
    long int m_userSeed;

    /// Systematic filtering map.
    /// Note that the output of the map could be something other than
    /// SystematicSet. We could use pair<SysEnum, value>, for example, which
    /// actually might make the applyCorrection method cleaner.
    typedef std::unordered_map<CP::SystematicSet, CP::SystematicSet> SysFiterMap_t;
    SysFiterMap_t m_sysFilterMap;

    /// Points to the current systematic configuration
    const CP::SystematicSet* m_sysConfig;

}; // class JERSmearingTool

#endif
