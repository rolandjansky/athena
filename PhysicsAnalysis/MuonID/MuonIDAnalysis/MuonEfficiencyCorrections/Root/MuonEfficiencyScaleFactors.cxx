/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
#include "MuonEfficiencyCorrections/MuonEfficiencyType.h"
#include "MuonEfficiencyCorrections/EffiCollection.h"

#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PathResolver/PathResolver.h"

#include <TTree.h>
#include <TFile.h>
namespace CP {
    static SG::AuxElement::ConstAccessor<unsigned int> acc_rnd("RandomRunNumber");

    MuonEfficiencyScaleFactors::MuonEfficiencyScaleFactors(const std::string& name) :
                asg::AsgTool(name),
                m_wp("Medium"),
                m_sf_sets(),
                m_current_sf(),
                m_custom_dir(),
                m_custom_file_Combined(),
                m_custom_file_Calo(),
                m_custom_file_HighEta(),
                m_custom_file_LowPt(),
                m_custom_file_LowPtCalo(),
                m_filtered_sys_sets(),
                m_efficiency_decoration_name_data(),
                m_efficiency_decoration_name_mc(),
                m_sf_decoration_name(),
                m_calibration_version("180905_TriggerUpdate"),
                m_lowpt_threshold(15.e3),
                m_affectingSys(),
                m_init(false),
                m_seperateSystBins(false),
                m_breakDownSyst(false),
                m_Type(CP::MuonEfficiencyType::Undefined) {

        declareProperty("WorkingPoint", m_wp);

        // these are for debugging / testing, *not* for general use!
        declareProperty("CustomInputFolder", m_custom_dir);
        declareProperty("CustomFileCaloTag", m_custom_file_Calo);
        declareProperty("CustomFileCombined", m_custom_file_Combined);
        declareProperty("CustomFileHighEta", m_custom_file_HighEta);
        declareProperty("CustomFileLowPt", m_custom_file_LowPt);
        declareProperty("CustomFileLowPtCalo", m_custom_file_LowPtCalo);

        // Set specific names for the decorations of the scale-factors to the muon
        declareProperty("DataEfficiencyDecorationName", m_efficiency_decoration_name_data);
        declareProperty("MCEfficiencyDecorationName", m_efficiency_decoration_name_mc);
        declareProperty("ScaleFactorDecorationName", m_sf_decoration_name);
      
        declareProperty("CalibrationRelease", m_calibration_version);
        declareProperty("LowPtThreshold", m_lowpt_threshold);
        declareProperty("UncorrelateSystematics", m_seperateSystBins);
        declareProperty("BreakDownSystematics", m_breakDownSyst);
    }

