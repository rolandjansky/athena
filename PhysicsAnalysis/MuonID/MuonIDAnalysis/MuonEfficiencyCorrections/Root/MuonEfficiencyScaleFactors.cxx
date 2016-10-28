/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonEfficiencyScaleFactors.cxx
 *
 *  Created on: Apr 9, 2014
 *      Author: goblirsc
 */

#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PathResolver/PathResolver.h"

namespace CP {

    static std::string CURRENT_MEC_VERSION = "MuonEfficiencyCorrections-03-03-17";

    MuonEfficiencyScaleFactors::MuonEfficiencyScaleFactors(const std::string& name) :
                asg::AsgTool(name),
                m_wp("Medium"),
                m_sf_sets(),
                m_current_sf(0),
                m_custom_dir(),
                m_custom_file_Combined(),
                m_custom_file_Calo(),
                m_custom_file_HighEta(),
                m_custom_file_LowPt(),
                m_custom_file_LowPtCalo(),
                m_doAudit(false),
                m_audit_processed(),
                m_version_string(),
                m_sys_string(),
                m_audit_last_evt(-1),
                m_filtered_sys_sets(),
                m_efficiency_decoration_name_data(),
                m_efficiency_decoration_name_mc(),
                m_sf_decoration_name(),
                m_sf_replica_decoration_name(),
                m_eff_replica_decoration_name(),
                m_mc_eff_replica_decoration_name(),
                m_calibration_version("160928_PostICHEP"),
                m_effType(),
                m_lowpt_threshold(15.e3),
                m_effDec(0),
                m_MCeffDec(0),
                m_sfDec(0),
                m_sfrDec(0),
                m_effrDec(0),
                m_MCeffrDec(0),
                m_affectingSys(),
                m_init(false) {

        declareProperty("WorkingPoint", m_wp);
        declareProperty("doAudit", m_doAudit);

        // these are for debugging / testing, *not* for general use!
        declareProperty("CustomInputFolder", m_custom_dir);
        declareProperty("CustomFileCaloTag", m_custom_file_Calo);
        declareProperty("CustomFileCombined", m_custom_file_Combined);
        declareProperty("CustomFileHighEta", m_custom_file_HighEta);
        declareProperty("CustomFileLowPt", m_custom_file_LowPt);
        declareProperty("CustomFileLowPtCalo", m_custom_file_LowPtCalo);

        declareProperty("DataEfficiencyDecorationName", m_efficiency_decoration_name_data);
        declareProperty("MCEfficiencyDecorationName", m_efficiency_decoration_name_mc);
        declareProperty("ScaleFactorDecorationName", m_sf_decoration_name);
        declareProperty("ScaleFactorReplicaDecorationName", m_sf_replica_decoration_name);
        declareProperty("DataEfficiencyReplicaDecorationName", m_eff_replica_decoration_name);
        declareProperty("MCEfficiencyReplicaDecorationName", m_mc_eff_replica_decoration_name);

        declareProperty("CalibrationRelease", m_calibration_version);
        declareProperty("EfficiencyType", m_effType);
        declareProperty("LowPtThreshold", m_lowpt_threshold);
    }

    MuonEfficiencyScaleFactors::~MuonEfficiencyScaleFactors() {

        // remember to clean up afterwards...
        for (boost::unordered_map<SystematicSet, EffiCollection*>::iterator it = m_sf_sets.begin(); it != m_sf_sets.end(); ++it) {
            if (it->second) delete it->second;
        }
        if (m_effDec) delete m_effDec;
        if (m_sfDec) delete m_sfDec;
        if (m_MCeffDec) delete m_MCeffDec;

        if (m_sfrDec) delete m_sfrDec;
        if (m_effrDec) delete m_effrDec;
        if (m_MCeffrDec) delete m_MCeffrDec;
    }

