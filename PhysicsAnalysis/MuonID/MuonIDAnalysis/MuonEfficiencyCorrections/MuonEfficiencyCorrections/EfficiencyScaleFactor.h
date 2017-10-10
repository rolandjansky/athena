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

#include <PATInterfaces/CorrectionCode.h>
#include <MuonEfficiencyCorrections/HistHandler.h>
#include <MuonEfficiencyCorrections/MuonEfficiencyType.h>
#include <MuonEfficiencyCorrections/KinematicSystHandler.h>
#include <PATInterfaces/ISystematicsTool.h>

// further ROOT includes
#include <TFile.h>
#include <TDirectory.h>

// STL includes
#include <string>
#include <iostream>
#include <exception>
#include <map>
#include <cmath>
#include <memory>

namespace CP {

    typedef std::shared_ptr<IKinematicSystHandler> IKinematicSystHandler_Ptr;



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
            EfficiencyScaleFactor(const  std::string& toolname, const std::string &file, const std::string &time_unit, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, bool is_lowpt = false, bool hasPtDepSys = false);
            //Constructor to connect the EfficiencyScaleFactor with the Nominal Version
            EfficiencyScaleFactor(EfficiencyScaleFactor* Nominal, const std::string& toolname, const std::string &file, const std::string &time_unit, MuonEfficiencySystType sysType, CP::MuonEfficiencyType effType, bool is_lowpt = false, bool hasPtDepSys = false);

            EfficiencyScaleFactor(const EfficiencyScaleFactor & other);
            EfficiencyScaleFactor & operator =(const EfficiencyScaleFactor & other);

            /// Read SF histrograms from a given input file.
            bool ReadFromFile(const std::string &file, const std::string& time_unit);
            HistHandler_Ptr ReadHistFromFile(const std::string& name, TFile* f, const std::string& time_unit);

            // mangle the histograms as required by the systematic being run
            void ApplySysVariation();

            /// check the consistency - all histograms should be well defined
            bool CheckConsistency() const;
            // Get the number of the bins
            int nBinsSF() const;

            int nBinsEfficiency() const;


            std::string GetBinName(int bin) const;
            int FindBinSF(const xAOD::Muon & mu) const;

            //The function letting the world implode... Fixes the bin to which the systematic will be applied
            bool SetSystematicBin(int bin);

            /// the important bits - extract SF info

            /// scale factors...
            CorrectionCode ScaleFactor(const xAOD::Muon& mu, float & SF) const;
            CorrectionCode ScaleFactorReplicas(const xAOD::Muon& mu, std::vector<float> & SF);

            /// ... and absolute efficiencies
            CorrectionCode DataEfficiency(const xAOD::Muon& mu, float & Eff) const;
            CorrectionCode DataEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & eff);

            CorrectionCode MCEfficiency(const xAOD::Muon& mu, float & Eff) const;
            CorrectionCode MCEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & eff);

            /// debug method
            void DebugPrint();

            virtual ~EfficiencyScaleFactor();

            // a string name assigned to each sys variation
            std::string sysname() const;
            std::string toolname() const;

        private:
            // use some maps for easy histo loading / arithmetics by name


            typedef std::vector<HistHandler_Ptr> SFvec;
            typedef std::vector<HistHandler_Ptr>::iterator iSFvec;
            typedef std::vector<HistHandler_Ptr>::const_iterator ciSFvec;

            /// read the content of the correct bin in one of my histos. MCefficiencies actually do  not need a pt-dependet systematic
            CorrectionCode GetContentFromHist(HistHandler_Ptr Hist, IKinematicSystHandler_Ptr PtDepHist, const xAOD::Muon& mu, float & SF, bool PtDepHistNeeded) const;
            /// read a vector of replica contents in the correct bin in one of my histos
            CorrectionCode GetContentReplicasFromHist(EfficiencyScaleFactor::SFvec &replicas, const xAOD::Muon& mu, std::vector<float> & SF);

            // adds weight*staterr to each bin (used for systematics)
            void AddStatErrors(float weight);
            void AddStatErrors_histo(HistHandler_Ptr h, float weight);
            // adds weight*syserr to each bin (used for systematics)
            void AddSysErrors(float weight);
            void AddSysErrors_histo(HistHandler_Ptr h, HistHandler_Ptr hsys, float weight);
            void AddSysErrors_vector(EfficiencyScaleFactor::SFvec &Vec, HistHandler_Ptr hsys, float weight);
            // package a TH1 in a HistHandler
            HistHandler_Ptr package_histo(TH1* h);

            /// two more auxiliary methods that handle adding and scaling for
            /// any histo type, including the dreadful TH2Poly...
            void AddHistos(HistHandler_Ptr & add_to, HistHandler_Ptr add_this, float weight);
            void ScaleHisto(HistHandler_Ptr & h, float weight);

            // replica generation
            void GenerateReplicas(int nrep, int seed);
            void GenerateReplicasFromHist(HistHandler_Ptr h, int nrep, int seed, EfficiencyScaleFactor::SFvec &repVector);

            //Some functions to clean up and copy the data
            void Clear();
            void DeleteOldReplicas(EfficiencyScaleFactor::SFvec &Vec, bool ClearVec = false);
            void CopyContent(const EfficiencyScaleFactor &other);
            void CopyHistHandler(HistHandler_Ptr &own, const HistHandler_Ptr other);
            void CopyReplicaVec(EfficiencyScaleFactor::SFvec &own, const EfficiencyScaleFactor::SFvec &other);

            // Some unique toolname in order not to clash with ROOT memory management
            std::string m_toolname;
            /// the histograms needed to run
            HistHandler_Ptr m_sf;
            HistHandler_Ptr m_eff;
            HistHandler_Ptr m_mc_eff;
            HistHandler_Ptr m_sf_sys;
            HistHandler_Ptr m_eff_sys;
            HistHandler_Ptr m_mc_eff_sys;
            // these are for the continuous pt dependent systematic if we have one
            IKinematicSystHandler_Ptr m_sf_KineDepsys;
            IKinematicSystHandler_Ptr m_eff_KineDepsys;

            // replicas, in case we use them
            SFvec m_sf_replicas;
            SFvec m_eff_replicas;
            SFvec m_mc_eff_replicas;

            // the systematic variation type associated with this instance
            MuonEfficiencySystType m_sysType;

            // states that this SF should respond to low pt systematics rather than high pt ones
            bool m_is_lowpt;
            // steers the pt dependent systematics
            bool m_respond_to_kineDepSyst;
            // default efficiency value (in case of OutOfValidityRange)
            float m_default_eff;
            // default TTVA efficiency value (in case of MuonStandAlone for |eta|>2.5)
            float m_default_eff_ttva;
            CP::MuonEfficiencyType m_Type;

            EfficiencyScaleFactor* m_NominalFallBack;
            int m_SystematicBin;

    };
} /* namespace CP */

#endif /* EFFICIENCYSCALEFACTOR_H_ */