    MuonEfficiencyScaleFactors::~MuonEfficiencyScaleFactors() {
    }
    float MuonEfficiencyScaleFactors::lowPtTransition() const{
        return m_lowpt_threshold;
    }
    CP::MuonEfficiencyType MuonEfficiencyScaleFactors::measurement() const{
        return m_Type;
    }
    std::string MuonEfficiencyScaleFactors::sf_decoration() const {
        return m_sf_decoration_name.empty() ? std::string("SF") + m_wp : m_sf_decoration_name;
    }
    std::string MuonEfficiencyScaleFactors::data_effi_decoration() const{
        return m_efficiency_decoration_name_data.empty() ? std::string("DataEffi") + m_wp : m_efficiency_decoration_name_data;
    }
    std::string MuonEfficiencyScaleFactors::mc_effi_decoration() const{
        return m_efficiency_decoration_name_mc.empty() ? std::string("MC`Effi") + m_wp : m_efficiency_decoration_name_mc;
    }    
    std::string MuonEfficiencyScaleFactors::sf_replica_decoration() const {
        return std::string("Replica") + sf_decoration();
    }
    std::string MuonEfficiencyScaleFactors::data_effi_replica_decoration() const{
        return std::string("Replica") + data_effi_decoration();
    }
    std::string MuonEfficiencyScaleFactors::mc_effi_replica_deocration() const{
        return std::string("Replica") + mc_effi_decoration();
    }
    size_t MuonEfficiencyScaleFactors::getPosition(const EffiCollection* coll) const {
        std::vector< std::unique_ptr<EffiCollection>>::const_iterator itr = std::find_if(m_sf_sets.begin(), m_sf_sets.end(), [coll] (const std::unique_ptr<EffiCollection>& a){
                                                                                                                                        return a.get() == coll;});
        if (itr == m_sf_sets.end()) return -1;
        return (itr- m_sf_sets.begin());
    }               
    size_t MuonEfficiencyScaleFactors::getNCollections() const{
        return m_sf_sets.size();
    }
    StatusCode MuonEfficiencyScaleFactors::initialize() {
        if (m_init) {
            ATH_MSG_INFO("The tool using working point" << m_wp << " is already initialized.");
            return StatusCode::SUCCESS;
        }
        if (m_wp.find("Iso") != std::string::npos) {
            m_Type = CP::MuonEfficiencyType::Iso;

        } else if (m_wp.find("BadMuon") != std::string::npos) {
            m_Type = CP::MuonEfficiencyType::BadMuonVeto;

        } else if (m_wp.find("TTVA") != std::string::npos) {
            m_Type = CP::MuonEfficiencyType::TTVA;
        } else {
            m_Type = CP::MuonEfficiencyType::Reco;
        }
        ATH_MSG_INFO("Efficiency type is = " << EfficiencyTypeName(m_Type));

        /// for isolation efficiencies, we don't use a low pt component for now - set the low pt threshold to -1
        /// same holds for TTVA SF, and for the HighPt WP
        if (m_Type == CP::MuonEfficiencyType::Iso || m_Type == CP::MuonEfficiencyType::TTVA || m_Type == MuonEfficiencyType::BadMuonVeto || (m_Type == CP::MuonEfficiencyType::Reco && m_wp.find("HighPt") != std::string::npos)) {
            ATH_MSG_DEBUG("We are running Isolation or TTVA or High Pt reco SF, so we use Zmumu based SF for the whole pt range!");
            m_lowpt_threshold = -1;
        } else if (m_lowpt_threshold < 0) {
            ATH_MSG_INFO("Low pt SF turned off as crossover threshold is negative! Using Zmumu based SF for all pt values.");
        } else {
            ATH_MSG_INFO("JPsi based low pt SF will start to rock below " << m_lowpt_threshold / 1000. << " GeV!");
        }
        std::set<std::string> decorations{
            sf_decoration() ,
            data_effi_decoration(),
            mc_effi_decoration() ,
            sf_replica_decoration(),
            data_effi_replica_decoration(),
            mc_effi_replica_deocration()
        };
        if (decorations.size() != 6){
            ATH_MSG_FATAL("At least one of the decoration names for scale-factor/ data-efficiency / mc-efficiency is not uniquely defined... Please check your properties");
            return StatusCode::FAILURE;
        }
        
        if (!m_custom_dir.empty()) ATH_MSG_WARNING("Note: setting up with user specified input file location " << m_custom_dir << " - this is not encouraged!");
        if (!m_custom_file_Calo.empty()) ATH_MSG_WARNING("Note: setting up with user specified CaloTag input file " << m_custom_file_Calo << " - this is not encouraged!");
        if (!m_custom_file_Combined.empty()) ATH_MSG_WARNING("Note: setting up with user specified Central muon input file " << m_custom_file_Combined << " - this is not encouraged! ");
        if (!m_custom_file_HighEta.empty()) ATH_MSG_WARNING("Note: setting up with user specified High Eta input file " << m_custom_file_HighEta << " - this is not encouraged! ");
        if (!m_custom_file_LowPt.empty()) ATH_MSG_WARNING("Note: setting up with user specified Low Pt input file " << m_custom_file_LowPt << " - this is not encouraged! ");
        if (!m_custom_file_LowPtCalo.empty()) ATH_MSG_WARNING("Note: setting up with user specified Low Pt CaloTag input file " << m_custom_file_LowPtCalo << " - this is not encouraged! ");
        if (m_custom_dir.empty()) ATH_MSG_INFO("Trying to initialize, with working point " << m_wp << ", using calibration release " << m_calibration_version);

        ATH_CHECK(LoadInputs());
        
        // m_init has to be true for affectingSystematics()
        m_init = true;
        m_affectingSys = affectingSystematics();

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

        return StatusCode::SUCCESS;
    }
    unsigned int MuonEfficiencyScaleFactors::getRandomRunNumber(const xAOD::EventInfo* info) const {
        if (!info && !evtStore()->retrieve(info, "EventInfo")) {
            ATH_MSG_ERROR("Could not retrieve the xAOD::EventInfo. Return 999999");
            return 999999;
        }
        if (!info->eventType(xAOD::EventInfo::IS_SIMULATION)) {
            ATH_MSG_DEBUG("The current event is a data event. Return runNumber instead.");
            return info->runNumber();
        }
        if (!acc_rnd.isAvailable(*info)) {
            ATH_MSG_WARNING("Failed to find the RandomRunNumber decoration. Please call the apply() method from the PileupReweightingTool before hand in order to get period dependent SFs. You'll receive SFs from the most recent period.");
            return 999999;
        } else if (acc_rnd(*info) == 0) {
            ATH_MSG_DEBUG("Pile up tool has given runNumber 0. Return SF from latest period.");
            return 999999;
        }
        return acc_rnd(*info);
    }
    CorrectionCode MuonEfficiencyScaleFactors::getEfficiencyScaleFactor(const xAOD::Muon& mu, float& sf, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet.");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->ScaleFactor(mu, sf);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyEfficiencyScaleFactor(const xAOD::Muon& mu, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->ApplyScaleFactor(mu);
    }