    StatusCode MuonEfficiencyScaleFactors::initialize() {
        if (m_init) {
            ATH_MSG_INFO("The tool using working point" << m_wp << " is already initialized");
            return StatusCode::SUCCESS;
        }
        ATH_MSG_INFO("Trying to initialize, with working point " << m_wp << ", using calibration release " << m_calibration_version);

        /// so the decoration will work with other Efficiency types

        if (m_effType == "") {
            if (m_wp.find("Iso") != std::string::npos) {
                m_effType = "ISO";
            } else if (m_wp.find("TTVA") != std::string::npos) {
                m_effType = "TTVA";
            } else {
                m_effType = "EFF";
            }
        }
        ATH_MSG_INFO("Efficiency type is = " << m_effType);

        /// for isolation efficiencies, we don't use a low pt component for now - set the low pt threshold to -1
        /// same holds for TTVA SF, and for the HighPt WP
        if (m_effType == "ISO" || m_effType == "TTVA" || (m_effType == "EFF" && m_wp.find("HighPt") != std::string::npos)) {
            ATH_MSG_DEBUG("We are running Isolation or TTVA or High Pt reco SF, so we use Zmumu based SF for the whole pt range!");
            m_lowpt_threshold = -1;
        } else if (m_lowpt_threshold < 0) {
            ATH_MSG_INFO("Low pt SF turned off as crossover threshold is negative! Using Zmumu based SF for all pt values.");
        } else {
            ATH_MSG_INFO("JPsi based low pt SF will start to rock below " << m_lowpt_threshold / 1000. << " GeV!");
        }
        ATH_CHECK(CreateDecorator(m_effDec, m_efficiency_decoration_name_data, "Efficiency"));
        ATH_CHECK(CreateDecorator(m_MCeffDec, m_efficiency_decoration_name_mc, "mcEfficiency"));
        ATH_CHECK(CreateDecorator(m_sfDec, m_sf_decoration_name, "EfficiencyScaleFactor"));

        ATH_CHECK(CreateVecDecorator(m_sfrDec, m_sf_replica_decoration_name, "EfficiencyScaleFactorReplicas"));
        ATH_CHECK(CreateVecDecorator(m_effrDec, m_eff_replica_decoration_name, "EfficiencyReplicas"));
        ATH_CHECK(CreateVecDecorator(m_MCeffrDec, m_mc_eff_replica_decoration_name, "mcEfficiencyReplicas"));
        /// other
        m_affectingSys = affectingSystematics();

        /// continue with the orignal code
        if (!m_custom_dir.empty()) ATH_MSG_WARNING("Note: setting up with user specified input file location " << m_custom_dir << " - this is not encouraged!");
        if (!m_custom_file_Calo.empty()) ATH_MSG_WARNING("Note: setting up with user specified CaloTag input file " << m_custom_file_Calo << " - this is not encouraged!");
        if (!m_custom_file_Combined.empty()) ATH_MSG_WARNING("Note: setting up with user specified Central muon input file " << m_custom_file_Combined << " - this is not encouraged! ");
        if (!m_custom_file_HighEta.empty()) ATH_MSG_WARNING("Note: setting up with user specified High Eta input file " << m_custom_file_HighEta << " - this is not encouraged! ");
        if (!m_custom_file_LowPt.empty()) ATH_MSG_WARNING("Note: setting up with user specified Low Pt input file " << m_custom_file_LowPt << " - this is not encouraged! ");
        if (!m_custom_file_LowPtCalo.empty()) ATH_MSG_WARNING("Note: setting up with user specified Low Pt CaloTag input file " << m_custom_file_LowPtCalo << " - this is not encouraged! ");

        if (!LoadInputs()) {
            ATH_MSG_ERROR("Error setting up the MuonEfficiencyScaleFactors tool");
            return StatusCode::FAILURE;
        } else {
            // set up for default running without systematics
            if (!applySystematicVariation(SystematicSet())) {
                ATH_MSG_ERROR("loading the central value systematic set failed");
                return StatusCode::FAILURE;
            }
            // Add the affecting systematics to the global registry
            SystematicRegistry& registry = SystematicRegistry::getInstance();
            if (!registry.registerSystematics(*this)) {
                ATH_MSG_ERROR("unable to register the systematics");
                return StatusCode::FAILURE;
            }
            ATH_MSG_INFO("Successfully initialized! ");
            if (m_doAudit) ATH_MSG_INFO("Note: Running with Audit trail active. If not needed, turn off to save CPU time");

        }
        m_init = true;
        return StatusCode::SUCCESS;
    }
    StatusCode MuonEfficiencyScaleFactors::CreateDecorator(SG::AuxElement::Decorator<float>* &Dec, std::string &DecName, const std::string& defaultName) {
        if (Dec) delete Dec;
        if (DecName.empty()) DecName = (m_effType == "EFF") ? defaultName : m_effType + defaultName;
        ATH_MSG_INFO(defaultName << " decoration_name is " << DecName);
        ATH_CHECK(IsDecoratorNameUnique(DecName));
        Dec = new SG::AuxElement::Decorator<float>(DecName);
        return StatusCode::SUCCESS;
    }
    StatusCode MuonEfficiencyScaleFactors::CreateVecDecorator(SG::AuxElement::Decorator<std::vector<float>>* &Dec, std::string &DecName, const std::string& defaultName) {
        if (Dec) {
            delete Dec;
            Dec = 0;
        }
        if (DecName.empty()) DecName = (m_effType == "EFF") ? defaultName : m_effType + defaultName;
        ATH_MSG_INFO(defaultName << " decoration_name is " << DecName);
        ATH_CHECK(IsDecoratorNameUnique(DecName));
        Dec = new SG::AuxElement::Decorator<std::vector<float>>(DecName);
        return StatusCode::SUCCESS;
    }

