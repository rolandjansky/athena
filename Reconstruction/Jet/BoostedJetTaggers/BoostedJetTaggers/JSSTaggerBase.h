/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_JSSTAGGERBASE_H
#define BOOSTEDJETSTAGGERS_JSSTAGGERBASE_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

#include "JetAnalysisInterfaces/IJetSelectorTool.h"

#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include "ParticleJetTools/JetTruthLabelingTool.h"
#include "BoostedJetTaggers/TagResultEnum.h"

#include "PATCore/TAccept.h"

#include "PathResolver/PathResolver.h"

#include <TFile.h>
#include <TEnv.h>
#include <TH2.h>
#include <TF1.h>

#include <atomic>

/// JSS moment accessors
const static SG::AuxElement::ConstAccessor<float> acc_Tau1_wta( "Tau1_wta" );
const static SG::AuxElement::ConstAccessor<float> acc_Tau2_wta( "Tau2_wta" );
const static SG::AuxElement::ConstAccessor<float> acc_Tau3_wta( "Tau3_wta" );
const static SG::AuxElement::ConstAccessor<float> acc_Tau4_wta( "Tau4_wta" );

const static SG::AuxElement::ConstAccessor<float> acc_Tau21_wta( "Tau21_wta" );
const static SG::AuxElement::ConstAccessor<float> acc_Tau32_wta( "Tau32_wta" );
const static SG::AuxElement::ConstAccessor<float> acc_Tau42_wta( "Tau42_wta" );

const static SG::AuxElement::ConstAccessor<float> acc_ECF1( "ECF1" );
const static SG::AuxElement::ConstAccessor<float> acc_ECF2( "ECF2" );
const static SG::AuxElement::ConstAccessor<float> acc_ECF3( "ECF3" );

const static SG::AuxElement::ConstAccessor<float> acc_C2( "C2" );
const static SG::AuxElement::ConstAccessor<float> acc_D2( "D2" );
const static SG::AuxElement::ConstAccessor<float> acc_e3( "e3" );

const static SG::AuxElement::ConstAccessor<float> acc_Split12( "Split12" );
const static SG::AuxElement::ConstAccessor<float> acc_Split23( "Split23" );

const static SG::AuxElement::ConstAccessor<float> acc_Qw( "Qw" );

const static SG::AuxElement::ConstAccessor<float> acc_ThrustMaj( "ThrustMaj" );

const static SG::AuxElement::ConstAccessor<float> acc_L2( "L2" );
const static SG::AuxElement::ConstAccessor<float> acc_L3( "L3" );

const static SG::AuxElement::ConstAccessor<float> acc_ECFG_3_3_1( "ECFG_3_3_1" );
const static SG::AuxElement::ConstAccessor<float> acc_ECFG_3_1_1( "ECFG_3_1_1" );
const static SG::AuxElement::ConstAccessor<float> acc_ECFG_2_1_2( "ECFG_2_1_2" );

const static SG::AuxElement::ConstAccessor<ElementLink<xAOD::JetContainer>> acc_parent("Parent");
const static SG::AuxElement::ConstAccessor< std::vector<int> > acc_NumTrkPt500("NumTrkPt500");
 
/// JSS moment ratio decorators
const static SG::AuxElement::Decorator<float> dec_Tau21_wta( "Tau21_wta" );
const static SG::AuxElement::Decorator<float> dec_Tau32_wta( "Tau32_wta" );
const static SG::AuxElement::Decorator<float> dec_Tau42_wta( "Tau42_wta" );

const static SG::AuxElement::Decorator<float> dec_C2( "C2" );
const static SG::AuxElement::Decorator<float> dec_D2( "D2" );
const static SG::AuxElement::Decorator<float> dec_e3( "e3" );

const static SG::AuxElement::Decorator<float> dec_L2( "L2" );
const static SG::AuxElement::Decorator<float> dec_L3( "L3" );


