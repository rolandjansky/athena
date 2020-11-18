/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetObjectCollectionMaker.h 809674 2017-08-23 14:10:24Z iconnell $
#ifndef ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_JETOBJECTCOLLECTIONMAKER_H
#define ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_JETOBJECTCOLLECTIONMAKER_H

/**
 * @author John Morris <john.morris@cern.ch>
 *
 * @brief JetObjectCollectionMaker
 *   Makes all systematic variations of jets
 *
 *   This is actually quite a complex tool
 *   The steps are as follows
 *   (1) Calibrate data/MC jets and save this collection to TStore
 *   (2) Loop over the various NP JES scenarios, using the output of (1) as an input
 *   (3) Loop over the various NP JER scenarios, using the output of (1) as an input
 *   (2.a , 3.a) Re-calculate the JVT for each jet
 *
 * $Revision: 809674 $
 * $Date: 2017-08-23 15:10:24 +0100 (Wed, 23 Aug 2017) $
 *
 **/

// system include(s):
#include <memory>
#include <set>
#include <list>
#include <unordered_map>


// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "xAODJet/JetConstituentVector.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

// CP Tool include(s):
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCPInterfaces/ICPJetUncertaintiesTool.h"
#include "JetInterface/IJetUpdateJvt.h"

#include "TopJetSubstructure/TopJetSubstructure.h"

#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"
#include "JetAnalysisInterfaces/IJetSelectorTool.h"
#include "ParticleJetTools/JetTruthLabelingTool.h"

// Forward declaration(s):
namespace top {
  class TopConfig;
}

namespace top {
  class JetObjectCollectionMaker final: public asg::AsgTool {
    using systMap = std::unordered_map<CP::SystematicSet, CP::SystematicSet>;
  public:
    explicit JetObjectCollectionMaker(const std::string& name);
    virtual ~JetObjectCollectionMaker() {}

    // Delete Standard constructors
    JetObjectCollectionMaker(const JetObjectCollectionMaker& rhs) = delete;
    JetObjectCollectionMaker(JetObjectCollectionMaker&& rhs) = delete;
    JetObjectCollectionMaker& operator = (const JetObjectCollectionMaker& rhs) = delete;

    StatusCode initialize();

    StatusCode executeJets(bool);
    StatusCode executeLargeRJets(bool);
    StatusCode executeTrackJets(bool);

    StatusCode printoutJets();
    StatusCode printoutLargeRJets();

    // return specific Systematic
    inline virtual const std::list<CP::SystematicSet>& specifiedSystematics()   const {return m_specifiedSystematics;}
    inline virtual const std::list<CP::SystematicSet>& specifiedSystematicsLargeR()   const {
      return m_specifiedSystematicsLargeR;
    }

    // return all recommendedSystematics
    inline const std::list<CP::SystematicSet>& recommendedSystematics()   const {return m_recommendedSystematics;}
  protected:
  
    // Function will return prefix which will be added to systematic names obtained from tools
    virtual std::string getLargeRModName(const std::string& NPModel) const;
    // specify Systematic
    virtual void addSystematics(const std::set<std::string>& specifiedSystematics,
                                const CP::SystematicSet& recommendedSysts, std::unordered_map<CP::SystematicSet,
                                                                                              CP::SystematicSet>& map,
                                const std::string& modName, bool isLargeR = false,
                                bool onlyJER = false);

    StatusCode execute(const bool isLargeR, bool executeNominal);

    StatusCode calibrate(const bool isLargeR);
    virtual StatusCode applySystematic(ToolHandle<ICPJetUncertaintiesTool>& tool,
                                       const std::unordered_map<CP::SystematicSet, CP::SystematicSet>& map,
                                       bool isLargeR = false);

    StatusCode printout(const bool isLargeR);

  private:
    std::shared_ptr<top::TopConfig> m_config;
    bool m_doJER;
    bool m_doFull_JER;
    bool m_doFull_JER_Pseudodata;
    bool m_isMC;
    bool m_doMultipleJES;
    bool m_do_fjvt = false;

    std::list<CP::SystematicSet> m_specifiedSystematics;
    std::list<CP::SystematicSet> m_specifiedSystematicsLargeR;
    std::list<CP::SystematicSet> m_specifiedSystematicsTrackJets;
    std::list<CP::SystematicSet> m_recommendedSystematics;
    CP::SystematicSet m_nominalSystematicSet;

    ToolHandle<IJetCalibrationTool> m_jetCalibrationTool;
    ToolHandle<IJetCalibrationTool> m_jetCalibrationToolLargeR;

    ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesTool;
    ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolFrozenJMS;
    ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolReducedNPScenario1;
    ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolReducedNPScenario2;
    ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolReducedNPScenario3;
    ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolReducedNPScenario4;

    ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolLargeR;
    std::unordered_map<std::string, ToolHandle<ICPJetUncertaintiesTool> > m_tagSFuncertTool;

    // do decorate the large-R jets with the boosted-tagging flags
    // and decorate jets with TAccept object containing detailed tag result informaiton
    // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BoostedJetTaggingRecommendation2017#TAcceptUsageSection
    std::unordered_map<std::string, ToolHandle<IJetSelectorTool> > m_boostedJetTaggers;

    ToolHandle<IJetUpdateJvt> m_jetUpdateJvtTool;
    ToolHandle<IJetModifier> m_jetSelectfJvtTool;

    std::string m_truthJetCollForHS;

    std::unique_ptr<top::TopJetSubstructure> m_jetSubstructure;

    systMap m_systMap_AllNP;
    systMap m_systMap_AllNP_FrozenJMS;
    systMap m_systMap_ReducedNPScenario1;
    systMap m_systMap_ReducedNPScenario2;
    systMap m_systMap_ReducedNPScenario3;
    systMap m_systMap_ReducedNPScenario4;
    systMap m_systMap_LargeR;

    typedef std::unordered_map<CP::SystematicSet, CP::SystematicSet>::const_iterator Itr;

    StatusCode decorateBJets(xAOD::Jet& jet);
    StatusCode decorateHSJets();
    StatusCode decorateMatchedTruth();
    StatusCode tagLargeRJet(const xAOD::Jet& jet);
    StatusCode tagNominalLargeRJets();

    ///-- Large R jet truth labeling --///
    std::unique_ptr<JetTruthLabelingTool> m_jetTruthLabelingTool;

    // DL1 decoration
    std::unordered_map<std::string, ToolHandle<IBTaggingSelectionTool> > m_btagSelToolsDL1Decor;
    std::unordered_map<std::string, ToolHandle<IBTaggingSelectionTool> > m_btagSelToolsDL1Decor_trkJet;
    StatusCode decorateDL1(bool trackJets = false);

    std::unordered_map<std::string, SG::AuxElement::Decorator<float>> DLx;
  };
} // namespace
#endif
