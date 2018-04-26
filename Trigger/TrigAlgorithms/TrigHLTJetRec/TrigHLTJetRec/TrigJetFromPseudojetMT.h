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


#ifndef TRIGHLTJETREC_TRIGJETFROMPSEUDOJETMT_H
#define TRIGHLTJETREC_TRIGJETFROMPSEUDOJETMT_H


#include "JetRec/JetFromPseudojetMT.h"


class TrigJetFromPseudojetMT : public JetFromPseudojetMT
{
    ASG_TOOL_CLASS(TrigJetFromPseudojetMT, IJetFromPseudojetMT)

    public:
        TrigJetFromPseudojetMT(std::string name);

        using JetFromPseudojetMT::add;
        virtual xAOD::Jet* add(const fastjet::PseudoJet& pj,
                               const PseudoJetContainer&,
                               xAOD::JetContainer& jets,
                               const xAOD::Jet* pparent) const;
};


#endif


