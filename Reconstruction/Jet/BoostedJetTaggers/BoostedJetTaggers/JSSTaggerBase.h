/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_JSSTAGGERBASE_H
#define BOOSTEDJETSTAGGERS_JSSTAGGERBASE_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

#include "JetInterface/IJetDecorator.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include "BoostedJetTaggers/TagResultEnum.h"

#include "PATCore/AcceptData.h"

#include "PathResolver/PathResolver.h"

#include "AsgDataHandles/WriteDecorHandle.h"
#include "AsgDataHandles/ReadDecorHandle.h"

#include <TFile.h>
#include <TEnv.h>
#include <TH2.h>
#include <TF1.h>

#include <atomic>

class JSSTaggerBase :   public asg::AsgTool ,
  virtual public IJetDecorator
{
  ASG_TOOL_CLASS1(JSSTaggerBase, IJetDecorator )

  protected:

    /// Default constructor - to be used in all derived classes
    JSSTaggerBase(const std::string &name);

    /// Default destructor - to be used in all derived classes
    ~JSSTaggerBase() {};

    /// Initialize the tool
    virtual StatusCode initialize() override;

    /// Decorate jet collection with tagging info
    virtual StatusCode decorate( const xAOD::JetContainer& jets ) const override;

    /// Decorate single jet with tagging info
    virtual StatusCode tag( const xAOD::Jet& jet ) const = 0;

    /// TEnv instance to read config files
    TEnv m_configReader;

    /// Object that stores the results for a jet
    asg::AcceptInfo m_acceptInfo;

    /// WriteDecorHandle keys for tagging bools
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decTaggedKey{this, "TaggedName", "Tagged", "SG key for Tagged"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decValidPtRangeHighKey{this, "ValidPtRangeHighName", "ValidPtRangeHigh", "SG key for ValidPtRangeHigh"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decValidPtRangeLowKey{this, "ValidPtRangeLowName", "ValidPtRangeLow", "SG key for ValidPtRangeLow"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decValidEtaRangeKey{this, "ValidEtaRangeName", "ValidEtaRange", "SG key for ValidEtaRange"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decValidJetContentKey{this, "ValidJetContentName", "ValidJetContent", "SG key for ValidJetContent"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decValidEventContentKey{this, "ValidEventContentName", "ValidEventContent", "SG key for ValidEventContent"};
    
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decPassMassKey{this, "PassMassName", "PassMass", "SG key for PassMass"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decPassScoreKey{this, "PassScoreName", "PassScore", "SG key for PassScore"};
    
    /// Maximum number of warnings
    const int m_nWarnMax = 10;

    /// Warning counters
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

    /// Jet container name
    std::string m_containerName;

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

    /// WriteDecorHandle keys for JSS moments
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decTau21WTAKey{this, "Tau21WTAName", "Tau21_wta", "SG key for Tau21_wta"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decTau32WTAKey{this, "Tau32WTAName", "Tau32_wta", "SG key for Tau32_wta"};
    
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decC2Key{this, "C2Name", "C2", "SG key for C2"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decD2Key{this, "D2Name", "D2", "SG key for D2"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decE3Key{this, "e3Name", "e3", "SG key for e3"};
    
    /// ReadDecorHandle keys for JSS moments
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readTau1WTAKey{this, "Tau1WTAName", "Tau1_wta", "SG key for Tau1_wta"};
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readTau2WTAKey{this, "Tau2WTAName", "Tau2_wta", "SG key for Tau2_wta"};
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readTau3WTAKey{this, "Tau3WTAName", "Tau3_wta", "SG key for Tau3_wta"};
 
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readTau21WTAKey{this, "Tau21WTAName", "Tau21_wta", "SG key for Tau21_wta"};
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readTau32WTAKey{this, "Tau32WTAName", "Tau32_wta", "SG key for Tau32_wta"};
    
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readECF1Key{this, "ECF1Name", "ECF1", "SG key for ECF1"};
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readECF2Key{this, "ECF2Name", "ECF2", "SG key for ECF2"};
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readECF3Key{this, "ECF3Name", "ECF3", "SG key for ECF3"};
    
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readC2Key{this, "C2Name", "C2", "SG key for C2"};
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readD2Key{this, "D2Name", "D2", "SG key for D2"};
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readE3Key{this, "e3Name", "e3", "SG key for e3"};
    
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readSplit12Key{this, "Split12Name", "Split12", "SG key for Split12"};
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readSplit23Key{this, "Split23Name", "Split23", "SG key for Split23"};
    
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readQwKey{this, "QwName", "Qw", "SG key for Qw"};

    /// Strings for cut functions
    std::string m_strMassCutLow;
    std::string m_strMassCutHigh;
    std::string m_strScoreCut;

    /// TF1 for cut functions
    std::unique_ptr<TF1> m_funcMassCutLow;
    std::unique_ptr<TF1> m_funcMassCutHigh;
    std::unique_ptr<TF1> m_funcScoreCut;

    /// Decoration name
    std::string m_decorationName;

    /// Flag to calculate scale factor
    bool m_calcSF;
    bool m_isMC;

    /// String for scale factor decoration names
    std::string m_weightDecorationName;
    std::string m_weightFileName;
    std::string m_weightHistogramName;
    std::string m_efficiencyHistogramName;
    std::string m_weightFlavors;

    /// Histograms for scale factors
    std::unique_ptr<TFile> m_weightConfig;
    std::map<std::string, std::unique_ptr<TH2D>> m_weightHistograms;
    std::map<std::string, std::unique_ptr<TH2D>> m_efficiencyHistograms;

    /// Truth label options
    bool m_truthLabelUseTRUTH3;
    std::string m_truthParticleContainerName;
    std::string m_truthBosonContainerName;
    std::string m_truthTopQuarkContainerName;
    std::string m_truthLabelName;

    /// Truth label ReadDecorHandle key
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_readTruthLabelKey{this, "truthLabelName", "truthLabel", "SG key for truthLabel"};

    /// WriteDecorHandle keys for cut values
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decCutMLowKey{this, "CutMLowName", "Cut_mlow", "SG key for Cut_mlow"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decCutMHighKey{this, "CutMHighName", "Cut_mhigh", "SG key for Cut_mhigh"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decScoreCutKey{this, "CutScoreName", "Cut_score", "SG key for Cut_score"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decScoreValueKey{this, "ScoreName", "Score", "SG key for Score"};

    /// WriteDecorHandle keys for SF
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decWeightKey{this, "weightName", "weight", "SG key for weight"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decEfficiencyKey{this, "efficiencyName", "efficiency", "SG key for efficiency"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_decEffSFKey{this, "effSFName", "effSF", "SG key for effSF"};

    /// Get configReader StatusCode
    StatusCode getConfigReader();

    /// Reset cuts
    StatusCode resetCuts( asg::AcceptData &acceptData ) const;

    /// Check if jet passes kinematic constraints
    bool passKinRange( const xAOD::Jet &jet ) const;

    /// Check and record if jet passes kinematic constraints
    StatusCode checkKinRange( const xAOD::Jet &jet, asg::AcceptData &acceptData ) const;

    /// Calculate JSS moment ratios in case they are not already saved
    /// TODO: Remove this once JSSMomentTools is modified to take const jets
    int calculateJSSRatios( const xAOD::Jet &jet ) const;

    /// Get SF weight
    StatusCode getWeight( const xAOD::Jet& jet, bool passSel, asg::AcceptData &acceptData ) const;

    /// Get scale factor and efficiency
    std::pair<double,double> getSF( const xAOD::Jet& jet, asg::AcceptData &acceptData ) const;

    /// Print configured cuts
    void printCuts() const;

};

#endif
