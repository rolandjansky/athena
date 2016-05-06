/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetMuonSegmentMomentsTool.h

#ifndef JETMOMENTTOOLS_JETMUONSEGMENTMOMENTSTOOL_H
#define JETMOMENTTOOLS_JETMUONSEGMENTMOMENTSTOOL_H

/// Steven Schramm \n
/// February 2014
/// 
/// Tool to calculate general muon segment based jet moments
/// Primarily intended for punch-through studies and corrections
///
/// Moments to calculate:
///     None yet...

#include "JetRec/JetModifierBase.h"

class JetMuonSegmentMomentsTool : public JetModifierBase
{
    ASG_TOOL_CLASS(JetMuonSegmentMomentsTool,IJetModifier)

    public:
        // Constructor from tool name
        JetMuonSegmentMomentsTool(const std::string& name);

        // Inherited methods to modify a jet
        virtual int modifyJet(xAOD::Jet& jet) const;

    private:
        // Configurable parameters
        std::string m_assocMuonSegName;
};

#endif

