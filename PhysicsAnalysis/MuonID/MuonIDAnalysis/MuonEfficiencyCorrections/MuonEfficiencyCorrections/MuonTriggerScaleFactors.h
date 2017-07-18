/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

/*
 *  MuonTriggerScaleFactors.h
 *
 *  Created on: Oct 22, 2014
 *      Author: Kota Kasahara <kota.kasahara@cern.ch>
 *
 *  Updates for 2016: Jun 20, 2016
 *      Author: Lidia Dell'Asta <dellasta@cern.ch> 
 */

#ifndef MUONTRIGGERSCALEFACTORS_H_
#define MUONTRIGGERSCALEFACTORS_H_

#include "MuonEfficiencyCorrections/IMuonTriggerScaleFactors.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicRegistry.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>

// ROOT classes we need
#include "TTree.h"
#include "TLorentzVector.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TDirectory.h"
#include "TH2.h"
class TLorentzVector;
class TH2;
class TFile;
class TDirectory;

#include <boost/unordered_map.hpp>

namespace CP {

    class MuonTriggerScaleFactors: public CP::IMuonTriggerScaleFactors, public asg::AsgTool {
            ASG_TOOL_CLASS( MuonTriggerScaleFactors, CP::IMuonTriggerScaleFactors )

            public:

            struct Storage {
                const std::vector<std::string> *type;
                const std::vector<std::string> *region;
                const std::vector<std::string> *quality;
                const std::vector<std::string> *isolation;
                const std::vector<std::string> *bins;
                const std::vector<std::string> *trigger;
                const std::vector<std::string> *period;
                const std::vector<std::string> *systematic;

                Storage(const std::vector<std::string> *type_=0,
                                const std::vector<std::string> *region_=0,
                                const std::vector<std::string> *quality_=0,
                                const std::vector<std::string> *isolation_=0,
                                const std::vector<std::string> *bins_=0,
                                const std::vector<std::string> *trigger_=0,
                                const std::vector<std::string> *period_=0,
                                const std::vector<std::string> *systematic_=0) :
                type(type_),
                region(region_),
                quality(quality_),
                isolation(isolation_),
                bins(bins_),
                trigger(trigger_),
                period(period_),
                systematic(systematic_) {}
            };

            MuonTriggerScaleFactors(const std::string& name);

            virtual ~MuonTriggerScaleFactors();

            virtual StatusCode initialize(void);

            virtual CorrectionCode setRunNumber(Int_t runNumber);

            // for single lepton triggers
            virtual CorrectionCode getTriggerScaleFactor(const xAOD::MuonContainer& mucont,
                            Double_t& triggersf,
                            const std::string& trigger);

            virtual CorrectionCode getTriggerEfficiency(const xAOD::Muon& mu,
                            Double_t& efficiency,
                            const std::string& trigger,
                            Bool_t dataType);

            virtual bool isAffectedBySystematic(const CP::SystematicVariation& systematic) const;

            virtual CP::SystematicSet affectingSystematics() const;

            virtual CP::SystematicSet recommendedSystematics() const;

            virtual CP::SystematicCode applySystematicVariation(const CP::SystematicSet& systConfig);

            virtual bool maskFeetRegions(const xAOD::Muon& mu, double dR);

            virtual bool inFeetRegion(unsigned int SectorAddress, unsigned int RoI);

            virtual double dR(const double eta1, const double phi1, const double eta2, const double phi2);

            //
            virtual int getReplica_index(std::string sysBaseName, const std::string trigStr);
            private:

            virtual CorrectionCode getMuonEfficiency(Double_t& eff,
                            const TrigMuonEff::Configuration& configuration,
                            const xAOD::Muon& muon,
                            const std::string& trigger,
                            const std::string& systematic);

            virtual CorrectionCode GetTriggerSF_dimu(Double_t& TriggerSF,
                            TrigMuonEff::Configuration& configuration,
                            const xAOD::MuonContainer& mucont,
                            const std::string& trigger);

            virtual CorrectionCode GetTriggerSF(Double_t& TriggerSF,
                            TrigMuonEff::Configuration& configuration,
                            const xAOD::MuonContainer& mucont,
                            const std::string& trigger);

