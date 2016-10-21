/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********
//
// NAME:     TrigJetFromPseudojet.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
// 
// AUTHOR:   Steven Schramm
//
// Description: Helper class to support jet trimming in the HLT (needed due to bystream auxstore limitations where offline tools are trying to add dynamic ElementLink vectors)
//
// **********


#ifndef TRIGHLTJETREC_TRIGJETFROMPSEUDOJET_H
#define TRIGHLTJETREC_TRIGJETFROMPSEUDOJET_H


#include "JetRec/JetFromPseudojet.h"


class TrigJetFromPseudojet : public JetFromPseudojet
{
    ASG_TOOL_CLASS(TrigJetFromPseudojet, IJetFromPseudojet)

    public:
        TrigJetFromPseudojet(std::string name);

        virtual xAOD::Jet* add(const fastjet::PseudoJet& pj, xAOD::JetContainer& jets, const xAOD::Jet* pparent) const;
};


#endif


