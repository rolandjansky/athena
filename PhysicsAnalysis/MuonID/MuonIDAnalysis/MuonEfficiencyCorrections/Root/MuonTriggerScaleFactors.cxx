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
                    m_runNumber(300345),
                    m_classname(name.c_str()),
                    m_max_period(TrigMuonEff::period_undefined),
                    m_allowZeroSF(false) {
        declareProperty("MuonQuality", m_muonquality = "Medium"); // HighPt,Tight,Medium,Loose
        declareProperty("Isolation", m_isolation = ""); // "", "IsoGradient", "IsoLoose", "IsoTight"
        declareProperty("CalibrationRelease", m_calibration_version = "170128_Moriond"); // 160624_ICHEP

        declareProperty("Year", m_year = "2016"); // 2015 or 2016
        declareProperty("MC", m_mc = "mc15c"); // mc15a or mc15c

        // these are for debugging / testing, *not* for general use!
        declareProperty("filename", m_fileName = "muontrigger_sf_2016_mc15c_v02.root"); // default is data16 vs mc15c
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

        std::string SFfile_2015_mc15a = "muontrigger_sf_2015_mc15a.root";
        std::string SFfile_2015_mc15c = "muontrigger_sf_2015_mc15c_v01.root";

        // Selecting the correct SF file if different from default
        if (m_year == "2015" && m_mc == "mc15a") {
            m_fileName = SFfile_2015_mc15a;
        } else if (m_year == "2015" && m_mc == "mc15c") {
            m_fileName = SFfile_2015_mc15c;
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

        m_max_period = TrigMuonEff::period_undefined;
        TDirectory* origDir = gDirectory;
        Storage storage;
        setStorage(storage);

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

        for (size_t iqu = 0; iqu < storage.quality->size(); ++iqu) {
            for (size_t ibins = 0; ibins < storage.bins->size(); ++ibins) {
                for (size_t iperiod = 0; iperiod < storage.period->size(); ++iperiod) {
                    for (size_t iregion = 0; iregion < storage.region->size(); ++iregion) {
                        for (size_t itype = 0; itype < storage.type->size(); ++itype) {
                            for (size_t iiso = 0; iiso < storage.isolation->size(); ++iiso) {

                                std::string histname = ("_MuonTrigEff_" + storage.period->at(iperiod) + "_" + storage.trigger->at(iperiod) + "_" + storage.quality->at(iqu) + "_" + storage.isolation->at(iiso) + "_EtaPhi_" + storage.bins->at(ibins) + "_" + storage.region->at(iregion) + "_" + storage.type->at(itype));

                                for (size_t isys = 0; isys < storage.systematic->size(); ++isys) {

                                    if (storage.type->at(itype).find("data") != std::string::npos && storage.systematic->at(isys).find("syst") != std::string::npos) continue;

                                    std::string histnameSys = histname + "_" + storage.systematic->at(isys);

                                    std::string path = (storage.quality->at(iqu) + storage.isolation->at(iiso) + "/" + +"Period" + storage.period->at(iperiod) + "/" + storage.trigger->at(iperiod) + "/" + +"eff_etaphi_" + storage.bins->at(ibins) + "_" + storage.region->at(iregion) + "_" + storage.type->at(itype) + "_" + storage.systematic->at(isys));

                                    TH2* hist = dynamic_cast<TH2*>(file->Get(path.c_str()));
                                    if (not hist) {
                                        ATH_MSG_FATAL("MuonTriggerScaleFactors::initialize : not available");
                                        ATH_MSG_FATAL("MuonTriggerScaleFactors::initialize : cannot find " << path);
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
                                if (m_replicaSet.find(storage.trigger->at(iperiod)) != m_replicaSet.end() && storage.type->at(itype).find("data") != std::string::npos) {

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
        TrigMuonEff::DataPeriod period = getDataPeriod(runNumber);
        if (period == TrigMuonEff::period_undefined) {
            ATH_MSG_WARNING("I am using run #" << runNumber << " but I cannot find corresponding run period. Now setting to use 2016 period B. This might give problems! Please check which year and mc you have set up!");

            m_runNumber = 300345;
            return CorrectionCode::Ok;
        }
        m_runNumber = runNumber;
        return CorrectionCode::Ok;
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::getTriggerScaleFactor
    // ==================================================================================
    CorrectionCode MuonTriggerScaleFactors::getTriggerScaleFactor(const xAOD::MuonContainer& mucont, Double_t& triggersf, const std::string& trigger) {
        TrigMuonEff::Configuration configuration;
        configuration.runNumber = m_runNumber;
        configuration.setByUser = false;

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
        TrigMuonEff::Configuration configuration;
        configuration.isData = dataType;
        configuration.runNumber = m_runNumber;
        configuration.setByUser = false;
        configuration.trigger = trigger;
        configuration.replicaIndex = -1;
        const Double_t threshold = getThresholds(trigger);
        if (mu.pt() < threshold) {
            efficiency = 0;
            return CorrectionCode::Ok;
        } else {
            if (not configuration.setByUser) {
                if (not setConfiguration(const_cast<TrigMuonEff::Configuration&>(configuration))) {
                    efficiency = 0;
                    return CorrectionCode::Ok;
                }
            }
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

    // ==================================================================================
    // == MuonTriggerScaleFactors::maskFeetRegions
    // ==================================================================================
    bool MuonTriggerScaleFactors::maskFeetRegions(const xAOD::Muon& mu, double dR) {
        // Want to take a muon (which will ideally have been previously identified as a triggering muon),
        // double check that the muon fires a L1 trigger (TrigMuonMatching::matchL1) and then
        // find the corresponding RoI from the list of fired RoI's by eta/phi matching
        // Then take the sector address and RoI and compare them to the forbidden ones, return true if forbidden

        //First thing: find the RoI that the muon is in, return a warning or something if it doesn't find one
        //get the event's RoI's for Lvl1 Muons
        const xAOD::MuonRoIContainer* muonrois = 0;
        StatusCode sc = evtStore()->retrieve(muonrois, "LVL1MuonRoIs"); //muonRoI's for this event
        if (!sc) {
            ATH_MSG_ERROR("MuonTriggerScaleFactor::maskFeetRegions : could not retrieve LVL1MuonRoIs");
            return false; //should return error codes
        }

        bool trigMatch = false;
        unsigned int muSectorAddress = 0;
        int muRoI = 0;
        //loop over muon RoI's, see if they fall within some dR of the muon's eta/phi
        for (xAOD::MuonRoIContainer::const_iterator muroi_itr = muonrois->begin(); muroi_itr != muonrois->end(); ++muroi_itr) {
            //if the muon etaphi falls within dR of the RoI's etaphi, we found our match
            if (MuonTriggerScaleFactors::dR(mu.eta(), mu.phi(), (*muroi_itr)->eta(), (*muroi_itr)->phi()) < dR) {
                //if(trigMatch = true)
                //ATH_MSG_ERROR("MuonTriggerScaleFactor::maskFeetRegions : dR too big, overlaps multiple RoI's");
                //warning: dR is too big and we have overlapping RoI's
                trigMatch = true;
                muSectorAddress = (*muroi_itr)->getSectorAddress();
                muRoI = (*muroi_itr)->getRoI();
                std::cout << "MuonTriggerScaleFactor: RoI-muon Match found; sec = " << muSectorAddress << ", RoI = " << muRoI << ", RoI eta = " << (*muroi_itr)->eta() << ", phi = " << (*muroi_itr)->phi() << ", Mu eta = " << mu.eta() << ", phi = " << mu.phi();
                break;
            }
        }

        if (!trigMatch) return false; // status code that indicates no RoI has been found for this muon

        //now return true if sector address and RoI fall within the feet region
        if (MuonTriggerScaleFactors::inFeetRegion(muSectorAddress, muRoI)) {
            std::cout << ", within feet region" << std::endl;
            return true; //status code success?
        } else {
            std::cout << ", outside of feet region" << std::endl;
            return false; //status code failure?
        }

        //May need to change this code to return status codes, in the event that something goes wrong
    }

    bool MuonTriggerScaleFactors::inFeetRegion(unsigned int SectorAddress, unsigned int RoI) {
        //massimo's code for finding feet regions
        bool bad_roi = false;

        if ((SectorAddress & 0xC0) == 0) { // barrel
            unsigned int tow = RoI / 4; // barrel tower
            int sec = ((SectorAddress & 0x3E) >> 1) + 32 * (SectorAddress & 0x1); // barrel sector 0-63

            if ((sec == 21 || sec == 22 || sec == 25 || sec == 26 || sec == 53 || sec == 54 || sec == 57 || sec == 58) && // feet sectors
            (tow == 2 || tow == 4 || tow == 5 || tow == 7)) bad_roi = true; // new feet towers

            if ((sec == 23 || sec == 24 || sec == 55 || sec == 56) && // bottom sectors
            (tow == 3)) bad_roi = true; // not working elevator towers

            // for debugging:
            if (bad_roi) std::cout << " sec = " << sec << " tow = " << tow;
        }

        return bad_roi;
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

        const std::string isolation = (m_isolation.empty() ? "" : "_" + m_isolation);

        const std::string chain = (trigger.empty() ? configuration.trigger : trigger);

        const std::string histname = "_MuonTrigEff_" + configuration.period + chain + "_" + quality + "_" + isolation + "_EtaPhi_" + configuration.binning + region + type + "_" + systematic;

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

        if (not configuration.setByUser) {
            if (not setConfiguration(const_cast<TrigMuonEff::Configuration&>(configuration))) {
                TriggerSF = 0;
                return CorrectionCode::Ok;
            }
        }

        ATH_MSG_DEBUG("The trigger that you choose : " << trigger);
        const Bool_t is_single = ((trigger.find("HLT_mu10") != std::string::npos) || (trigger.find("HLT_mu14") != std::string::npos) || (trigger.find("HLT_mu20_iloose_L1MU15") != std::string::npos) || (trigger.find("HLT_mu20_iloose_L1MU15_OR_HLT_mu40") != std::string::npos) || (trigger.find("HLT_mu20_iloose_L1MU15_OR_HLT_mu50") != std::string::npos) || (trigger.find("HLT_mu24_iloose_L1MU15_OR_HLT_mu24_iloose") != std::string::npos) || (trigger.find("HLT_mu24_iloose_L1MU15_OR_HLT_mu24_iloose_OR_HLT_mu40") != std::string::npos) || (trigger.find("HLT_mu24_iloose_L1MU15_OR_HLT_mu24_iloose_OR_HLT_mu50") != std::string::npos) || (trigger.find("HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15") != std::string::npos) || (trigger.find("HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15_OR_HLT_mu40") != std::string::npos) || (trigger.find("HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15_OR_HLT_mu50") != std::string::npos) || (trigger.find("HLT_mu24_iloose_L1MU15") != std::string::npos) || (trigger.find("HLT_mu24_iloose_L1MU15_OR_HLT_mu40") != std::string::npos) || (trigger.find("HLT_mu24_iloose_L1MU15_OR_HLT_mu50") != std::string::npos) || (trigger.find("HLT_mu24_imedium") != std::string::npos) || (trigger.find("HLT_mu24_imedium_OR_HLT_mu40") != std::string::npos) || (trigger.find("HLT_mu24_imedium_OR_HLT_mu50") != std::string::npos) || (trigger.find("HLT_mu24_ivarmedium") != std::string::npos) || (trigger.find("HLT_mu24_ivarmedium_OR_HLT_mu40") != std::string::npos) || (trigger.find("HLT_mu24_ivarmedium_OR_HLT_mu50") != std::string::npos) || (trigger.find("HLT_mu26_imedium") != std::string::npos) || (trigger.find("HLT_mu26_imedium_OR_HLT_mu40") != std::string::npos) || (trigger.find("HLT_mu26_imedium_OR_HLT_mu50") != std::string::npos) || (trigger.find("HLT_mu26_ivarmedium") != std::string::npos) || (trigger.find("HLT_mu26_ivarmedium_OR_HLT_mu40") != std::string::npos) || (trigger.find("HLT_mu26_ivarmedium_OR_HLT_mu50") != std::string::npos) || (trigger.find("HLT_mu40") != std::string::npos) || (trigger.find("HLT_mu50") != std::string::npos));

        const Bool_t is_dimu = (trigger.find("HLT_2mu10") != std::string::npos || trigger.find("HLT_2mu14") != std::string::npos);

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
        if (is_single and is_dimu) {
            CorrectionCode result = getSingleOrDimuonEfficiency(eff_data, configuration, mucont, trigger, data_err);
            if (result != CorrectionCode::Ok) return result;
        } else if (is_dimu) {
            CorrectionCode result = getDimuonEfficiency(eff_data, configuration, mucont, trigger, data_err);
            if (result != CorrectionCode::Ok) return result;
        } else {
            ATH_MSG_FATAL("MuonTriggerScaleFactors::GetTriggerSF;unknown trigger combination");
            throw std::runtime_error("unknown trigger combination");
        }

        configuration.isData = false;
        configuration.replicaIndex = -1;
        if (is_single and is_dimu) {
            CorrectionCode result = getSingleOrDimuonEfficiency(eff_mc, configuration, mucont, trigger, mc_err);
            if (result != CorrectionCode::Ok) return result;
        } else if (is_dimu) {
            CorrectionCode result = getDimuonEfficiency(eff_mc, configuration, mucont, trigger, mc_err);
            if (result != CorrectionCode::Ok) return result;
        }

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
        const Double_t threshold = getThresholds(trigger);

        double rate_not_fired_data = 1.;
        double rate_not_fired_mc = 1.;

        for (const auto& mu : mucont) {

            double eff_data = 0., eff_mc = 0.;

            if (mu->pt() < threshold) {

                eff_data = 0.;
                eff_mc = 0.;

            } else {

                if (not configuration.setByUser) {
                    if (not setConfiguration(const_cast<TrigMuonEff::Configuration&>(configuration))) {
                        TriggerSF = 0;
                        return CorrectionCode::Ok;
                    }
                }

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
    // == MuonTriggerScaleFactors::getSingleOrDimuonEfficiency
    // ==================================================================================
    CorrectionCode MuonTriggerScaleFactors::getSingleOrDimuonEfficiency(Double_t& eff, const TrigMuonEff::Configuration& config, const xAOD::MuonContainer& mucont, const std::string& chain, const std::string& systematic)

    {
        ATH_MSG_DEBUG("getSingleOrDimuonEfficiency : the argument chain is " << chain);
        ATH_MSG_DEBUG("getSingleOrDimuonEfficiency : Configuration.trigger is " << config.trigger);

        const Double_t threshold = getThresholds(chain);

        xAOD::MuonContainer::const_iterator mu1 = mucont.begin();
        xAOD::MuonContainer::const_iterator mu2 = mucont.begin() + 1;

        // single muon trigger
        std::string singletrig = "";
        if (chain.find("HLT_mu20_iloose_L1MU15_OR_HLT_mu40") != std::string::npos) { // they should be in this order!!
            singletrig = "HLT_mu20_iloose_L1MU15_OR_HLT_mu40";
        } else if (chain.find("HLT_mu24_iloose_L1MU15_OR_HLT_mu40") != std::string::npos) {
            singletrig = "HLT_mu24_iloose_L1MU15_OR_HLT_mu40";
        } else if (chain.find("HLT_mu24_iloose_L1MU15_OR_HLT_mu24_iloose_OR_HLT_mu40") != std::string::npos) {
            singletrig = "HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15_OR_HLT_mu40";
        } else if (chain.find("HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15_OR_HLT_mu40") != std::string::npos) {
            singletrig = "HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15_OR_HLT_mu40";
        } else if (chain.find("HLT_mu24_imedium_OR_HLT_mu40") != std::string::npos) {
            singletrig = "HLT_mu24_imedium_L1MU15_OR_HLT_mu40";
        } else if (chain.find("HLT_mu24_ivarmedium_OR_HLT_mu40") != std::string::npos) {
            singletrig = "HLT_mu24_ivarmedium_L1MU15_OR_HLT_mu40";
        } else if (chain.find("HLT_mu26_imedium_OR_HLT_mu40") != std::string::npos) {
            singletrig = "HLT_mu26_imedium_L1MU15_OR_HLT_mu40";
        } else if (chain.find("HLT_mu26_ivarmedium_OR_HLT_mu40") != std::string::npos) {
            singletrig = "HLT_mu26_ivarmedium_L1MU15_OR_HLT_mu40";
        } else if (chain.find("HLT_mu20_iloose_L1MU15_OR_HLT_mu50") != std::string::npos) {
            singletrig = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
        } else if (chain.find("HLT_mu24_iloose_L1MU15_OR_HLT_mu50") != std::string::npos) {
            singletrig = "HLT_mu24_iloose_L1MU15_OR_HLT_mu50";
        } else if (chain.find("HLT_mu24_iloose_L1MU15_OR_HLT_mu24_iloose_OR_HLT_mu50") != std::string::npos) {
            singletrig = "HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15_OR_HLT_mu50";
        } else if (chain.find("HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15_OR_HLT_mu50") != std::string::npos) {
            singletrig = "HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15_OR_HLT_mu50";
        } else if (chain.find("HLT_mu24_imedium_OR_HLT_mu50") != std::string::npos) {
            singletrig = "HLT_mu24_imedium_L1MU15_OR_HLT_mu50";
        } else if (chain.find("HLT_mu24_ivarmedium_OR_HLT_mu50") != std::string::npos) {
            singletrig = "HLT_mu24_ivarmedium_L1MU15_OR_HLT_mu50";
        } else if (chain.find("HLT_mu26_imedium_OR_HLT_mu50") != std::string::npos) {
            singletrig = "HLT_mu26_imedium_L1MU15_OR_HLT_mu50";
        } else if (chain.find("HLT_mu26_ivarmedium_OR_HLT_mu50") != std::string::npos) {
            singletrig = "HLT_mu26_ivarmedium_L1MU15_OR_HLT_mu50";
        } else if (chain.find("HLT_mu24_iloose_L1MU15_OR_HLT_mu24_iloose") != std::string::npos) {
            singletrig = "HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15";
        } else if (chain.find("HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15") != std::string::npos) {
            singletrig = "HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15";
        } else if (chain.find("HLT_mu20_iloose_L1MU15") != std::string::npos) {
            singletrig = "HLT_mu20_iloose_L1MU15";
        } else if (chain.find("HLT_mu24_iloose_L1MU15") != std::string::npos) {
            singletrig = "HLT_mu24_iloose_L1MU15";
        } else if (chain.find("HLT_mu24_imedium") != std::string::npos) {
            singletrig = "HLT_mu24_imedium";
        } else if (chain.find("HLT_mu24_ivarmedium") != std::string::npos) {
            singletrig = "HLT_mu24_ivarmedium";
        } else if (chain.find("HLT_mu26_imedium") != std::string::npos) {
            singletrig = "HLT_mu26_imedium";
        } else if (chain.find("HLT_mu26_ivarmedium") != std::string::npos) {
            singletrig = "HLT_mu26_ivarmedium";
        } else if (chain.find("HLT_mu40") != std::string::npos) {
            singletrig = "HLT_mu40";
        } else if (chain.find("HLT_mu50") != std::string::npos) {
            singletrig = "HLT_mu50";
        } else {
            ATH_MSG_ERROR("MuonTriggerScaleFactors::getSingleOrDimuonEfficiency ; Invalid dimuon or combination of single and dimuon trigger chain name given");
        }
        double mu1_eff = 0;
        if ((**mu1).pt() * 0.001 > threshold) {
            CorrectionCode result_mu1 = getMuonEfficiency(mu1_eff, config, **mu1, singletrig, systematic);
            if (result_mu1 != CorrectionCode::Ok) return result_mu1;
        }
        double mu2_eff = 0;
        if ((**mu1).pt() * 0.001 > threshold) {
            CorrectionCode result_mu2 = getMuonEfficiency(mu2_eff, config, **mu2, singletrig, systematic);
            if (result_mu2 != CorrectionCode::Ok) return result_mu2;
        }

        Double_t SingleEff = mu1_eff + mu2_eff - mu1_eff * mu2_eff;

        // Di-muon trigger
        std::string dimutrig = "";
        if (chain.find("HLT_2mu10") != std::string::npos) {
            dimutrig = "HLT_2mu10";
        } else if (chain.find("HLT_2mu14") != std::string::npos) {
            dimutrig = "HLT_2mu14";
        } else {
            ATH_MSG_ERROR("MuonTriggerScaleFactors::getSingleOrDimuonEfficiency ; Invalid dimuon or combination of single and dimuon trigger chain name given");
        }
        Double_t dimu_eff_2mu14;
        CorrectionCode result = getDimuonEfficiency(dimu_eff_2mu14, config, mucont, dimutrig, systematic);
        if (result != CorrectionCode::Ok) return result;

        Double_t SingleOrDimu = 0;

        SingleOrDimu = SingleEff + dimu_eff_2mu14 - SingleEff * dimu_eff_2mu14;

        eff = SingleOrDimu;

        return CorrectionCode::Ok;
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::getDimuonEfficiency
    // ==================================================================================
    CorrectionCode MuonTriggerScaleFactors::getDimuonEfficiency(Double_t& eff, const TrigMuonEff::Configuration& configuration, const xAOD::MuonContainer& mucont, const std::string& chain, const std::string& systematic) {

        DileptonThresholds thresholds;
        getDileptonThresholds(thresholds);

        double threshold_leg1 = 0.;
        double threshold_leg2 = 0.;

        if (chain.find("2mu10") != std::string::npos) {
            threshold_leg1 = thresholds.mu10;
            threshold_leg2 = thresholds.mu10;
        } else if (chain.find("2mu14") != std::string::npos) {
            threshold_leg1 = thresholds.mu14;
            threshold_leg2 = thresholds.mu14;
        } else {
            ATH_MSG_ERROR("MuonTriggerScaleFactors::getDimuonEfficiency ; Invalid dimuon or combination of single and dimuon trigger chain name given");
        }

        DileptonTrigger dimuon;
        getDileptonLegs(chain, dimuon);

        xAOD::MuonContainer::const_iterator mu1 = mucont.begin();
        xAOD::MuonContainer::const_iterator mu2 = mucont.begin() + 1;

        // data
        Double_t eff1 = 0;
        if ((**mu1).pt() * 0.001 > threshold_leg1) {
            CorrectionCode result1 = getMuonEfficiency(eff1, configuration, (**mu1), dimuon.leg1, systematic);
            if (result1 != CorrectionCode::Ok) return result1;
        }
        Double_t eff2 = 0;
        if ((**mu2).pt() * 0.001 > threshold_leg2) {
            CorrectionCode result2 = getMuonEfficiency(eff2, configuration, (**mu2), dimuon.leg2, systematic);
            if (result2 != CorrectionCode::Ok) return result2;
        }

        //    Double_t TrigEff = 1 - (1 - eff1)*(1 - eff2);
        Double_t TrigEff = eff1 * eff2;

        eff = TrigEff;

        return CorrectionCode::Ok;
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::setMaxPeriod
    // ==================================================================================
    CorrectionCode MuonTriggerScaleFactors::setMaxPeriod(const TrigMuonEff::DataPeriod x) {
        m_max_period = x;
        return CorrectionCode::Ok;
    }

    // Private functions
    // ==================================================================================
    // == MuonTriggerScaleFactors::getDileptonLegs
    // ==================================================================================
    void MuonTriggerScaleFactors::getDileptonLegs(const std::string& chain, DileptonTrigger& dilepton) {
        if (chain.find("2mu10") != std::string::npos) {
            dilepton.leg1 = "HLT_mu10";
            dilepton.leg2 = "HLT_mu10";
            dilepton.bothLegs = "HLT_mu10";
        } else if (chain.find("2mu14") != std::string::npos) {
            dilepton.leg1 = "HLT_mu14";
            dilepton.leg2 = "HLT_mu14";
            dilepton.bothLegs = "HLT_mu14";
        } else {
            dilepton.leg1 = "";
            dilepton.leg2 = "";
            dilepton.bothLegs = "";
            //ATH_MSG_ERROR( "MuonTriggerScaleFactors::getDileptonLegs Invalid dilepton or combination of single and dilepton trigger chain name given");
        }
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::getThresholds
    // ==================================================================================
    Double_t MuonTriggerScaleFactors::getThresholds(const std::string& trigger) {
        if (trigger.find("HLT_mu6") != std::string::npos) return 10000;
        if (trigger.find("HLT_mu8") != std::string::npos) return 10000;
        if (trigger.find("HLT_mu10") != std::string::npos) return 10000 * 1.05;
        if (trigger.find("HLT_mu14") != std::string::npos) return 14000 * 1.05;
        if (trigger.find("HLT_mu18") != std::string::npos) return 18000 * 1.05;
        if (trigger.find("HLT_mu20") != std::string::npos) return 20000 * 1.05;
        if (trigger.find("HLT_mu22") != std::string::npos) return 22000 * 1.05;
        if (trigger.find("HLT_mu24") != std::string::npos) return 24000 * 1.05;
        if (trigger.find("HLT_mu26") != std::string::npos) return 26000 * 1.05;
        if (trigger.compare("HLT_mu40") == 0) return 40000 * 1.05;
        if (trigger.compare("HLT_mu50") == 0) return 50000 * 1.05;
        return 10000;
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::setConfiguration
    // ==================================================================================
    bool MuonTriggerScaleFactors::setConfiguration(TrigMuonEff::Configuration& config, TrigMuonEff::DataPeriod period) const {
        if (period == TrigMuonEff::period_undefined) {
            period = getDataPeriod(config.runNumber);
            if (m_max_period != TrigMuonEff::period_undefined) {
                if (period > m_max_period) {
                    static bool hasWarned = false;
                    if (not hasWarned) {
                        ATH_MSG_WARNING("MuonTriggerScaleFactors::period out of range:setting it to ");
                        hasWarned = true;
                    }
                    period = m_max_period;
                }
            }

            if (period == TrigMuonEff::period_undefined) {
                ATH_MSG_ERROR("MuonTriggerScaleFactors::setConfiguration RunNumber is not in 2015 dataset. No scale factors calculated.");
                return false;
            }

            if (period == TrigMuonEff::period_runnumber_zero) {
                ATH_MSG_WARNING("MuonTriggerScaleFactors::setConfiguration;RunNumber is 0. If the value of pileupReweight is 0, this is no problem. (Trigger SF also returns 0)");
                return false;
            }
        }
        switch (period) {
            case TrigMuonEff::per2015AC:
                config.period = "AC_";
                config.binning = m_binning;
                config.trigger = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2015D:
                config.period = "D_";
                config.binning = m_binning;
                config.trigger = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2015E:
                config.period = "E_";
                config.binning = m_binning;
                config.trigger = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2015F:
                config.period = "F_";
                config.binning = m_binning;
                config.trigger = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2015G:
                config.period = "G_";
                config.binning = m_binning;
                config.trigger = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2015H:
                config.period = "H_";
                config.binning = m_binning;
                config.trigger = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2015I:
                config.period = "I_";
                config.binning = m_binning;
                config.trigger = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2015J:
                config.period = "J_";
                config.binning = m_binning;
                config.trigger = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2016A:
                config.period = "A_";
                config.binning = m_binning;
                config.trigger = "HLT_mu26_ivarmedium_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2016B:
                config.period = "B_";
                config.binning = m_binning;
                config.trigger = "HLT_mu26_ivarmedium_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2016C:
                config.period = "C_";
                config.binning = m_binning;
                config.trigger = "HLT_mu26_ivarmedium_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2016D1D3:
                config.period = "D1D3_";
                config.binning = m_binning;
                config.trigger = "HLT_mu26_ivarmedium_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2016D4D8:
                config.period = "D4D8_";
                config.binning = m_binning;
                config.trigger = "HLT_mu26_ivarmedium_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2016E:
                config.period = "E_";
                config.binning = m_binning;
                config.trigger = "HLT_mu26_ivarmedium_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2016F:
                config.period = "F_";
                config.binning = m_binning;
                config.trigger = "HLT_mu26_ivarmedium_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2016G:
                config.period = "G_";
                config.binning = m_binning;
                config.trigger = "HLT_mu26_ivarmedium_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2016I:
                config.period = "I_";
                config.binning = m_binning;
                config.trigger = "HLT_mu26_ivarmedium_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2016K:
                config.period = "K_";
                config.binning = m_binning;
                config.trigger = "HLT_mu26_ivarmedium_OR_HLT_mu50";
                break;

            case TrigMuonEff::per2016L:
                config.period = "L_";
                config.binning = m_binning;
                config.trigger = "HLT_mu26_ivarmedium_OR_HLT_mu50";
                break;

            default:
                ATH_MSG_ERROR("MuonTriggerScaleFactors::setConfiguration;Unknown data period " << config.runNumber << " requested");
                return false;
        }

        return true;
    }

    // ==================================================================================
    // ==
    // ==================================================================================
    // ==================================================================================
    // == MuonTriggerScaleFactors::getDileptonThresholds
    // ==================================================================================
    void MuonTriggerScaleFactors::getDileptonThresholds(DileptonThresholds& thresholds) {
        thresholds.mu6 = 7;
        thresholds.mu10 = 11;
        thresholds.mu14 = 15;
        thresholds.mu18 = 19;
        thresholds.mu20 = 21;
        thresholds.mu22 = 23;
        thresholds.mu24 = 25;
        thresholds.mu26 = 27;
        thresholds.mu8noL1 = 10;
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::getDataPeriod
    // ==================================================================================
    TrigMuonEff::DataPeriod MuonTriggerScaleFactors::getDataPeriod(int runNumber) {
        // 2015
        if (runNumber >= 266904 && runNumber <= 272531) return TrigMuonEff::per2015AC;
        if (runNumber >= 276073 && runNumber <= 276954) return TrigMuonEff::per2015D;
        if (runNumber >= 278727 && runNumber <= 279928) return TrigMuonEff::per2015E;
        if (runNumber >= 279932 && runNumber <= 280422) return TrigMuonEff::per2015F;
        if (runNumber >= 280423 && runNumber <= 281075) return TrigMuonEff::per2015G;
        if (runNumber >= 281130 && runNumber <= 281411) return TrigMuonEff::per2015H;
        if (runNumber >= 281662 && runNumber <= 282482) return TrigMuonEff::per2015I; // special ALFA run
        if (runNumber >= 282625 && runNumber <= 284484) return TrigMuonEff::per2015J;

        // 2016
        if (runNumber >= 296939 && runNumber <= 300287) return TrigMuonEff::per2016A;
        if (runNumber >= 300345 && runNumber <= 300908) return TrigMuonEff::per2016B;
        if (runNumber >= 301912 && runNumber <= 302393) return TrigMuonEff::per2016C;
        if (runNumber >= 302737 && runNumber <= 302872) return TrigMuonEff::per2016D1D3;
        if (runNumber >= 302919 && runNumber <= 303560) return TrigMuonEff::per2016D4D8;
        if (runNumber >= 303638 && runNumber <= 303892) return TrigMuonEff::per2016E;
        if (runNumber >= 303943 && runNumber <= 304494) return TrigMuonEff::per2016F;
        if (runNumber >= 305291 && runNumber <= 306714) return TrigMuonEff::per2016G;
        if (runNumber >= 307124 && runNumber <= 308084) return TrigMuonEff::per2016I;
        if (runNumber >= 309311 && runNumber <= 309759) return TrigMuonEff::per2016K;
        if (runNumber >= 310015 && runNumber <= 311481) return TrigMuonEff::per2016L;

        return TrigMuonEff::perUnDefined;
    }

    // ==================================================================================
    // == MuonTriggerScaleFactors::setStorage
    // ==================================================================================
    void MuonTriggerScaleFactors::setStorage(Storage& storage) const {
        static const std::string type_[] = { "data", "mc" };
        static const std::vector<std::string> type(type_, type_ + sizeof(type_) / sizeof(std::string));

        static const std::string region_[] = { "barrel", "endcap" };
        static const std::vector<std::string> region(region_, region_ + sizeof(region_) / sizeof(std::string));

        // Adding only quality and iso used in Init to reduce the memory usage of the tool
        static std::vector<std::string> quality;
        quality.clear();
        quality.push_back(m_muonquality);

        static std::vector<std::string> isolation;
        isolation.clear();
        const std::string my_isolation = (m_isolation.empty() ? "" : "_" + m_isolation);
        isolation.push_back(my_isolation);

        static const std::string bins_[] = { "coarse", "fine" };
        static const std::vector<std::string> bins(bins_, bins_ + sizeof(bins_) / sizeof(std::string));

        // 2015 - mc15a
        static const std::string trigger2015mc15a_[] = {

        "HLT_mu20_iloose_L1MU15", // AC
        "HLT_mu20_iloose_L1MU15", // D
        "HLT_mu20_iloose_L1MU15", // E
        "HLT_mu20_iloose_L1MU15", // F
        "HLT_mu20_iloose_L1MU15", // G
        "HLT_mu20_iloose_L1MU15", // H
        "HLT_mu20_iloose_L1MU15", // J

        "HLT_mu24_iloose_L1MU15", // AC
        "HLT_mu24_iloose_L1MU15", // D
        "HLT_mu24_iloose_L1MU15", // E
        "HLT_mu24_iloose_L1MU15", // F
        "HLT_mu24_iloose_L1MU15", // G
        "HLT_mu24_iloose_L1MU15", // H
        "HLT_mu24_iloose_L1MU15", // J

        "HLT_mu24_imedium", // AC
        "HLT_mu24_imedium", // D
        "HLT_mu24_imedium", // E
        "HLT_mu24_imedium", // F
        "HLT_mu24_imedium", // G
        "HLT_mu24_imedium", // H
        "HLT_mu24_imedium", // J

        "HLT_mu26_imedium", // AC
        "HLT_mu26_imedium", // D
        "HLT_mu26_imedium", // E
        "HLT_mu26_imedium", // F
        "HLT_mu26_imedium", // G
        "HLT_mu26_imedium", // H
        "HLT_mu26_imedium", // J

        "HLT_mu50", // AC
        "HLT_mu50", // D
        "HLT_mu50", // E
        "HLT_mu50", // F
        "HLT_mu50", // G
        "HLT_mu50", // H
        "HLT_mu50", // J

        "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", // AC
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", // D
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", // E
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", // F
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", // G
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", // H
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", // J

        "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", // AC
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", // D
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", // E
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", // F
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", // G
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", // H
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", // J

        "HLT_mu24_imedium_OR_HLT_mu50", // AC
        "HLT_mu24_imedium_OR_HLT_mu50", // D
        "HLT_mu24_imedium_OR_HLT_mu50", // E
        "HLT_mu24_imedium_OR_HLT_mu50", // F
        "HLT_mu24_imedium_OR_HLT_mu50", // G
        "HLT_mu24_imedium_OR_HLT_mu50", // H
        "HLT_mu24_imedium_OR_HLT_mu50", // J

        "HLT_mu26_imedium_OR_HLT_mu50", // AC
        "HLT_mu26_imedium_OR_HLT_mu50", // D
        "HLT_mu26_imedium_OR_HLT_mu50", // E
        "HLT_mu26_imedium_OR_HLT_mu50", // F
        "HLT_mu26_imedium_OR_HLT_mu50", // G
        "HLT_mu26_imedium_OR_HLT_mu50", // H
        "HLT_mu26_imedium_OR_HLT_mu50", // J

        "HLT_mu10", // AC
        "HLT_mu10", // D
        "HLT_mu10", // E
        "HLT_mu10", // F
        "HLT_mu10", // G
        "HLT_mu10", // H
        "HLT_mu10", // J

        "HLT_mu14", // AC
        "HLT_mu14", // D
        "HLT_mu14", // E
        "HLT_mu14", // F
        "HLT_mu14", // G
        "HLT_mu14", // H
        "HLT_mu14", // J

        "HLT_mu18", // AC
        "HLT_mu18", // D
        "HLT_mu18", // E
        "HLT_mu18", // F
        "HLT_mu18", // G
        "HLT_mu18", // H
        "HLT_mu18", // J

        "HLT_mu22", // AC
        "HLT_mu22", // D
        "HLT_mu22", // E
        "HLT_mu22", // F
        "HLT_mu22", // G
        "HLT_mu22", // H
        "HLT_mu22", // J

        "HLT_mu24", // AC
        "HLT_mu24", // D
        "HLT_mu24", // E
        "HLT_mu24", // F
        "HLT_mu24", // G
        "HLT_mu24", // H
        "HLT_mu24", // J

        "HLT_mu8noL1", // AC
        "HLT_mu8noL1", // D
        "HLT_mu8noL1", // E
        "HLT_mu8noL1", // F
        "HLT_mu8noL1", // G
        "HLT_mu8noL1", // H
        "HLT_mu8noL1" // J

        };

        // 2015 - mc15c
        static const std::string trigger2015mc15c_[] = {

        "HLT_mu20_iloose_L1MU15", // D
        "HLT_mu20_iloose_L1MU15", // E
        "HLT_mu20_iloose_L1MU15", // F
        "HLT_mu20_iloose_L1MU15", // G
        "HLT_mu20_iloose_L1MU15", // H
        "HLT_mu20_iloose_L1MU15", // J

        "HLT_mu24_iloose_L1MU15", // D
        "HLT_mu24_iloose_L1MU15", // E
        "HLT_mu24_iloose_L1MU15", // F
        "HLT_mu24_iloose_L1MU15", // G
        "HLT_mu24_iloose_L1MU15", // H
        "HLT_mu24_iloose_L1MU15", // J

        "HLT_mu24_imedium", // D
        "HLT_mu24_imedium", // E
        "HLT_mu24_imedium", // F
        "HLT_mu24_imedium", // G
        "HLT_mu24_imedium", // H
        "HLT_mu24_imedium", // J

        "HLT_mu26_imedium", // D
        "HLT_mu26_imedium", // E
        "HLT_mu26_imedium", // F
        "HLT_mu26_imedium", // G
        "HLT_mu26_imedium", // H
        "HLT_mu26_imedium", // J

        "HLT_mu40", // D
        "HLT_mu40", // E
        "HLT_mu40", // F
        "HLT_mu40", // G
        "HLT_mu40", // H
        "HLT_mu40", // J

        "HLT_mu50", // D
        "HLT_mu50", // E
        "HLT_mu50", // F
        "HLT_mu50", // G
        "HLT_mu50", // H
        "HLT_mu50", // J

        "HLT_mu20_iloose_L1MU15_OR_HLT_mu40", // D
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu40", // E
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu40", // F
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu40", // G
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu40", // H
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu40", // J

        "HLT_mu24_iloose_L1MU15_OR_HLT_mu40", // D
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu40", // E
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu40", // F
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu40", // G
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu40", // H
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu40", // J

        "HLT_mu24_imedium_OR_HLT_mu40", // D
        "HLT_mu24_imedium_OR_HLT_mu40", // E
        "HLT_mu24_imedium_OR_HLT_mu40", // F
        "HLT_mu24_imedium_OR_HLT_mu40", // G
        "HLT_mu24_imedium_OR_HLT_mu40", // H
        "HLT_mu24_imedium_OR_HLT_mu40", // J

        "HLT_mu26_imedium_OR_HLT_mu40", // D
        "HLT_mu26_imedium_OR_HLT_mu40", // E
        "HLT_mu26_imedium_OR_HLT_mu40", // F
        "HLT_mu26_imedium_OR_HLT_mu40", // G
        "HLT_mu26_imedium_OR_HLT_mu40", // H
        "HLT_mu26_imedium_OR_HLT_mu40", // J

        "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", // D
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", // E
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", // F
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", // G
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", // H
        "HLT_mu20_iloose_L1MU15_OR_HLT_mu50", // J

        "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", // D
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", // E
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", // F
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", // G
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", // H
        "HLT_mu24_iloose_L1MU15_OR_HLT_mu50", // J

        "HLT_mu24_imedium_OR_HLT_mu50", // D
        "HLT_mu24_imedium_OR_HLT_mu50", // E
        "HLT_mu24_imedium_OR_HLT_mu50", // F
        "HLT_mu24_imedium_OR_HLT_mu50", // G
        "HLT_mu24_imedium_OR_HLT_mu50", // H
        "HLT_mu24_imedium_OR_HLT_mu50", // J

        "HLT_mu26_imedium_OR_HLT_mu50", // D
        "HLT_mu26_imedium_OR_HLT_mu50", // E
        "HLT_mu26_imedium_OR_HLT_mu50", // F
        "HLT_mu26_imedium_OR_HLT_mu50", // G
        "HLT_mu26_imedium_OR_HLT_mu50", // H
        "HLT_mu26_imedium_OR_HLT_mu50", // J

        "HLT_mu6", // D
        "HLT_mu6", // E
        "HLT_mu6", // F
        "HLT_mu6", // G
        "HLT_mu6", // H
        "HLT_mu6", // J

        "HLT_mu10", // D
        "HLT_mu10", // E
        "HLT_mu10", // F
        "HLT_mu10", // G
        "HLT_mu10", // H
        "HLT_mu10", // J

        "HLT_mu14", // D
        "HLT_mu14", // E
        "HLT_mu14", // F
        "HLT_mu14", // G
        "HLT_mu14", // H
        "HLT_mu14", // J

        "HLT_mu18", // D
        "HLT_mu18", // E
        "HLT_mu18", // F
        "HLT_mu18", // G
        "HLT_mu18", // H
        "HLT_mu18", // J

        "HLT_mu22", // D
        "HLT_mu22", // E
        "HLT_mu22", // F
        "HLT_mu22", // G
        "HLT_mu22", // H
        "HLT_mu22", // J

        "HLT_mu24", // D
        "HLT_mu24", // E
        "HLT_mu24", // F
        "HLT_mu24", // G
        "HLT_mu24", // H
        "HLT_mu24", // J

        "HLT_mu8noL1", // D
        "HLT_mu8noL1", // E
        "HLT_mu8noL1", // F
        "HLT_mu8noL1", // G
        "HLT_mu8noL1", // H
        "HLT_mu8noL1" // J

        };

        // 2016 - mc15c
        static const std::string trigger2016mc15c_[] = {

        "HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15", // A

        "HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15_OR_HLT_mu40", // A

        "HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15_OR_HLT_mu50", // A

        "HLT_mu24_imedium", // A
        "HLT_mu24_imedium", // B
        "HLT_mu24_imedium", // C
        "HLT_mu24_imedium", // D1D3

        "HLT_mu24_imedium_OR_HLT_mu40", // A

        "HLT_mu24_imedium_OR_HLT_mu50", // A
        "HLT_mu24_imedium_OR_HLT_mu50", // B
        "HLT_mu24_imedium_OR_HLT_mu50", // C
        "HLT_mu24_imedium_OR_HLT_mu50", // D1D3

        "HLT_mu24_ivarmedium", // A
        "HLT_mu24_ivarmedium", // B
        "HLT_mu24_ivarmedium", // C
        "HLT_mu24_ivarmedium", // D1D3

        "HLT_mu24_ivarmedium_OR_HLT_mu40", // A

        "HLT_mu24_ivarmedium_OR_HLT_mu50", // A
        "HLT_mu24_ivarmedium_OR_HLT_mu50", // B
        "HLT_mu24_ivarmedium_OR_HLT_mu50", // C
        "HLT_mu24_ivarmedium_OR_HLT_mu50", // D1D3

        "HLT_mu26_imedium", // A
        "HLT_mu26_imedium", // B
        "HLT_mu26_imedium", // C
        "HLT_mu26_imedium", // D1D3
        "HLT_mu26_imedium", // D4D8
        "HLT_mu26_imedium", // E
        "HLT_mu26_imedium", // F

        "HLT_mu26_imedium_OR_HLT_mu40", // A

        "HLT_mu26_imedium_OR_HLT_mu50", // A
        "HLT_mu26_imedium_OR_HLT_mu50", // B
        "HLT_mu26_imedium_OR_HLT_mu50", // C
        "HLT_mu26_imedium_OR_HLT_mu50", // D1D3
        "HLT_mu26_imedium_OR_HLT_mu50", // D4D8
        "HLT_mu26_imedium_OR_HLT_mu50", // E
        "HLT_mu26_imedium_OR_HLT_mu50", // F

        "HLT_mu26_ivarmedium", // A
        "HLT_mu26_ivarmedium", // B
        "HLT_mu26_ivarmedium", // C
        "HLT_mu26_ivarmedium", // D1D3
        "HLT_mu26_ivarmedium", // D4D8
        "HLT_mu26_ivarmedium", // E
        "HLT_mu26_ivarmedium", // F
        "HLT_mu26_ivarmedium", // G
        "HLT_mu26_ivarmedium", // I
        "HLT_mu26_ivarmedium", // K
        "HLT_mu26_ivarmedium", // L

        "HLT_mu26_ivarmedium_OR_HLT_mu40", // A

        "HLT_mu26_ivarmedium_OR_HLT_mu50", // A
        "HLT_mu26_ivarmedium_OR_HLT_mu50", // B
        "HLT_mu26_ivarmedium_OR_HLT_mu50", // C
        "HLT_mu26_ivarmedium_OR_HLT_mu50", // D1D3
        "HLT_mu26_ivarmedium_OR_HLT_mu50", // D4D8
        "HLT_mu26_ivarmedium_OR_HLT_mu50", // E
        "HLT_mu26_ivarmedium_OR_HLT_mu50", // F
        "HLT_mu26_ivarmedium_OR_HLT_mu50", // G
        "HLT_mu26_ivarmedium_OR_HLT_mu50", // I
        "HLT_mu26_ivarmedium_OR_HLT_mu50", // K
        "HLT_mu26_ivarmedium_OR_HLT_mu50", // L

        "HLT_mu40", // A

        "HLT_mu50", // A
        "HLT_mu50", // B
        "HLT_mu50", // C
        "HLT_mu50", // D1D3
        "HLT_mu50", // D4D8
        "HLT_mu50", // E
        "HLT_mu50", // F
        "HLT_mu50", // G
        "HLT_mu50", // I
        "HLT_mu50", // K
        "HLT_mu50", // L

        "HLT_mu6", // A
        "HLT_mu6", // B
        "HLT_mu6", // C
        "HLT_mu6", // D1D3
        "HLT_mu6", // D4D8
        "HLT_mu6", // E
        "HLT_mu6", // F
        "HLT_mu6", // G
        "HLT_mu6", // I
        "HLT_mu6", // K
        "HLT_mu6", // L

        "HLT_mu10", // A

        "HLT_mu14", // A
        "HLT_mu14", // B
        "HLT_mu14", // C
        "HLT_mu14", // D1D3
        "HLT_mu14", // D4D8
        "HLT_mu14", // E
        "HLT_mu14", // F
        "HLT_mu14", // G
        "HLT_mu14", // I
        "HLT_mu14", // K
        "HLT_mu14", // L

        "HLT_mu20", // A
        "HLT_mu20", // B
        "HLT_mu20", // C
        "HLT_mu20", // D1D3

        "HLT_mu22", // A
        "HLT_mu22", // B
        "HLT_mu22", // C
        "HLT_mu22", // D1D3
        "HLT_mu22", // D4D8
        "HLT_mu22", // E
        "HLT_mu22", // F
        "HLT_mu22", // G
        "HLT_mu22", // I
        "HLT_mu22", // K
        "HLT_mu22", // L

        "HLT_mu24", // A
        "HLT_mu24", // B
        "HLT_mu24", // C
        "HLT_mu24", // D1D3
        "HLT_mu24", // D4D8
        "HLT_mu24", // E
        "HLT_mu24", // F
        "HLT_mu24", // G
        "HLT_mu24", // I
        "HLT_mu24", // K
        "HLT_mu24", // L

        "HLT_mu26", // A
        "HLT_mu26", // B
        "HLT_mu26", // C
        "HLT_mu26", // D1D3
        "HLT_mu26", // D4D8
        "HLT_mu26", // E
        "HLT_mu26", // F
        "HLT_mu26", // G
        "HLT_mu26", // I
        "HLT_mu26", // K
        "HLT_mu26", // L

        "HLT_mu8noL1", // A
        "HLT_mu8noL1", // B
        "HLT_mu8noL1", // C
        "HLT_mu8noL1", // D1D3
        "HLT_mu8noL1", // D4D8
        "HLT_mu8noL1", // E
        "HLT_mu8noL1", // F
        "HLT_mu8noL1", // G
        "HLT_mu8noL1", // I
        "HLT_mu8noL1", // K
        "HLT_mu8noL1" // L

        };

        static const std::vector<std::string> trigger2015mc15a(trigger2015mc15a_, trigger2015mc15a_ + sizeof(trigger2015mc15a_) / sizeof(std::string));
        static const std::vector<std::string> trigger2015mc15c(trigger2015mc15c_, trigger2015mc15c_ + sizeof(trigger2015mc15c_) / sizeof(std::string));
        static const std::vector<std::string> trigger2016mc15c(trigger2016mc15c_, trigger2016mc15c_ + sizeof(trigger2016mc15c_) / sizeof(std::string));

        static const std::string period2015mc15a_[] = { "AC", "D", "E", "F", "G", "H", "J", "AC", "D", "E", "F", "G", "H", "J", "AC", "D", "E", "F", "G", "H", "J", "AC", "D", "E", "F", "G", "H", "J", "AC", "D", "E", "F", "G", "H", "J", "AC", "D", "E", "F", "G", "H", "J", "AC", "D", "E", "F", "G", "H", "J", "AC", "D", "E", "F", "G", "H", "J", "AC", "D", "E", "F", "G", "H", "J", "AC", "D", "E", "F", "G", "H", "J", "AC", "D", "E", "F", "G", "H", "J", "AC", "D", "E", "F", "G", "H", "J", "AC", "D", "E", "F", "G", "H", "J", "AC", "D", "E", "F", "G", "H", "J", "AC", "D", "E", "F", "G", "H", "J" };

        static const std::string period2015mc15c_[] = { "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J", "D", "E", "F", "G", "H", "J" };

        static const std::string period2016mc15c_[] = { "A", "A", "A", "A", "B", "C", "D1D3", "A", "A", "B", "C", "D1D3", "A", "B", "C", "D1D3", "A", "A", "B", "C", "D1D3", "A", "B", "C", "D1D3", "D4D8", "E", "F", "A", "A", "B", "C", "D1D3", "D4D8", "E", "F", "A", "B", "C", "D1D3", "D4D8", "E", "F", "G", "I", "K", "L", "A", "A", "B", "C", "D1D3", "D4D8", "E", "F", "G", "I", "K", "L", "A", "A", "B", "C", "D1D3", "D4D8", "E", "F", "G", "I", "K", "L", "A", "B", "C", "D1D3", "D4D8", "E", "F", "G", "I", "K", "L", "A", "A", "B", "C", "D1D3", "D4D8", "E", "F", "G", "I", "K", "L", "A", "B", "C", "D1D3", "A", "B", "C", "D1D3", "D4D8", "E", "F", "G", "I", "K", "L", "A", "B", "C", "D1D3", "D4D8", "E", "F", "G", "I", "K", "L", "A", "B", "C", "D1D3", "D4D8", "E", "F", "G", "I", "K", "L", "A", "B", "C", "D1D3", "D4D8", "E", "F", "G", "I", "K", "L" };

        static const std::vector<std::string> period2015mc15a(period2015mc15a_, period2015mc15a_ + sizeof(period2015mc15a_) / sizeof(std::string));
        static const std::vector<std::string> period2015mc15c(period2015mc15c_, period2015mc15c_ + sizeof(period2015mc15c_) / sizeof(std::string));
        static const std::vector<std::string> period2016mc15c(period2016mc15c_, period2016mc15c_ + sizeof(period2016mc15c_) / sizeof(std::string));

        static const std::string systematic_[] = { "nominal", "stat_up", "stat_down", "syst_up", "syst_down" };

        static const std::vector<std::string> systematic(systematic_, systematic_ + sizeof(systematic_) / sizeof(std::string));

        storage.type = &type;
        storage.region = &region;
        storage.systematic = &systematic;

        if (m_year == "2015" && m_mc == "mc15a") {
            storage.trigger = &trigger2015mc15a;
            storage.period = &period2015mc15a;
        } else if (m_year == "2015" && m_mc == "mc15c") {
            storage.trigger = &trigger2015mc15c;
            storage.period = &period2015mc15c;
        } else {
            storage.trigger = &trigger2016mc15c;
            storage.period = &period2016mc15c;
        }

        storage.quality = &quality;
        storage.isolation = &isolation;
        storage.bins = &bins;

        if (storage.period->size() != storage.trigger->size()) {
            ATH_MSG_FATAL("MuonTriggerScaleFactors::setStorage : size of period and trigger is different : check configuration [#periods=" << storage.period->size() << ",#triggers=" << storage.trigger->size() << "]");
        }

        return;
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
