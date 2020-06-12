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
    length = len(alg.triggerList)
    mbEffAllGroup.defineHistogram( "PurityPassed,whichTrigger",type = 'TEfficiency',title="PurityPassed;whichTrigger",xbins=length, xmin=0, xmax=length, xlabels = list(alg.triggerList))
    mbEffAllGroup.defineHistogram( "whichTrigger",title="count of triggers;HLT", xbins=length, xmin=0, xmax=length, xlabels = list(alg.triggerList))
    mbEffAllGroup.defineHistogram(  "whichTrigger;No. of events", type='TH1I',title='Event per Trigger;HLT',xbins=length, xmin=0, xmax=length, xlabels = list(alg.triggerList))
    mbEffAllGroup.defineHistogram(  "decision,whichTrigger", type='TEfficiency',title='Efficiency of selecting Events with One Good Trk;TriggerName',xbins=length, xmin=0, xmax=length, xlabels = list(alg.triggerList))


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
        mbEffGroup.defineHistogram( "NumGoodOnlineTracks", title="NumGoodOnlineTracks", xbins=100, xmin=0, xmax=2000)
        mbEffGroup.defineHistogram( "NumGoodOfflineTracks", title="NumGoodOfflineTracks", xbins=100, xmin=0, xmax=2000)
        mbEffGroup.defineHistogram( "NumGoodOnlineTracks,NumGoodOfflineTracks",type = 'TH2F', title="NumGoodOnlineTracks;NumGoodOfflineTracks", xbins=100, xmin=0, xmax=2000, ybins=100, ymin=0, ymax=2000)
        mbEffGroup.defineHistogram( "decision,NumGoodOfflineTracks", type='TEfficiency',title='Efficiency;Offline Good nTrk',xbins=1000,xmin=0,xmax=1000)
        mbEffGroup.defineHistogram( "decision,nTrk",type = 'TEfficiency', title="Efficiency;nTrk", xbins=1000,xmin=0,xmax=1000)


    return monConfig.result()
if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Constants import DEBUG
    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    # ConfigFlags.Input.Files = ['/afs/cern.ch/user/s/somadutt/public/testUPC2.AOD.pool.root'] #Local HI-UPC file

    # data AOD file
    ConfigFlags.Input.Files = ['/eos/atlas/atlascerngroupdisk/data-art/build-output/master/Athena/x86_64-centos7-gcc8-opt/2020-06-01T2139/TrigP1Test/test_trigP1_v1PhysP1_T0Mon_build/AOD.pool.root']

    # ConfigFlags.Input.Files = ['/eos/atlas/atlascerngroupdisk/data-art/build-output/master/Athena/x86_64-centos7-gcc8-opt/2020-05-22T2142/TrigAnalysisTest/test_trigAna_RDOtoT0Mon_mt1_build/AOD.pool.root']
    # ConfigFlags.Input.isMC = True  #un-Comment this line for MC AOD files, comment for data-AOD files
    ConfigFlags.Output.HISTFileName = 'TestMonitorOutput.root'

    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)

    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(TrigMinBias(ConfigFlags))

    # If you want to turn on more detailed messages ...
    # cfg.getEventAlgo('HLTMinBiasMonAlgMT').OutputLevel = 2 # DEBUG #either this line or the next works!!
    cfg.getEventAlgo('HLTMinBiasMonAlgMT').OutputLevel = DEBUG # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info
    cfg.run() #use cfg.run(20) to only run on first 20 events
    # to run:
    # python -m TrigMinBiasMonitoring.TrigMinBiasMonitoringMT
