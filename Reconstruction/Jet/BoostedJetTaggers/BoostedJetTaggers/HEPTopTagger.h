// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPTopTagger_H
#define HEPTopTagger_H

/*
 *  This tool uses the HEPTopTagger algorithm (JHEP 1301 (2013) 116, arXiv:1006.2833, arXiv:0910.5472) to reconstruct hadronic top quarks.
 *
 * Original code by:
 * Sebastian Schaetzel (schaetzel@physi.uni-heidelberg.de)
 * Gregor Kasieczka (g.kasieczka@gmail.com)
 * Christoph Anders (canders@cern.ch)
 *
 * https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/HEPTopTagger
 *
 * C++ only version: Christoph Eckardt (christoph.eckardt@cern.ch)
 *
 * Re-written for Run2 Atlas software : P-A Delsart
 */

// general include(s):
#include "fastjet/PseudoJet.hh"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"


#include "JetInterface/IJetFromPseudojet.h"
#include "JetInterface/IJetModifier.h"
#include "JetCalibTools/IJetCalibrationTool.h"

namespace HTTxAOD {




  ///******************************************************
  class HEPTopTagger : public asg::AsgTool,
                       virtual public IJetModifier {
    ASG_TOOL_CLASS(HEPTopTagger, IJetModifier)
    public:


    /// a set of quantities related to a top candidate.
    /// this is used as a return value from the main runTagger() function
    struct HTTResult {
      bool validCandidate;
      fastjet::PseudoJet top;
      std::vector<fastjet::PseudoJet> topSubJets;
      std::vector<fastjet::PseudoJet> topConstituents;
      float drmaxpairsubstruct ;
      float drmax3substruct ;
      bool isTagged;
      int n_top_cands;
    };

    /// Default constructor
    HEPTopTagger(const std::string& name);
    /// Default destructor
    ~HEPTopTagger();

    StatusCode initialize();

    /// Implements IJetModifier
    /// Builds and records tagged jets from the input jet container.
    int modify(xAOD::JetContainer& jets) const;

    /// Fills topCandidates
    int findTopCandidates(xAOD::JetContainer & input,
                          xAOD::JetContainer & topCandidates,
                          xAOD::JetContainer & topSubJets,
                          xAOD::JetContainer & groomedFatJets) const ;

    HTTResult runTagger(const fastjet::PseudoJet &jet) const ;

    void notTagged(xAOD::Jet *j) const;

    void UseNormalTagger();
    void UseDefTagger();
    void UsePtordTagger();
    void UseAcutTagger();
    void UseDefR03Tagger();
    void UseDefR02Tagger();
    void UseDefNf6Tagger();
    void UseTightTagger();
    void UseLooseTagger();
    void UseSuperLooseTagger();
    void UseSJCalibXTagger();
    void UseSJ2DCalibXTagger();
    void UseSJ3DCalibXTagger();
    void UseSJNDCalibXTagger();
    void UseNoAShapeTagger();

    void setSJNDCalibPtEtaBins(int ver = 0);

    // Output functions

  private:
    std::string m_name;


    ToolHandle<IJetFromPseudojet> m_jetFromPJTool;  // Tool to build jets.
    ToolHandleArray<IJetCalibrationTool> m_jetCalibTools;
    ToolHandleArray<IJetCalibrationTool> m_jetCalibToolsCA15;

    std::string m_preDefSettings;


    std::string m_topContName;
    //std::string m_topSubjetContName;
    std::string m_topCandLink;
    std::string m_topCandSubJetLink;
    std::string m_topgroomedFatJetLink;
    std::string m_groomedFatJetLink;

    // Functions
    struct BWWIndex { int b,w1,w2 ; double m12, m13,m23; };
    BWWIndex extractBWMij(const std::vector<fastjet::PseudoJet>& top_subs) const ;
    bool inMassRange(double mass) const;
    void fillJetContainer(xAOD::JetContainer& cont, std::vector<fastjet::PseudoJet> & pjVec) const ;

    const IJetCalibrationTool * getCalibrator(double r) const;
    const IJetCalibrationTool * getCalibratorCA15() const;

    double findTopSubJetsRparam(const fastjet::ClusterSequence * cs , fastjet::PseudoJet & pj) const ;

    std::vector<fastjet::PseudoJet> inclCsFunction( fastjet::ClusterSequence *cs ) const ;
    std::vector<fastjet::PseudoJet> exclCsFunction( fastjet::ClusterSequence *cs,
                                               const int njets ) const ;


    void FindHardSubst(const fastjet::PseudoJet &this_jet,
                       std::vector<fastjet::PseudoJet> &t_parts,
                       const fastjet::ClusterSequence &cs) const ;

    double check_cos_theta(const fastjet::PseudoJet &jet,
                           const fastjet::PseudoJet &subj1,
                           const fastjet::PseudoJet &subj2) const;

    double r_max_3jets(const fastjet::PseudoJet &jet1,
                       const fastjet::PseudoJet &jet2,
                       const fastjet::PseudoJet &jet3) const;

    bool check_mass_criteria(const double rmin,
                             const double rmax,
                             const std::vector<fastjet::PseudoJet> &top_subs) const;

    void groomFatJet(xAOD::Jet *j,
			xAOD::Jet *&groomedJet,
			xAOD::JetContainer & groomedFatJetsCont,
			float Rjet,
			int NFiltJets,
			float fracPt) const;

   void applyInSituJESRescale(fastjet::PseudoJet& pj) const;


    // Properties of HEPTopTagger algorithm
    // bool   m_isData;

    bool   m_CutCalibratedSubjetPt;
    // bool   m_InclusiveSubjets_areaCorrection;
    // bool   m_ExclusiveSubjets_areaCorrection;
    bool   m_ExclusiveSubjets_Rparam;
    bool   m_CorrectSubjetJES;
    bool   m_SortTopCandInPt;
    bool   m_AcutBeforeSorting;
    double m_ScaleFactor_JES;
    std::vector<double> m_ScaleFactor_JES_EtaPT;
    std::vector<double> m_ScaleFactor_JES_aetamin;
    std::vector<double> m_ScaleFactor_JES_aetamax;
    std::vector<double> m_ScaleFactor_JES_ptmin;
    std::vector<double> m_ScaleFactor_JES_ptmax;

    int    m_TaggerAlgorithm;

    // double m_MinFatjetPt;
    double m_MinFatjetM;
    double m_MaxFatjetEta;

    double m_MassCutoff;
    double m_MassDrop;

    double m_RadiusFilter ;

    double m_MinSubjetPt;
    double m_MaxSubjetEta;

    int    m_NJetsFilter;
    double m_lowerMassCut;
    double m_upperMassCut;

    double m_TopMass;
    double m_WMass;
    double m_LowerXBound;
    double m_UpperXBound;
    double m_LowerYBound;
    double m_RelMassWidth;

    double m_MinCandPt;

    int    m_FilterNSubJets;
    double m_FilterR;
    std::string m_groomedContName;

    // bool   debug;

    // Output Variables

  }; // class HEPTopTagger
}

#endif // HEPTopTagger_H


