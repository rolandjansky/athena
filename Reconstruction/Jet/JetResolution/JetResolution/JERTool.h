/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRESOLUTION_JERTOOL_H
#define JETRESOLUTION_JERTOOL_H

// ROOT includes
#include "TClass.h"
#include "TFile.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TAxis.h"

// Framework includes
#include "AsgTools/AsgMetadataTool.h"

// Local includes
#include "JetResolution/IJERTool.h"


/// A dual-use tool which provides the jet energy resolution and
/// associated uncertainties. For more information, see:
/// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetEnergyResolutionXAODTools
///
/// Resolution and uncertainty are retrieved using the getRelResolutionMC,
/// getRelResolutionData, and getUncertainty methods.
///
/// @author Steve Farrell <steven.farrell@cern.ch>
/// @date July 2014
///
class JERTool : public virtual IJERTool,
                public asg::AsgMetadataTool
{

    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS( JERTool, IJERTool )

  public:

    /// Create a constructor for standalone usage
    JERTool(const std::string& name);

    /// Copy constructor needed for reflex dictionary generation in Athena
    JERTool(const JERTool&);

    /// We shouldn't need assignment, but regardless it isn't implemented.
    JERTool& operator=(const JERTool&) = delete;

    /// Destructor
    ~JERTool();


    ///-----------------------------------------------------------------------
    /// @name Methods implementing the asg::IAsgTool interface
    /// @{

    /// Initialize the tool
    virtual StatusCode initialize();

    /// @}

    ///-----------------------------------------------------------------------
    /// @name Methods implementing the IJERTool interface.
    /// @{

    /// Read the resolution from the MC parameterization
    double getRelResolutionMC(const xAOD::Jet* jet);

    /// Obtain the resolution for data (sigma_MC + offset)
    double getRelResolutionData(const xAOD::Jet* jet);

    /// Calculate resolution uncertainty
    double getUncertainty(const xAOD::Jet* jet,
                          JER::Uncert errType = JER::JER_NP_ALL);

    /// @}

  protected:

    /// Attempt auto-configuration of tool using Metadata.
    /// This currently only works in Athena.
    StatusCode autoConfigure();

    /// Helper method for parsing the configuration
    StatusCode parseConfiguration();
    /// Helper method for loading the JER inputs
    StatusCode loadJERInputs();

    /// Get eta bin corresponding for this jet.
    /// Indexing convention of eta regions is off-by-one
    /// with respect to the TAxis (etaBin = TAxisBin - 1)
    int getEtaBin(const xAOD::Jet* jet);

  private:

    /// Binning
    /// TODO: Think of a better way to do this.
    /// For example, can't we take these from the input file??
    //  static const unsigned int m_nNP = 9;
    static const unsigned int m_nEtaBins = 7;
    //static const unsigned int m_nJetCol = 4;

    TAxis* m_etaAxis;

    /// Configuration
    std::string m_fileName;
    int m_jetAlgo; // JER::JetAlg
    std::string m_collectionName;
    //std::string m_simulationType;
    //bool m_useAutoConfig;

    /// Nuisance parameter graphs.
    /// TODO: think about a better structure for these.
    /// Previously, these maps were keyed by eta. Maybe we could use TGraph2D
    /// and instead key them by NP.
    TGraph* m_jerNP[JER::JETALG_N][m_nEtaBins][JER::JER_NP_ALL];

}; // class JERTool

#endif // JETRESOLUTION_JERTOOL_H
