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
#include "JetInterface/IJetResolutionTool.h"
#include "ParticleJetTools/LargeRJetLabelEnum.h"

// Local includes
#include "JetUncertainties/Helpers.h"
#include "JetUncertainties/RootHelpers.h"


// Other includes
#include <TFile.h>
#include <TH2.h>
#include <TH3.h>
#include "TEnv.h"
#include "TMath.h"

/// Implementation of the Forward Folding (FF) Jet smearing tool interface
///
/// This tool allows to smear and unsmear the mass of Large R jets using 
/// the Forward Folding procedure to evaluate systematic uncertainties.
/// For information, see the Twiki:
/// https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/FFJetSmearingTool
///
/// If you find any bug, please, contact <alberto.prades.ibanez@cern.ch>
///



namespace FFAllowedMassDef{
    enum TypeEnum
    {
        UNKNOWN=0,
        Calo, //Calorimeter
        TA,   //Track Assisted
        Comb, //Combined
    };
    inline TypeEnum stringToEnum(const TString& name)
    {
        if (name.EqualTo("Calo",TString::kIgnoreCase))
            return Calo;
        if (name.EqualTo("TA",TString::kIgnoreCase))
            return TA;
        if (name.EqualTo("Comb",TString::kIgnoreCase))
            return Comb;
        return UNKNOWN;
    }
    inline TString enumToString(const TypeEnum type)
    {
        switch (type)
        {
            case Calo:         return "Calo";
            case TA:           return "TA";
            case Comb:         return "Comb";
            default:           return "";
        }
    } 

}

class FFJetSmearingTool : public asg::AsgTool, virtual public IJetResolutionTool 
{

    /// Proper constructor for Athena
    ASG_TOOL_CLASS( FFJetSmearingTool,  IJetResolutionTool  )

    public:
        // Constructor/destructor/init
        FFJetSmearingTool(const std::string name);      
        virtual ~FFJetSmearingTool();	

        virtual StatusCode initialize();



        //New systematic functions
        /// @name Methods implementing the ISystematicsTool interface
        /// @{

        /// Specify whether tool is affected by provided systematic
        virtual bool isAffectedBySystematic(const CP::SystematicVariation& systematic) const;

        /// List of all systematics affecting this tool
        virtual CP::SystematicSet affectingSystematics() const;
       
        /// List of all systematics recommended for this tool
        virtual CP::SystematicSet recommendedSystematics() const;

        /// Configure tool to apply systematic variation
        virtual CP::SystematicCode applySystematicVariation
        (const CP::SystematicSet& systematics);

        /// @}


        CP::CorrectionCode applyCorrection(xAOD::Jet* jet_reco);// The user has to use this function to smear it's jet mass

        StatusCode getMatchedTruthJet( xAOD::Jet* jet_reco, xAOD::Jet& jet_truth_matched);

    private:


        StatusCode readFFJetSmearingToolSimplifiedData(TEnv& settings);

        StatusCode getJMSJMR( xAOD::Jet* jet_reco, double jet_mass,  FFAllowedMassDef::TypeEnum MassDef_of_syst ,std::string jetTopology, double& JMS_err, double& JMR_err);

        StatusCode getJetTopology( xAOD::Jet* jet_reco, std::string& jetTopology);

        double Read3DHistogram(TH3* histo, double x, double y, double z);



        // Private members
        bool m_isInit;
        std::string m_name;
        std::string m_release;
        std::string m_truth_jetColl;
        std::string m_truthlabelaccessor;
        float m_EtaRange;
        float m_MassRange;
        float m_PtRange;
        std::string m_calibArea;
        std::string m_histFileName;
        std::string m_MassDef_string;
        FFAllowedMassDef::TypeEnum m_MassDef;
        std::string m_configFile;
        std::string m_path;
        std::string  m_HistogramsFilePath;

        //Response matrix
        TH2D* m_CALO_ResponseMap;
        TH2D* m_TA_ResponseMap;

        //Two histograms to extract the Calo and TA weights in the Combined mass of the jet
        TH3F* m_caloMassWeight;
        TH3F* m_TAMassWeight;


        //The list of systemaics
        CP::SystematicSet  m_SysList;

        //Maps that relates the systematic name with some of its caracteristics
        std::map<std::string,std::string> m_Syst_HistPath_map;
        std::map<std::string,std::string> m_Syst_MassDefAffected_map;
        std::map<std::string,std::string> m_Syst_TopologyAffected_map;
        std::map<std::string,TH2D*> m_Syst_Hist_map;
        std::map<std::string,std::string> m_Syst_Affects_JMSorJMR;

        int m_InfoWarnings;

        typedef std::unordered_map<CP::SystematicSet, CP::SystematicSet> SysFiterMap_t;
        SysFiterMap_t m_sysFilterMap;

        /// Points to the current systematic configuration
        const CP::SystematicSet* m_sysConfig;

}; // Class FFJetSmearingTool


#endif


