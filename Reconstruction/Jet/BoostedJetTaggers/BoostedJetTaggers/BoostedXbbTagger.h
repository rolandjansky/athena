/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JSSXBBTAGGER_H_
#define JSSXBBTAGGER_H_

//////////////////////////////////////////////////
// boosted Xbb tagger
// Felix Mueller <fmueller@cern.ch>
// Eric Takasugi <eric.hayato.takasugi@cern.ch>
//
// based on original BoostedXbbTag tool in JetSubStructureUtils by
// Giordon Stark <gstark@cern.ch>
//////////////////////////////////////////////////

#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "AsgTools/AsgTool.h"

// c++ includes
#include <set>
#include <string>
#include <memory>
#include "TF1.h"

// EDM includes
#include <xAODJet/JetContainer.h>
#include <xAODMuon/MuonContainer.h>
#include "MuonSelectorTools/MuonSelectionTool.h"
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"

class BoostedXbbTagger : public JSSTaggerBase {

  ASG_TOOL_CLASS0(BoostedXbbTagger)

  public:
    enum MuonCorrectionScheme {
        Combined = 0,
        Calorimeter,
        TrackAssisted,
        SimpleMuon,
        NoMuon
    };

    // standard tool constructor
    BoostedXbbTagger(const std::string& name );

    // destructor
    ~BoostedXbbTagger();

    StatusCode initialize();
    StatusCode finalize();

    virtual Root::TAccept tag(const xAOD::Jet& jet) const;

    // return a vector of track jets which are btagged by the tool
    std::vector<const xAOD::Jet*> getTrackJets(const xAOD::Jet& jet) const;

    // return the calibrated, matched muons
    std::vector<const xAOD::Muon*> getCorrectionMuons(const xAOD::Jet& jet) const;

    // return the TLorentzVector for the jet after muon-in-jet correction
    TLorentzVector getCorrectedJetTLV(const xAOD::Jet& jet) const;

    // return the TLorentzVector for the calibrated muon that was used for jet correction (if uncalibrated muonContainer was set)
    TLorentzVector getCalibratedMuonTLV(const xAOD::Muon& muon) const;

    // return lower bound of pT dependent mass window
    float getMassMin(const xAOD::Jet& jet) const;

    // return upper bound of pT dependent mass window
    float getMassMax(const xAOD::Jet& jet) const;

  private:
    StatusCode decorateWithMuons(const xAOD::Jet& jet) const;
    StatusCode getMuonCorrectionScheme(std::string scheme_name, MuonCorrectionScheme& scheme) const;
    std::string getMuonCorrectionSchemeName(MuonCorrectionScheme scheme) const;
    const xAOD::JetFourMom_t getMuonCorrectedJetFourMom(const xAOD::Jet& jet, std::vector<const xAOD::Muon*> muons, MuonCorrectionScheme scheme, bool useJMSScale = false) const;
    StatusCode getJSSVar(float& value, const xAOD::Jet& jet, std::string name) const;

    // general properties of the tool configuration
    std::string m_name;
    std::string m_wkpt;
    std::string m_configFile;

    // the jet mass cut values
    std::string m_jetMassMinStr;
    std::string m_jetMassMaxStr;
    TF1 * m_jetMassMaxTF1;
    TF1 * m_jetMassMinTF1;

    // the substructure variable and cut value
    std::string m_jetSubVarStr;
    std::string m_jetSubCutStr;
    TF1* m_jetSubCutTF1;

    // track jet properties
    float m_trackJetPtMin;
    float m_trackJetEtaMax;
    float m_trackJetNConst;
    std::string m_trackJetContName;

    // b-tagging parameters
    std::string m_bTagAlg;
    int m_numBTags;
    float m_bTagCut;

    // muon properties
    std::string m_muonContName;
    std::string m_muonCorrectionSchemeName;
    MuonCorrectionScheme m_muonCorrectionScheme;
    float m_muonPtMin;
    float m_muonEtaMax;
    float m_muonMatchDR;
    std::unique_ptr<CP::MuonSelectionTool> m_muonSelectionTool;
    std::unique_ptr<CP::MuonCalibrationAndSmearingTool> m_muonCalibrationAndSmearingTool;

    // generic accessors used
    static SG::AuxElement::ConstAccessor<ElementLink<xAOD::JetContainer>> parent;
    static SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::IParticleContainer> > > ghostMatchedTrackJets;

    // string for decorating jets with DNN output
    std::string m_decorationName;

    // generic decorations used
    const SG::AuxElement::Decorator<float>* jetMassMinDecor;
    const SG::AuxElement::Decorator<float>* jetMassMaxDecor;
    const SG::AuxElement::Decorator<float>* jssCutDecor;
    const SG::AuxElement::Decorator<TLorentzVector>* correctedJetDecor;
    const SG::AuxElement::Decorator<TLorentzVector>* calibratedMuonDecor;
    const SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::IParticleContainer> > >* muonsInTrackJetLink;
    const SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::IParticleContainer> > >* muonsInFatJetLink;
    const SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::IParticleContainer> > >* trackJetsInFatJet;
};
#endif