/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * EfficiencyScaleFactor.h
 *
 *  Created on: Apr 10, 2014
 *      Author: goblirsc
 */

#ifndef EFFICIENCYSCALEFACTOR_H_
#define EFFICIENCYSCALEFACTOR_H_

// EDM include(s):
#include "xAODMuon/Muon.h"
// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"

#include "PATInterfaces/CorrectionCode.h"
#include "MuonEfficiencyCorrections/HistHandler.h"
#include "PATInterfaces/ISystematicsTool.h"

// further ROOT includes
#include <TFile.h>
#include <TDirectory.h>

// STL includes
#include <string>
#include <iostream>
#include <exception>
#include <map>
#include <cmath>

namespace CP {
    class SystematicSet;
    class EfficiencyScaleFactor {

            /// @class EfficiencyScaleFactor
            /// @brief Utility class to manage scale factor histograms
            /// Simple container class for scale factor information
            /// Contains histograms for Efficiency, Scale Factor and Systematic errors
            /// Handles loading from files and manages the histos
            /// The user should not have to interact with this directly!

        public:

            /// default constructor - will create the histos as NULL pointers
            EfficiencyScaleFactor();
            /// constructor to use in real life - will read in the histos from the given file
            EfficiencyScaleFactor(std::string file, std::string time_unit, SystematicSet sys, std::string effType = "EFF", bool is_lowpt = false, bool hasPtDepSys = false);

            EfficiencyScaleFactor(const EfficiencyScaleFactor & other);
            EfficiencyScaleFactor & operator =(const EfficiencyScaleFactor & other);

            /// Read SF histrograms from a given input file.
            bool ReadFromFile(std::string file, std::string time_unit);
            HistHandler* ReadHistFromFile(std::string name, TFile* f, std::string time_unit);

            // mangle the histograms as required by the systematic being run
            void ApplySysVariation();

            /// check the consistency - all histograms should be well defined
            bool CheckConsistency();

            /// the important bits - extract SF info

            /// scale factors...
            CorrectionCode ScaleFactor(const xAOD::Muon& mu, float & SF);
            CorrectionCode ScaleFactorReplicas(const xAOD::Muon& mu, std::vector<float> & SF);

            /// ... and absolute efficiencies
            CorrectionCode DataEfficiency(const xAOD::Muon& mu, float & Eff);
            CorrectionCode DataEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & eff);

            CorrectionCode MCEfficiency(const xAOD::Muon& mu, float & Eff);
            CorrectionCode MCEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & eff);

            /// debug method
            void DebugPrint();

            virtual ~EfficiencyScaleFactor();

            // a string name assigned to each sys variation
            std::string sysname();

            // directly interact with a histo
            HistHandler* get_sf() {
                return m_sf;
            }
            HistHandler* get_eff() {
                return m_eff;
            }
            HistHandler* get_sf_sys() {
                return m_sf_sys;
            }
            HistHandler* get_eff_sys() {
                return m_eff_sys;
            }
            HistHandler* get_sf_PtDepsys() {
                return m_sf_PtDepsys;
            }
            HistHandler* get_eff_PtDepsys() {
                return m_eff_PtDepsys;
            }
            HistHandler* get_mc_eff() {
                return m_mc_eff;
            }
            HistHandler* get_mc_eff_sys() {
                return m_mc_eff_sys;
            }
            HistHandler* get_mc_eff_PtDepsys() {
                return m_mc_eff_PtDepsys;
            }
        private:

            // use some maps for easy histo loading / arithmetics by name

            typedef std::vector<HistHandler*> SFvec;
            typedef std::vector<HistHandler*>::iterator iSFvec;
            typedef std::vector<HistHandler*>::const_iterator ciSFvec;

            /// read the content of the correct bin in one of my histos
            CorrectionCode GetContentFromHist(HistHandler* Hist, HistHandler* PtDepHist, const xAOD::Muon& mu, float & SF);
            /// read a vector of replica contents in the correct bin in one of my histos
            CorrectionCode GetContentReplicasFromHist(EfficiencyScaleFactor::SFvec &replicas, const xAOD::Muon& mu, std::vector<float> & SF);

            // adds weight*staterr to each bin (used for systematics)
            void AddStatErrors(float weight);
            void AddStatErrors_histo(HistHandler* h, float weight);
            // adds weight*syserr to each bin (used for systematics)
            void AddSysErrors(float weight);
            void AddSysErrors_histo(HistHandler* h, HistHandler* hsys, float weight);
            void AddSysErrors_vector(EfficiencyScaleFactor::SFvec &Vec, HistHandler* hsys, float weight);
            // package a TH1 in a HistHandler
            HistHandler *package_histo(TH1* h);

            /// two more auxiliary methods that handle adding and scaling for
            /// any histo type, including the dreadful TH2Poly...
            void AddHistos(HistHandler* & add_to, HistHandler* add_this, float weight);
            void ScaleHisto(HistHandler* & h, float weight);

            // replica generation
            void GenerateReplicas(int nrep, int seed);
            void GenerateReplicasFromHist(HistHandler* h, int nrep, int seed, EfficiencyScaleFactor::SFvec &repVector);

            //Some functions to clean up and copy the data
            void Clear();
            void DeleteOldReplicas(EfficiencyScaleFactor::SFvec &Vec, bool ClearVec = false);
            void CopyContent(const EfficiencyScaleFactor &other);
            void CopyHistHandler(HistHandler* &own, const HistHandler* other);
            void CopyReplicaVec(EfficiencyScaleFactor::SFvec &own, const EfficiencyScaleFactor::SFvec &other);
            /// the histograms needed to run
            HistHandler* m_sf;
            HistHandler* m_eff;
            HistHandler* m_mc_eff;
            HistHandler* m_sf_sys;
            HistHandler* m_eff_sys;
            HistHandler* m_mc_eff_sys;
            // these are for the continuous pt dependent systematic if we have one
            HistHandler* m_sf_PtDepsys;
            HistHandler* m_eff_PtDepsys;
            HistHandler* m_mc_eff_PtDepsys;

            // replicas, in case we use them
            SFvec m_sf_replicas;
            SFvec m_eff_replicas;
            SFvec m_mc_eff_replicas;

            // utility class to work with a fine eta phi binning
            fineEtaPhiBinning m_etaphi;

            // the systematic variation associated with this instance
            SystematicSet m_sys;

            std::string m_effType;

            // states that this SF should respond to low pt systematics rather than high pt ones
            bool m_is_lowpt;
            // steers the pt dependent systematics
            bool m_respond_to_PtDepsys;
            float m_PtDepsys_weight;
            // default efficiency value (in case of OutOfValidityRange)
            float m_default_eff;

    };
} /* namespace CP */

#endif /* EFFICIENCYSCALEFACTOR_H_ */
