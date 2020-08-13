/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

///////////////////////////////////////////////////////////////////
// JetQuarkLabel.h
//   Header file for class JetQuarkLabel
///////////////////////////////////////////////////////////////////
/**
 * <b>Purpose:</b> label jets with b or c quarks.<br>
 * <b>Author:</b> Jean-Baptiste de Vivie<br>
 * <b>Email:</b> devivie@cppm.in2p3.fr <br>
 * <b>Created:</b> 2005/02/25 <br>
 */

#ifndef PARTICLEJETTOOLS_JETQUARKLABEL_H
#define PARTICLEJETTOOLS_JETQUARKLABEL_H

#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgTools/AsgTool.h"
#include "ParticleJetTools/IJetTruthMatching.h"
#include "EventPrimitives/EventPrimitives.h"
#include <string>
#include <map>
#include "xAODJet/Jet.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"

//class McEventCollection;
class Jet;

namespace Analysis
{

class JetQuarkLabel : public asg::AsgTool, virtual public IJetTruthMatching {
    ASG_TOOL_CLASS(JetQuarkLabel, IJetTruthMatching)
    public:
        JetQuarkLabel(const std::string& name);
        virtual ~JetQuarkLabel();
        virtual StatusCode initialize() override;

        /* Method to truth tag a jet.
         * NB: for this particular algorithm, matchJet is TRUE if the jet is matched 
         * to a b-quark OR a c-quark. The jetLabel is set accordingly and is no 
         * longer a job option.
         */

         virtual bool matchJet(const xAOD::Jet& myJet,
                               MatchInfo* info = nullptr) const override;

        virtual void printParameterSettings() const override;

        inline void EventSelection(short s) { m_inTime = s; }
        inline short EventSelection() const { return m_inTime; }

    private:
        SG::ReadHandleKey<xAOD::TruthEventContainer> m_truthEventContainerKey{this,"McEventCollection","TruthEvents","ReadHandleKey for xAOD::TruthEventContainer"};
        std::string m_mcEventCollection;
        double m_deltaRCut; //!< deltaR cut value of the cone matching (max distance between Jet axis and momentum of truth particel)
        double m_ptCut;     //!< pT cut for partons
        bool   m_noDoc;
        short  m_inTime;
        bool testJet(const xAOD::Jet&, const xAOD::TruthEventContainer*,
                     MatchInfo* info) const;
};


}
#endif // TRUTHMATCHTOOLS_JETQUARKLABEL_H