    CorrectionCode MuonEfficiencyScaleFactors::getEfficiencyScaleFactorReplicas(const xAOD::Muon& mu, std::vector<float> & sfs, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->ScaleFactorReplicas(mu, sfs);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyEfficiencyScaleFactorReplicas(const xAOD::Muon& mu, int nreplicas, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->ApplyScaleFactorReplicas(mu, nreplicas);
    }
    CorrectionCode MuonEfficiencyScaleFactors::getDataEfficiency(const xAOD::Muon& mu, float& eff, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->DataEfficiency(mu, eff);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyDataEfficiency(const xAOD::Muon& mu, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->ApplyDataEfficiency(mu);
    }
    CorrectionCode MuonEfficiencyScaleFactors::getDataEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->DataEfficiencyReplicas(mu, sf_err);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyDataEfficiencyReplicas(const xAOD::Muon& mu, int nreplicas, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->ApplyDataEfficiencyReplicas(mu, nreplicas);
    }
    CorrectionCode MuonEfficiencyScaleFactors::getMCEfficiency(const xAOD::Muon& mu, float& eff, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->MCEfficiency(mu, eff);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyMCEfficiency(const xAOD::Muon& mu, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->ApplyMCEfficiency(mu);
    }
    CorrectionCode MuonEfficiencyScaleFactors::getMCEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->MCEfficiencyReplicas(mu, sf_err);
    }
    CorrectionCode MuonEfficiencyScaleFactors::applyMCEfficiencyReplicas(const xAOD::Muon& mu, int nreplicas, const xAOD::EventInfo* info) const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CorrectionCode::Error;
        }
        unsigned int RunNumber = getRandomRunNumber(info);
        return m_current_sf->retrieveSF(mu, RunNumber)->ApplyMCEfficiencyReplicas(mu, nreplicas);
       }

    std::string MuonEfficiencyScaleFactors::resolve_file_location(const std::string &filename) const{
        if (!m_custom_dir.empty()) {
            return m_custom_dir + "/" + filename;
        }
        std::string fullPathToFile = PathResolverFindCalibFile(Form("MuonEfficiencyCorrections/%s/%s", m_calibration_version.c_str(), filename.c_str()));
        if (fullPathToFile.empty()) {
            ATH_MSG_ERROR("Unable to resolve the input file " << m_calibration_version << "/" << filename << " via the PathResolver!");
        }
        return fullPathToFile;
    }

