/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
//#include "BoostedJetTaggers/JSSTaggerBase.h"

#include "JetUncertainties/Helpers.h"

/*
#include "BoostedJetTaggers/IJetSelectorLabelTool.h"

#include "BoostedJetTaggers/FatjetLabelEnum.h"
*/

//#include "JetUncertainties/JetUncertaintiesTool.h" //We comment JetUncertainties because this dependence is not needed any more. We directly read the Calo_weights histogram

//#include "JetUncertainties/CombinedMassUncertaintyComponent.h"//To extract the calo weight after the smearing


// Local includes
#include "JetUncertainties/IFFJetSmearingTool.h"

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
/// https://twiki.cern.chXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
///
/// @author Alberto Prades <alberto.prades.ibanez@cern.ch>
///








class FFJetSmearingTool : public asg::AsgTool, virtual public IFFJetSmearingTool//, // my class inherits from  the asg::AsgTool interface . This let us use the evtStore()->retrieve() function. And from the interface of the tool IFFJetSmearingTool 
{


    /// Proper constructor for Athena
    ASG_TOOL_CLASS( FFJetSmearingTool, IFFJetSmearingTool )

    public:
        // Constructor/destructor/init
        FFJetSmearingTool(const std::string name);        //To create the object we just ask for a name, the rest of the properties have to be declared with setProperty
        virtual ~FFJetSmearingTool();	

        virtual StatusCode initialize();

//         StatusCode executeTool(xAOD::TEvent& event);




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

    protected:
//TO COMPLETE

    private:


        StatusCode readFFJetSmearingToolSimplifiedData(TEnv& settings);

	StatusCode getJMSJMR( xAOD::Jet* jet_reco, double jet_mass, std::string CALO_or_TA,std::string jetTopology, double& JMS, double& JMS_err, double& JMR, double& JMR_err);

        StatusCode getJetTopology( xAOD::Jet* jet_reco, std::string& jetTopology);

        double Read3DHistogram(TH3* histo, double x, double y, double z);




        // Private members
	bool m_isInit;
        std::string m_name;
        std::string m_release;
        std::string m_truth_jetColl;
	float m_EtaRange;
        TString m_histFileName;
	TString m_MassDef;
	std::string m_ConfigFile;


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


//JetUncertaintiesTool* m_jetuncertaintiestool;// Not needed any more. We read directly the calo weights map
//std::unique_ptr<JetUncertaintiesTool> m_jetuncertaintiestool(new JetUncertaintiesTool(("FFJetSmearingTool_JetUncertaintiesTool")));
    /// Systematic filtering map.
    /// Note that the output of the map could be something other than
    /// SystematicSet. We could use pair<SysEnum, value>, for example, which
    /// actually might make the applyCorrection method cleaner.
    typedef std::unordered_map<CP::SystematicSet, CP::SystematicSet> SysFiterMap_t;
    SysFiterMap_t m_sysFilterMap;

    /// Points to the current systematic configuration
    const CP::SystematicSet* m_sysConfig;

}; // Class FFJetSmearingTool


#endif


