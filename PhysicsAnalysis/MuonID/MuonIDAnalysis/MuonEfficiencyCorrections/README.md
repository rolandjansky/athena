
This package provides Reconstruction/TTVA/Isolation/Trigger Efficiency values and Scale Factors for Muons.

Contents of this Readme:
   -- Reconstruction efficiencies - MuonEfficiencyScaleFactors
      1 - Some source files possibly interesting for reference
      2 - Supported working points
      3 - Supported data periods
      4 - Detailed instructions for use 
      5 - Evaluation of Systematics 
   -- for the isolation scale factors
      1 - Some source files possibly interesting for reference
      2 - Supported working points
      3 - Supported data periods
      4 - Detailed instructions for use
      5 - Evaluation of Systematics
   -- Trigger Efficiencies - MuonTriggerScaleFactors
      1 - Some source files possibly interesting for reference

////////////////////////////////////////////////////    
Reconstruction efficiencies - MuonEfficiencyScaleFactors
////////////////////////////////////////////////////    

1) Reference material:
- The MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h interface class provides an overview of user methods
- utils/MuonEfficiencyScaleFactorsTest.cxx is an example application within AnalysisBase/AthAnalysis
- share/MuonEfficiencyCorrections_xAOD_Testing_jobOptions.py (using src/MuonEfficiencyCorrections_TestAlg.cxx) is an example when running in AthAnalysis/Athena
- You can find the full documentation and latest prescriptions at https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisWinterMC16

2) Supported working points:
There is support for the official MCP working points -  cf. https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisWinterMC16

3) Supported data periods:
Several data periods are supported, the LastRun of the latest data period is set to 999999 meaning, for new runs always the most recent SFs are provided.
   
4) Detailed Instructions
- include the headers: 
    #include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"
    
- create your tool 
    *  with AnalysisBase/AthAnalysis:
    
        // create one tool per muon working point 
        CP::MuonEfficiencyScaleFactors m_effi_corr("SomeNameYouLike");
        
    *  with AthAnalysis/Athena:
    
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
- You can find the full documentation and latest prescriptions at https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisWinterMC16

2) Supported working points:
There is support for the official MCP working points -  cf. https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisWinterMC16

3) Supported data periods:
Several data periods are supported, the LastRun of the latest data period is set to 999999 meaning, for new runs always the most recent SFs are provided.
   
4) Detailed Instructions
   See above (reconstruction efficiencies)

5) Evaluating Uncertainties:
   Two nuisance parameters, MUON_ISO_STAT and MUON_ISO_SYS, are provided within the standard ASG systematics framework
        
////////////////////////////////////////////////////        
Trigger Efficiencies - MuonTriggerScaleFactors
////////////////////////////////////////////////////    
1) Reference material:
   - The MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h interface class provides an overview of user methods
   - utils/MuonTriggerSFRootCoreTest.cxx is an example application within AnalysisBase/AthAnalysis
   - share/TriggerSF_Tester_jobOptions.py is an example when running in AthAnalysis/Athena
   - You can find the full documentation and latest prescriptions at https://twiki.cern.ch/twiki/bin/view/Atlas/MuonTriggerPhysicsRecommendationsRel212017