    StatusCode MuonEfficiencyScaleFactors::IsDecoratorNameUnique(std::string &name) {
        if (name.empty()) return StatusCode::SUCCESS;
        if (&name != &m_efficiency_decoration_name_data && name == m_efficiency_decoration_name_data) ATH_MSG_ERROR("Decorator " << name << " corresponds to the name of the data efficiencies");
        else if (&name != &m_efficiency_decoration_name_mc && name == m_efficiency_decoration_name_mc) ATH_MSG_ERROR("Decorator " << name << " corresponds to the name of the MC efficiencies");
        else if (&name != &m_sf_decoration_name && name == m_sf_decoration_name) ATH_MSG_ERROR("Decorator " << name << " corresponds to the name of the SF themselves");
        else if (&name != &m_sf_replica_decoration_name && name == m_sf_replica_decoration_name) ATH_MSG_ERROR("Decorator " << name << " corresponds to name of the SF replicas");
        else if (&name != &m_eff_replica_decoration_name && name == m_eff_replica_decoration_name) ATH_MSG_ERROR("Decorator " << name << " corresponds to name of the data efficiency replicas");
        else if (&name != &m_mc_eff_replica_decoration_name && name == m_mc_eff_replica_decoration_name) ATH_MSG_ERROR("Decorator " << name << " corresponds to name of the MC efficiency replicas");
        else return StatusCode::SUCCESS;
        return StatusCode::FAILURE;
    }
    unsigned int MuonEfficiencyScaleFactors::getRandomRunNumber(const xAOD::EventInfo* info) {
        if (!info && !evtStore()->retrieve(info, "EventInfo")) {
            ATH_MSG_ERROR("Could not retrieve the xAOD::EventInfo. Return 999999");
            return 999999;
        }
        static SG::AuxElement::Decorator<unsigned int> dec_rnd("RandomRunNumber");
        if (!dec_rnd.isAvailable(*info)) {
            ATH_MSG_WARNING("Failed to find the RandomRunNumber decoration. Please call the apply() method from the PileupReweightingTool before hand in order to get period dependent SFs. You'll receive SFs from the most recent period.");
            return 999999;
        } else if (dec_rnd(*info) == 0) {
            ATH_MSG_DEBUG("Pile up tool has given runNumber 0. Return SF from latest period.");
            return 999999;
        }
        return dec_rnd(*info);
    }
    CorrectionCode MuonEfficiencyScaleFactors::getEfficiencyScaleFactor(const xAOD::Muon& mu, float& sf, const xAOD::EventInfo* info) {
        if (m_doAudit) ApplyAuditInfo(mu);
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet.");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->ScaleFactor(mu, sf);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyEfficiencyScaleFactor(const xAOD::Muon& mu, const xAOD::EventInfo* info) {
        float sf = 0;
        CorrectionCode result = getEfficiencyScaleFactor(mu, sf, info);
        // Decorate the muon
        if (m_sfDec) (*m_sfDec)(mu) = sf;
        return result;
    }

