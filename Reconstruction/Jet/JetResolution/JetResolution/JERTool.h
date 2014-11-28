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
/// associated uncertainties as described on the TWiki:
/// https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/JetEnergyResolutionProvider
///
/// Configuration
///
/// Config properties are set via setProperty(NAME, VALUE). Currently,
/// the properties of this tool are all strings:
///
/// - PlotFileName: path to ROOT file with the resolution graphs.
///   You must use the PathResolver-friendly path, e.g.
///   JetResolution/JERProviderPlots_2012.root
/// - CollectionName: name of the jet algorithm, e.g.
///   AntiKt4LCTopoJets, AntiKt6EMTopoJets, etc.
/// - BeamEnergy: 7TeV or 8TeV are supported.
/// - SimulationType: FullSim or AFII.
/// - ErrorMethod: Default or Alternate. See Twiki for details.
///
/// Interface
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

    /// Jet algorithm enumeration
    enum JETALGO
    {
      JETALGO_UNDEFINED = -1,
      AKt4EM = 0,
      AKt4LC,
      AKt6EM,
      AKt6LC,
      JETALGO_N
    };

    /// Create a constructor for standalone usage
    JERTool(const std::string& name);

    /// Constructor with initialization
    JERTool(const std::string& toolName, const std::string& plotFileName,
            const std::string& jetCollection);

    /// Copy constructor needed for reflex dictionary generation in Athena
    JERTool(const JERTool&);

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
    double getUncertainty(const xAOD::Jet* jet);

    /// @}


    ///-----------------------------------------------------------------------
    /// @name Tool configuration
    /// @{

    /// Set input file name
    void setFileName(const std::string& fileName)
    { m_fileName = fileName; }

    /// Set the input jet collection
    void setJetCollection(const std::string& jetCollection)
    { m_collectionName = jetCollection; }

    /// Set the JER method
    /// Note, only "Truth" is currently supported
    void setJERMethod(const std::string& jerMethod)
    { m_jerMethod = jerMethod; }

    /// Set 7-TeV flag
    void setIs7TeV(bool use7TeV = true) { m_is7TeV = use7TeV; }

    /// The alternate uncertainty is a more optimistic scenario
    /// in which the uncertainty is constrained by the insitu fit error
    void setUseAltError(bool useAlt = true) { m_useAltErr = useAlt; }

    /// @}


    ///-----------------------------------------------------------------------
    /// @name Lower-level methods for multiple tool configurations.
    /// I'm not sure if I will continue to support these, or if it's
    /// preferable to require users to configure multiple tool instances.
    /// @{

    /// Read the resolution from the MC parameterization
    double getRelResolutionMC(const xAOD::Jet* jet, bool isAFII);

    /// Calculate resolution uncertainty
    double getUncertainty(const xAOD::Jet* jet, bool isAFII, bool altErr);

    /// Calculate resolution uncertainty for 7TeV analysis only
    double getUncertainty_7TeV(const xAOD::Jet* jet);

    /// @}


    ///-----------------------------------------------------------------------
    /// Pt/eta-based methods for retrieving the resolution
    /// @{

    /// Read the resolution from the MC parameterization
    double getRelResolutionMC(double pt, double eta);
    /// Obtain the resolution for AFII MC
    double getRelResolutionMC(double pt, double eta, bool isAFII);
    /// Obtain the resolution for data (sigma_MC + offset)
    double getRelResolutionData(double pt, double eta);

    /// @}


    ///-----------------------------------------------------------------------
    /// Access to the insitu measurements and their errors
    /// @{
    double getInsituRelResolutionMC(const xAOD::Jet* jet);
    double getInsituRelResolutionMC(double pt, double eta);
    double getInsituRelResolutionData(const xAOD::Jet* jet);
    double getInsituRelResolutionData(double pt, double eta);
    double getInsituUncertMC(const xAOD::Jet* jet);
    double getInsituUncertMC(double pt, double eta);
    double getInsituUncertData(const xAOD::Jet* jet);
    double getInsituUncertData(double pt, double eta);
    double getInsituUncert(const xAOD::Jet* jet);
    double getInsituUncert(double pt, double eta);
    /// @}


    ///-----------------------------------------------------------------------
    /// Insitu measurements for 7TeV
    /// @{
    double getSystematics_7TeV(int etaBin);
    double getInsituDiffDataMC(const xAOD::Jet* jet);
    double getInsituDiffDataMC(double pt, double eta);
    double getInsituDiffDataMCError(const xAOD::Jet* jet);
    double getInsituDiffDataMCError(double pt, double eta);
    /// @}


  protected:

    /// Attempt auto-configuration of tool using Metadata.
    /// This currently only works in Athena.
    StatusCode autoConfigure();

    /// Extract an object from the input file
    template<class T> StatusCode pullFromFile(std::string name, T*& obj)
    {
      obj = dynamic_cast<T*>(m_inputFile->Get(name.c_str()));
      if(obj == NULL){
        ATH_MSG_FATAL("Unable to retrieve " << T::Class()->GetName() <<
                      " with name " << name);
        return StatusCode::FAILURE;
      }
      // Why rename the object?
      obj->SetName(("the" + name).c_str());
      return StatusCode::SUCCESS;
    }

  protected:

    /// Get eta bin corresponding for this jet.
    /// Indexing convention of eta regions is off-by-one
    /// with respect to the TAxis (etaBin = TAxisBin - 1)
    int getEtaBin(const xAOD::Jet* jet);
    int getEtaBin(double eta);

    /// Read the offset from data/MC for the pt bin considered
    double getOffset(const xAOD::Jet* jet);
    double getOffset(double pt, double eta);

    /// Extract an in situ measurement from one of the TGraph maps.
    /// All of these measurements use truncations in eta and pt.
    template<class T> double getInsituMeasurement(double pt, double eta,
                                                  std::map<int, T*> graphMap)
    {
      const double GeV = 1.e3;
      const double invGeV = 1.e-3;
      // Truncate eta bin due to lacking stats in data/mc
      int etaBin = std::min(getEtaBin(eta), 3);
      pt = std::min(1000.*GeV, pt);
      if(fabs(eta) > 2.1 && pt > 300.*GeV) pt = 300.*GeV;
      return graphMap[etaBin]->Eval(pt*invGeV);
    }

  private:

    /// Binning
    static const unsigned int m_nEtaBins = 6;
    TAxis* m_etaAxis;

    /// Configuration
    bool m_useAutoConfig;
    std::string m_fileName;
    std::string m_collectionName;
    std::string m_jerMethod;
    std::string m_beamEnergy;
    std::string m_simulationType;
    std::string m_errorMethod;
    bool m_is7TeV;
    bool m_isAFII;
    bool m_useAltErr;
    JETALGO m_jetAlgo;

    /// Input file
    TFile* m_inputFile;

    /// @name Resolution results in maps keyed by eta region number.
    /// Might want to use a different set of data structures for this.
    /// @{

    /// Resolution functions
    std::map<int, TF1*> m_jerFunc;
    std::map<int, TF1*> m_jerFuncAFII;

    /// In situ data resolution graphs
    std::map<int, TGraphErrors*> m_jerData;
    std::map<int, TGraph*> m_errorData;

    /// In situ MC resolution graphs
    std::map<int, TGraphErrors*> m_jerMC;
    std::map<int, TGraph*> m_errorMC;

    /// Data-MC diff in 7 TeV
    std::map<int, TGraphErrors*> m_diffDataMC;
    std::map<int, TGraph*> m_errorDataMC;

    /// @}

}; // class JERTool

#endif // JETRESOLUTION_JERTOOL_H