    std::string MuonEfficiencyScaleFactors::filename_Central() const{
        if (!m_custom_file_Combined.empty()) return (resolve_file_location(m_custom_file_Combined));
        else if (m_Type == CP::MuonEfficiencyType::Iso) {
            return resolve_file_location(Form("Iso_%s_Z.root", m_wp.c_str()));
        } else if (m_Type == CP::MuonEfficiencyType::TTVA) {
            return resolve_file_location("TTVA_Z.root");
        } else if (m_Type == CP::MuonEfficiencyType::BadMuonVeto) {
            return resolve_file_location("BadMuonVeto_HighPt_Z.root");
        } else if (m_Type == CP::MuonEfficiencyType::Reco) {
            return resolve_file_location(Form("Reco_%s_Z.root", m_wp.c_str()));
        }
        ATH_MSG_ERROR("What?");
        return "";
    }
    std::string MuonEfficiencyScaleFactors::filename_Calo() const{

        if (!m_custom_file_Calo.empty()) return resolve_file_location(m_custom_file_Calo);
        else if (m_Type != CP::MuonEfficiencyType::Reco) {
            return filename_Central();
        } else return resolve_file_location("Reco_CaloTag_Z.root");
    }
    std::string MuonEfficiencyScaleFactors::filename_HighEta()const {

        if (!m_custom_file_HighEta.empty()) return resolve_file_location(m_custom_file_HighEta);
        else if (m_Type != CP::MuonEfficiencyType::Reco) {
            return filename_Central();
        } else return resolve_file_location("Reco_HighEta_Z.root");
    }
    std::string MuonEfficiencyScaleFactors::filename_LowPt()const {

        if (!m_custom_file_LowPt.empty()) return resolve_file_location(m_custom_file_LowPt);
        // for the no reco WPs, we currently use the existing Z SF also for the low pt regime
        else if (m_Type != CP::MuonEfficiencyType::Reco || m_lowpt_threshold < 0) {
            return filename_Central();
        } else return resolve_file_location(Form("Reco_%s_JPsi.root", m_wp.c_str()));
    }
    std::string MuonEfficiencyScaleFactors::filename_LowPtCalo() const{

        if (!m_custom_file_LowPtCalo.empty()) return resolve_file_location(m_custom_file_LowPtCalo);
        // for the no reco WPs, we currently use the existing Z SF also for the low pt regime
        else if (m_Type != CP::MuonEfficiencyType::Reco || m_lowpt_threshold < 0) {
            return filename_Central();
        } else return resolve_file_location("Reco_CaloTag_JPsi.root");
    }
    // load the SF histos
    
