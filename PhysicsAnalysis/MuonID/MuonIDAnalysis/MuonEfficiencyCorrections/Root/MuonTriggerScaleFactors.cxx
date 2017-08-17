/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

/*
 *  MuonTriggerScaleFactors.cxx
 *
 *  Created on: Oct 22, 2014
 *      Author: Kota Kasahara <kota.kasahara@cern.ch>
 *
 *  Updates for 2016: Jun 20, 2016
 *      Author: Lidia Dell'Asta <dellasta@cern.ch> 
 */

#include <sstream>
#include <TRandom3.h>

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"
#include "xAODEventInfo/EventInfo.h"

#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h"

#include "PathResolver/PathResolver.h"

//static const double commonSystMTSG = 0.01;
static const double muon_barrel_endcap_boundary = 1.05;

namespace CP {

    // ==================================================================================
    // == MuonTriggerScaleFactors::MuonTriggerScaleFactors
    // ==================================================================================
    MuonTriggerScaleFactors::MuonTriggerScaleFactors(const std::string& name) :
                    asg::AsgTool(name),
                    m_appliedSystematics(0),
                    m_dataPeriod(""),
                    m_classname(name.c_str()),
                    m_allowZeroSF(false) {
        declareProperty("MuonQuality", m_muonquality = "Medium"); // HighPt,Tight,Medium,Loose
        declareProperty("Isolation", m_isolation = ""); // "", "IsoGradient", "IsoLoose", "IsoTight"
        declareProperty("CalibrationRelease", m_calibration_version = "170128_Moriond"); // 160624_ICHEP

        declareProperty("Year", m_year = "2016"); // 2015 or 2016
        declareProperty("MC", m_mc = "mc15c"); // mc15a or mc15c

        // these are for debugging / testing, *not* for general use!
        declareProperty("filename", m_fileName = "");
        declareProperty("CustomInputFolder", m_custom_dir = "");
        declareProperty("Binning", m_binning = "fine"); // fine or coarse

        //Properties needed for TOY setup for a given trigger: No replicas if m_replicaTriggerList is empty
        declareProperty("ReplicaTriggerList", m_replicaTriggerList = std::vector<std::string>(), "List of triggers on which we want to generate stat. uncertainty toy replicas.");
        declareProperty("NReplicas", m_nReplicas = 100, "Number of generated toy replicas, if replicas are required.");
        declareProperty("ReplicaRandomSeed", m_ReplicaRandomSeed = 1234, "Random seed for toy replica generation.");
        declareProperty("AllowZeroSF", m_allowZeroSF, "If a trigger is not available will return 0 instead of throwing an error. More difficult to spot configuration issues. Use at own risk");
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::~MuonTriggerScaleFactors()
    // ==================================================================================
    MuonTriggerScaleFactors::~MuonTriggerScaleFactors() {
        for (auto itMap : m_efficiencyMap)
            delete itMap.second;
        m_efficiencyMap.clear();

        for (auto itMap : m_efficiencyMapReplicaArray) {
            std::vector<TH2*> vec = itMap.second;
            for (uint i = 0; i < vec.size(); ++i)
                delete vec[i];
        }
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::initialize()
    // ==================================================================================
    StatusCode MuonTriggerScaleFactors::initialize() {

        if (m_fileName.empty()) {
            std::string SFfile_2015_mc15a = "muontrigger_sf_2015_mc15a.root";
            std::string SFfile_2015_mc15c = "muontrigger_sf_2015_mc15c_v01.root";
            std::string SFfile_2016_mc15c = "muontrigger_sf_2016_mc15c_v02.root";

            if (m_year == "2015" && m_mc == "mc15a") {
                m_fileName = SFfile_2015_mc15a;
            } else if (m_year == "2015" && m_mc == "mc15c") {
                m_fileName = SFfile_2015_mc15c;
            } else if (m_year == "2016" && m_mc == "mc15c") {
                m_fileName = SFfile_2016_mc15c;
            }
        }

        ATH_MSG_INFO("MuonQuality = '" << m_muonquality << "'");
        ATH_MSG_INFO("Binning = '" << m_binning << "'");
        ATH_MSG_INFO("Year = '" << m_year << "'");
        ATH_MSG_INFO("MC = '" << m_mc << "'");
        ATH_MSG_INFO("filename = '" << m_fileName);
        ATH_MSG_INFO("CalibrationRelease = '" << m_calibration_version << "'");
        ATH_MSG_INFO("CustomInputFolder = '" << m_custom_dir << "'");
        ATH_MSG_INFO("AllowZeroSF = " << m_allowZeroSF);

        // Giving a bunch of errors if year, mc and SF file have problems
        if (m_year != "2015" && m_year != "2016") {
            ATH_MSG_ERROR("Note: you have set year " << m_year << ", which is not supported. You should use either 2015 or 2016.");
            return StatusCode::FAILURE;
        }
        if (m_mc != "mc15a" && m_mc != "mc15c") {
            ATH_MSG_ERROR("Note: you have set " << m_mc << ", which is not supported. You should use either mc15a or mc15c.");
            return StatusCode::FAILURE;
        }
        if (m_year == "2016" && m_mc == "mc15a") {
            ATH_MSG_ERROR("Note: you have set year " << m_year << " and " << m_mc << " . This combination is not provided.");
            return StatusCode::FAILURE;
        }
        
        // Set default value to data period
        if (m_year == "2015") m_dataPeriod = "AC";
        else if (m_year == "2016") m_dataPeriod = "B";

        // isolation workinig points have been merged as they have no big difference
        if (!m_isolation.empty()) {
            ATH_MSG_INFO("Note: you have set " << m_isolation << " but isolation working points have been merged.");
            m_isolation = "";
        }

        if (registerSystematics() != CP::SystematicCode::Ok) {
            return StatusCode::FAILURE;
        }

        if (applySystematicVariation(CP::SystematicSet()) != CP::SystematicCode::Ok) {
            ATH_MSG_ERROR("Could not configure for nominal settings");
            return StatusCode::FAILURE;
        }

        TDirectory* origDir = gDirectory;

        //const std::string filePath = getPathName(m_directory, m_fileName);
        std::string filePath = "";

        if (m_custom_dir == "") {
            filePath = PathResolverFindCalibFile(Form("MuonEfficiencyCorrections/%s/%s", m_calibration_version.c_str(), m_fileName.c_str()));
            if (filePath == "") {
                ATH_MSG_ERROR("Unable to resolve the input file " << m_fileName << " via PathResolver.");
            }
        } else {
            ATH_MSG_INFO("Note: setting up with user specified input file location " << m_custom_dir << " - this is not encouraged!");
            filePath = Form("%s/%s", m_custom_dir.c_str(), m_fileName.c_str());
        }

        TFile* file = TFile::Open(filePath.c_str());

        if (not file) {
            ATH_MSG_FATAL("MuonTriggerScaleFactors::initialize Couldn't open file " << filePath);
            return StatusCode::FAILURE;
        }

        TDirectory* tempDir = getTemporaryDirectory();
        tempDir->cd();

        // Initialize indexes of replicas for trigges which are asked
        for (auto trigToy : m_replicaTriggerList)
            m_replicaSet.insert(trigToy);
	
        static const std::vector<std::string> type { "data", "mc" };
        static const std::vector<std::string> region { "barrel", "endcap" };
        static const std::vector<std::string> bins { "coarse", "fine" };
        static const std::vector<std::string> systematic { "nominal", "stat_up", "stat_down", "syst_up", "syst_down" };

        const std::string quality = m_muonquality;
        TDirectory* qualityDirectory = file->GetDirectory(quality.c_str());
        if (not qualityDirectory) {
            ATH_MSG_FATAL("MuonTriggerScaleFactors::initialize cannot find directory with selected quality");
        }
        TKey* periodKey;
        TIter nextPeriod(qualityDirectory->GetListOfKeys());
        while ((periodKey = (TKey*)nextPeriod())) {
           if (not periodKey->IsFolder()) continue;
           TDirectory* periodDirectory = qualityDirectory->GetDirectory(periodKey->GetName());
           std::string periodName = std::string(periodKey->GetName());

           TKey* triggerKey;
   	       TIter nextTrigger(periodDirectory->GetListOfKeys());
           while ((triggerKey = (TKey*)nextTrigger())) {
               if (not triggerKey->IsFolder()) continue;
               TDirectory* triggerDirectory = periodDirectory->GetDirectory(triggerKey->GetName());
               std::string triggerName = std::string(triggerKey->GetName());
               
               for (size_t ibins = 0; ibins < bins.size(); ++ibins) {
                   for (size_t iregion = 0; iregion < region.size(); ++iregion) {
                       for (size_t itype = 0; itype < type.size(); ++itype) {

                           std::string histname = ("_MuonTrigEff_" + periodName + "_" + triggerName + "_" + quality + "_" + "_EtaPhi_" + bins[ibins] + "_" + region[iregion] + "_" + type[itype]);

                           for (size_t isys = 0; isys < systematic.size(); ++isys) {
                               if (type[itype].find("data") != std::string::npos && systematic[isys].find("syst") != std::string::npos) continue;
                               std::string histnameSys = histname + "_" + systematic[isys];
                               std::string path = "eff_etaphi_" + bins[ibins] + "_" + region[iregion] + "_" + type[itype] + "_" + systematic[isys];
                               
                               TH2* hist = dynamic_cast<TH2*>(triggerDirectory->Get(path.c_str()));
                               if (not hist) {
                                    ATH_MSG_FATAL("MuonTriggerScaleFactors::initialize " << path << " not found under trigger " << triggerName << " and period " << periodName);
                                    continue;
                               }
                               hist->SetDirectory(0);
                               std::pair<EfficiencyMap::iterator, bool> rc = m_efficiencyMap.insert(EfficiencyPair(histnameSys, hist));
                               if (not rc.second) {
                                   ATH_MSG_FATAL("MuonTriggerScaleFactors::initialize histogram \"%s\" duplicated");
                               }
                           }

                           //If the trigger is chosen for toy evaluation, generate all the replicas from
                           // NOMINAL with STAT variations stored in the data hist, load them in corresponding vector
                           if (m_replicaSet.find(triggerName) != m_replicaSet.end() && type[itype].find("data") != std::string::npos) {

                                TH2F tmp_h2 = *dynamic_cast<TH2F*>(m_efficiencyMap[histname + "_nominal"]->Clone("tmp_h2"));
                                const int xbins = tmp_h2.GetNbinsX(), ybins = tmp_h2.GetNbinsY();
                                for (int x_i = 0; x_i <= xbins; ++x_i) {
                                    for (int y_i = 0; y_i <= ybins; ++y_i) {
                                        double statErr = fabs(tmp_h2.GetBinContent(x_i, y_i) - m_efficiencyMap[histname + "_stat_up"]->GetBinContent(x_i, y_i));
                                        // std::cout<<tmp_h2.GetBinContent(x_i, y_i)<<" "<<m_efficiencyMap[histname+"_stat_up"]->GetBinContent(x_i, y_i)<<" "<<statErr<<std::endl;
                                        tmp_h2.SetBinError(x_i, y_i, statErr);
                                    }
                                }

                                //Set specific container in the map for each of the triggers/hists needing a replica
                                m_efficiencyMapReplicaArray[histname + "_replicas"] = generateReplicas(&tmp_h2, m_nReplicas, m_ReplicaRandomSeed);
                            }
                        }
                    }
                }
            }
        }

        file->Close();
        delete file;

        origDir->cd();

        ATH_MSG_INFO("MuonTriggerScaleFactors::initialize");

        return StatusCode::SUCCESS;
    } // end of initialize function

    ///////////////////////
    // Public functions  //
    ///////////////////////

    // ==================================================================================
    // == MuonTriggerScaleFactors::setRunNumber
    // ==================================================================================
    CorrectionCode MuonTriggerScaleFactors::setRunNumber(Int_t runNumber) {
        std::string period = getDataPeriod(runNumber, m_year);
        if (period.empty()) {
            ATH_MSG_ERROR("Run number #" << runNumber << " does not match with any run period in year " << m_year);
            return CorrectionCode::Error;
        }
        m_dataPeriod = period;
        return CorrectionCode::Ok;
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::getTriggerScaleFactor
    // ==================================================================================
    CorrectionCode MuonTriggerScaleFactors::getTriggerScaleFactor(const xAOD::MuonContainer& mucont, Double_t& triggersf, const std::string& trigger) {
        if (trigger.empty()) {
            ATH_MSG_ERROR("MuonTriggerScaleFactors::getTriggerScaleFactor Trigger must have value.");
            return CorrectionCode::Error;
        }
        TrigMuonEff::Configuration configuration;

        if (trigger == "HLT_mu8noL1") {
            //if(trigger == "HLT_mu8noL1" || trigger == "HLT_mu10" || trigger == "HLT_mu14" || trigger == "HLT_mu18" || trigger == "HLT_mu20" || trigger == "HLT_mu22" || trigger == "HLT_mu24" || trigger == "HLT_mu26"){
            ATH_MSG_WARNING("What you are trying to do is not correct. For di-muon triggers you should get the efficiency with getTriggerEfficiency and compute the SF by yourself.");
        } else if (trigger.find("HLT_2mu10") != std::string::npos || trigger.find("HLT_2mu14") != std::string::npos) {
            CorrectionCode cc = GetTriggerSF_dimu(triggersf, configuration, mucont, trigger);
            return cc;
        } else {
            CorrectionCode cc = GetTriggerSF(triggersf, configuration, mucont, trigger);
            return cc;
        }
        return CorrectionCode::Ok;
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::getReplica_index
    // ==================================================================================
    // Gets  replica index correponding to the toy.
    // Also checks if the sys_name contains "MCTOY" and if the trigger has replicas generated.
    // Returns -1 if conditions are note satisfied
    int MuonTriggerScaleFactors::getReplica_index(std::string sysBaseName, const std::string trigStr) {
        if (m_replicaSet.find(trigStr) == m_replicaSet.end()) return -1; //No toys for this trigger
        std::size_t pos = sysBaseName.find("MCTOY");
        if (pos == std::string::npos) return -1; //sys variation not affected by TOYS
        return atoi(sysBaseName.substr(pos + 5, pos + 8).c_str()); //toys for this trigger are around get the 3-digit number
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::getTriggerEfficiency
    // ==================================================================================
    CorrectionCode MuonTriggerScaleFactors::getTriggerEfficiency(const xAOD::Muon& mu, Double_t& efficiency, const std::string& trigger, Bool_t dataType) {
        if (trigger.empty()) {
            ATH_MSG_ERROR("MuonTriggerScaleFactors::getTriggerEfficiency Trigger must have value.");
            return CorrectionCode::Error;
        }
        TrigMuonEff::Configuration configuration;
        configuration.isData = dataType;
        configuration.replicaIndex = -1;
        Int_t threshold;
        CorrectionCode result = getThreshold(threshold, trigger);
        if (result != CorrectionCode::Ok) return result;
        if (mu.pt() < threshold) {
            efficiency = 0;
            return CorrectionCode::Ok;
        } 

        // Pre-define uncertainty variations
        static const CP::SystematicVariation stat_up("MUON_EFF_TrigStatUncertainty", 1);
        static const CP::SystematicVariation stat_down("MUON_EFF_TrigStatUncertainty", -1);
        static const CP::SystematicVariation syst_up("MUON_EFF_TrigSystUncertainty", 1);
        static const CP::SystematicVariation syst_down("MUON_EFF_TrigSystUncertainty", -1);

        std::string systype = "";
        if (appliedSystematics().matchSystematic(syst_down)) {
            systype = "syst_down";
        } else if (appliedSystematics().matchSystematic(syst_up)) {
            systype = "syst_up";
        } else if (appliedSystematics().matchSystematic(stat_down)) {
            systype = "stat_down";
        } else if (appliedSystematics().matchSystematic(stat_up)) {
            systype = "stat_up";
        } else {
            systype = "nominal";
        }

        // Toys, if found, will overwrite the data hists stat with the generated toy
        //+++++++++++++
        // The best way is the use of filterByName with the 000MCTOY at the end. See:
        // if( !(appliedSystematics().filterByBaseName("MUON_EFF_Trig_MCTOY000")).empty()){//The following is a hack!!!
        //++++++++++THE FOLLOWING IS A PARTIAL HACK!!!
        if (!appliedSystematics().empty() && configuration.isData == true) {
            configuration.replicaIndex = getReplica_index(appliedSystematics().begin()->basename(), trigger);
            if (configuration.replicaIndex != -1) systype = "replicas";
        }

        CorrectionCode cc = getMuonEfficiency(efficiency, configuration, mu, trigger, systype);
        return cc;
    }

    double MuonTriggerScaleFactors::dR(const double eta1, const double phi1, const double eta2, const double phi2) {
        double deta = fabs(eta1 - eta2);
        double dphi = fabs(phi1 - phi2) < TMath::Pi() ? fabs(phi1 - phi2) : 2 * TMath::Pi() - fabs(phi1 - phi2);
        return sqrt(deta * deta + dphi * dphi);
    }

    ///////////////////////
    // Private functions //
    ///////////////////////

    // ==================================================================================
    // == MuonTriggerScaleFactors::generateReplicas
    // ==================================================================================
    // Generate replicas of h for Toys with each bin of h varied with Gaussian distribution
    // with mean from bin content and sigma from bin error
    std::vector<TH2*> MuonTriggerScaleFactors::generateReplicas(TH2* h, int nrep, int seed) {
        TRandom3 Rndm(seed);
        std::vector<TH2*> replica_v(nrep);
        const int xbins = h->GetNbinsX(), ybins = h->GetNbinsY();
        ;

        for (int t = 0; t < nrep; ++t) {
            TH2* replica = dynamic_cast<TH2*>(h->Clone(Form("rep%d_%s", t, h->GetName())));

            for (int x_i = 0; x_i <= xbins; ++x_i) {
                for (int y_i = 0; y_i <= ybins; ++y_i) {
                    replica->SetBinContent(x_i, y_i, Rndm.Gaus(h->GetBinContent(x_i, y_i), h->GetBinError(x_i, y_i)));
                }
            }
            replica_v.at(t) = replica;
        }
        return replica_v;
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::getMuonEfficiency
    // ==================================================================================
    CorrectionCode MuonTriggerScaleFactors::getMuonEfficiency(Double_t& eff, const TrigMuonEff::Configuration& configuration, const xAOD::Muon& muon, const std::string& trigger, const std::string& systematic) {
        const double mu_eta = muon.eta();
        const double mu_phi = muon.phi();

        const std::string type = (configuration.isData ? "_data" : "_mc");
        const std::string region = ((fabs(mu_eta) < muon_barrel_endcap_boundary) ? "_barrel" : "_endcap");
        const std::string quality = m_muonquality;

        const std::string histname = "_MuonTrigEff_Period" + m_dataPeriod + "_" + trigger + "_" + quality + "_" + "_EtaPhi_" + m_binning + region + type + "_" + (configuration.isData?"nominal":systematic);

        TH2* eff_h2 = nullptr;
        if (configuration.replicaIndex >= 0) { //Only look into the replicas if asking for them
            std::map<std::string, std::vector<TH2*> >::const_iterator cit = m_efficiencyMapReplicaArray.find(histname);
            if (cit == m_efficiencyMapReplicaArray.end()) {
                if (m_allowZeroSF) {
                    eff = 0.;
                    return CorrectionCode::Ok;
                }

                else {
                    ATH_MSG_ERROR("Could not find what you are looking for in the efficiency map. The trigger you are looking for, year and mc are not consistent, or the trigger is unavailable in this data period. Please check how you set up the tool.");
                    return CorrectionCode::OutOfValidityRange;
                }
            }

            if (configuration.replicaIndex >= (int) cit->second.size()) {
                ATH_MSG_ERROR("MuonTriggerScaleFactors::getMuonEfficiency ; index for replicated histograms is out of range.");
                return CorrectionCode::OutOfValidityRange;
            }

            eff_h2 = cit->second[configuration.replicaIndex];
        } else { //Standard case, look into the usual eff map
            EfficiencyMap::const_iterator cit = m_efficiencyMap.find(histname);
            if (cit == m_efficiencyMap.end()) {
                if (m_allowZeroSF) {
                    eff = 0.;
                    return CorrectionCode::Ok;
                } else {
                    ATH_MSG_ERROR("Could not find what you are looking for in the efficiency map. The trigger you are looking for, year and mc are not consistent, or the trigger is unavailable in this data period. Please check how you set up the tool.");
                    return CorrectionCode::OutOfValidityRange;
                }
            }
            eff_h2 = cit->second;
        }

        double mu_phi_corr = mu_phi;
        if (mu_phi_corr < eff_h2->GetYaxis()->GetXmin()) mu_phi_corr += 2.0 * TMath::Pi();
        if (mu_phi_corr > eff_h2->GetYaxis()->GetXmax()) mu_phi_corr -= 2.0 * TMath::Pi();

        /*
         if( mu_phi_corr > eff_h2->GetYaxis()->GetXmax() ||
         mu_phi_corr < eff_h2->GetYaxis()->GetXmin() ||
         mu_eta > eff_h2->GetYaxis()->GetXmax() ||
         mu_eta < eff_h2->GetYaxis()->GetXmin() ){
         ATH_MSG_ERROR("MuonTriggerScaleFactors::getMuonEfficiency ; eta/phi is out of bound in the SF map. muon eta/phi="
         << mu_eta << "/" << mu_phi_corr
         << " map range eta/phi=["
         << eff_h2->GetXaxis()->GetXmin()
         << ","
         << eff_h2->GetXaxis()->GetXmax()
         << "]/["
         << eff_h2->GetYaxis()->GetXmin()
         << ","
         << eff_h2->GetYaxis()->GetXmax()
         << "]");
         }
         */

        const int bin = eff_h2->FindFixBin(mu_eta, mu_phi_corr);
        const double efficiency = eff_h2->GetBinContent(bin);

        eff = efficiency;

        ATH_MSG_DEBUG("getMuonEfficiency histname=" << histname);
        ATH_MSG_DEBUG("getMuonEfficiency [eta,phi,phi_corr]=[" << mu_eta << "," << mu_phi << "," << mu_phi_corr << "], ibin=" << bin << " eff=" << eff);

        return CorrectionCode::Ok;

    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::GetTriggerSF_dimu
    // ==================================================================================
    CorrectionCode MuonTriggerScaleFactors::GetTriggerSF_dimu(Double_t& TriggerSF, TrigMuonEff::Configuration& configuration, const xAOD::MuonContainer& mucont, const std::string& trigger) {

        if (mucont.size() != 2) {
            ATH_MSG_FATAL("MuonTriggerScaleFactors::GetTriggerSF;Currently dimuon trigger chains only implemented for events with exactly 2 muons.");
        }

        ATH_MSG_DEBUG("The trigger that you choose : " << trigger);

        Double_t eff_data = 0;
        Double_t eff_mc = 0;

        std::string data_err = "";
        std::string mc_err = "";

        if (appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", -1))) {
            data_err = "nominal";
            mc_err = "syst_up";
        } else if (appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", 1))) {
            data_err = "nominal";
            mc_err = "syst_down";
        } else if (appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", -1))) {
            data_err = "stat_down";
            mc_err = "nominal";
        } else if (appliedSystematics().matchSystematic(CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", 1))) {
            data_err = "stat_up";
            mc_err = "nominal";
        } else {
            data_err = "nominal";
            mc_err = "nominal";
        }

        //Toys, if found, will overwrite the data hists with the sys generated with one toy
        //+++++++++++++
        //The best way is the use of filterByName with the 000MCTOY at the end. See:
        // if( !(appliedSystematics().filterByBaseName("MUON_EFF_Trig_MCTOY000")).empty()){//The following is a hack!!!
        //++++++++++THE FOLLOWING IS A PARTIAL HACK!!!
        if (!appliedSystematics().empty()) {
            configuration.replicaIndex = getReplica_index(appliedSystematics().begin()->basename(), trigger);
            if (configuration.replicaIndex != -1) data_err = "replicas";
        }

        configuration.isData = true;
        CorrectionCode result = getDimuonEfficiency(eff_data, configuration, mucont, trigger, data_err);
        if (result != CorrectionCode::Ok) return result;
        
        configuration.isData = false;
        configuration.replicaIndex = -1;
        result = getDimuonEfficiency(eff_mc, configuration, mucont, trigger, mc_err);
        if (result != CorrectionCode::Ok) return result;
        
        double event_SF = 1.;

        if (fabs(1. - eff_mc) > 0.0001) {
            event_SF = eff_data / eff_mc;
        }

        TriggerSF = event_SF;
        return CorrectionCode::Ok;
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::GetTriggerSF
    // ==================================================================================
    CorrectionCode MuonTriggerScaleFactors::GetTriggerSF(Double_t& TriggerSF, TrigMuonEff::Configuration& configuration, const xAOD::MuonContainer& mucont, const std::string& trigger) {
        Int_t threshold;
        CorrectionCode result = getThreshold(threshold, trigger);
        if (result != CorrectionCode::Ok) return result;

        double rate_not_fired_data = 1.;
        double rate_not_fired_mc = 1.;

        for (const auto& mu : mucont) {

            double eff_data = 0., eff_mc = 0.;

            if (mu->pt() < threshold) {

                eff_data = 0.;
                eff_mc = 0.;

            } else {

                std::string muon_trigger_name = trigger;
                std::string data_err = "";
                std::string mc_err = "";

                // Pre-define uncertainty variations
                static const CP::SystematicVariation stat_up("MUON_EFF_TrigStatUncertainty", 1);
                static const CP::SystematicVariation stat_down("MUON_EFF_TrigStatUncertainty", -1);
                static const CP::SystematicVariation syst_up("MUON_EFF_TrigSystUncertainty", 1);
                static const CP::SystematicVariation syst_down("MUON_EFF_TrigSystUncertainty", -1);

                if (appliedSystematics().matchSystematic(syst_down)) {
                    data_err = "nominal";
                    mc_err = "syst_up";
                } else if (appliedSystematics().matchSystematic(syst_up)) {
                    data_err = "nominal";
                    mc_err = "syst_down";
                } else if (appliedSystematics().matchSystematic(stat_down)) {
                    data_err = "stat_down";
                    mc_err = "nominal";
                } else if (appliedSystematics().matchSystematic(stat_up)) {
                    data_err = "stat_up";
                    mc_err = "nominal";
                } else {
                    data_err = "nominal";
                    mc_err = "nominal";
                }

                //Toys, if found, will overwrite the data hists, on which toys for stat uncertainty have been generated
                //+++++++++++++
                //The best way is the use of filterByName with the 000MCTOY at the end. See:
                // if( !(appliedSystematics().filterByBaseName("MUON_EFF_Trig_MCTOY000")).empty()){//The following is a hack!!!
                //++++++++++The following is a hack!!!
                if (!appliedSystematics().empty()) {
                    configuration.replicaIndex = getReplica_index(appliedSystematics().begin()->basename(), trigger);
                    if (configuration.replicaIndex != -1) data_err = "replicas";
                }

                configuration.isData = true;
                CorrectionCode result_data = getMuonEfficiency(eff_data, configuration, *mu, muon_trigger_name, data_err);
                if (result_data != CorrectionCode::Ok) return result_data;
                configuration.isData = false;
                configuration.replicaIndex = -1;
                CorrectionCode result_mc = getMuonEfficiency(eff_mc, configuration, *mu, muon_trigger_name, mc_err);
                if (result_mc != CorrectionCode::Ok) return result_data;
            }
            rate_not_fired_data *= (1. - eff_data);
            rate_not_fired_mc *= (1. - eff_mc);
        }

        double event_SF = 1.;
        if (1 - rate_not_fired_data == 0) event_SF = 0;
        if ((mucont.size()) and (fabs(1. - rate_not_fired_mc) > 0.0001)) {

            event_SF = (1. - rate_not_fired_data) / (1. - rate_not_fired_mc);

        }
        TriggerSF = event_SF;

        return CorrectionCode::Ok;
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::getDimuonEfficiency
    // ==================================================================================
    CorrectionCode MuonTriggerScaleFactors::getDimuonEfficiency(Double_t& eff, const TrigMuonEff::Configuration& configuration, const xAOD::MuonContainer& mucont, const std::string& chain, const std::string& systematic) {

        std::string trigger = getTriggerCorrespondingToDimuonTrigger(chain);
        Int_t threshold;
        CorrectionCode result = getThreshold(threshold, trigger);
        if (result != CorrectionCode::Ok) return result;

        xAOD::MuonContainer::const_iterator mu1 = mucont.begin();
        xAOD::MuonContainer::const_iterator mu2 = mucont.begin() + 1;

        Double_t eff1 = 0;
        if ((**mu1).pt() > threshold) {
            CorrectionCode result1 = getMuonEfficiency(eff1, configuration, (**mu1), trigger, systematic);
            if (result1 != CorrectionCode::Ok) return result1;
        }
        Double_t eff2 = 0;
        if ((**mu2).pt() > threshold) {
            CorrectionCode result2 = getMuonEfficiency(eff2, configuration, (**mu2), trigger, systematic);
            if (result2 != CorrectionCode::Ok) return result2;
        }

        eff = eff1 * eff2;
        return CorrectionCode::Ok;
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::getTriggerCorrespondingToDimuonTrigger
    // ==================================================================================
    std::string MuonTriggerScaleFactors::getTriggerCorrespondingToDimuonTrigger(const std::string& trigger) {
        if (trigger.find("2mu10") != std::string::npos) return "HLT_mu10";
        if (trigger.find("2mu14") != std::string::npos) return "HLT_mu14";
        throw std::runtime_error("Unknown dimuon trigger");
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::getThreshold
    // ==================================================================================
    CorrectionCode MuonTriggerScaleFactors::getThreshold(Int_t& threshold, const std::string& trigger) {
        std::size_t index = trigger.find("HLT_mu");
        if (index != std::string::npos) {
            std::string rawNumber = trigger.substr(index + 6);
            if (!rawNumber.empty() && isdigit(rawNumber[0])) {
                std::stringstream(rawNumber) >> threshold;
                if (threshold < 10) threshold = 10000;
                else threshold = (threshold + 1) * 1000;
                return CorrectionCode::Ok;
            }
        }
        ATH_MSG_ERROR("MuonTriggerScaleFactors::getThreshold Could not extract threshold for trigger " << trigger);
        return CorrectionCode::Error;
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::getDataPeriod
    // ==================================================================================
    std::string MuonTriggerScaleFactors::getDataPeriod(int runNumber, const std::string& year) {
        if (year == "2015") {
            if (runNumber >= 266904 && runNumber <= 272531) return "AC";
            if (runNumber >= 276073 && runNumber <= 276954) return "D";
            if (runNumber >= 278727 && runNumber <= 279928) return "E";
            if (runNumber >= 279932 && runNumber <= 280422) return "F";
            if (runNumber >= 280423 && runNumber <= 281075) return "G";
            if (runNumber >= 281130 && runNumber <= 281411) return "H";
            if (runNumber >= 281662 && runNumber <= 282482) return "I"; // special ALFA run
            if (runNumber >= 282625 && runNumber <= 284484) return "J";
        }
        else if (year == "2016") {
            if (runNumber >= 296939 && runNumber <= 300287) return "A";
            if (runNumber >= 300345 && runNumber <= 300908) return "B";
            if (runNumber >= 301912 && runNumber <= 302393) return "C";
            if (runNumber >= 302737 && runNumber <= 302872) return "D1D3";
            if (runNumber >= 302919 && runNumber <= 303560) return "D4D8";
            if (runNumber >= 303638 && runNumber <= 303892) return "E";
            if (runNumber >= 303943 && runNumber <= 304494) return "F";
            if (runNumber >= 305291 && runNumber <= 306714) return "G";
            if (runNumber >= 307124 && runNumber <= 308084) return "I";
            if (runNumber >= 309311 && runNumber <= 309759) return "K";
            if (runNumber >= 310015 && runNumber <= 311481) return "L";
        }
        return "";
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::getTemporaryDirectory
    // ==================================================================================
    TDirectory* MuonTriggerScaleFactors::getTemporaryDirectory(void) const {
        gROOT->cd();
        TDirectory* tempDir = 0;
        int counter = 0;
        while (not tempDir) {
            std::stringstream dirname;
            dirname << "MuonTriggerScaleFactorsTempDir_%i" << counter;
            if (gROOT->GetDirectory((dirname.str()).c_str())) {
                ++counter;
                continue;
            }
            tempDir = gROOT->mkdir((dirname.str()).c_str());
            if (not tempDir) {
                ATH_MSG_ERROR("getTemporaryDirectory::Temporary directory could not be created");
            }
        }
        return tempDir;
    }

    //=======================================================================
    //   Systematics Interface
    //=======================================================================
    bool MuonTriggerScaleFactors::isAffectedBySystematic(const CP::SystematicVariation& systematic) const {
        if (!systematic.empty()) {
            CP::SystematicSet sys = affectingSystematics();
            return sys.find(systematic) != sys.end();
        }
        return true;
    }

    /// returns: the list of all systematics this tool can be affected by
    CP::SystematicSet MuonTriggerScaleFactors::affectingSystematics() const {
        CP::SystematicSet mySysSet;

        mySysSet.insert(CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", 1));
        mySysSet.insert(CP::SystematicVariation("MUON_EFF_TrigSystUncertainty", -1));

        //Consider full statUncertainty if  TOY replicas are not used
        if (m_replicaTriggerList.size() == 0) {
            mySysSet.insert(CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", 1));
            mySysSet.insert(CP::SystematicVariation("MUON_EFF_TrigStatUncertainty", -1));
        } else {
            for (int i = 0; i < m_nReplicas; ++i) { //TOFIX Hack with just up variations! needs ASG reserved words for a clean handling//+++++++
                mySysSet.insert(CP::SystematicVariation(Form("MUON_EFF_Trig_MCTOY%03d", i), 1));
            }
        }

        return mySysSet;
    }

    // Register the systematics with the registry and add them to the recommended list
    CP::SystematicCode MuonTriggerScaleFactors::registerSystematics() {
        CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
        if (registry.registerSystematics(*this) != CP::SystematicCode::Ok) {
            ATH_MSG_ERROR("Failed to add systematic to list of recommended systematics.");
            return CP::SystematicCode::Unsupported;
        }
        return CP::SystematicCode::Ok;
    }

    /// returns: the list of all systematics this tool recommends to use
    CP::SystematicSet MuonTriggerScaleFactors::recommendedSystematics() const {
        return affectingSystematics();
    }

    CP::SystematicCode MuonTriggerScaleFactors::applySystematicVariation(const CP::SystematicSet& systConfig) {
        // First, check if we already know this systematic configuration
        auto itr = m_systFilter.find(systConfig);

        // If it's a new input set, we need to filter it
        if (itr == m_systFilter.end()) {

            // New systematic. We need to parse it.
            static CP::SystematicSet affectingSys = affectingSystematics();
            CP::SystematicSet filteredSys;
            if (!CP::SystematicSet::filterForAffectingSystematics(systConfig, affectingSys, filteredSys)) {
                ATH_MSG_ERROR("Unsupported combination of systematics passed to the tool!");
                return CP::SystematicCode::Unsupported;
            }

            // Insert filtered set into the map
            itr = m_systFilter.insert(std::make_pair(systConfig, filteredSys)).first;
        }

        CP::SystematicSet& mySysConf = itr->second;

        // Check to see if the set of variations tries to add in the uncertainty up and down. Since the errors
        // are symetric this would result in 0 and so should not be done.
        static const CP::SystematicVariation stat_up("MUON_EFF_TrigStatUncertainty", 1);
        static const CP::SystematicVariation stat_down("MUON_EFF_TrigStatUncertainty", -1);
        static const CP::SystematicVariation syst_up("MUON_EFF_TrigSystUncertainty", 1);
        static const CP::SystematicVariation syst_down("MUON_EFF_TrigSystUncertainty", -1);

        if ((mySysConf.matchSystematic(stat_up) && mySysConf.matchSystematic(stat_down)) || (mySysConf.matchSystematic(syst_up) && mySysConf.matchSystematic(syst_down))) {
            return CP::SystematicCode::Unsupported;
        }

        m_appliedSystematics = &mySysConf;
        return CP::SystematicCode::Ok;
    }

} /* namespace CP */
