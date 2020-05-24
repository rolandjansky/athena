#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigBjetMonitorAlgorithm.py
@author E. Nagy
@author T. Bold
@date 2019-04-16
@brief Example trigger python configuration for the Run III AthenaMonitoring package, based on the example by C Burton and P Onyisi
'''

def TrigBjetMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    # EN: not needed here now
    # from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    # result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigBjetAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    #The added algorithm must exist as a .h file 

    from TrigBjetMonitoring.TrigBjetMonitoringConf import TrigBjetMonitorAlgorithm
    trigBjetMonAlg = helper.addAlgorithm(TrigBjetMonitorAlgorithm,'TrigBjetMonAlg')

    # You can actually make multiple instances of the same algorithm and give 
    # them different configurations
    shifterTrigBjetMonAlg = helper.addAlgorithm(TrigBjetMonitorAlgorithm,'ShifterTrigBjetMonAlg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # trigBjetMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    #trigBjetMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'
    #trigBjetMonAlg.TriggerChain = 'HLT_e24_lhtight_nod0'
    trigBjetMonAlg.TriggerChain = ''

    ### STEP 4 ###
    # Read in the Bjet trigger chain names

    # Directly from TrigBjetMonitCategory
    #from TrigBjetMonitoring import TrigBjetMonitCategory
    #bjet_triglist = TrigBjetMonitCategory.monitoring_bjet

    # From the hltmonList where TrigHLTMonitoring filtered the run type
    from TrigHLTMonitoring.HLTMonTriggerList import hltmonList
    bjet_triglist = hltmonList.monitoring_bjet
    bjet_triglist += hltmonList.monitoring_mujet
    # print " ==> bjet_triglist: ", bjet_triglist

    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # First, add a tool that's set up by a different configuration function. 
    # # In this case, CaloNoiseToolCfg returns its own component accumulator, 
    # # which must be merged with the one from this function.
    # from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    # caloNoiseAcc, caloNoiseTool = CaloNoiseToolCfg(inputFlags)
    # result.merge(caloNoiseAcc)
    # trigBjetMonAlg.CaloNoiseTool = caloNoiseTool

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # trigBjetMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    myGroup = helper.addGroup(
        trigBjetMonAlg,
        'TrigBjetMonitor',
        'HLT/BjetMon/Expert/'
    )

    # Add a GMT for the other example monitor algorithm
    shifterGroup = helper.addGroup(shifterTrigBjetMonAlg,'TrigBjetMonitor','HLT/BjetMon/Shifter/')

    ### STEP 5 ###
    # Configure histograms
    #NB! The histograms defined here must match the ones in the cxx file exactly

    # Offline PV histograms - common for all trigger chains

    shifterGroup.defineHistogram('Off_NVtx', title='Number of Offline Vertices;NVtx;Events',
                                 path='Offline',xbins=100,xmin=0.0,xmax=100.)
    shifterGroup.defineHistogram('Off_xVtx', title='Offline xVtx;xVtx;Events',
                                 path='Offline',xbins=200,xmin=-1.5,xmax=+1.5)
    shifterGroup.defineHistogram('Off_yVtx', title='Offline yVtx;yVtx;Events',
                                 path='Offline',xbins=200,xmin=-1.5,xmax=+1.5)
    shifterGroup.defineHistogram('Off_zVtx', title='Offline zVtx;zVtx;Events',
                                 path='Offline',xbins=200,xmin=-200.0,xmax=+200.0)

    # Histograms which depend on the trigger chain


    AllChains = []
    for chain in bjet_triglist :
        AllChains.append(chain[2:])
        # print " inside bjet_triglist chain[2:8] : " , chain[2:8]
        if chain[2:8] == 'HLT_mu' : # mu-jets
            # print "        mu-jet histogram is defined for ", chain[2:]

            HistName = 'jetPt_' + chain[2:]
            if chain[0:1] == "E" :
                myGroup.defineHistogram(HistName, title='Distribution of Pt_jet;Pt_jet;Events',
                                        path=chain[2:],xbins=100,xmin=-0.0,xmax=750.0)
                # print " ==> histogam ",HistName," is defined for myGroup"
            if chain[0:1] == "S" :
                shifterGroup.defineHistogram(HistName, title='Distribution of Pt_jet;Pt_jet;Events',
                                             path=chain[2:],xbins=100,xmin=-0.0,xmax=750.0)
                # print " ==> histogam ",HistName," is defined for shifterGroup"

            continue
        else :                      # b-jets
            # print "        b-jet histogram is defined for ", chain[2:]

            HistName = 'PVz_tr_' + chain[2:]
            if chain[0:1] == "E" :
                myGroup.defineHistogram(HistName, title='Distribution of online zPV;zPV;Events',
                                        path=chain[2:],xbins=200,xmin=-200.0,xmax=200.0)
                # print " ==> histogam ",HistName," is defined for myGroup"
            if chain[0:1] == "S" :
                shifterGroup.defineHistogram(HistName, title='Distribution of online zPV;zPV;Events',
                                             path=chain[2:],xbins=200,xmin=-200.0,xmax=200.0)
                # print " ==> histogam ",HistName," is defined for shifterGroup"

            HistName = 'd0_' + chain[2:]
            if chain[0:1] == "E" :
                myGroup.defineHistogram(HistName, title='Distribution of d0;d0;Events',
                                        path=chain[2:],xbins=200,xmin=-2.0,xmax=2.0)
                # print " ==> histogam ",HistName," is defined for myGroup"
            if chain[0:1] == "S" :
                shifterGroup.defineHistogram(HistName, title='Distribution of d0;d0;Events',
                                             path=chain[2:],xbins=200,xmin=-2.0,xmax=2.0)
                # print " ==> histogam ",HistName," is defined for shifterGroup"

            HistName = 'jetPt_' + chain[2:]
            if chain[0:1] == "E" :
                myGroup.defineHistogram(HistName, title='Distribution of Pt_jet;Pt_jet;Events',
                                        path=chain[2:],xbins=100,xmin=-0.0,xmax=750.0)
                # print " ==> histogam ",HistName," is defined for myGroup"
            if chain[0:1] == "S" :
                shifterGroup.defineHistogram(HistName, title='Distribution of Pt_jet;Pt_jet;Events',
                                             path=chain[2:],xbins=100,xmin=-0.0,xmax=750.0)
                # print " ==> histogam ",HistName," is defined for shifterGroup"

            HistName = 'wMV2c20_' + chain[2:]
            if chain[0:1] == "E" :
                myGroup.defineHistogram(HistName, title='Distribution of MV2c20 discriminant;MV2c20;Events',
                                        path=chain[2:],xbins=200,xmin=-1.0,xmax=1.0)
                # print " ==> histogam ",HistName," is defined for myGroup"
            if chain[0:1] == "S" :
                shifterGroup.defineHistogram(HistName, title='Distribution of MV2c20 discriminant;MV2c20;Events',
                                             path=chain[2:],xbins=200,xmin=-1.0,xmax=1.0)
                # print " ==> histogam ",HistName," is defined for shifterGroup"
            continue


    # print " ==> In TrigBjetMonitorAlgorithm.py: AllChains list: ", AllChains
    trigBjetMonAlg.AllChains = AllChains
    shifterTrigBjetMonAlg.AllChains = AllChains



    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    return helper.result()
    
    # # Otherwise, merge with result object and return
    # acc = helper.result()
    # result.merge(acc)
    # return result

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

    # Original data input file from P.Onyisi and C.Burton:
    #nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    #file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    #ConfigFlags.Input.Files = [nightly+file]
    #ConfigFlags.Input.isMC = False

    # MC file found by me but can be used only w/ asetup Athena,r2019-06-28,master
    #nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    #file = 'mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.merge.AOD.e5458_s3126_r9364_r9315/AOD.11182705._000001.pool.root.1'
    #ConfigFlags.Input.Files = [nightly+file]
    #ConfigFlags.Input.isMC = True

    # MC input files proposed by Tim Martin in https://its.cern.ch/jira/browse/ATR-19881 for Run-3
    # file = '/afs/cern/ch/user/t/tamartin/public/ESD.pool.root'
    # file = '/afs/cern/ch/user/t/tamartin/public/AOD.pool.root'
    # file = '/afs/cern.ch/work/e/enagy/public/Run3TrigFeatureAccessTest_1/run/legacy.AOD.pool.root'
    file = '/afs/cern.ch/work/e/enagy/public/Run3TrigFeatureAccessTest_1/run/mt.AOD.pool.root'
    ConfigFlags.Input.Files = [file]
    ConfigFlags.Input.isMC = True

    # Output file (root)

    ConfigFlags.Output.HISTFileName = 'TrigBjetMonitorOutput.root'

    ConfigFlags.Trigger.triggerMenuSetup="Physics_pp_v7_primaries"
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigBjetMonitorAcc = TrigBjetMonConfig(ConfigFlags)
    cfg.merge(trigBjetMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigBjetMonitorAcc.getEventAlgo('TrigBjetMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    Nevents = 25
    #cfg.run(Nevents)
    cfg.run() #use cfg.run(20) to only run on first 20 events