    StatusCode MuonEfficiencyScaleFactors::LoadInputs() {
        if (!m_sf_sets.empty()){
            ATH_MSG_DEBUG("Input already loaded will not do it again");
            return StatusCode::SUCCESS;
        }
        std::map<std::string, unsigned int> systematics = lookUpSystematics();
        /// We've at least the stat and sys errors and nothing went wrong
        /// during loading the files
        if (systematics.empty()){
            return StatusCode::FAILURE;
        }
        
        /// Push back the nominal options
        m_sf_sets.push_back( std::make_unique<EffiCollection>(*this));
        
        EffiCollection* nominal = m_sf_sets.back().get();
        /// Now we can fill the map with the individual sets
        for (const auto& syst: systematics){
            /// Filter the bits which are not assigning the files
            if ( (syst.second & EffiCollection::ZAnalysis) &&  (syst.second & EffiCollection::JPsiAnalysis)){
                /// Z-stream
                m_sf_sets.push_back(std::make_unique<EffiCollection>(nominal,*this, syst.first, syst.second ~EffiCollection::ZAnalysis, false));
                m_sf_sets.push_back(std::make_unique<EffiCollection>(nominal,*this, syst.first, syst.second ~EffiCollection::ZAnalysis, true));
                /// J-psi stream
                m_sf_sets.push_back(std::make_unique<EffiCollection>(nominal,*this, syst.first, syst.second ~EffiCollection::JPsiAnalysis, true));
                m_sf_sets.push_back(std::make_unique<EffiCollection>(nominal,*this, syst.first, syst.second ~EffiCollection::JPsiAnalysis, false));
            } else {
                m_sf_sets.push_back(std::make_unique<EffiCollection>(nominal,*this, syst.first, syst.second, false));
                m_sf_sets.push_back(std::make_unique<EffiCollection>(nominal,*this, syst.first, syst.second, true));
            }
        }
        for(auto& sf: m_sf_sets){
            if (!sf->CheckConsistency()) return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
    }
    std::map<std::string, unsigned int> MuonEfficiencyScaleFactors::lookUpSystematics(){
        std::map<std::string, unsigned int> syst_map;
        /// All file_name methods return the central filename if there is no proper map for that component
        /// use a set to sort out all non-defined maps
        std::set<std::string>  files_to_look_up{
            filename_Central(),
            filename_Calo(),
            filename_HighEta(),
            filename_LowPt(),
            filename_LowPtCalo()
        };
        std::function<int(const std::string&)> get_bit = [this](const std::string& file_name){
                if (file_name == filename_Central()) return EffiCollection::Central;
                if (file_name == filename_LowPt()) return EffiCollection::CentralLowPt;
                if (file_name == filename_Calo()) return EffiCollection::Calo;
                if (file_name == filename_LowPtCalo()) return EffiCollection::CaloLowPt;
                // last file which remains is forward
                return EffiCollection::Forward;
        };
        
        std::function<void(const std::string&,int)> insert_bit =[&syst_map](const std::string& key, unsigned int bit){
            if (syst_map.find(key) == syst_map.end()) syst_map.insert(std::pair<std::string, unsigned int>(key,bit));
            else syst_map[key] = syst_map[key] | bit;
        }; 
        for (const auto& look_up : files_to_look_up){
            std::unique_ptr<TFile> root_file(TFile::Open(look_up.c_str(), "READ"));
            if (!root_file || !root_file->IsOpen()){
                ATH_MSG_FATAL("Could not open file "<<look_up);
                syst_map.clear();
                break;
            }
            insert_bit("STAT", get_bit(look_up));
          
            /// If the systematics shall be split into bins
            if (m_seperateSystBins) insert_bit("STAT", EffiCollection::UnCorrelated);
                          
            TTree* syst_tree = nullptr;
            root_file->GetObject("Systematics", syst_tree);
             
            /// At the moment the systematic break down is not part of all files
            /// ignore if there is no break down 
            if (!m_breakDownSyst || syst_tree == nullptr){
                ATH_MSG_DEBUG("The file "<<look_up<<" does not contain any systematic tree. No break down for that one will be considered");
                insert_bit("SYS",get_bit(look_up));
                /// Activate the pt-dependent systematic for the old calibration files by hand
                if (measurement() == ){
                
                }
                continue;
            }
            ///  Read out the systematic tree from the scale-factor files
            std::string* syst_name = nullptr;
            unsigned int is_symmetric(0), has_pt_sys(0), uncorrelated(0);
            if (syst_tree->SetBranchAddress("Systematic", &syst_name) != 0 || 
                syst_tree->SetBranchAddress("IsSymmetric", &is_symmetric) != 0 || 
                syst_tree->SetBranchAddress("HasPtDependentSys", &has_pt_sys) !=0 ||
                syst_tree->SetBranchAddress("CanBeUncorrelated", &uncorrelated) !=0){
                
            
                ATH_MSG_FATAL("Although "<<look_up<<" has a systematic tree. There is no proper connection to the branches");
                syst_map.clear();
                break;
            }
            for (int i =0; syst_tree->GetEntry(i); ++i){
                insert_bit( *syst_name, get_bit(look_up));
                if (is_symmetric) insert_bit(*syst_name, EffiCollection::Symmetric);
                if (has_pt_sys)   insert_bit(*syst_name, EffiCollection::PtDependent);
                if (m_seperateSystBins && uncorrelated) insert_bit(*syst_name, EffiCollection::UnCorrelated);
            }
        }
        return syst_map;
    }
    bool MuonEfficiencyScaleFactors::isAffectedBySystematic(const SystematicVariation& systematic) const {
        SystematicSet sys = affectingSystematics();
        return sys.find(systematic) != sys.end();
    }
    /// returns: the list of all systematics this tool can be affected by
    SystematicSet MuonEfficiencyScaleFactors::affectingSystematics() const {
        SystematicSet result;
        for (auto& collection : m_sf_sets){
            if (!collection->getSystSet()){
                ATH_MSG_FATAL("No systematic defined for scale-factor map. ");
                return SystematicSet();
            }
            result.insert(*collection->getSystSet());
        }        
        return result;
    }
    
    /// returns: the list of all systematics this tool recommends to use
    SystematicSet MuonEfficiencyScaleFactors::recommendedSystematics() const {
        if (!m_init) {
            ATH_MSG_ERROR("The tool has not been initialized yet");
            return CP::SystematicSet();
        }
        return affectingSystematics();
    }
    SystematicCode MuonEfficiencyScaleFactors::applySystematicVariation(const SystematicSet& systConfig) {
        if (!m_init) {
            ATH_MSG_ERROR("Initialize first the tool!");
            return SystematicCode::Unsupported;
        }
        SystematicSet mySysConf;
        if (m_filtered_sys_sets.find(systConfig) == m_filtered_sys_sets.end()) {
            if (!SystematicSet::filterForAffectingSystematics(systConfig, m_affectingSys, mySysConf)) {
                ATH_MSG_ERROR("Unsupported combination of systematics passed to the tool! ");
                return SystematicCode::Unsupported;
            }
            m_filtered_sys_sets[systConfig] = mySysConf;
        }
        mySysConf = m_filtered_sys_sets[systConfig];
        ATH_MSG_DEBUG(mySysConf.name() << " made it into applySystematicVariation()");

        unsigned int currentBinNumber = 0;
        /*MuonEfficiencySystType currentEfficiencySystType = MuonEfficiencySystType::Nominal;
        if ((*m_Sys1Down) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::Sys1Down;
        else if ((*m_Sys1Up) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::Sys1Up;
        else if (m_LowPtSys1Up && (*m_LowPtSys1Up) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::LowPtSys1Up;
        else if (m_LowPtSys1Down && (*m_LowPtSys1Down) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::LowPtSys1Down;
        else if (!m_seperateSystBins) {
            if ((*m_Stat1Down) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::Stat1Down;
            else if ((*m_Stat1Up) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::Stat1Up;
            else if (m_LowPtStat1Up && (*m_LowPtStat1Up) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::LowPtStat1Up;
            else if (m_LowPtStat1Down && (*m_LowPtStat1Down) == mySysConf) currentEfficiencySystType = MuonEfficiencySystType::LowPtStat1Down;
        } 
        
        else if (!mySysConf.name().empty()) {
            for (std::set<SystematicVariation>::iterator t = mySysConf.begin(); t != mySysConf.end(); ++t) {
                if ((*t).isToyVariation()) {
                    std::pair<unsigned, float> pair = (*t).getToyVariation();
                    currentBinNumber = pair.first;
                    currentEfficiencySystType = MuonEfficiencySystType(pair.second);
                }
            }
            ATH_MSG_DEBUG("need to access currentBinNumber=" << currentBinNumber);
        }*/ 
        
        std::vector<std::unique_ptr<EffiCollection>>::const_iterator SFiter= std::find_if(m_sf_sets.begin(), m_sf_sets.end(), [&mySysConf](const std::unique_ptr<EffiCollection>& a){
                    return a->isAffectedBySystematic(mySysConf);
        });
        if (SFiter != m_sf_sets.end()) {
            m_current_sf = SFiter->get();
            if (m_seperateSystBins && currentBinNumber != 0) {
                if (!m_current_sf->SetSystematicBin(currentBinNumber)) {
                    ATH_MSG_WARNING("Could not apply systematic " << mySysConf.name() << " for bin " << currentBinNumber);
                    return SystematicCode::Unsupported;
                }
            }
            return SystematicCode::Ok;
        } else {
            ATH_MSG_ERROR("Illegal combination of systematics passed to the tool! Did you maybe request multiple variations at the same time? You passed " << mySysConf.name());
            ATH_MSG_DEBUG("List of relevant systematics included in your combination:");
            for (SystematicSet::const_iterator t = mySysConf.begin(); t != mySysConf.end(); ++t) {
                ATH_MSG_DEBUG("\t" << (*t).name());
            }
        } 
        return SystematicCode::Unsupported;
    }
    
    std::string MuonEfficiencyScaleFactors::getUncorrelatedSysBinName(unsigned int Bin) const {
        return "";
    }
    int MuonEfficiencyScaleFactors::getUnCorrelatedSystBin(const xAOD::Muon& mu) const {
        return -1;
    }
    std::string MuonEfficiencyScaleFactors::getUncorrelatedSysBinName(const SystematicSet& systConfig) const {
                return "";
//         for (std::set<SystematicVariation>::iterator t = systConfig.begin(); t != systConfig.end(); ++t) {
//             if ((*t).isToyVariation()) {
//                 std::pair<unsigned, float> pair = (*t).getToyVariation();
//                 return getUncorrelatedSysBinName(pair.first);
// 
//             }
//         }
//         ATH_MSG_ERROR("The given systematic " << systConfig.name() << " is not an unfolded one. Return  unknown bin ");
//         return "unknown bin";
//     }
//     int MuonEfficiencyScaleFactors::getUnCorrelatedSystBin(const xAOD::Muon& mu) const {
//         if (!m_init) {
//             ATH_MSG_ERROR("The tool is not initialized yet");
//             return -1;
// 
//         }
//         return m_sf_sets.at(MuonEfficiencySystType::Nominal)->getUnCorrelatedSystBin(mu);
// 
    }

} /* namespace CP */
