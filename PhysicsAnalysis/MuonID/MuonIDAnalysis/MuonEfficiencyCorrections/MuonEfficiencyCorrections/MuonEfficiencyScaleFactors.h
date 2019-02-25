/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONEFFICIENCYSCALEFACTORS_H_
#define MUONEFFICIENCYSCALEFACTORS_H_



#include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"
#include "MuonEfficiencyCorrections/MuonEfficiencyType.h"
#include "MuonEfficiencyCorrections/EfficiencyScaleFactor.h"
#include "MuonEfficiencyCorrections/EffiCollection.h"

#include <AsgTools/ToolHandle.h>
#include <AsgTools/AsgTool.h>

#include <string>
#include <memory>
#include <map>
namespace CP {
    class MuonEfficiencyScaleFactors: virtual public CP::IMuonEfficiencyScaleFactors, public asg::AsgTool {

        public:
            MuonEfficiencyScaleFactors(const std::string& name);

            virtual ~MuonEfficiencyScaleFactors();
            //Proper constructor for Athena
            ASG_TOOL_CLASS2( MuonEfficiencyScaleFactors, CP::IMuonEfficiencyScaleFactors, CP::ISystematicsTool )

            /// initialize the tool once all settings are in place!
            virtual StatusCode initialize();

            /// Retrieve the Scale factor and decorate the muon
            virtual CorrectionCode getEfficiencyScaleFactor(const xAOD::Muon& mu, float& sf, const xAOD::EventInfo* info = 0) const;
            virtual CorrectionCode applyEfficiencyScaleFactor(const xAOD::Muon& mu, const xAOD::EventInfo* info = 0) const;
            /// replica generation
            virtual CorrectionCode getEfficiencyScaleFactorReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info = 0) const;
            virtual CorrectionCode applyEfficiencyScaleFactorReplicas(const xAOD::Muon& mu, int nreplicas = 50, const xAOD::EventInfo* info = 0) const;

            /// returns: whether this tool is affected by the given systematis
            virtual bool isAffectedBySystematic(const SystematicVariation& systematic) const;

            /// returns: the list of all systematics this tool can be affected by
            virtual SystematicSet affectingSystematics() const;

            /// returns: the list of all systematics this tool recommends to use
            virtual SystematicSet recommendedSystematics() const;

            virtual SystematicCode applySystematicVariation(const SystematicSet& systConfig);

            /// Obtain the muon efficiency measured using the data
            virtual CorrectionCode getDataEfficiency(const xAOD::Muon& mu, float& eff, const xAOD::EventInfo* info = 0) const;
            virtual CorrectionCode applyDataEfficiency(const xAOD::Muon& mu, const xAOD::EventInfo* info = 0) const;

