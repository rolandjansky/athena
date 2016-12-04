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
#include "JetSubStructureMomentTools/SubjetRecorderTool.h"
#include "JetSubStructureUtils/SubjetFinder.h"

namespace Analysis{

class ExKtbbTagTool : public JetModifierBase {
  ASG_TOOL_CLASS(ExKtbbTagTool, IJetModifier)

  public:
    // Constructor from tool name.
    ExKtbbTagTool(std::string myname);

    virtual StatusCode initialize();

    // Inherited method to modify a jet.
    virtual int modifyJet(xAOD::Jet& jet) const;

  private:
    // subjet finding parameter
    std::string m_JetAlgorithm;
    float       m_JetRadius;
    float       m_PtMin;
    int         m_ExclusiveNJets;

    // related to subjet recording
    std::string m_InputJetContainerName;             // Mainly used to build correct link from subjet to parent jet
    ToolHandle<ISubjetRecorderTool> m_SubjetRecorderTool;     


    std::string m_SubjetLabel;                       // must be exactly the same label as the one used by SubjetRecorder inside SubjetFinder. 
                                                     // At this moment, there is no way to figure out this name given SubjetFinderTool
    std::string m_SubjetContainerName;               // must be exactly the same container name as the one used by SubjetRecorder inside SubjetFinder.
                                                     // At this moment, there is no way to figure out this name given SubjetFinderTool
    std::string m_SubjetAlgorithm_BTAG;
    float       m_SubjetRadius_BTAG;

    std::vector<fastjet::PseudoJet> constituentPseudoJets(xAOD::Jet& jet) const;
};

}

#endif
