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
    expertTrigMETMonAlg = helper.addAlgorithm(TrigMETMonitorAlgorithm,'TrigMETMonAlg')

    # You can actually make multiple instances of the same algorithm and give 
    # them different configurations
    shifterTrigMETMonAlg = helper.addAlgorithm(TrigMETMonitorAlgorithm,'ShifterTrigMETMonAlg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # expertTrigMETMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    #expertTrigMETMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if TriggerFlags.EDMDecodingVersion==3:
        expertTrigMETMonAlg.TriggerChain = 'HLT_xe30_cell_L1XE10'
        expertTrigMETMonAlg.hlt_cell_key = 'HLT_MET'
        shifterTrigMETMonAlg.hlt_cell_key = 'HLT_MET'
        TrigMETMonitorAlgorithm.hlt_cell_key = 'HLT_MET'
    else:
        expertTrigMETMonAlg.TriggerChain = 'HLT_xe110_pufit_xe65_L1XE50'
        TrigMETMonitorAlgorithm.hlt_cell_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET'
        TrigMETMonitorAlgorithm.hlt_mt_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht'
        TrigMETMonitorAlgorithm.hlt_tc_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl'
        TrigMETMonitorAlgorithm.hlt_tcpufit_key = 'HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC'
        


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
    expertGroup = helper.addGroup(expertTrigMETMonAlg,'TrigMETMonitor','HLT/METMon/Expert/')

    # Add a GMT for the other example monitor algorithm
    shifterGroup = helper.addGroup(shifterTrigMETMonAlg,'TrigMETMonitor','HLT/METMon/Shifter/')

    ### STEP 5 ###
    # Configure histograms
    #NB! The histograms defined here must match the ones in the cxx file exactly
    shifterGroup.defineHistogram('L1_Ex',title='L1 Missing E_{x};E_{x} (GeV);Events',
                            path='L1',xbins=199,xmin=-298.5,xmax=298.5)
    shifterGroup.defineHistogram('L1_Ey',title='L1 Missing E_{y};E_{y} (GeV);Events',
                            path='L1',xbins=199,xmin=-298.5,xmax=298.5)
    shifterGroup.defineHistogram('L1_Et',title='L1 Missing E_{T};E_{T} (GeV);Events',
                            path='L1',xbins=205,xmin=-13.5,xmax=401.5)
    shifterGroup.defineHistogram('cell_Ex',title='cell Missing E_{x};E_{x} (GeV);Events',
                            path='cell',xbins=199,xmin=-298.5,xmax=298.5)
    shifterGroup.defineHistogram('cell_Ey',title='cell Missing E_{y};E_{y} (GeV);Events',
                            path='cell',xbins=199,xmin=-298.5,xmax=298.5)
    shifterGroup.defineHistogram('cell_Et',title='cell Missing E_{T};E_{T} (GeV);Events',
                            path='cell',xbins=205,xmin=-13.5,xmax=401.5)
    shifterGroup.defineHistogram('tcpufit_Ex',title='tcpufit Missing E_{x};E_{x} (GeV);Events',
                            path='tcpufit',xbins=199,xmin=-298.5,xmax=298.5)
    shifterGroup.defineHistogram('tcpufit_Ey',title='tcpufit Missing E_{y};E_{y} (GeV);Events',
                            path='tcpufit',xbins=199,xmin=-298.5,xmax=298.5)
    shifterGroup.defineHistogram('tcpufit_Et',title='tcpufit Missing E_{T};E_{T} (GeV);Events',
                            path='tcpufit',xbins=205,xmin=-13.5,xmax=401.5)
    expertGroup.defineHistogram('mht_Ex',title='mht Missing E_{x};E_{x} (GeV);Events',
                         path='mht',xbins=199,xmin=-298.5,xmax=298.5)
    expertGroup.defineHistogram('mht_Ey',title='mht Missing E_{y};E_{y} (GeV);Events',
                         path='mht',xbins=199,xmin=-298.5,xmax=298.5)
    expertGroup.defineHistogram('mht_Et', title='mht E_{T};E_{T} (GeV);Events',
                            path='mht',xbins=205,xmin=-13.5,xmax=401.5)
    expertGroup.defineHistogram('tc_Ex',title='tc Missing E_{x};E_{x} (GeV);Events',
                         path='tc',xbins=199,xmin=-298.5,xmax=298.5)
    expertGroup.defineHistogram('tc_Ey',title='tc Missing E_{y};E_{y} (GeV);Events',
                         path='tc',xbins=199,xmin=-298.5,xmax=298.5)
    expertGroup.defineHistogram('tc_Et', title='tc E_{T};E_{T} (GeV);Events',
                            path='tc',xbins=205,xmin=-13.5,xmax=401.5)

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

