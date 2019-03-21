
This package provides Reconstruction/TTVA/Isolation/Trigger Efficiency values and Scale Factors for Muons.

Contents of this Readme:
   -- Reconstruction efficiencies - MuonEfficiencyScaleFactors
      1 - Some source files possibly interesting for reference
      2 - Supported working points
      3 - Supported data periods
      4 - Detailed instructions for ROOTCore use 
      5 - Evaluation of Systematics 
   -- for the isolation scale factors
      1 - Some source files possibly interesting for reference
      2 - Supported working points
      3 - Supported data periods
      4 - Detailed instructions for ROOTCore use
      5 - Evaluation of Systematics
   -- Trigger Efficiencies - MuonTriggerScaleFactors
      1 - Some source files possibly interesting for reference
      2 - Supported triggers
      3 - Supported data periods
      4 - Detailed instructions for ROOTCore use
      5 - Ecaluation of Systematics

////////////////////////////////////////////////////    
Reconstruction efficiencies - MuonEfficiencyScaleFactors
////////////////////////////////////////////////////    

1) Reference material:
- The IMuonEfficiencyScaleFactor interface class provides an overview of user methods
- utils/MuonEfficiencyCorrectionsRootCoreTest.cxx is an example application within ROOTCore
- share/MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py (using src/MuonEfficiencyCorrections_TestAlg.cxx) is an example when running in athena

2) Supported working points:
There is support for the official MCP working points -  "Loose","Medium","Tight","HighPt", "LowPt".

3) Supported data periods:
Several data periods are supported, the LastRun of the latest data period is set to 999999 meaning, for new runs always the most recent SFs are provided.
   
4) Detailed Instructions
- include the headers: 
    #include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"
    
- create your tool 
    *  with ROOTCore:
    
        // create one tool per muon working point 
        CP::MuonEfficiencyScaleFactors m_effi_corr("SomeNameYouLike");
        
    *  with Athena:
    
        from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors
        m_effi_corr = CP__MuonEfficiencyScaleFactors("SomeNameYouLike")
        
    
- configure it by providing the working point 
        m_effi_corr.setProperty("WorkingPoint","Medium");   // Or "Loose" or "Tight"...
        
- initialize the tool 
        
    CHECK (!m_effi_corr.initialize().isFailure());
        
- there are two ways of retrieving information:
     * direct access: 
     
         float sf = 0.0;
         CHECK( m_effi_corr.getEfficiencyScaleFactor( my_muon, sf ) );
         // sf will now contain the scale factor for my_muon
            
     * decorating mode: 
     
         CHECK( m_effi_corr.applyEfficiencyScaleFactor(mu) );
         
         double my_sf = my_muon.auxdata< float >( "EfficiencyScaleFactor" )

5) Evaluating Uncertainties:
    Two nuisance parameters, MUON_EFF_STAT and MUON_EFF_SYS, are provided within the standard ASG systematics framework

////////////////////////////////////////////////////    
Isolation efficiencies - MuonEfficiencyScaleFactors
////////////////////////////////////////////////////    
It uses the same class as MuonEfficiencyScaleFactor. Isolation Configuration is needed.
1) Reference material:
   - The IMuonEfficiencyScaleFactor interface class provides an overview of user methods
   - utils/MuonIsolationEfficiencyCorrectionsRootCoreTest.cxx is an example application within ROOTCore
   - share/MuonIsolationEfficiencyCorrections_xAOD_Testing_jobOptions.py (using src/MuonEfficiencyCorrections_TestAlg.cxx) is an example when running in athena

2) Supported working points:
There is support for the five official MCP working points -  "TightIso", "LooseIso", "VeryLooseIso", "VeryLooseTrackOnlyIso", "GradientIso" and "GradientLooseIso".

3) Supported data periods:
Several data periods are supported, the LastRun of the latest data period is set to 999999 meaning, for new runs always the most recent SFs are provided.
   
4) Detailed Instructions (RootCore)
   Same as "MuonEfficiencyScaleFactor". Make sure to use the isolation working points and period.

5) Evaluating Uncertainties:
   Two nuisance parameters, MUON_ISO_STAT and MUON_ISO_SYS, are provided within the standard ASG systematics framework
        
////////////////////////////////////////////////////        
Trigger Efficiencies - MuonTriggerScaleFactors
////////////////////////////////////////////////////    
1) Reference material:
   - The IMuonTriggerScaleFactor interface class provides an overview of user methods
   - utils/MuonEfficiencyCorrectionsRootCoreTest.cxx is an example application within ROOTCore
   - share/MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py (using src/MuonEfficiencyCorrections_TestAlg.cxx) is an example when running in athena

2) Supported triggers
   - for single muon triggers
     - HLT_mu20_iloose_L1MU15 || HLT_mu50

     
3) Supported data periods
So far, this tool supports for the following data taking periods:
   - "2015": The future 13 TeV data will be added when available. Currently, it is filled the Run-1 trigger scale factors.
If you need to use 2011 7TeV pp data or 2012 8TeV pp data, please use TrigMuonEfficiency(see https://twiki.cern.ch/twiki/bin/viewauth/Atlas/TrigMuonEfficiency).

4) Detailed instructions for ROOTCore use
- include the header :

    #include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"

- create your tool :

    CP::MuonTriggerScaleFactors m_trig_sf("SomeNameYouLike");

- configure it :
  * provide a data taking period
    
    m_trig_sf.setProperty("year", 2015);	

  * probide a run number

    m_trig_sf.setProperty("runNumber",220000);

- initialize the tool

    CHECK(!m_trig_sf.initialize().isFailure());

- How to get a trigger scale factor 
  * for the single muon trigger
    
    Double_t sf = 0;
    CHECK(m_trig_sf.getTriggerScaleFactor(selectedmuons, sf));

    * Currently, the trigger scale factor tool is only HLT_mu20_iloose_L1MU15 || HLT_mu50, no need to fill the trigger chain name.
    * selectedmuons should be defined as xAOD::MuonContainer.
          
  * for the dimuon triggers
    
    Double_t sf = 0;
    std::string dimuon_trig = "2mu14"; // or mu18_mu8noL1
    CHECK(m_trig_sf.getTriggerScaleFactor(selectedmuons, sf, dimuon_trig));

  * for the single + dimuon triggers

    Double_t sf = 0;
    std::string trigger = "mu20_iloose_L1MU15_or_mu50_or_2mu14"
    CHECK(m_trig_sf.getTriggerScaleFactor(selectedmuons, sf, trigger));

5) Evaluating Uncertainties :
If you want to consider the systematic uncertainties, you need to type as follows :

    CP::SystematicSet trigsfsys;
    trigsfsys.insert(CP::SystematicVariation("MUON_EFF_TrigUncertainty", 1));
    if(m_trig_sf.applySystematicVariation(trigsfSys)==CP::SystematicCode::Unsupported){
	ATH_MSG_ERROR("Unsuppoted");
    }
    Double_t sf = 0;
    CHECK(m_trig_sf.getTriggerScaleFactor(selectedmuons, sf));
    
You can use the following systematic options via Property "SystematicOption".
    m_trig_sf.setProperty("SystematicOption", 0) : systematic uncertainty and statistical uncertainty
    m_trig_sf.setProperty("SystematicOption", 1) : systematic uncertainty
    m_trig_sf.setProperty("SystematicOption", 2) : statistical uncertainty
