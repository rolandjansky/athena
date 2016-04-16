/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
        StatusCode initialize();
        StatusCode finalize();

        /* Method to truth tag a jet.
         * NB: for this particular algorithm, matchJet is TRUE if the jet is matched 
         * to a b-quark OR a c-quark. The jetLabel is set accordingly and is no 
         * longer a job option.
         */

        virtual bool matchJet(const xAOD::Jet& myJet);

        virtual void m_printParameterSettings();

        /** NEXT METHODS ARE ONLY ACCESSIBLE AFTER CASTING!! */

        /** Return barcode */
        int barcode() const;

        /** Return pdg to match */
        int pdgCode() const;

        /** Return the predefined name to label the jets passing the matching: */
        inline const int& jetLabel() const { return m_jetLabel; }

        /** Return the min distance to quarks: */
        double deltaRMinTo(const std::string&) const;
        inline const std::map<std::string, double>& distanceToQuarks() const { return m_distanceToQuarks; }

        /** Return the B decay vertex position: */
        inline const Eigen::Vector3d& BDecVtx() const { return m_BDecVtx; }
        int Bpdg() const { return m_Bpdg; }

        /** Get the number of MC Events in the McEventCollection: */
        inline int NEventInCollection() const { return m_NEventInCollection; }
        inline void EventSelection(short s) { m_inTime = s; }
        inline short EventSelection() const { return m_inTime; }

    private:
        std::string m_mcEventCollection; 
        double m_deltaRCut; //!< deltaR cut value of the cone matching (max distance between Jet axis and momentum of truth particel)
        double m_ptCut;     //!< pT cut for partons
        bool   m_noDoc;
        short  m_inTime;
        bool m_testJet(const xAOD::Jet&, const xAOD::TruthEventContainer*);
        int m_jetLabel; //!< label to use for matching jets
        std::map<std::string, double> m_distanceToQuarks; //!< keep track of distances to quarks
        Eigen::Vector3d m_BDecVtx; //!< positon of the lowest lying B hadron vertex decay
        int    m_pdg;       //!< pdg code of the parton/baryon the jet has been matched to (which was closest)
        int    m_barcode;   //!< barcode of the matched parton (to be able to find the parton in the McEventColl)
        int m_Bpdg;
        int m_NEventInCollection;
};

/** Return barcode */
inline int JetQuarkLabel::barcode()  const { return m_barcode; }

/** Return pdg to match */
inline int JetQuarkLabel::pdgCode()  const { return m_pdg; }

}
#endif // TRUTHMATCHTOOLS_JETQUARKLABEL_H

