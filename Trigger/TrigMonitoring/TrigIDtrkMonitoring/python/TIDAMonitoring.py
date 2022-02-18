#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# create all the histograms for each analysis - this will get called once for each 
# configured chain - we can set the HistPath either already here, or from the c++ 
# code 


def TIDAMonitoring( name ) :  

#       print( "SUTT creating monitoring: ", name )

        tools = []

        from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigR3Mon
        from TrigIDtrkMoniotring.TIDAChains import getchains
        


        #### electron #### 

        tidaegamma = TrigR3Mon(name = "IDEgammaTool" )
        tidaegamma.AnalysisConfig = "Tier0"
        tidaegamma.SliceTag       = "HLT/TRIDT/Egamma/Expert"
        tidaegamma.UseHighestPT   = True

        chains = getchains( [ "HLT_e.*idperf.*:key=HLT_IDTrack_Electron_FTF:roi=HLT_Roi_FastElectron",  
                              "HLT_e.*idperf.*:key=HLT_IDTrack_Electron_FTF:roi=HLT_Roi_FastElectron",  
                              "HLT_e.*idperf.*:key=HLT_IDTrack_Electron_IDTrig",
                              "HLT_e.*gsf.*:key=HLT_IDTrack_Electron_GSF",
                              "HLT_e26_lhtight_ivarloose_e5_lhvloose_idperf_probe_L1EM22VHI:key=HLT_IDTrack_Electron_FTF:roi=HLT_Roi_FastElectron:extra=el0_tag:te=0",
                              "HLT_e26_lhtight_ivarloose_e5_lhvloose_idperf_probe_L1EM22VHI:key=HLT_IDTrack_Electron_FTF:roi=HLT_Roi_FastElectron:extra=el0_probe:te=1",
                              "HLT_e26_lhtight_e14_etcut_idperf_probe_50invmAB130_L1eEM26M:key=HLT_IDTrack_Electron_FTF:roi=HLT_Roi_FastElectron:extra=el1_tag:te=0",
                              "HLT_e26_lhtight_e14_etcut_idperf_probe_50invmAB130_L1eEM26M:key=HLT_IDTrack_Electron_FTF:roi=HLT_Roi_FastElectron:extra=el1_probe:te=1",
                              "HLT_e26_lhtight_e14_etcut_idperf_probe_50invmAB130_L1EM22VHI:key=HLT_IDTrack_Electron_FTF:roi=HLT_Roi_FastElectron:extra=el2_tag:te=0",
                              "HLT_e26_lhtight_e14_etcut_idperf_probe_50invmAB130_L1EM22VHI:key=HLT_IDTrack_Electron_FTF:roi=HLT_Roi_FastElectron:extra=el2_probe:te=1" ] )

        tidaegamma.ntupleChainNames = chains
        
        tidaegamma.MonTools = createMonTools(  tidaegamma.SliceTag, chains )

        tools += [ tidaegamma ]



        #### LRT Egamma ####
        
        tidaegammalrt = TrigR3Mon(name = "IDEgammaLRTTool" ) 
                                  
        tidaegammalrt.AnalysisConfig = "Tier0"
        tidaegammalrt.SliceTag = "HLT/TRIDT/Egamma/Expert"

        chains = getchains( [ "HLT_e.*idperf_loose_lrtloose.*:HLT_IDTrack_ElecLRT_FTF:HLT_Roi_FastElectron_LRT",
                              "HLT_e.*idperf_loose_lrtloose.*:HLT_IDTrack_ElecLRT_IDTrig:HLT_Roi_FastElectron_LRT" ] )
        
        tidaegammalrt.ntupleChainNames = chains

        tidaegammalrt.MonTools = createMonTools( tidaegammalrt.SliceTag, chains )
        
        tools += [ tidaegammalrt ]
                
                


        #### muon ####

        tidamuon = TrigR3Mon(name = "IDMuonTool" )
        tidamuon.AnalysisConfig = "Tier0"
        tidamuon.SliceTag = "HLT/TRIDT/Muon/Expert"
        tidamuon.UseHighestPT = True

        chains = getchains( [ "HLT_mu.*idperf.*:key=HLT_IDTrack_Muon_FTF:roi=HLT_Roi_L2SAMuon",
                              "HLT_mu.*idperf.*:key=HLT_IDTrack_Muon_IDTrig:roi=HLT_Roi_L2SAMuon",
                              "HLT_mu.*ivarperf.*:key=HLT_IDTrack_MuonIso_FTF:roi=HLT_Roi_MuonIso",
                              "HLT_mu.*ivarperf.*:key=HLT_IDTrack_MuonIso_IDTrig:roi=HLT_Roi_MuonIso" ] )
                              

        tidamuon.ntupleChainNames += chains
        
        tidamuon.MonTools = createMonTools(  tidamuon.SliceTag, chains )
        

        tools += [ tidamuon ]




        #### tau ####

        tidatau = TrigR3Mon(name = "IDTauTool" )
        tidatau.AnalysisConfig = "Tier0"
        tidatau.SliceTag = "HLT/TRIDT/Tau/Expert"
        tidatau.UseHighestPT = True

        chains = getchains( [ "HLT_tau.*idperf.*tracktwo.*:key=HLT_IDTrack_TauCore_FTF:roi=HLT_Roi_TauCore",
                              "HLT_tau.*idperf.*tracktwo.*:key=HLT_IDTrack_TauIso_FTF:roi=HLT_Roi_TauIso",
                              "HLT_tau.*idperf.*tracktwo.*:key=HLT_IDTrack_Tau_IDTrig:roi=HLT_Roi_TauIso",
                              "HLT_tau.*idperf.*BDT.*:key=HLT_IDTrack_TauIso_FTF:roi=HLT_Roi_TauIsoBDT",
                              "HLT_tau.*idperf.*BDT.*:key=HLT_IDTrack_Tau_IDTrig:roi=HLT_Roi_TauIsoBDT" ] )

        tidatau.ntupleChainNames += chains
        
        tidatau.MonTools = createMonTools(  tidatau.SliceTag, chains )

        tools += [ tidatau ]




        #### bjets ####

        tidabjet = TrigR3Mon(name = "IDBjetTool" )
        tidabjet.AnalysisConfig = "Tier0"
        tidabjet.SliceTag = "HLT/TRIDT/Bjet/Expert"
        
        chains = getchains( [ "HLT_j45_ftf_L1J15:key=HLT_IDTrack_FS_FTF:roi=HLT_FSRoI:vtx=HLT_IDVertex_FS",
                              "HLT_j.*_ftf.*boffperf.*:key=HLT_IDTrack_FS_FTF:roi=HLT_FSRoI:vtx=HLT_IDVertex_FS",
                              "HLT_j.*.*boffperf.*:key=HLT_IDTrack_Bjet_FTF",
                              "HLT_j.*.*boffperf.*:key=HLT_IDTrack_Bjet_IDTrig" ] )
                        
        tidabjet.ntupleChainNames += chains

        tidabjet.MonTools = createMonTools(  tidabjet.SliceTag, chains )

        tools += [ tidabjet ]


        return tools






# create a separate specific monTool for each analysis chain
# - simplifies the overall analysis configuration

def createMonTools( label, chains, excludeTagChains=True ):
        tools = []
        from TrigInDetAnalysisExample.chainString import chainString
        from TrigInDetAnalysisExample.TIDAMonTool import createMonTool
        for mt in chains :
                if excludeTagChains and "tag" in chainString(mt).extra:
                        continue
                tool = createMonTool( label, mt )
                tools += [ tool ]
        return tools
        