class JSSTaggerBase :   public asg::AsgTool ,
  virtual public IJetSelectorTool
{
  ASG_TOOL_CLASS1(JSSTaggerBase, IJetSelectorTool )

  protected:

    /// Default constructor - to be used in all derived classes
    JSSTaggerBase(const std::string &name);

    /// Default destructor - to be used in all derived classes
    ~JSSTaggerBase() {};

    /// Initialize the tool
    virtual StatusCode initialize() override;

    /// TEnv instance to read config files
    TEnv m_configReader;

    /// Object that stores the results for a jet
    mutable Root::TAccept m_accept;

    /// Maximum number of warnings
    const int m_nWarnMax = 10;

    /// Warning counters
    mutable std::atomic<int> m_nWarnKin;
    mutable std::atomic<int> m_nWarnVar;

    /// Flag to indicate units pT is defined in
    /// Set to false by default
    bool m_ptGeV = false;

    /// Flag to indicate if mass window cut is used
    bool m_useMassCut = false;

    /// Flag to indicate if a discriminant score is used
    bool m_useScoreCut = false;

    /// TAGTYPE enum
    enum TAGCLASS{Unknown, WBoson, ZBoson, TopQuark};
    TAGCLASS m_tagClass;

    /// Configurable members

    /// Path to the SF configuration root file
    std::string m_weightConfigPath;

    /// Configuration file name
    std::string m_configFile;

    /// Location where config files live on cvmfs
    std::string m_calibArea;

    /// Keras configurations for ML taggers
    std::string m_kerasConfigFileName;
    std::string m_kerasConfigFilePath;
    std::string m_kerasConfigOutputName;
    std::string m_kerasCalibArea;

    /// TMVA configurations for BDT taggers
    std::string m_tmvaConfigFileName;
    std::string m_tmvaConfigFilePath;
    std::string m_tmvaCalibArea;

    /// Tagger information
    std::string m_wkpt;
    std::string m_tagType;

    /// Kinematic bounds for the jet - the units are controlled by m_ptGeV
    float m_jetPtMin;
    float m_jetPtMax;
    float m_jetEtaMax;

    /// Flags controlling whether generalized ECF moments or L-series ratios are needed
    /// TODO: Implement the functionality controlled by these
    bool m_useECFG = false;
    bool m_useLSeries = false;

    /// Strings for cut functions
    std::string m_strMassCutLow;
    std::string m_strMassCutHigh;
    std::string m_strScoreCut;

    /// TF1 for cut functions
    std::unique_ptr<TF1> m_funcMassCutLow;
    std::unique_ptr<TF1> m_funcMassCutHigh;
    std::unique_ptr<TF1> m_funcScoreCut;

    /// Flag controlling whether jet will be decorated
    bool m_decorate;

    /// Decoration name
    std::string m_decorationName;

    /// Flag to calculate scale factor
    bool m_calcSF;
    bool m_isMC;

    /// String for scale factor decoration names
    std::string m_weightdecorationName;
    std::string m_weightFileName;
    std::string m_weightHistogramName;
    std::string m_efficiencyHistogramName;
    std::string m_weightFlavors;

    /// Histograms for scale factors
    std::unique_ptr<TFile> m_weightConfig;
    std::map<std::string, std::unique_ptr<TH2D>> m_weightHistograms;
    std::map<std::string, std::unique_ptr<TH2D>> m_efficiencyHistograms;

    /// Truth labeling tool
    asg::AnaToolHandle<JetTruthLabelingTool> m_JetTruthLabelingTool;

    /// Truth label options
    bool m_truthLabelUseTRUTH3;
    std::string m_truthParticleContainerName;
    std::string m_truthBosonContainerName;
    std::string m_truthTopQuarkContainerName;
    std::string m_truthLabelName;

    /// Truth label accessor
    std::unique_ptr< SG::AuxElement::ConstAccessor<int> > m_acc_truthLabel;

    /// Decorators for cut values
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_mcutL;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_mcutH;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_scoreCut;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_scoreValue;

    /// Decorators for SF
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_weight;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_efficiency;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_effSF;
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_sigeffSF;
    std::unique_ptr< SG::AuxElement::Decorator<int> > m_dec_accept;

    /// Get configReader StatusCode
    StatusCode getConfigReader();

    /// Reset cuts
    void resetCuts() const;

    /// Check if jet passes kinematic constraints
    bool passKinRange( const xAOD::Jet &jet ) const;

    /// Check and record if jet passes kinematic constraints
    void checkKinRange( const xAOD::Jet &jet ) const;

    /// Calculate JSS moment ratios in case they are not already saved
    int calculateJSSRatios( const xAOD::Jet &jet ) const;

    /// Get SF weight
    void getWeight( const xAOD::Jet& jet, bool passSel ) const;

    /// Get scale factor and efficiency
    std::string getTruthLabelStr( const xAOD::Jet& jet ) const;
    std::pair<double,double> getSF( const xAOD::Jet& jet, const std::string truthLabelStr ) const;

    /// Print configured cuts
    void printCuts() const;

};

#endif
