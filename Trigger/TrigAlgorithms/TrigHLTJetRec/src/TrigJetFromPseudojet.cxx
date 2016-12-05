/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigHLTJetRec/TrigJetFromPseudojet.h"
#include "fastjet/PseudoJet.hh"

TrigJetFromPseudojet::TrigJetFromPseudojet(std::string name)
    : JetFromPseudojet(name)
{ }


xAOD::Jet*
TrigJetFromPseudojet::add(const fastjet::PseudoJet& pj, xAOD::JetContainer& jets,
                      const xAOD::Jet* pparent) const
{
    // Build the jet without the parent to avoid the creation of a dynamic link
    std::vector<std::string> noghosts;
    xAOD::Jet* pjet = addjet(pj, jets, 0, &noghosts);
    if (!pjet) return pjet;

    // Now add information from the parent
    // Same as what's done in the offline tool, except with the dynamic parent link removed
    if (!pparent)
        ATH_MSG_VERBOSE("  No parent jet.");
    else
    {
        ATH_MSG_VERBOSE("  Setting input type from parent.");
        xAOD::JetInput::Type inputtype = pparent->getInputType();
        pjet->setInputType(inputtype);
        ATH_MSG_VERBOSE("  Setting algorithm type from parent.");
        pjet->setAlgorithmType(pparent->getAlgorithmType());
        ATH_MSG_VERBOSE("  Setting size parameter from parent.");
        pjet->setSizeParameter(pparent->getSizeParameter());
        ATH_MSG_VERBOSE("  Setting signal state from parent.");
        pjet->setConstituentsSignalState(pparent->getConstituentsSignalState());
        ATH_MSG_VERBOSE("  Setting ghost area from parent.");
        pjet->setAttribute("JetGhostArea", pparent->getAttribute<float>("JetGhostArea"));
        ATH_MSG_VERBOSE("  Setting EM scale momentum.");
        buildAndSetEMScaleMom(pjet, inputtype );
        ATH_MSG_VERBOSE("  Done adding parent info");
    }

    return pjet;
}

