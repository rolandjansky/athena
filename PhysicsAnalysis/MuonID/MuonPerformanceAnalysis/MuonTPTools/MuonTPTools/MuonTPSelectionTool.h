/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonTPSelectionTool
//  ASG Tool performing tag and probe selection
//
//  (c) ATLAS Muon Combined Performance Software
//////////////////////////////////////////////////////////////////////////////
#ifndef MuonTPSelectionTool_H
#define MuonTPSelectionTool_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "MuonTPTools/IMuonTPSelectionTool.h"
#include "MuonTPTools/IIDTrackIsolationDecoratorTool.h"
#include "MuonTPTools/IIDTrackCaloDepositsDecoratorTool.h"
#include "MuonTPTools/IMuonTPTrigUtils.h"
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "AsgTools/AsgToolsConf.h"
#include "AsgTools/AsgMetadataTool.h"
#include "GoodRunsLists/IGoodRunsListSelectionTool.h"


class MuonTPSelectionTool
        : public asg::AsgTool,
          virtual public IMuonTPSelectionTool {
    //   ASG_TOOL_CLASS(MuonTPSelectionTool, IMuonTPSelectionTool)

          public:

    MuonTPSelectionTool(std::string myname);

    virtual StatusCode initialize();

    /// Select Probes
    ProbeContainer* selectProbes(const xAOD::MuonContainer*, const xAOD::IParticleContainer*) const;

    /// Get Efficiency Flag. Used to name the tools etc
    std::string efficiencyFlag() {return m_efficiencyFlag;}

    /// Check if Probe and Tag are the same object
    bool isTag(const xAOD::Muon* tag, const xAOD::IParticle* probe) const;

    /// return the product q_1 * q_2
    int ChargeProd(const xAOD::Muon* tag, const xAOD::IParticle* probe) const;

    // helper Method to get the absolute value of delta phi between tag and probe
    double DeltaPhiTP (const xAOD::Muon* tag, const xAOD::IParticle* probe) const;

    // helper method to select Truth probes
    bool isFinalStateTruthMuon(const xAOD::IParticle* part) const;

    // helper to check if a probe is truth matched
    bool isTruthMatched(const xAOD::IParticle* part) const;

    // select muons that 'probably' fired the trigger - workaround for 19.1 xAODs without trigger info!
    bool passDummyTrigger(const xAOD::Muon* tag) const;

    //  checks if the event fired one of the triggers specified in the tool properties
    bool Event_Trigger (void) const;

    //  performs a tag-side trigger match
    bool TagTriggerMatch (const xAOD::Muon* tag) const;

    //  performs a tag-side trigger match using triggers from rerunning trigger menu on selected events w/o prescales
    //  this is needed to use single muon triggers (prescaled in the menu) to compute multi-muon trigger efficiencies
    bool TagTriggerMatch_RerunMode (const xAOD::Muon* tag) const;

    // apply impact parameter cuts
    bool PassIPCuts(const xAOD::TrackParticle* probe, double d0cut, double d0signcut, double z0cut) const;

    // apply GRL
    bool passGRL(const xAOD::EventInfo* info) const;

    // check if the tool represents a nominal selection
    bool isNominal() const {return m_isNominal;}

    // check if the probe is decorated with calorimeter energy depositionsa
    bool probeHasCaloDeposits() const { return m_addCaloDeposits; }

    // check if the tool is a systematic variation that can not be evaluated using the nominal ntuples
    virtual bool notIncludedInNominal() const {return m_isNotPartOfNominal;}

    virtual void AddCutFlowHist(MuonTPCutFlowBase* hist);
    virtual void FillCutFlows(std::string step, double weight =1.) const ;

    const std::vector<std::string>& tagTriggerList         () const {return m_tag_Triggers;}
    const std::vector<std::string>& tagTriggerListRerunMode() const {return m_tag_Triggers_RerunMode;}

    const std::vector<std::string>& observerTriggerList(const std::string &key) const;

          protected:


    double m_tagPtCut;
    double m_tagEtaCut;
    double m_probePtCut;
    double m_probeEtaCut;
    double m_highMassWindow;
    double m_lowMassWindow;
    bool m_isNominal;
    bool m_isNotPartOfNominal;

    bool m_addCaloDeposits;
    bool m_is_on_DAOD;

    std::string              m_efficiencyFlag;
    std::vector<std::string> m_tag_Triggers;
    std::vector<std::string> m_tag_Triggers_RerunMode;
    std::vector<std::string> m_observer_Triggers_L1;
    std::vector<std::string> m_observer_Triggers_HLT;

    ToolHandle<CP::IMuonSelectionTool>     m_selection_tool;
    std::vector<MuonTPCutFlowBase*>        m_cutFlows;
    ToolHandle<IMuonTPTrigUtils>    m_trigUtils;
    ToolHandle<IGoodRunsListSelectionTool> m_grlTool;
    ToolHandle<IIDTrackIsolationDecoratorTool> m_trk_iso_tool;
    ToolHandle<IIDTrackCaloDepositsDecoratorTool> m_trk_calodep_tool;
};

#endif
