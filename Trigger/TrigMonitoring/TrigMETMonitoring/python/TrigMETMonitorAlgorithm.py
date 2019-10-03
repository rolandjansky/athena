#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigMETMonitoringAlgorithm.py
@author K. Hamano
@author G. Gallardo
@date 2019-05-13
@brief MET Trigger python configuration for Run 3 AthenaMonitoring package

'''


def TrigMETMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigMETAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    #The added algorithm must exist as a .h file 

    from TrigMETMonitoring.TrigMETMonitoringConf import TrigMETMonitorAlgorithm
    TrigMETMonAlg = helper.addAlgorithm(TrigMETMonitorAlgorithm,'TrigMETMonAlg')

    # You can actually make multiple instances of the same algorithm and give 
    # them different configurations
    TrigMETMonChainAlg = helper.addAlgorithm(TrigMETMonitorAlgorithm,'TrigMETMonChainAlg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # expertTrigMETMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    # TrigMETMonAlg.TriggerChain = 'HLT_xe30_cell_L1XE10'
    # possible chains as of Sep 2019 are
    #    L1_XE10,L1_XE50,
    #    HLT_xe30_cell_L1XE10,HLT_xe30_cell_xe30_tcpufit_L1XE10,HLT_xe30_tcpufit_L1XE10,HLT_xe65_cell_L1XE50,
    #    HLT_xe110_pufit_xe65_L1XE50,HLT_xe110_pufit_xe70_L1XE50,
    #    HLT_xe110_pufit_xe65_L1gXERHO50,HLT_xe110_pufit_xe65_L1gXEPUFIT50,
    #    HLT_xe0noL1_l2fsperf_L1gXERHO50,HLT_xe0noL1_l2fsperf_L1gXEPUFIT50,
    #    HLT_xe0noL1_l2fsperf_pufit_L1gXERHO50,HLT_xe0noL1_l2fsperf_pufit_L1gXEPUFIT50
    # without filters, all events are processed.
    TrigMETMonAlg.TriggerChain = 'L1_XE10'
    TrigMETMonChainAlg.TriggerChain = 'HLT_xe30_tcpufit_L1XE10'
    

    ### use the follwoing if you run on Run2 AOD
    #TrigMETMonAlg.hlt_cell_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET'
    #TrigMETMonAlg.hlt_mt_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht'
    #TrigMETMonAlg.hlt_tc_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl'
    #TrigMETMonAlg.hlt_tcpufit_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC'
       


    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # First, add a tool that's set up by a different configuration function. 
    # # In this case, CaloNoiseToolCfg returns its own component accumulator, 
    # # which must be merged with the one from this function.
    # from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    # caloNoiseAcc, caloNoiseTool = CaloNoiseToolCfg(inputFlags)
    # result.merge(caloNoiseAcc)
    # expertTrigMETMonAlg.CaloNoiseTool = caloNoiseTool

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # expertTrigMETMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    metGroup = helper.addGroup(TrigMETMonAlg,'TrigMETMonitor','HLT/METMon/')

    # Add a GMT for the other example monitor algorithm
    metChainGroup = helper.addGroup(TrigMETMonChainAlg,'TrigMETMonitor','HLT/METMon/Chain/')

    ### STEP 5 ###
    # Configure histograms
    #NB! The histograms defined here must match the ones in the cxx file exactly
    metGroup.defineHistogram('L1_Ex',title='L1 Missing E_{x};E_{x} (GeV);Events',
                                 path='Shifter/L1',xbins=199,xmin=-298.5,xmax=298.5)
    metGroup.defineHistogram('L1_Ey',title='L1 Missing E_{y};E_{y} (GeV);Events',
                                 path='Shifter/L1',xbins=199,xmin=-298.5,xmax=298.5)
    metGroup.defineHistogram('L1_Et',title='L1 Missing E_{T};E_{T} (GeV);Events',
                                 path='Shifter/L1',xbins=205,xmin=-13.5,xmax=401.5)
    metGroup.defineHistogram('cell_Ex',title='cell Missing E_{x};E_{x} (GeV);Events',
                                 path='Shifter/cell',xbins=199,xmin=-298.5,xmax=298.5)
    metGroup.defineHistogram('cell_Ey',title='cell Missing E_{y};E_{y} (GeV);Events',
                                 path='Shifter/cell',xbins=199,xmin=-298.5,xmax=298.5)
    metGroup.defineHistogram('cell_Et',title='cell Missing E_{T};E_{T} (GeV);Events',
                                 path='Shifter/cell',xbins=205,xmin=-13.5,xmax=401.5)
    metGroup.defineHistogram('tcpufit_Ex',title='tcpufit Missing E_{x};E_{x} (GeV);Events',
                                 path='Shifter/tcpufit',xbins=199,xmin=-298.5,xmax=298.5)
    metGroup.defineHistogram('tcpufit_Ey',title='tcpufit Missing E_{y};E_{y} (GeV);Events',
                                 path='Shifter/tcpufit',xbins=199,xmin=-298.5,xmax=298.5)
    metGroup.defineHistogram('tcpufit_Et',title='tcpufit Missing E_{T};E_{T} (GeV);Events',
                                 path='Shifter/tcpufit',xbins=205,xmin=-13.5,xmax=401.5)
    metGroup.defineHistogram('mht_Ex',title='mht Missing E_{x};E_{x} (GeV);Events',
                                path='Expert/mht',xbins=199,xmin=-298.5,xmax=298.5)
    metGroup.defineHistogram('mht_Ey',title='mht Missing E_{y};E_{y} (GeV);Events',
                                path='Expert/mht',xbins=199,xmin=-298.5,xmax=298.5)
    metGroup.defineHistogram('mht_Et', title='mht E_{T};E_{T} (GeV);Events',
                                path='Expert/mht',xbins=205,xmin=-13.5,xmax=401.5)
    metGroup.defineHistogram('tc_Ex',title='tc Missing E_{x};E_{x} (GeV);Events',
                                path='Expert/tc',xbins=199,xmin=-298.5,xmax=298.5)
    metGroup.defineHistogram('tc_Ey',title='tc Missing E_{y};E_{y} (GeV);Events',
                                path='Expert/tc',xbins=199,xmin=-298.5,xmax=298.5)
    metGroup.defineHistogram('tc_Et', title='tc E_{T};E_{T} (GeV);Events',
                                path='Expert/tc',xbins=205,xmin=-13.5,xmax=401.5)
    metChainGroup.defineHistogram('tcpufit_Ex',title='tcpufit Missing E_{x};E_{x} (GeV);Events',
                                 path='HLT_xe30_tcpufit_L1XE10/tcpufit',xbins=199,xmin=-298.5,xmax=298.5)
    metChainGroup.defineHistogram('tcpufit_Ey',title='tcpufit Missing E_{y};E_{y} (GeV);Events',
                                 path='HLT_xe30_tcpufit_L1XE10/tcpufit',xbins=199,xmin=-298.5,xmax=298.5)
    metChainGroup.defineHistogram('tcpufit_Et',title='tcpufit Missing E_{T};E_{T} (GeV);Events',
                                 path='HLT_xe30_tcpufit_L1XE10/tcpufit',xbins=205,xmin=-13.5,xmax=401.5)

    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    return helper.result()
    
if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    #ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.Files = ['/hep300/data/khamano/data18_athenaMT/fromElin/AOD.pool.root']
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'TrigMETMonitorOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigMETMonitorAcc = TrigMETMonConfig(ConfigFlags)
    cfg.merge(trigMETMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigMETMonitorAcc.getEventAlgo('TrigMETMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events

