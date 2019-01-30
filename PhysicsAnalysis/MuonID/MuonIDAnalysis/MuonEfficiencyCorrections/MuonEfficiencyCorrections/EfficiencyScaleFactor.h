/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
#ifndef  MUONEFFICIENCYCORRECITONS_EFFICIENCYSCALEFACTOR_H_
#define MUONEFFICIENCYCORRECITONS_EFFICIENCYSCALEFACTOR_H_

// EDM include(s):
#include "xAODMuon/Muon.h"
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
    
    class SystematicSet;
    class MuonEfficiencyScaleFactors;
    class EfficiencyScaleFactor {

            /// @class EfficiencyScaleFactor
            /// @brief Utility class to manage scale factor histograms
            /// Simple container class for scale factor information
            /// Contains histograms for Efficiency, Scale Factor and Systematic errors
            /// Handles loading from files and manages the histos
            /// The user should not have to interact with this directly!
        public:
            /// Nominal constructor to use having the measurement (Reco/TTVA/Iso/BadMuon)
            /// path to the scale-factor map & the time unit to consider
            EfficiencyScaleFactor(const MuonEfficiencyScaleFactors& ref_tool,
                                  const std::string &file, 
                                  const std::string &time_unit);
           
            /// Constructor to use for systematic evaluations the nominal scale-factor
            /// is given as a fallback map in case the user want to activate uncorrelated
            /// systematic -- a shared ptr is used to ensure that the map is always defined
            /// allthough the nominal container has been deleted somehow -- the name of the
            /// systematic encodes the systematic to retrieve from the file. To ensure backwards
            /// compability with older files the names "STAT" and "SYS" refer to the total systematic
            EfficiencyScaleFactor(std::shared_ptr<EfficiencyScaleFactor> nominal,
                                  const MuonEfficiencyScaleFactors& ref_tool,
                                  const std::string& file,
                                  const std::string& time_unit,
                                  const std::string& syst_name,
                                  int syst_type_bitmap);
            /// Do not allow for copy constructors and other assignments of the class
            EfficiencyScaleFactor(const EfficiencyScaleFactor&) =delete;
            EfficiencyScaleFactor& operator =(const EfficiencyScaleFactor&) =delete;
        
            
            /// check the consistency of the scale-factor map. Needed histograms should be defined
            /// and the nominal fall back map is assigned properly as well as the map has a valid run number range
            bool CheckConsistency();
            
            /// How many bins does the scale-factor map have. Only the bins of the primary map are counted.
            /// If the map has an extra kinematic systematic on top, these bins cannot be resolved at the moment
            int nBins() const;

            /// Returns the name of the bin  using the axis title from the scale-factor map
            std::string GetBinName(int bin) const;
            
            /// Finds the bin to which the muon corresponds to
            int FindBinSF(const xAOD::Muon & mu) const;

            /// This function will let the world implode since it allows to decorrelate the sysstematics
            /// bin by bin
            bool SetSystematicBin(int bin);
            
            bool SeperateSystBins() const;
            bool IsUpVariation() const;
            

            /// the important bits - extract SF info

            /// scale factors...
            CorrectionCode ScaleFactor(const xAOD::Muon& mu, float & SF) const;
            CorrectionCode ScaleFactorReplicas(const xAOD::Muon& mu, std::vector<float> & SF);
            
            /// or you can just decorate the scale-factor to the muon
            CorrectionCode ApplyScaleFactor(const xAOD::Muon& mu) const;
            CorrectionCode ApplyScaleFactorReplicas(const xAOD::Muon& mu, int n_replicas);
            
            /// ... and absolute efficiencies
            CorrectionCode DataEfficiency(const xAOD::Muon& mu, float & Eff) const;
            CorrectionCode DataEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & eff);
            /// You can decorate the data efficiecny as well
            CorrectionCode ApplyDataEfficiency(const xAOD::Muon& mu) const;
            CorrectionCode ApplyDataEfficiencyReplicas(const xAOD::Muon& mu, int n_replicas);
            
            /// ... mc efficiency 
            CorrectionCode MCEfficiency(const xAOD::Muon& mu, float & Eff) const;
            CorrectionCode MCEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & eff);
            /// ... or decorate them directly
            CorrectionCode ApplyMCEfficiency(const xAOD::Muon& mu) const;
            CorrectionCode ApplyMCEfficiencyReplicas(const xAOD::Muon& mu, int n_replicas);
            
            /// debug method
            void DebugPrint() const;

          
            /// A string having the full name of the systematic including the  measurement. If the
            /// systematic string is empty then an empty string is returned. The flag controls whether
            /// the suffix __1UP or __1DN is appended after the string.
            std::string sysname(bool with_direction = true) const;
           
            /// First run to which the map belongs to. If the measurement is not carried out in time
            /// then it's usally 1
            unsigned int firstRun() const;
            /// Last run to which the map belongs to. The last run number is given exclusvely...
            unsigned int lastRun() const;
            
            /// Check if a run of data-taking is covered by this map
            bool coversRunNumber(unsigned int run) const;
          
            /// Set the run-number coverage
            void setFirstLastRun(unsigned int first, unsigned int last);
        private:
            
            typedef std::vector<std::unique_ptr<HistHandler>> SFReplicaVec;
           
            /// Read SF histrograms from a given input file.
            bool ReadFromFile(const std::string &file, const std::string& time_unit);
            std::unique_ptr<HistHandler> ReadHistFromFile(const std::string& name, TFile* f, const std::string& time_unit);
            
            // use some maps for easy histo loading / arithmetics by name
            
            /// read the content of the correct bin in one of my histos. MCefficiencies actually do  not need a pt-dependet systematic
            CorrectionCode GetContentFromHist(HistHandler* Hist, const xAOD::Muon& mu, float & SF, bool add_kine_syst) const;
            /// read a vector of replica contents in the correct bin in one of my histos
            CorrectionCode GetContentReplicasFromHist(EfficiencyScaleFactor::SFReplicaVec &replicas, const xAOD::Muon& mu, std::vector<float> & SF, bool add_kine_syst);

            
            // package a TH1 in a HistHandler
            std::unique_ptr<HistHandler> package_histo(TH1* h);

            
            // replica generation
            void GenerateReplicas(int nrep, int seed);
            void GenerateReplicasFromHist(HistHandler* h, int nrep, int seed, EfficiencyScaleFactor::SFReplicaVec &repVector);
  
            ///
            ///   Section of variables of the tool
            ///
            CP::MuonEfficiencyType m_measurement;
            std::string m_syst_name;
            
            /// Boolean whether the scale-factor is varying upwards or downwards
            /// Only relevant if the systname is not empty
            bool m_is_up;
            /// states that this SF should respond to low pt systematics rather than high pt ones
            bool m_is_lowpt;
            /// steers the pt dependent systematics
            bool m_respond_to_kineDepSyst;
            /// Can the systematic be decorrelated
            bool m_seperateBinSyst;
            /// the histograms needed to run
            std::unique_ptr<HistHandler> m_sf;
            std::unique_ptr<HistHandler> m_eff;
            std::unique_ptr<HistHandler> m_mc_eff;

            /// Typedef to make the definition of the follow decorators a bit shorted
            typedef SG::AuxElement::Decorator<float> FloatDecorator;
            typedef SG::AuxElement::Decorator<std::vector<float>> FloatVectorDecorator;
            
            std::unique_ptr<FloatDecorator> m_sf_decor;
            std::unique_ptr<FloatDecorator> m_eff_decor;           
            std::unique_ptr<FloatDecorator> m_mc_eff_decor;
            
            std::unique_ptr<FloatVectorDecorator> m_sf_rep_decor;
            std::unique_ptr<FloatVectorDecorator> m_eff_rep_decor;
            std::unique_ptr<FloatVectorDecorator> m_mc_eff_rep_decor;

            
            // these are for the continuous pt dependent systematic if we have one
            std::unique_ptr<IKinematicSystHandler> m_sf_KineDepsys;
   
            
            /// replicas, in case we use them
            SFReplicaVec m_sf_replicas;
            SFReplicaVec m_eff_replicas;
            SFReplicaVec m_mc_eff_replicas;

            /// Nominal fall-back scale-factor
            std::shared_ptr<EfficiencyScaleFactor> m_NominalFallBack;
            int m_SystematicBin;
            
            
            /// default efficiency value (in case of OutOfValidityRange)
            float m_default_eff;
            /// default TTVA efficiency value (in case of MuonStandAlone for |eta|>2.5)
            float m_default_eff_ttva;
            
            mutable unsigned int m_warnsPrinted;
            static unsigned int m_warningLimit;
            
            unsigned int m_firstRun;
            unsigned int m_lastRun;
    };
} /* namespace CP */

#endif /* EFFICIENCYSCALEFACTOR_H_ */