    CorrectionCode MuonEfficiencyScaleFactors::getEfficiencyScaleFactorReplicas(const xAOD::Muon& mu, std::vector<float> & sfs, const xAOD::EventInfo* info) {
        if (m_doAudit) ApplyAuditInfo(mu);
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->ScaleFactorReplicas(mu, sfs);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyEfficiencyScaleFactorReplicas(const xAOD::Muon& mu, int nreplicas, const xAOD::EventInfo* info) {
        std::vector<float> replicas(nreplicas);
        CorrectionCode result = getEfficiencyScaleFactorReplicas(mu, replicas, info);
        // Decorate the muon
        if (m_sfrDec) (*m_sfrDec)(mu) = replicas;
        return result;
    }
    CorrectionCode MuonEfficiencyScaleFactors::getDataEfficiency(const xAOD::Muon& mu, float& eff, const xAOD::EventInfo* info) {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        if (m_doAudit) ApplyAuditInfo(mu);
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->DataEfficiency(mu, eff);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyDataEfficiency(const xAOD::Muon& mu, const xAOD::EventInfo* info) {
        float eff = 0;
        CorrectionCode result = getDataEfficiency(mu, eff, info);
        // Decorate the muon
        if (m_effDec) (*m_effDec)(mu) = eff;
        return result;
    }
    CorrectionCode MuonEfficiencyScaleFactors::getDataEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info) {
        if (m_doAudit) ApplyAuditInfo(mu);
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->DataEfficiencyReplicas(mu, sf_err);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyDataEfficiencyReplicas(const xAOD::Muon& mu, int nreplicas, const xAOD::EventInfo* info) {
        std::vector<float> replicas(nreplicas);
        CorrectionCode result = getDataEfficiencyReplicas(mu, replicas, info);
        // Decorate the muon
        if (m_effrDec) (*m_effrDec)(mu) = replicas;
        return result;
    }
    CorrectionCode MuonEfficiencyScaleFactors::getMCEfficiency(const xAOD::Muon& mu, float& eff, const xAOD::EventInfo* info) {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        if (m_doAudit) ApplyAuditInfo(mu);
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->MCEfficiency(mu, eff);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyMCEfficiency(const xAOD::Muon& mu, const xAOD::EventInfo* info) {
        float eff = 0;
        CorrectionCode result = getMCEfficiency(mu, eff, info);
        // Decorate the muon
        if (m_MCeffDec) (*m_MCeffDec)(mu) = eff;
        return result;
    }
    CorrectionCode MuonEfficiencyScaleFactors::getMCEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info) {
        if (m_doAudit) ApplyAuditInfo(mu);
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->MCEfficiencyReplicas(mu, sf_err);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyMCEfficiencyReplicas(const xAOD::Muon& mu, int nreplicas, const xAOD::EventInfo* info) {
        std::vector<float> replicas(nreplicas);
        CorrectionCode result = getMCEfficiencyReplicas(mu, replicas, info);
        // Decorate the muon
        if (m_MCeffrDec) (*m_MCeffrDec)(mu) = replicas;
        return result;
    }

    std::string MuonEfficiencyScaleFactors::resolve_file_location(std::string filename) {

        // can be overridden by u   ser defined directory - mainly for development, not for everyday use!
        if (!m_custom_dir.empty()) return m_custom_dir + "/" + filename;
        std::string fullPathToFile = PathResolverFindCalibFile(Form("MuonEfficiencyCorrections/%s/%s", m_calibration_version.c_str(), filename.c_str()));
        if (fullPathToFile.empty()) {
            ATH_MSG_ERROR("Unable to resolve the input file " << m_calibration_version << "/" << filename << " via the PathResolver!");
        }
        return fullPathToFile;
    }

    std::string MuonEfficiencyScaleFactors::filename_Central() {

        if (m_custom_file_Combined != "") return (resolve_file_location(m_custom_file_Combined));
        else if (m_wp.find("Iso") != std::string::npos) {
            return resolve_file_location(Form("Iso_%s_Z.root", m_wp.c_str()));
        } else if (m_wp.find("TTVA") != std::string::npos) {
            return resolve_file_location("TTVA_Z.root");
        } else return resolve_file_location(Form("Reco_%s_Z.root", m_wp.c_str()));
    }
    std::string MuonEfficiencyScaleFactors::filename_Calo() {

        if (m_custom_file_Calo != "") return resolve_file_location(m_custom_file_Calo);
        else if (m_wp.find("Iso") != std::string::npos || m_wp.find("TTVA") != std::string::npos) {
            return filename_Central();
        } else return resolve_file_location("Reco_CaloTag_Z.root");
    }

    std::string MuonEfficiencyScaleFactors::filename_HighEta() {

        if (m_custom_file_HighEta != "") return resolve_file_location(m_custom_file_HighEta);
        else if (m_wp.find("Iso") != std::string::npos || m_wp.find("TTVA") != std::string::npos) {
            return filename_Central();
        } else return resolve_file_location("Reco_HighEta_Z.root");
    }

    std::string MuonEfficiencyScaleFactors::filename_LowPt() {

        if (m_custom_file_LowPt != "") return resolve_file_location(m_custom_file_LowPt);
        // for the no reco WPs, we currently use the existing Z SF also for the low pt regime
        else if (m_wp.find("Iso") != std::string::npos || m_wp.find("TTVA") != std::string::npos) {
            return filename_Central();
        } else if (m_lowpt_threshold < 0) { // catch cases where JPsi is turned off
            return filename_Central();
        } else return resolve_file_location(Form("Reco_%s_JPsi.root", m_wp.c_str()));
    }

    std::string MuonEfficiencyScaleFactors::filename_LowPtCalo() {

        if (m_custom_file_LowPtCalo != "") return resolve_file_location(m_custom_file_LowPtCalo);
        // for the no reco WPs, we currently use the existing Z SF also for the low pt regime
        else if (m_wp.find("Iso") != std::string::npos || m_wp.find("TTVA") != std::string::npos) {
            return filename_Central();
        } else if (m_lowpt_threshold < 0) { // catch cases where JPsi is turned off
            return filename_Calo();
        } else return resolve_file_location("Reco_CaloTag_JPsi.root");
    }

// load the SF histos
    bool MuonEfficiencyScaleFactors::LoadInputs() {
        std::set<SystematicVariation>::iterator it;
        CP::SystematicSet sys = affectingSystematics();
        bool success = LoadEffiSet(SystematicSet());
        for (it = sys.begin(); it != sys.end(); ++it) {
            CP::SystematicSet Set;
            Set.insert(*it);
            success &= LoadEffiSet(Set);
        }
        return success;
    }
    bool MuonEfficiencyScaleFactors::LoadEffiSet(SystematicSet sys) {

        // delete already existing SF sets first
        if (m_sf_sets.find(sys) != m_sf_sets.end()) {
            delete m_sf_sets[sys];
        }
        EffiCollection* ec = new EffiCollection(filename_Central(), filename_Calo(), filename_HighEta(), filename_LowPt(), filename_LowPtCalo(), sys, m_effType, m_lowpt_threshold);
        m_sf_sets.insert(std::make_pair(sys, ec));

        return ec->CheckConsistency();

    }
    bool MuonEfficiencyScaleFactors::isAffectedBySystematic(const SystematicVariation& systematic) const {
        SystematicSet sys = affectingSystematics();
        return sys.find(systematic) != sys.end();
    }

/// returns: the list of all systematics this tool can be affected by
    SystematicSet MuonEfficiencyScaleFactors::affectingSystematics() const {
        SystematicSet result;
        result.insert(SystematicVariation("MUON_" + m_effType + "_STAT", 1));
        result.insert(SystematicVariation("MUON_" + m_effType + "_STAT", -1));

        result.insert(SystematicVariation("MUON_" + m_effType + "_SYS", 1));
        result.insert(SystematicVariation("MUON_" + m_effType + "_SYS", -1));

        if (m_effType == "EFF" && m_lowpt_threshold > 0) {
            result.insert(SystematicVariation("MUON_" + m_effType + "_STAT_LOWPT", 1));
            result.insert(SystematicVariation("MUON_" + m_effType + "_STAT_LOWPT", -1));

            result.insert(SystematicVariation("MUON_" + m_effType + "_SYS_LOWPT", 1));
            result.insert(SystematicVariation("MUON_" + m_effType + "_SYS_LOWPT", -1));

        }
        return result;
    }

/// returns: the list of all systematics this tool recommends to use
    SystematicSet MuonEfficiencyScaleFactors::recommendedSystematics() const {
        return affectingSystematics();
    }

    SystematicCode MuonEfficiencyScaleFactors::applySystematicVariation(const SystematicSet& systConfig) {
        SystematicSet mySysConf;
        if (m_filtered_sys_sets.find(systConfig) == m_filtered_sys_sets.end()) {
            if (!SystematicSet::filterForAffectingSystematics(systConfig, m_affectingSys, mySysConf)) {
                ATH_MSG_ERROR("Unsupported combination of systematics passed to the tool! ");
                return SystematicCode::Unsupported;
            }
            m_filtered_sys_sets[systConfig] = mySysConf;
        }
        mySysConf = m_filtered_sys_sets[systConfig];
        boost::unordered_map<SystematicSet, EffiCollection*>::iterator SFiter = m_sf_sets.find(mySysConf);
        if (SFiter != m_sf_sets.end()) {
            m_current_sf = SFiter->second;
            if (m_doAudit) {
                m_sys_string = m_current_sf->sysname();
            }
            return SystematicCode::Ok;
        } else {
            ATH_MSG_ERROR("Illegal combination of systematics passed to the tool! Did you maybe request multiple variations at the same time? ");
            ATH_MSG_DEBUG(" List of relevant systematics included in your combination:");
            for (std::set<SystematicVariation>::iterator t = mySysConf.begin(); t != mySysConf.end(); ++t) {
                ATH_MSG_DEBUG("\t" << (*t).name());
            }
            return SystematicCode::Unsupported;
        }
    }

    bool MuonEfficiencyScaleFactors::AlreadyApplied(const xAOD::Muon & mu) {
        if (m_doAudit) CheckAuditEvent();
        return (m_audit_processed.find(std::make_pair(m_current_sf, const_cast<xAOD::Muon*>(&mu))) != m_audit_processed.end());
    }
    bool MuonEfficiencyScaleFactors::AlreadyApplied(xAOD::Muon & mu) {
        if (m_doAudit) CheckAuditEvent();
        return (m_audit_processed.find(std::make_pair(m_current_sf, &mu)) != m_audit_processed.end());
    }

    void MuonEfficiencyScaleFactors::ApplyAuditInfo(const xAOD::Muon& mu) {
        if (m_doAudit) CheckAuditEvent();
        if (m_audit_processed.find(std::make_pair(m_current_sf, const_cast<xAOD::Muon*>(&mu))) == m_audit_processed.end()) {
            m_audit_processed.insert(std::make_pair(m_current_sf, const_cast<xAOD::Muon*>(&mu)));
        }
        static SG::AuxElement::Decorator<bool> effboolDec("MuonEfficiencyCorrections");
        effboolDec(mu) = true;
        static SG::AuxElement::Decorator<std::string> versionDec("MuonEfficiencyCorrectionsVersion");
        versionDec(mu) = m_version_string + "_" + m_sys_string;
        static SG::AuxElement::Decorator<std::string> appliedDec("AppliedCorrections");
        appliedDec(mu) += " MuonEfficiencyCorrections";
        if (m_effType != "EFF") appliedDec(mu) += m_effType;
    }

    void MuonEfficiencyScaleFactors::CheckAuditEvent() {
        const xAOD::EventInfo* ei = 0;
        if (evtStore()->retrieve(ei, "EventInfo").isFailure()) {
            ATH_MSG_WARNING("No EventInfo object could be retrieved");
            ATH_MSG_WARNING("Random number generation not configured correctly");
        }
        const unsigned long long eventNumber = ei ? ei->eventNumber() : 0;
        if (m_audit_last_evt != eventNumber) {
            m_audit_processed.clear();
            m_audit_last_evt = eventNumber;
        }
    }

    MuonEfficiencyScaleFactors::MuonEfficiencyScaleFactors(const MuonEfficiencyScaleFactors& toCopy) :
                MuonEfficiencyScaleFactors(toCopy.name() + "_copy") {
        CopyInformation(toCopy);

    }

    void MuonEfficiencyScaleFactors::CopyInformation(const MuonEfficiencyScaleFactors & toCopy) {
        m_wp = toCopy.m_wp;
        m_sf_sets = toCopy.m_sf_sets;
        m_custom_dir = toCopy.m_custom_dir;
        m_custom_file_Combined = toCopy.m_custom_file_Combined;
        m_custom_file_Calo = toCopy.m_custom_file_Calo;
        m_custom_file_HighEta = toCopy.m_custom_file_HighEta;
        m_custom_file_LowPt = toCopy.m_custom_file_LowPt;
        m_custom_file_LowPtCalo = toCopy.m_custom_file_LowPtCalo;
        m_doAudit = toCopy.m_doAudit;
        m_version_string = toCopy.m_version_string;
        m_sys_string = toCopy.m_sys_string;
        m_efficiency_decoration_name_data = toCopy.m_efficiency_decoration_name_data;
        m_efficiency_decoration_name_mc = toCopy.m_efficiency_decoration_name_mc;
        m_sf_decoration_name = toCopy.m_sf_decoration_name;
        m_sf_replica_decoration_name = toCopy.m_sf_replica_decoration_name;
        m_eff_replica_decoration_name = toCopy.m_eff_replica_decoration_name;
        m_mc_eff_replica_decoration_name = toCopy.m_mc_eff_replica_decoration_name;

        m_calibration_version = toCopy.m_calibration_version;
        m_effType = toCopy.m_effType;
        m_lowpt_threshold = toCopy.m_lowpt_threshold;
        m_init = false;

    }

    MuonEfficiencyScaleFactors & MuonEfficiencyScaleFactors::operator =(const MuonEfficiencyScaleFactors & tocopy) {

        if (this == &tocopy) {
            return *this;
        }
        CopyInformation(tocopy);
        return *this;

    }
} /* namespace CP */
