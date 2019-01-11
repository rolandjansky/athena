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

#include <unordered_map>

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

            // copy constructor, to make reflex happy...
            MuonEfficiencyScaleFactors(const MuonEfficiencyScaleFactors& tocopy);

            // to make coverity happy
            MuonEfficiencyScaleFactors & operator =(const MuonEfficiencyScaleFactors & tocopy);

        private:
            typedef SG::AuxElement::Decorator<float> FloatDecorator;
            typedef SG::AuxElement::Decorator<std::vector<float> > FloatVectorDecorator;
            typedef std::shared_ptr<EffiCollection> EffiCollection_Ptr;
           
            
            unsigned int getRandomRunNumber(const xAOD::EventInfo* info) const;
            
            
            /// load the SF histos
            bool LoadEffiSet(MuonEfficiencySystType sysType);
            bool LoadInputs();
        public:
        
            /// construct the name of the input files from the configuration
            std::string filename_Central()const;
            std::string filename_Calo()const;
            std::string filename_HighEta()const;
            std::string filename_LowPt()const;
            std::string filename_LowPtCalo()const;
            
            float LowPtTransition() const;
            
        private:
            // utility method to 'dress' a filename using the path resolver
            std::string resolve_file_location(const std::string &filename)const;

            //Some util functions
            void CopyInformation(const MuonEfficiencyScaleFactors & tocopy);
            //These are functions needed during initialization
            StatusCode CreateDecorator(std::unique_ptr<FloatDecorator> &Dec,  std::string& DecName, const std::string& defaultName);
            StatusCode CreateVecDecorator(std::unique_ptr<FloatVectorDecorator> &Dec, std::string& DecName, const std::string& defaultName);
           
            StatusCode IsDecoratorNameUnique(std::string &name)const;
            SystematicSet SetupSystematics(bool doUnfolded = false) const;
            void SetupCheckSystematicSets();
            /// the working point to operate on
            std::string m_wp;
            std::unordered_map<MuonEfficiencySystType, EffiCollection_Ptr> m_sf_sets;

            EffiCollection_Ptr m_current_sf;

            std::string m_custom_dir;
            std::string m_custom_file_Combined;
            std::string m_custom_file_Calo;
            std::string m_custom_file_HighEta;
            std::string m_custom_file_LowPt;
            std::string m_custom_file_LowPtCalo;

            std::map<CP::SystematicSet, CP::SystematicSet> m_filtered_sys_sets;

            std::string m_efficiency_decoration_name_data;
            std::string m_efficiency_decoration_name_mc;
            std::string m_sf_decoration_name;
            std::string m_sf_replica_decoration_name;
            std::string m_eff_replica_decoration_name;
            std::string m_mc_eff_replica_decoration_name;

            // subfolder to load from the calibration db
            std::string m_calibration_version;

            // threshold below which low-pt SF (i.e. from JPsi) should be used
            float m_lowpt_threshold;
            
            // decorators to quickly apply the eff and SF
            std::unique_ptr<FloatDecorator> m_effDec;
            std::unique_ptr<FloatDecorator> m_MCeffDec;
            std::unique_ptr<FloatDecorator> m_sfDec;

            std::unique_ptr<FloatVectorDecorator> m_sfrDec;
            std::unique_ptr<FloatVectorDecorator> m_effrDec;
            std::unique_ptr<FloatVectorDecorator> m_MCeffrDec;

            CP::SystematicSet m_affectingSys;
            
            bool m_init;
            bool m_seperateSystBins;
            bool m_breakDownSyst;
            CP::MuonEfficiencyType m_Type;
    };

} /* namespace CP */

#endif /* MUONEFFICIENCYSCALEFACTORS_H_ */
