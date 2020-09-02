/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "MuonEfficiencyCorrections/MuonSFTestHelper.h"
#include "MuonEfficiencyCorrections/UtilFunctions.h"
#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"


#include "PATInterfaces/SystematicsUtil.h"
#include <TH1D.h>
#define CHECK_CPCorr(Arg) \
    if (Arg.code() == CP::CorrectionCode::Error){    \
        Error(#Arg,"Correction Code 'Error' (returned in line %i) ",__LINE__); \
        return CP::CorrectionCode::Error;   \
    } 

namespace TestMuonSF {
        template<typename T> T getProperty(const asg::IAsgTool* interface_tool, const std::string& prop_name) {
        const asg::AsgTool* asg_tool = dynamic_cast<const asg::AsgTool*>(interface_tool);
        T prop;
        const T* HandlePtr = asg_tool->getProperty < T > (prop_name);
        if (!HandlePtr) Error("getProperty()", "Failed to retrieve property %s ", prop_name.c_str());
        else prop = (*HandlePtr);
        return prop;
    }

    //###########################################################
    //                       SFBranches
    //###########################################################
    SFBranches::SFBranches(TTree* tree) :
                m_tree(tree) {
    }
    //############################################################
    //                   TriggerSFBranches
    //############################################################
    TriggerSFBranches::TriggerSFBranches(TTree* tree, const ToolHandle<CP::IMuonTriggerScaleFactors>& Handle, const std::string& Trigger) :
                SFBranches(tree),
                m_handle(Handle),
                m_trigger(Trigger),
                m_nominal_SF(1.),
                m_stat_up_SF(1.),
                m_stat_down_SF(1.),
                m_sys_up_SF(1.),
                m_sys_down_SF(1.) {
    }
    CP::CorrectionCode TriggerSFBranches::fill(const xAOD::MuonContainer* muons) {
        if (getSF(muons, m_nominal_SF, CP::SystematicVariation("", 0)) == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
        if (getSF(muons, m_stat_up_SF, CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", +1)) == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
        if (getSF(muons, m_stat_down_SF, CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", -1)) == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
        if (getSF(muons, m_sys_up_SF, CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", +1)) == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
        if (getSF(muons, m_sys_down_SF, CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", -1)) == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
        return CP::CorrectionCode::Ok;
    }
    bool TriggerSFBranches::init() {
        if (!initBranch(m_nominal_SF, "")) return false;
        if (!initBranch(m_stat_up_SF, "STAT_UP")) return false;
        if (!initBranch(m_stat_down_SF, "STAT_DOWN")) return false;
        if (!initBranch(m_sys_up_SF, "SYS_UP")) return false;
        if (!initBranch(m_sys_down_SF, "SYS_DOWN")) return false;
        return true;
    }
    std::string TriggerSFBranches::name() const {
        return m_trigger;
    }
    CP::CorrectionCode TriggerSFBranches::getSF(const xAOD::MuonContainer* muons, double &Var, const CP::SystematicVariation &syst) {
        if (muons->empty()) {
            return CP::CorrectionCode::Ok;
        }
        CP::SystematicSet syst_set;
        syst_set.insert(syst);
        if (m_handle->applySystematicVariation(syst_set) != CP::SystematicCode::Ok) {
            return CP::CorrectionCode::Error;
        }
        return m_handle->getTriggerScaleFactor(*muons, Var, name());
    }
    //############################################################
    //                   MuonEffiBranches
    //############################################################
    MuonEffiBranches::MuonEffiBranches(TTree* tree) :
                SFBranches(tree) {
    }
    //############################################################
    //                   MuonSFBranches
    //############################################################
    MuonSFBranches::MuonSFBranches(TTree* tree, const ToolHandle<CP::IMuonEfficiencyScaleFactors> &handle, const std::string& rel_name) :
                MuonEffiBranches(tree),
                m_handle(handle),
                m_uncorrelate_sys(dynamic_cast<const CP::MuonEfficiencyScaleFactors*>(handle.operator->())->uncorrelate_sys()),
                m_release(rel_name),
                m_SFs() {
    }
    CP::CorrectionCode MuonSFBranches::fill(const xAOD::Muon& muon) {
        /// Only the raw systematic sets have been activated
        /// We can loop over each set and fill it properly        
        if (!m_uncorrelate_sys){
            for (auto& Syst_SF : m_SFs) {            
                CP::CorrectionCode cc =  fill_systematic(muon,Syst_SF);
                if (cc == CP::CorrectionCode::Error) return cc;
            }
        }  else {            
            int bin =  m_handle->getUnCorrelatedSystBin(muon);
            if (bin < 0){
                Warning("MuonSFBranches()", "Did not find a valid bin for muon  with pT: %.2f GeV, eta: %.2f, phi: %2.f",
                        muon.pt(), muon.eta(),muon.phi());
                return CP::CorrectionCode::OutOfValidityRange;
            }        
            for (auto& Syst_SF: m_SFs){
                /// Only process the nominal set or the set matching the syst bin
                bool process = false;
                if (Syst_SF.first.name().empty()) process =true;
                else {                    
                     for (std::set<CP::SystematicVariation>::const_iterator t = Syst_SF.first.begin(); t != Syst_SF.first.end(); ++t) {
                        if ((*t).isToyVariation()) {
                            std::pair<unsigned, float> pair = (*t).getToyVariation();
                            if (pair.first == (unsigned) bin){
                                process = true;
                                break;
                            }
                        }
                    }
                }
                if (process) {
                    CP::CorrectionCode cc = fill_systematic(muon, Syst_SF);
                    if (cc == CP::CorrectionCode::Error) return cc;
                } else {
                    /// Assign a dummy value
                    Syst_SF.second.scale_factor = Syst_SF.second.mc_eff = Syst_SF.second.data_eff = -1.; 
                }                
            }           
        } 
        return CP::CorrectionCode::Ok;
    }
    CP::CorrectionCode MuonSFBranches::fill_systematic(const xAOD::Muon muon, std::pair<const CP::SystematicSet, MuonSFBranches::SFSet>&  Syst_SF){
        if (m_handle->applySystematicVariation(Syst_SF.first) != CP::SystematicCode::Ok) {
            Error("MuonSFBranches()", "Failed to apply variation %s for %s", Syst_SF.first.name().c_str(), name().c_str());
            return CP::CorrectionCode::Error;
        }            
        CP::CorrectionCode cc = m_handle->getEfficiencyScaleFactor(muon, Syst_SF.second.scale_factor);
        if (cc == CP::CorrectionCode::Error) {
            Error("MuonSFBranches()", "Failed to retrieve %s scale-factor for variation %s", name().c_str(), Syst_SF.first.name().c_str());
            return CP::CorrectionCode::Error;
        } 
       
        /// No data-mc efficiencies provided for eta beyond 2.5
        if (std::fabs(muon.eta()) > 2.5) {
            Syst_SF.second.data_eff = -1;
            Syst_SF.second.mc_eff = -1;
            return CP::CorrectionCode::Ok;
        }            
        cc = m_handle->getDataEfficiency(muon, Syst_SF.second.data_eff);
        if (cc == CP::CorrectionCode::Error) {
             Error("MuonSFBranches()", "Failed to retrieve %s data efficiency for variation %s", name().c_str(), Syst_SF.first.name().c_str());
            return CP::CorrectionCode::Error;
        }            
        cc = m_handle->getMCEfficiency(muon, Syst_SF.second.mc_eff);
        if (cc == CP::CorrectionCode::Error) {
            Error("MuonSFBranches()", "Failed to retrieve %s mc efficiency for variation %s", name().c_str(), Syst_SF.first.name().c_str());
            return CP::CorrectionCode::Error;
        }
        return CP::CorrectionCode::Ok;        
    }
    
    std::string MuonSFBranches::name() const {
        return m_release + (m_release.empty() ? "" : "_") + getProperty<std::string>(m_handle.operator->(), "WorkingPoint");
    }
    bool MuonSFBranches::init() {
        for (auto set : CP::make_systematics_vector(m_handle->recommendedSystematics())) {
            std::map<CP::SystematicSet, SFSet>::iterator Itr = m_SFs.find(set);
            if (Itr == m_SFs.end()) {
                m_SFs.insert(std::pair<CP::SystematicSet, SFSet>(set, SFSet()));
                Itr = m_SFs.find(set);
                if (!AddToTree(set, Itr->second)) return false;
            }
        }
        return true;
    }
    bool MuonSFBranches::AddToTree(const CP::SystematicSet& syst, MuonSFBranches::SFSet& ScaleFactor) {
        std::string SystName = (syst.name().empty() ? std::string("") : std::string("__")) + syst.name();
        if (!initBranch(ScaleFactor.scale_factor, std::string("SF") + SystName)) return false;
        if (!initBranch(ScaleFactor.data_eff, std::string("DataEff") + SystName)) return false;
        if (!initBranch(ScaleFactor.mc_eff, std::string("MCEff") + SystName)) return false;
        return true;
    }
    //############################################################
    //                   MuonReplicaBranches
    //############################################################
    MuonReplicaBranches::MuonReplicaBranches(TTree* tree, const ToolHandle<CP::IMuonEfficiencyScaleFactors> &handle, const std::string& rel_name) :
                MuonEffiBranches(tree),
                m_handle(handle),
                m_release(rel_name),
                m_SFs() {
    }
    CP::CorrectionCode MuonReplicaBranches::fill(const xAOD::Muon& muon) {
        for (auto& Syst_SF : m_SFs) {
            if (m_handle->applySystematicVariation(Syst_SF.first) != CP::SystematicCode::Ok) {
                return CP::CorrectionCode::Error;
            }
            CP::CorrectionCode cc = m_handle->getEfficiencyScaleFactorReplicas(muon, Syst_SF.second);
            if (cc == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
        }
        return CP::CorrectionCode::Ok;
    }
    std::string MuonReplicaBranches::name() const {
        return m_release + (m_release.empty() ? "" : "_") + getProperty<std::string>(m_handle.operator->(), "WorkingPoint");
    }
    bool MuonReplicaBranches::init() {
        for (auto set : CP::make_systematics_vector(m_handle->recommendedSystematics())) {
            std::map<CP::SystematicSet, std::vector<float>>::iterator Itr = m_SFs.find(set);
            if (Itr == m_SFs.end()) {
                m_SFs.insert(std::pair<CP::SystematicSet, std::vector<float> >(set, std::vector<float>()));
                Itr = m_SFs.find(set);
                std::string SystName = (set.name().empty() ? std::string("") : std::string("__")) + set.name();
                if (!initBranch(Itr->second, std::string("SFReplica") + SystName)) return false;
            }
        }
        return true;
    }
    //###########################################################
    //                  MuonInfoBranches
    //###########################################################
    MuonInfoBranches::MuonInfoBranches(TTree* tree, const ToolHandle<CP::IMuonSelectionTool>& sel_tool) :
                MuonEffiBranches(tree),
                m_selection_tool(sel_tool),
                m_pt(FLT_MAX),
                m_eta(FLT_MAX),
                m_phi(FLT_MAX),
                m_quality(-1),
                m_author(-1),
                m_type(-1),
                m_passLowPt(false),
                m_passHighPt(false),
                m_precLayers(0){
    }
    bool MuonInfoBranches::init() {
        if (!initBranch(m_pt, "pt")) return false;
        if (!initBranch(m_eta, "eta")) return false;
        if (!initBranch(m_phi, "phi")) return false;
        if (!initBranch(m_quality, "quality")) return false;
        if (!initBranch(m_author, "author")) return false;
        if (!m_selection_tool.isSet()) return true;
        if (!initBranch(m_passLowPt, "isLowPt")) return false;
        if (!initBranch(m_passHighPt, "isHighPt")) return false;
        if (!initBranch(m_precLayers, "PrecisionLayers")) return false;
        
       
        return true;
    }
    std::string MuonInfoBranches::name() const {
        return "Muon";
    }
    CP::CorrectionCode MuonInfoBranches::fill(const xAOD::Muon& muon) {
        m_pt = muon.pt();
        m_eta = muon.eta();
        m_phi = muon.phi();
        m_author = muon.author();
        m_type = muon.type();
        if (!muon.summaryValue(m_precLayers, xAOD::SummaryType::numberOfPrecisionLayers)){
            Error("MuonInfoBranches()", "Failed to retrieve the precision layers");
            return CP::CorrectionCode::Error;
        }
        if (m_selection_tool.isSet()){
            m_quality = m_selection_tool->getQuality(muon);
            m_passLowPt = m_selection_tool->passedLowPtEfficiencyCuts(muon);
            m_passHighPt = m_selection_tool->passedHighPtCuts(muon);
        } else m_quality = muon.quality();
        return CP::CorrectionCode::Ok;
    }

    //###########################################################
    //                  MuonSFTestHelper
    //###########################################################
    MuonSFTestHelper::MuonSFTestHelper(const std::string& release_name, bool HasOwnerShip) :
                m_name(release_name),
                m_tree(),
                m_tree_raw_ptr(new TTree("MuonEfficiencyTest", "MuonEfficiencyTest")),
                m_Branches(),
                m_sel_tool(){
        if (release_name.find("/") != std::string::npos) m_name = "c" + release_name.substr(release_name.rfind("/") + 1, m_name.size()); // branches cannot start with number
        m_Branches.push_back(std::make_unique<TestMuonSF::MuonInfoBranches>(tree(),m_sel_tool));
        if (HasOwnerShip) m_tree = std::shared_ptr < TTree > (m_tree_raw_ptr);
    }
    MuonSFTestHelper::MuonSFTestHelper(std::shared_ptr<TTree> tree, const std::string& release_name) :
                m_name(release_name),
                m_tree(tree),
                m_tree_raw_ptr(tree.get()),
                m_Branches(),
                m_sel_tool(){
        if (release_name.find("/") != std::string::npos) m_name = "c" + release_name.substr(release_name.rfind("/") + 1, m_name.size()); // branches cannot start with number
    }
    MuonSFTestHelper::MuonSFTestHelper(TTree* tree, const std::string& release_name) :
                m_name(release_name),
                m_tree(),
                m_tree_raw_ptr(tree),
                m_Branches(),
                m_sel_tool(){
        if (release_name.find("/") != std::string::npos) m_name = "c" + release_name.substr(release_name.rfind("/") + 1, m_name.size()); // branches cannot start with number
    }
    void MuonSFTestHelper::addTool(const asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> &handle) {
        addTool(handle.getHandle());
    }
    void MuonSFTestHelper::addTool(const ToolHandle<CP::IMuonEfficiencyScaleFactors>& handle) {
        m_Branches.push_back(std::make_unique<TestMuonSF::MuonSFBranches>(tree(), handle, m_name));
    }
    void MuonSFTestHelper::addReplicaTool(const asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> &handle) {
        addReplicaTool(handle.getHandle());
    }
    void MuonSFTestHelper::addReplicaTool(const ToolHandle<CP::IMuonEfficiencyScaleFactors>& handle) {
        m_Branches.push_back(std::make_unique<TestMuonSF::MuonReplicaBranches>(tree(), handle, m_name));
    }
    void MuonSFTestHelper::setSelectionTool(const asg::AnaToolHandle<CP::IMuonSelectionTool> & sel_tool){
        setSelectionTool(sel_tool.getHandle());
    }
    void MuonSFTestHelper::setSelectionTool(const ToolHandle<CP::IMuonSelectionTool> & sel_tool){
       m_sel_tool = sel_tool;    
    }
          
    bool MuonSFTestHelper::init() {
        if (m_Branches.empty()) {
            Error("init()", "No scale-factors have been defined thus far");
            return false;
        }
        for (auto& BR : m_Branches) {
            if (!BR->init()) return false;
        }
        return true;
    }
    CP::CorrectionCode MuonSFTestHelper::fill(const xAOD::Muon* mu) {
        return fill(*mu);
    }
    CP::CorrectionCode MuonSFTestHelper::fill(const xAOD::Muon& mu) {
        for (auto& br : m_Branches) {
            if (br->fill(mu) == CP::CorrectionCode::Error) {return CP::CorrectionCode::Error;}
        }
        return CP::CorrectionCode::Ok;
    }
    CP::CorrectionCode MuonSFTestHelper::fill(const xAOD::MuonContainer* muons) {
        for (const auto& mu : *muons) {
            if (fill(mu) == CP::CorrectionCode::Error) return CP::CorrectionCode::Error;
            fillTree();
        }
        return CP::CorrectionCode::Ok;
    }
    TTree* MuonSFTestHelper::tree() const {
        return m_tree_raw_ptr;
    }
    std::shared_ptr<TTree> MuonSFTestHelper::tree_shared() const {
        return m_tree;
    }
    void MuonSFTestHelper::fillTree() {
        tree()->Fill();
    }

}

