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
#include "TKey.h"
class TLorentzVector;
class TH2;
class TFile;
class TDirectory;

#include <boost/unordered_map.hpp>

namespace CP {

    class MuonTriggerScaleFactors: public CP::IMuonTriggerScaleFactors, public asg::AsgTool {
            ASG_TOOL_CLASS( MuonTriggerScaleFactors, CP::IMuonTriggerScaleFactors )

            public:

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

            virtual CorrectionCode getDimuonEfficiency(Double_t& eff,
                            const TrigMuonEff::Configuration& configuration,
                            const xAOD::MuonContainer& mucont,
                            const std::string& chain,
                            const std::string& systematic);

            const CP::SystematicSet& appliedSystematics() const {
                return *m_appliedSystematics;
            }

            CP::SystematicCode registerSystematics();

            boost::unordered_map<CP::SystematicSet, CP::SystematicSet> m_systFilter;

            CP::SystematicSet* m_appliedSystematics;

            std::string m_fileName;
            std::string m_dataPeriod;
            const char* m_classname;

            typedef std::map<std::string, TH2*> EfficiencyMap;
            EfficiencyMap m_efficiencyMap;
            typedef std::pair<std::string, TH2*> EfficiencyPair;
            mutable std::map<int, std::string> m_fileNameMap;

            std::map<std::string, std::vector<TH2*> > m_efficiencyMapReplicaArray;

            CorrectionCode getThreshold(Int_t& threshold, const std::string& trigger);

            static std::string getTriggerCorrespondingToDimuonTrigger(const std::string& trigger);

            static std::string getDataPeriod(int runNumber, const std::string& year);

            TDirectory* getTemporaryDirectory(void) const;

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