            virtual CorrectionCode getSingleOrDimuonEfficiency(Double_t& eff,
                            const TrigMuonEff::Configuration& config,
                            const xAOD::MuonContainer& mucont,
                            const std::string& chain,
                            const std::string& systematic);

            virtual CorrectionCode getDimuonEfficiency(Double_t& eff,
                            const TrigMuonEff::Configuration& configuration,
                            const xAOD::MuonContainer& mucont,
                            const std::string& chain,
                            const std::string& systematic);

            virtual CorrectionCode setMaxPeriod(const TrigMuonEff::DataPeriod x);

            const CP::SystematicSet& appliedSystematics() const {
                return *m_appliedSystematics;
            }

            CP::SystematicCode registerSystematics();

            boost::unordered_map<CP::SystematicSet, CP::SystematicSet> m_systFilter;

            CP::SystematicSet* m_appliedSystematics;

            std::string m_fileName;
            Int_t m_runNumber;
            const char* m_classname;

            typedef std::map<std::string, TH2*> EfficiencyMap;
            EfficiencyMap m_efficiencyMap;
            typedef std::pair<std::string, TH2*> EfficiencyPair;
            mutable std::map<int, std::string> m_fileNameMap;

            std::map<std::string, std::vector<TH2*> > m_efficiencyMapReplicaArray;

            Double_t getThresholds(const std::string& trigger);

            bool setConfiguration(TrigMuonEff::Configuration& config,
                            TrigMuonEff::DataPeriod period = TrigMuonEff::period_undefined) const;

            struct DileptonThresholds {
                    double mu6;
                    double mu10;
                    double mu14;
                    double mu18;
                    double mu20;
                    double mu22;
                    double mu24;
                    double mu26;
                    double mu8noL1;
                    DileptonThresholds(const double mu6_ = 7,
                                    const double mu10_ = 11,
                                    const double mu14_ = 15,
                                    const double mu18_ = 19,
                                    const double mu20_ = 21,
                                    const double mu22_ = 23,
                                    const double mu24_ = 25,
                                    const double mu26_ = 27,
                                    const double mu8noL1_ = 10) :
                                    mu6(mu6_),
                                    mu10(mu10_),
                                    mu14(mu14_),
                                    mu18(mu18_),
                                    mu20(mu20_),
                                    mu22(mu22_),
                                    mu24(mu24_),
                                    mu26(mu26_),
                                    mu8noL1(mu8noL1_) {
                    }
            };

            static void getDileptonThresholds(DileptonThresholds& thresholds);

            struct DileptonTrigger {
                    std::string leg1;
                    std::string leg2;
                    std::string bothLegs;
                    DileptonTrigger(const std::string& leg1_ = std::string(),
                                    const std::string& leg2_ = std::string(),
                                    const std::string& bothLegs_ = std::string()) :
                                    leg1(leg1_),
                                    leg2(leg2_),
                                    bothLegs(bothLegs_) {
                    }
            };

            static void getDileptonLegs(const std::string& chain,
                            DileptonTrigger& legs);

            static TrigMuonEff::DataPeriod getDataPeriod(int runNumber);

            void setStorage(Storage& storage) const;

            TDirectory* getTemporaryDirectory(void) const;

            TrigMuonEff::DataPeriod m_max_period;

            std::string m_muonquality;

            std::string m_year;
            std::string m_mc;

            // subfolder to load from the calibration db
            std::string m_calibration_version;
            std::string m_custom_dir;
            std::string m_isolation;
            std::string m_binning;

            bool m_allowZeroSF;
            //Variables for toy replicas setup
            std::vector<std::string> m_replicaTriggerList;
            std::set<std::string> m_replicaSet; //set of triggers for replicas, for fast searching
            int m_nReplicas;
            int m_ReplicaRandomSeed = 1234;

            //Generate replicas of h for Toys with each bin of h varied with Gaussian distribution
            //with mean from bin content and sigma from bin error
            std::vector<TH2*> generateReplicas(TH2* h, int nrep, int seed);

    };

}

#endif