            virtual CorrectionCode getDataEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info = 0) const;
            virtual CorrectionCode applyDataEfficiencyReplicas(const xAOD::Muon& mu, int nreplicas = 50, const xAOD::EventInfo* info = 0) const;

            /// Obtain the muon efficiency measured using the MC
            virtual CorrectionCode getMCEfficiency(const xAOD::Muon& mu, float& eff, const xAOD::EventInfo* info = 0) const;
            virtual CorrectionCode applyMCEfficiency(const xAOD::Muon& mu, const xAOD::EventInfo* info = 0) const;

            virtual CorrectionCode getMCEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info = 0) const;
            virtual CorrectionCode applyMCEfficiencyReplicas(const xAOD::Muon& mu, int nreplicas = 50, const xAOD::EventInfo* info = 0) const;

            virtual int getUnCorrelatedSystBin(const xAOD::Muon& mu) const;
            virtual std::string getUncorrelatedSysBinName(unsigned int Bin) const;
            virtual std::string getUncorrelatedSysBinName(const SystematicSet& systConfig) const;


        private:
            unsigned int getRandomRunNumber(const xAOD::EventInfo* info) const;
            /// load the SF histos
            StatusCode LoadInputs();
           
            /// Scale-factor files since  Moriond2019 contain the breakdown of systematics into
            /// their individual components. This method loads all systematics and looks their
            /// systematics up and returns them in a map together with a bitmask which files are
            /// affected by the systematic
            std::map<std::string, unsigned int> lookUpSystematics();
        public:
            /// The following methods are meant to propagate information from the central
            /// tool to the subtool managing the individual scale-factor maps to keep their
            /// constructors small in number of arguments. The users do not have to call them.
            
            
            /// Construct the name of the input files from the configuration
            /// Make these methods public such that they can be used by the
            ///  scale-factor managing EffiCollection class without piping
            ///  All arguments to the constructor again
            std::string filename_Central()const;
            /// Reconstruction scale-factors have a dedicated map
            /// for calo-tag muons around |\eta|<0.1. If the scale-factor
            /// is isolation/TTVA then the central file name is returned
            std::string filename_Calo()const;
            /// High-eta reconstruction scale-factors are not obtained by the means of 
            /// are not obtained by the means of tag & probe, but rather by building
            /// the double ratio. The map is delivered in a dedicated file whose path
            /// is returned here
            std::string filename_HighEta()const;
 
            /// Returns the scale-factor maps from a complementary scale-factor
            /// measurement using the J/Psi or Upsilon resonance
            std::string filename_LowPt()const;
            std::string filename_LowPtCalo()const;
            
            /// If the pt of the muon is below that threshold the J/Psi or Upsilon
            /// map is used given that it's available.
            float lowPtTransition() const;
            
            /// Returns the type of the measurement to be carried out... E.g. Reco/TTVA/Iso
            CP::MuonEfficiencyType measurement() const; 
            
            /// The apply<Blah> methods decorate their result directly to the muon. The name of the decorators
            /// can be set by the users themselves using several properties. To avoid that systmatics overwrite
            /// each other and the nominal the final maps are decorating the muon following the logic
            ///     <decoration>__<syst_name>
            
            /// The following methods propagate the basic decoration names to the maps            
            std::string sf_decoration() const;
            std::string data_effi_decoration() const;
            std::string mc_effi_decoration() const;
            
            std::string sf_replica_decoration() const;
            std::string data_effi_replica_decoration() const;
            std::string mc_effi_replica_deocration() const;
            
            /// Returns the position of the collection in the 
            /// syst set vector. If the collection is not part
            /// of this class -1 is returned
            size_t getPosition(const EffiCollection* coll) const;
            
            /// Returns the number of EffiCollections stored in this class
            size_t getNCollections() const;
       
    private:
            /// utility method to 'dress' a filename using the path resolver
            std::string resolve_file_location(const std::string &filename)const;
           
           
            /// the working point to operate on
            std::string m_wp;
           
            /// This vector stores all scale-factor maps
            std::vector<std::unique_ptr<EffiCollection>> m_sf_sets;
            
            /// Pointer to the current active map in terms of systematics
            EffiCollection* m_current_sf;

            std::string m_custom_dir;
            std::string m_custom_file_Combined;
            std::string m_custom_file_Calo;
            std::string m_custom_file_HighEta;
            std::string m_custom_file_LowPt;
            std::string m_custom_file_LowPtCalo;
   
            // Decorate the data-monteCarlo & scale-factors
            // to the muon. Decoration names can be set flexile
            std::string m_efficiency_decoration_name_data;
            std::string m_efficiency_decoration_name_mc;
            std::string m_sf_decoration_name;
      
            /// subfolder to load from the calibration db
            std::string m_calibration_version;

            /// threshold below which low-pt SF (i.e. from JPsi) should be used
            float m_lowpt_threshold;
         
            CP::SystematicSet m_affectingSys;
            /// It turned out that the 
            std::map<CP::SystematicVariation, EffiCollection*> m_filtered_sys_sets;
   
            bool m_init;
            bool m_seperateSystBins;
            bool m_breakDownSyst;
            CP::MuonEfficiencyType m_Type;
    };

} /* namespace CP */

#endif /* MUONEFFICIENCYSCALEFACTORS_H_ */
