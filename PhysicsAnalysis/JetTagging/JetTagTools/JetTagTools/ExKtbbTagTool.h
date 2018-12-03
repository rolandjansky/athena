/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_EXKTBBTAGTOOL_H
#define JETTAGTOOLS_EXKTBBTAGTOOL_H

/*
  Author  : Qi Zeng
  Email   : qzeng@cern.ch
  
  This is a Tool based on JetModifierBase, to run exclusive kt algorithm on any existing jet collection
  Many thanks to Giordon Stark and Miles Wu who actually implement the exclusive kt subjet finder in JetEtMiss pkg
  and help me setup this in b-tagging pkg

  This tool is supposed to be called by a stand-alone algorithm that will call in the jet collection from event store and 
  load JetModifiers. It should be run before the b-tagging, but after jet finding

  This is basically just an interface to call SubjetFinderTool. However, we need to overwrite some subjet collection information,
  which might be important for b-tagging purpose
*/

#include "JetRec/JetModifierBase.h"
#include "AsgTools/ToolHandle.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "JetSubStructureMomentTools/SubjetRecorderTool.h"
#include "JetSubStructureUtils/SubjetFinder.h"
#include "xAODTracking/VertexContainer.h"

namespace Analysis{

class ExKtbbTagTool : public JetModifierBase {
  ASG_TOOL_CLASS(ExKtbbTagTool, IJetModifier)

  public:
    // Constructor from tool name.
    ExKtbbTagTool(std::string myname);

    virtual StatusCode initialize();

    // Need to modify a base function to fix bug when jet collection is empty
    virtual int modify(xAOD::JetContainer& jets) const;

    // Inherited method to modify a jet.
    virtual int modifyJet(xAOD::Jet& jet) const;

  private:
    // subjet finding parameter
    std::string m_JetAlgorithm;
    float       m_JetRadius;
    float       m_PtMin;
    int         m_ExclusiveNJets;
    bool        m_doTrack;

    // related to subjet recording
    std::string m_InputJetContainerName;             // Mainly used to build correct link from subjet to parent jet
    ToolHandle<ISubjetRecorderTool> m_SubjetRecorderTool;     
    std::string m_SubjetLabel;                       // must be exactly the same label as the one used by SubjetRecorder inside SubjetFinder. 
                                                     // At this moment, there is no way to figure out this name given SubjetFinderTool
    std::string m_SubjetContainerName;               // must be exactly the same container name as the one used by SubjetRecorder inside SubjetFinder.
                                                     // At this moment, there is no way to figure out this name given SubjetFinderTool
    std::string m_SubjetAlgorithm_BTAG;
    float       m_SubjetRadius_BTAG;

    // for track selection (if m_doTrack is turned on)
    ToolHandle<InDet::IInDetTrackSelectionTool> m_TrkSelTool;
    std::string m_PrimaryVtxContainerName;

    // for CoM
    bool m_SubjetBoostConstituent;

    // for ghost association
    std::string m_GhostSources;                      // a list of sources where the ghost association link comes from, separated with ","
    std::string m_GhostLabels;                       // a list of ghost association labels done at parent jet level, separated with ","

    ////////////////////////////////////////////////////////////////////////////////////////

    std::vector<fastjet::PseudoJet> constituentPseudoJets(xAOD::Jet& jet) const;
    std::vector<fastjet::PseudoJet> getBoostedConstituents(xAOD::Jet& jet) const;

    const xAOD::Jet* GetParentJet(const xAOD::Jet* jet) const;
    std::vector<std::string> ConvertStringToVector(std::string inputString, std::string delim) const;
    std::vector<std::string> m_GhostSourceVector;    // internal vector converted from m_GhostSources;
    std::vector<std::string> m_GhostLabelVector;     // internal vector converted from m_GhostLabels

    bool TrackSelection(const xAOD::TrackParticle* track) const;

    mutable const xAOD::Vertex* m_priVtx;
    const xAOD::Vertex* findHSVertex(const xAOD::VertexContainer* vertices) const;

    mutable const xAOD::JetContainer* m_jets_original;

};

// a small class for pseudojet user info
class SimplePseudoJetUserInfo : public fastjet::PseudoJet::UserInfoBase{

public:

  // constructor
  SimplePseudoJetUserInfo() {m_jetsource = ""; m_label = ""; m_index = -1;}
  SimplePseudoJetUserInfo(std::string jetsource, std::string label, int index) {m_jetsource = jetsource; m_label = label; m_index = index;}

  std::string jetsource() {return m_jetsource;}
  std::string label()     {return m_label;}
  int         index()     {return m_index;}

private:

  std::string m_jetsource;
  std::string m_label;
  int         m_index;

};

}

#endif
