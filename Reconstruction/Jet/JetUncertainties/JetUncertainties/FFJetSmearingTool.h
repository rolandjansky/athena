/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef FFJETSMEARINGTOOL_FFJETSMEARINGTOOL_H
#define FFJETSMEARINGTOOL_FFJETSMEARINGTOOL_H


// Framework includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "PATInterfaces/CorrectionTool.h"


//includes for systematics ATLAS structure
#include "PATInterfaces/SystematicsTool.h"//To set the systematics as it is done in JetJvtEfficiency

#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PathResolver/PathResolver.h"


//xAOD EDM classes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/ShallowCopy.h"


// Other packages includes
#include "JetAnalysisInterfaces/IJetSelectorTool.h"
#include "JetCPInterfaces/ICPJetCorrectionTool.h"
#include "ParticleJetTools/LargeRJetLabelEnum.h"

// Local includes
#include "JetUncertainties/Helpers.h"
#include "JetUncertainties/JetHelpers.h"


// Other includes
#include <TFile.h>
#include <TH2.h>
#include <TH3.h>
#include "TEnv.h"
#include "TMath.h"

#include <memory> //to use make_unique

/// Implementation of the Forward Folding (FF) Jet smearing tool interface
///
/// This tool allows to smear and unsmear the mass of Large R jets using 
/// the Forward Folding procedure to evaluate systematic uncertainties.
/// For information, see the Twiki:
/// https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/FFJetSmearingTool
///
/// If you find any bug, please, contact <alberto.prades.ibanez@cern.ch>
///



namespace JetTools{
    enum class FFJetAllowedMassDefEnum
    {
        Calo,     ///Calorimeter
        TA,       ///Track Assisted
        Comb,     ///Combined
    };
    StatusCode stringToEnum(const TString& name, FFJetAllowedMassDefEnum& result)
    {
        if (name.EqualTo("Calo",TString::kIgnoreCase)){
            result = FFJetAllowedMassDefEnum::Calo;
            return StatusCode::SUCCESS;
        }
        if (name.EqualTo("TA",TString::kIgnoreCase)){
            result = FFJetAllowedMassDefEnum::TA;
            return StatusCode::SUCCESS;
        }
        if (name.EqualTo("Comb",TString::kIgnoreCase)){
            result = FFJetAllowedMassDefEnum::Comb;
            return StatusCode::SUCCESS;
        }
        return StatusCode::FAILURE;
    }
    TString enumToString(const FFJetAllowedMassDefEnum type)
    {
        switch (type)
        {
            case FFJetAllowedMassDefEnum::Calo:         return "Calo";
            case FFJetAllowedMassDefEnum::TA:           return "TA";
            case FFJetAllowedMassDefEnum::Comb:         return "Comb";
            default:           return "";
        }
    } 

}

namespace CP {
    class FFJetSmearingTool : public asg::AsgTool, virtual public ICPJetCorrectionTool 
    {

        /// Proper constructor for Athena
        ASG_TOOL_CLASS( FFJetSmearingTool,  ICPJetCorrectionTool  )

        public:
            // Constructor/destructor/init
            FFJetSmearingTool(const std::string name);      
            virtual ~FFJetSmearingTool();	

            virtual StatusCode initialize() override;



            //New systematic functions
            /// @name Methods implementing the ISystematicsTool interface
            /// @{

            /// Specify whether tool is affected by provided systematic
            virtual bool isAffectedBySystematic(const CP::SystematicVariation& systematic) const override;

            /// List of all systematics affecting this tool
            virtual CP::SystematicSet affectingSystematics() const override;
       
            /// List of all systematics recommended for this tool
            virtual CP::SystematicSet recommendedSystematics() const override;

            /// Configure tool to apply systematic variation
            virtual CP::SystematicCode applySystematicVariation
            (const CP::SystematicSet& systematics) override;

            /// @}


            CP::CorrectionCode applyCorrection(xAOD::Jet& jet_reco) override;// The user has to use this function to smear it's jet mass

            StatusCode getMatchedTruthJet( xAOD::Jet& jet_reco, xAOD::Jet& jet_truth_matched) const;

        private:


            StatusCode readFFJetSmearingToolSimplifiedData(TEnv& settings);

            StatusCode getJMSJMR( xAOD::Jet& jet_reco, double jet_mass,  JetTools::FFJetAllowedMassDefEnum MassDef_of_syst ,std::string jetTopology, double& JMS_err, double& JMR_err);

            StatusCode getJetTopology( xAOD::Jet& jet_reco, std::string& jetTopology) const;

            double Read3DHistogram(const TH3* histo, double x, double y, double z) const;



            // Private members
            bool m_isInit;
            std::string m_release;
            std::string m_truth_jetColl;
            std::string m_truthlabelaccessor;
            float m_EtaRange;
            float m_MaxMass;
            float m_MaxPt;
            std::string m_calibArea;
            std::string m_histFileName;
            std::string m_MassDef_string;
            JetTools::FFJetAllowedMassDefEnum m_MassDef;
            std::string m_configFile;
            std::string m_path;
            std::string  m_HistogramsFilePath;

            //Response matrix
            std::unique_ptr<TH2D> m_CALO_ResponseMap;
            std::unique_ptr<TH2D> m_TA_ResponseMap;

            //Two histograms to extract the Calo and TA weights in the Combined mass of the jet
            std::unique_ptr<TH3F> m_caloMassWeight;
            std::unique_ptr<TH3F> m_TAMassWeight;


            //The list of systemaics
            CP::SystematicSet  m_SysList;

            //Maps that relates the systematic name with some of its caracteristics
            std::map<std::string,std::string> m_Syst_HistPath_map;
            std::map<std::string,std::string> m_Syst_MassDefAffected_map;
            std::map<std::string,std::string> m_Syst_TopologyAffected_map;
            std::map<std::string,std::unique_ptr<TH2D>> m_Syst_Hist_map;
            std::map<std::string,std::string> m_Syst_Affects_JMSorJMR;

            int m_InfoWarnings;

            //The current systematic configuration
            struct SysData final
            {
                std::string SysBaseName {"None"};
                float SysParameter {0};
            };
            std::unordered_map<CP::SystematicSet,SysData> m_sysData;
            /// Points to the current systematic configuration
            SysData *m_currentSysData{nullptr};

            static constexpr float m_MeVtoGeV = 1.e-3;

    }; // Class FFJetSmearingTool

} // namespace CP

#endif
