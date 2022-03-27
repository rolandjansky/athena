#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# create all the histograms for each analysis - this will get called once for each 
# configured chain - we can set the HistPath either already here, or from the c++ 
# code 


# actual code to configure al;l the different algorithm instances for 
# the different slices 

def TIDAMonitoring( flags=None, name=None ) :  

#       print( "SUTT creating monitoring: ", name )

        tools = []

        from TrigIDtrkMonitoring.TIDAChains import getchains
        

        #### electron #### 

        tidaegamma = TrigR3Mon_builder( flags, name = "IDEgammaTool" )
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
        
        tidaegamma.MonTools = createMonTools( flags, tidaegamma.SliceTag, chains )

        tools += [ tidaegamma ]



        #### LRT Egamma ####
        
        tidaegammalrt = TrigR3Mon_builder( flags, name = "IDEgammaLRTTool" ) 
                                  
        tidaegammalrt.AnalysisConfig = "Tier0"
        tidaegammalrt.SliceTag = "HLT/TRIDT/Egamma/Expert"

        chains = getchains( [ "HLT_e.*idperf_loose_lrtloose.*:HLT_IDTrack_ElecLRT_FTF:HLT_Roi_FastElectron_LRT",
                              "HLT_e.*idperf_loose_lrtloose.*:HLT_IDTrack_ElecLRT_IDTrig:HLT_Roi_FastElectron_LRT" ] )
        
        tidaegammalrt.ntupleChainNames = chains

        tidaegammalrt.MonTools = createMonTools( flags, tidaegammalrt.SliceTag, chains )
        
        tools += [ tidaegammalrt ]
                
                


        #### muon ####

        tidamuon = TrigR3Mon_builder( flags, name = "IDMuonTool" )
        tidamuon.AnalysisConfig = "Tier0"
        tidamuon.SliceTag = "HLT/TRIDT/Muon/Expert"
        tidamuon.UseHighestPT = True

        chains = getchains( [ "HLT_mu.*idperf.*:key=HLT_IDTrack_Muon_FTF:roi=HLT_Roi_L2SAMuon",
                              "HLT_mu.*idperf.*:key=HLT_IDTrack_Muon_IDTrig:roi=HLT_Roi_L2SAMuon",
                              "HLT_mu.*ivarperf.*:key=HLT_IDTrack_MuonIso_FTF:roi=HLT_Roi_MuonIso",
                              "HLT_mu.*ivarperf.*:key=HLT_IDTrack_MuonIso_IDTrig:roi=HLT_Roi_MuonIso" ] )
                              

        tidamuon.ntupleChainNames += chains
        
        tidamuon.MonTools = createMonTools( flags,  tidamuon.SliceTag, chains )
        

        tools += [ tidamuon ]




        #### tau ####

        tidatau = TrigR3Mon_builder( flags, name = "IDTauTool" )
        tidatau.AnalysisConfig = "Tier0"
        tidatau.SliceTag = "HLT/TRIDT/Tau/Expert"
        tidatau.UseHighestPT = True

        chains = getchains( [ "HLT_tau.*idperf.*tracktwo.*:key=HLT_IDTrack_TauCore_FTF:roi=HLT_Roi_TauCore",
                              "HLT_tau.*idperf.*tracktwo.*:key=HLT_IDTrack_TauIso_FTF:roi=HLT_Roi_TauIso",
                              "HLT_tau.*idperf.*tracktwo.*:key=HLT_IDTrack_Tau_IDTrig:roi=HLT_Roi_TauIso",
                              "HLT_tau.*idperf.*BDT.*:key=HLT_IDTrack_TauIso_FTF:roi=HLT_Roi_TauIsoBDT",
                              "HLT_tau.*idperf.*BDT.*:key=HLT_IDTrack_Tau_IDTrig:roi=HLT_Roi_TauIsoBDT" ] )

        tidatau.ntupleChainNames += chains
        
        tidatau.MonTools = createMonTools( flags,  tidatau.SliceTag, chains )

        tools += [ tidatau ]




        #### bjets ####

        tidabjet = TrigR3Mon_builder( flags, name = "IDBjetTool" )
        tidabjet.AnalysisConfig = "Tier0"
        tidabjet.SliceTag = "HLT/TRIDT/Bjet/Expert"
        
        chains = getchains( [ "HLT_j45_pf_ftf_preselj20_L1J15:key=HLT_IDTrack_FS_FTF:roi=HLT_FSRoI:vtx=HLT_IDVertex_FS",
                              "HLT_j.*_ftf.*boffperf.*:key=HLT_IDTrack_FS_FTF:roi=HLT_FSRoI:vtx=HLT_IDVertex_FS",
                              "HLT_j.*.*boffperf.*:key=HLT_IDTrack_Bjet_FTF",
                              "HLT_j.*.*boffperf.*:key=HLT_IDTrack_Bjet_IDTrig" ] )
                        
        tidabjet.ntupleChainNames += chains

        tidabjet.MonTools = createMonTools( flags,  tidabjet.SliceTag, chains )

        tools += [ tidabjet ]


        return tools




