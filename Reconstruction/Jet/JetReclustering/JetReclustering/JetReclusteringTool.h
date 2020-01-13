/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JetReclustering_JetReclusteringTool_H
#define JetReclustering_JetReclusteringTool_H

// making it more like a tool
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"
#include "JetInterface/IJetExecuteTool.h"

#include <map>
#include <memory>

/*
  Author  : Giordon Stark
  Email   : gstark@cern.ch
  Thanks to Ben Nachman for inspiration, P-A for the help, Jon Burr for
  assisting in dual-use conversion.

  Takes a set of small-R jets and reclusters to large-R jets
*/

class IJetModifier;
//class IJetExecuteTool;
class IJetFromPseudojet;
class IJetFinder;
class IJetGroomer;
class IPseudoJetGetter;
class IJetPseudojetRetriever;

class JetReclusteringTool : public asg::AsgTool, virtual public IJetExecuteTool {
  public:
    ASG_TOOL_CLASS(JetReclusteringTool, IJetExecuteTool)
    JetReclusteringTool(std::string myname);

    // initialization - set up everything
    virtual StatusCode initialize() override;

    // execute - build jets
    virtual int execute() const override;

    // display the configuration
    virtual void print() const override;

  private:
    // hold the class name
    std::string m_APP_NAME = "JetReclusteringTool";

  /* Properties */
    // input jet container to use as a constituent proxy
    std::string m_inputJetContainer;
    // output jet container to store reclustered jets
    std::string m_outputJetContainer;
    // output jet container to store ungroomed reclustered jets
    std::string m_ungroomedJetContainer;
    // radius of the reclustered jets
    float m_radius;
    // reclustering algorithm to use
    std::string m_rc_alg;
  
  /* variable R reclustering */
    // minimum radius
    float m_varR_minR;
    // mass scale (GeV)
    float m_varR_mass;
  /* end variable R reclustering */
    
    // minimum pt of the constituents (GeV)
    float m_ptMin_input;
    // minimum pt of the reclustered jets (GeV)
    float m_ptMin_rc;
    // grooming algorithm to apply to reclustered jets
    std::string m_groomAlg;
    // trimming parameters
    float m_trim_ptFrac;
    float m_trim_subjet_radius;
    // softdrop parameters
    float m_sd_zcut;
    float m_sd_beta;
    float m_sd_R0;
    int m_sd_N;
    // enable to add area attributes form
    bool m_doArea;
    std::string m_areaAttributes;
    // options for substructure tools
    std::vector<float> m_ecf_betaVals;
    bool m_ecf_doC3;
    bool m_ecf_doDichroic;
    bool m_ecfg_doN3;
    bool m_ecfg_doLSeries;
  
  /* ghost association containers */
    float m_ghostScale;
    // tracks
    std::string m_ghostTracksInputContainer;
    std::string m_ghostTracksVertexAssociationName;
    // b-tagging jets
    std::string m_ghostBTagJetInputContainer;
    std::string m_ghostBTagJetLabel;
    // truth B quarks
    std::string m_ghostTruthBQuarksInputContainer;
    std::string m_ghostTruthBQuarksLabel;
    // truth B hadrons
    std::string m_ghostTruthBHadronsInputContainer;
    std::string m_ghostTruthBHadronsLabel;
    // truth C quarks
    std::string m_ghostTruthCQuarksInputContainer;
    std::string m_ghostTruthCQuarksLabel;
    // truth C hadrons
    std::string m_ghostTruthCHadronsInputContainer;
    std::string m_ghostTruthCHadronsLabel;
    // truth T quarks
    std::string m_ghostTruthTQuarksInputContainer;
    std::string m_ghostTruthTQuarksLabel;
    // truth W bosons
    std::string m_ghostTruthWBosonsInputContainer;
    std::string m_ghostTruthWBosonsLabel;
    // truth Z bosons
    std::string m_ghostTruthZBosonsInputContainer;
    std::string m_ghostTruthZBosonsLabel;
    // truth H bosons
    std::string m_ghostTruthHBosonsInputContainer;
    std::string m_ghostTruthHBosonsLabel;
    
    // make sure someone only calls a function once
    bool m_isInitialized = false;
    // this is for filtering input jets
    asg::AnaToolHandle<IJetModifier> m_jetFilterTool;
    asg::AnaToolHandle<IJetExecuteTool> m_inputJetFilterTool;
    // this is for reclustering using filtered input jets
    asg::AnaToolHandle<IPseudoJetGetter> m_pseudoJetGetterTool;
    asg::AnaToolHandle<IPseudoJetGetter> m_pseudoGhostTrackJetGetterTool;
    asg::AnaToolHandle<IJetFromPseudojet> m_jetFromPseudoJetTool;
    asg::AnaToolHandle<IJetFinder> m_jetFinderTool;
    asg::AnaToolHandle<IJetExecuteTool> m_reclusterJetTool;
    asg::AnaToolHandle<IJetExecuteTool> m_groomJetTool;

    // tool for calculating effectiveR
    asg::AnaToolHandle<IJetModifier> m_effectiveRTool;
    // tool for grooming reclustered jet
    asg::AnaToolHandle<IJetGroomer> m_jetGroomingTool;
    // tool for the jpjr for m_jetGroomTool
    asg::AnaToolHandle<IJetPseudojetRetriever> m_jetGroomTool_JPJR;
    // modifier tools for the reclustered jets
    asg::AnaToolHandle<IJetModifier> m_jetChargeTool;
    asg::AnaToolHandle<IJetModifier> m_jetPullTool;
    asg::AnaToolHandle<IJetModifier> m_energyCorrelatorTool;
    asg::AnaToolHandle<IJetModifier> m_energyCorrelatorRatiosTool;
    asg::AnaToolHandle<IJetModifier> m_energyCorrelatorGeneralizedTool;
    asg::AnaToolHandle<IJetModifier> m_energyCorrelatorGeneralizedRatiosTool;
    asg::AnaToolHandle<IJetModifier> m_ktSplittingScaleTool;
    asg::AnaToolHandle<IJetModifier> m_dipolarityTool;
    asg::AnaToolHandle<IJetModifier> m_centerOfMassShapesTool;
    asg::AnaToolHandle<IJetModifier> m_nSubjettinessTool;
    // pseudojet getters for ghost association
    asg::AnaToolHandle<IPseudoJetGetter> m_pseudoBTagJetGetterTool;
    asg::AnaToolHandle<IPseudoJetGetter> m_pseudoTruthBQuarksGetterTool;
    asg::AnaToolHandle<IPseudoJetGetter> m_pseudoTruthBHadronsGetterTool;
    asg::AnaToolHandle<IPseudoJetGetter> m_pseudoTruthCQuarksGetterTool;
    asg::AnaToolHandle<IPseudoJetGetter> m_pseudoTruthCHadronsGetterTool;
    asg::AnaToolHandle<IPseudoJetGetter> m_pseudoTruthTQuarksGetterTool;
    asg::AnaToolHandle<IPseudoJetGetter> m_pseudoTruthWBosonsGetterTool;
    asg::AnaToolHandle<IPseudoJetGetter> m_pseudoTruthZBosonsGetterTool;
    asg::AnaToolHandle<IPseudoJetGetter> m_pseudoTruthHBosonsGetterTool;
};

#endif
