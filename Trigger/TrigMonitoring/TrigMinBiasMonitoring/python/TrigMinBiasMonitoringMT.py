#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
@brief configuration for the min bias monitoring
'''

def TrigMinBias(configFlags):
    
    from AthenaMonitoring import AthMonitorCfgHelper
    monConfig = AthMonitorCfgHelper(configFlags,'HLTMinBiasMonAlgMTAcc')
    
    from AthenaConfiguration.ComponentFactory import CompFactory 
    alg    = monConfig.addAlgorithm(CompFactory.HLTMinBiasMonAlgMT,'HLTMinBiasMonAlgMT')
    mbEffAllGroup = monConfig.addGroup(
    alg,
    'EffAll',
    'HLT/MinBiasMon/'
    )
    alg.triggerList = [ "HLT_mb_sptrk_L1RD0_FILLED"]
    length = len(alg.triggerList)
    mbEffAllGroup.defineHistogram( "PurityAll,whichTrigger",type = 'TEfficiency',title="PurityAll;whichTrigger" ,xbins=length, xmin=0, xmax=length)
    mbEffAllGroup.defineHistogram( "PurityPassed,whichTrigger",type = 'TEfficiency',title="PurityPassed;whichTrigger",xbins=length, xmin=0, xmax=length, xlabels = alg.triggerList)
    mbEffAllGroup.defineHistogram( "EfficiencyAll,whichTrigger",type = 'TEfficiency', title="EfficiencyAll;whichTrigger",xbins=length, xmin=0, xmax=length)
    mbEffAllGroup.defineHistogram( "EfficiencyPassed,whichTrigger",type = 'TEfficiency', title="EfficiencyPassed;whichTrigger", xbins=length, xmin=0, xmax=length)
    mbEffAllGroup.defineHistogram( "EfficiencyPassed,trigNo",type = 'TEfficiency', title="EfficiencyPassed;trig No.", xbins=length, xmin=0, xmax=length, xlabels = alg.triggerList)
    mbEffAllGroup.defineHistogram( "whichTrigger",title="count of triggers", xbins=length, xmin=0, xmax=length)
    mbEffAllGroup.defineHistogram( "EfficiencyPassed,trigCount",type = 'TProfile',title="EfficiencyPassed;Trigger;EfficiencyPassed", xbins=length, xmin=0, xmax=length)

    
    for chain in alg.triggerList:
        mbGroup = monConfig.addGroup(
        alg,
        chain,
        'HLT/MinBiasMon/IDMinbias/'+chain+'/')
        mbEffGroup = monConfig.addGroup(
        alg,
        chain+'_Eff',
        'HLT/MinBiasMon/Purities&Efficiencies/'+chain+'/')
        
    
      # correct the titles of following histograms
        mbGroup.defineHistogram( "PixelSPLow", title="Number of SP in whole Pixels detector for all events in low range", xbins=100, xmin=0, xmax=100)
        mbGroup.defineHistogram( "PixelSPHigh", title="Number of SP in whole Pixels detector for all events in high range", xbins=100, xmin=0, xmax=30000)
        mbGroup.defineHistogram( "PixBarr_SP", title="Number of SP for all events in Barrel", xbins=100, xmin=0, xmax=30000)
        mbGroup.defineHistogram( "PixECA_SP", title="Number of SP for all events in ECA", xbins=100, xmin=0, xmax=30000)
        mbGroup.defineHistogram( "PixECC_SP", title="Number of SP for all events in ECC", xbins=100, xmin=0, xmax=30000)
        mbGroup.defineHistogram( "SctTot", title="Number of SP in whole SCT detector for all events", xbins=100, xmin=0, xmax=120000)
        mbGroup.defineHistogram( "SctBarr_SP", title="Number of SCT_SP for all events in Barrel", xbins=100, xmin=0, xmax=50000)
        mbGroup.defineHistogram( "SctECA_SP", title="Number of SCT_SP for all events in ECA", xbins=100, xmin=0, xmax=50000)
        mbGroup.defineHistogram( "SctECC_SP", title="Number of SCT_SP for all events in ECC", xbins=100, xmin=0, xmax=50000)
        mbGroup.defineHistogram( "nTrk", title="Number of offline reconstructed Trk for all events", xbins=100, xmin=0, xmax=30000)
        mbGroup.defineHistogram( "xaodnTrk", title="Number of offline reconstructed xaod Trk for all events", xbins=100, xmin=0, xmax=30000)
        mbGroup.defineHistogram( "nTrk,xaodnTrk",type = 'TH2F', title="nTrk;xaodnTrk", xbins=100, xmin=0, xmax=2000, ybins=100, ymin=0, ymax=2000)
        mbGroup.defineHistogram( "SctECA_SP,SctECC_SP",type = 'TH2F', title="SctECA_SP;SctECC_SP", xbins=100, xmin=0, xmax=30000, ybins=100, ymin=0, ymax=30000)
        mbGroup.defineHistogram( "PixECA_SP,PixECC_SP",type = 'TH2F', title="PixECA_SP;PixECC_SP", xbins=100, xmin=0, xmax=30000, ybins=100, ymin=0, ymax=30000)
        mbGroup.defineHistogram( "SctBarr_SP,PixBarr_SP",type = 'TH2F', title="SctBarr_SP;PixBarr_SP", xbins=100, xmin=0, xmax=120000, ybins=100, ymin=0, ymax=30000)
        mbEffGroup.defineHistogram( "decision,nTrk",type = 'TEfficiency', title="EfficiencyTracks;nTrk", xbins=100, xmin=0, xmax=30000)
        mbEffGroup.defineHistogram( "NumGoodOnlineTracks", title="NumGoodOnlineTracks", xbins=100, xmin=0, xmax=2000)
        mbEffGroup.defineHistogram( "NumGoodOfflineTracks", title="NumGoodOfflineTracks", xbins=100, xmin=0, xmax=2000)
        mbEffGroup.defineHistogram( "NumGoodOnlineTracks,NumGoodOfflineTracks",type = 'TH2F', title="NumGoodOnlineTracks;NumGoodOfflineTracks", xbins=100, xmin=0, xmax=2000, ybins=100, ymin=0, ymax=2000)
    return monConfig.result()
if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Constants import DEBUG
    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    # ConfigFlags.Input.Files = ['/afs/cern.ch/user/s/somadutt/public/testUPC2.AOD.pool.root'] #our file runs fine
    ConfigFlags.Input.Files = ['/afs/cern.ch/user/s/somadutt/public/AOD.pool.root'] #from  https://atlas-art-data.web.cern.ch/atlas-art-data/grid-output/master/Athena/x86_64-centos7-gcc8-opt/2020-02-10T2132//TrigAnalysisTest/test_trigAna_RDOtoT0Mon_mt1_grid/
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'TestMonitorOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    
    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(TrigMinBias(ConfigFlags))
    
    # If you want to turn on more detailed messages ...
    # cfg.getEventAlgo('HLTMinBiasMonAlgMT').OutputLevel = 2 # DEBUG #either this line or the next works!!
    cfg.getEventAlgo('HLTMinBiasMonAlgMT').OutputLevel = DEBUG # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info
    cfg.run() #use cfg.run(20) to only run on first 20 events
    # to run:
    # python -m TrigMinBiasMonitoring.TrigMinBiasMonitoringMT 