# create a separate specific monTool for each analysis chain
# - simplifies the overall analysis configuration

def createMonTools( flags, label, chains, excludeTagChains=True ):
        tools = []
        from TrigInDetAnalysisExample.chainString import chainString
        from TrigInDetAnalysisExample.TIDAMonTool import createMonTool
        for mt in chains :
                if excludeTagChains and "tag" in chainString(mt).extra:
                        continue
                tool = createMonTool( flags, label, mt )
                tools += [ tool ]
        return tools
        

# create the actual algorithm - calling with this wrapper lets us use the same 
# code for the old, or new configuration 
def TrigR3Mon_builder( flags=None, name="NoName" ):

        if flags is None:
                from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigR3Mon
                alg = TrigR3Mon( name = name )
        else:
                from AthenaConfiguration.ComponentFactory import CompFactory
                alg =  CompFactory.TrigR3Mon( name = name )

        return alg


#wrapper function for the central moniotring configuration 

def TrigInDetMonConfig( flags ):
        return TIDAMonitoringCA( flags )



# component accumulator wrapper around the overall monitoring functiom                

def TIDAMonitoringCA( flags ):

        from AthenaMonitoring import AthMonitorCfgHelper
        monConfig = AthMonitorCfgHelper(flags, "TrigIDMon")

        algs = TIDAMonitoring(flags, "SeeminglyIrrelevant")
        for a in algs:
                monConfig.addAlgorithm(a)

        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        ca = ComponentAccumulator()
        ca.merge(monConfig.result())
        return ca






def histsvc( flags ):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    ca = ComponentAccumulator()
    
    print( "histsvc: ", flags.Output.HISTFileName )
    
    from AthenaConfiguration.ComponentFactory import CompFactory
    THistSvc = CompFactory.THistSvc
        
    histsvc = THistSvc()
    if flags.Output.HISTFileName:
        histsvc.Output += ["%s DATAFILE='%s' OPT='RECREATE'" % (flags.DQ.FileKey, flags.Output.HISTFileName)]
            
    ca.addService(histsvc)
       
    return ca
            


if __name__=='__main__':

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)
    # from AthenaCommon.Constants import INFO
    # log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    # Input files
    # AOD file to be run w/ MT access and Mon Groups implemented
    file = '/afs/cern.ch/work/e/enagy/public/ARTfiles/MonGroupTest_030222.pool.root'

    ConfigFlags.Input.Files = [file]
    ConfigFlags.Input.isMC = True

    ConfigFlags.Output.HISTFileName = 'duff.root'

    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge( PoolReadCfg(ConfigFlags) )
    cfg.merge( histsvc(ConfigFlags) )

    cfg.merge( TrigInDetMonConfig( ConfigFlags ) ) 

    # If you want to turn on more detailed messages ...
    cfg.printConfig(withDetails=False) # set True for exhaustive info

    Nevents = 10
    cfg.run(Nevents)



