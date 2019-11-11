#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file tauMonitorAlgorithm.py
@author C. D. Burton
@author P. Onyisi
@date 2018-01-11
@brief tau python configuration for the Run III AthenaMonitoring package
'''

def tauMonitoringConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    cfgHelper = AthMonitorCfgHelper(inputFlags, monName='tauMonitorAlgFamily')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    from tauMonitoring.tauMonitoringConf import tauMonitorAlgorithm
    tauMonAlgBA = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgBA')
    tauMonAlgCR = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgCR')
    tauMonAlgEC = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgEC')



    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # exampleMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    #exampleMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    tauMonAlgBA.etaMin = -1.
    tauMonAlgBA.etaMax = 1.3
    tauMonAlgCR.etaMin = 1.3
    tauMonAlgCR.etaMax = 1.7
    tauMonAlgEC.etaMin = 1.7
    tauMonAlgEC.etaMax = 3.5

    tauMonAlgBA.kinGroupName = 'tauMonKinGroupBA'
    tauMonAlgCR.kinGroupName = 'tauMonKinGroupCR'
    tauMonAlgEC.kinGroupName = 'tauMonKinGroupEC'

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.


    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    myKinGroupBA = cfgHelper.addGroup(alg=tauMonAlgBA, name='tauMonKinGroupBA', topPath='Tau/BA/' )
    myKinGroupCR = cfgHelper.addGroup(alg=tauMonAlgCR, name='tauMonKinGroupCR', topPath='Tau/CR/' )
    myKinGroupEC = cfgHelper.addGroup(alg=tauMonAlgEC, name='tauMonKinGroupEC', topPath='Tau/EC/' )

    # Add a GMT for the other example monitor algorithm
    # anotherGroup = cfgHelper.addGroup(anotherTauMonAlg,'tauMonitor')


    ### STEP 5 ###
    # Configure histograms
    
    for itup in [(myKinGroupBA,'BA'),(myKinGroupCR,'CR'),(myKinGroupEC,'EC')]:
        (igroup, postfix) = itup

	### 1 ###

	### 2 ###
        igroup.defineHistogram('ntaus', title='Number of tau candidates;Number of Taus per Event', path='Kinematics', 
                               xbins=30, xmin=0., xmax=30.)


	### 3 ###
        igroup.defineHistogram('eta', title='Eta of tau candidates);Eta);Number of Candidates', path='Kinematics', 
                               xbins=40, xmin=-2., xmax=2.)

	### 4 ###
        igroup.defineHistogram('phi', title='Phi of tau candidates);Phi);Number of Candidates', path='Kinematics', 
                               xbins=80, xmin=-4., xmax=4.)

	### 5 ###

        igroup.defineHistogram('pt', title='Pt of tau candidates);pt);Number of Candidates', path='Kinematics', 
                               xbins=300, xmin=0., xmax=100000.)



	### 6 ###
        igroup.defineHistogram('charge', title='Charge of tau candidates;Charge;Number of Candidates', path='Kinematics', 
                               xbins=120, xmin=-6., xmax=6.)

        ### 7 ###
	igroup.defineHistogram('nTracks', title='nTracks;nTracks;Events', path='Kinematics', 
                               xbins=10, xmin=0., xmax=10.)

	### 8 ###
	igroup.defineHistogram('nClusters', title='nClusters;nClusters;Events', path='Kinematics', 
                               xbins=40, xmin=0., xmax=40.)



	### 9 ### ?????
        igroup.defineHistogram('pt15', title='Pt15 of tau candidates);pt);Number of Candidates', path='Kinematics', 
                               xbins=300, xmin=0., xmax=100000.)


 	
	### 10 ###
        igroup.defineHistogram('pt,eta', type='TH2F', title='PtVsEtaTitle;#eta;pt', path='Kinematics', 
                               xbins=300.,xmin=0.,xmax=100000.,ybins=40.,ymin=-2.,ymax=2.)



	### 11 ###
        igroup.defineHistogram('pt,phi', type='TH2F', title='PtVsPhiTitle;#eta;#phi',path='Kinematics', 
                               xbins=300.,xmin=0.,xmax=100000.,ybins=80.,ymin=-4.,ymax=4.)



	### 12 ###
        igroup.defineHistogram('eta,phi', type='TH2F', title='PhiVsEtaTitle;#eta;#phi', path='Kinematics', 
                               xbins=30,xmin=-3.0,xmax=3.0,ybins=32,ymin=-3.15,ymax=3.15)

	### 13 ###
        igroup.defineHistogram('eta15,phi15', type='TH2F', title='PhiVsEtaTitle;#eta;#phi', path='Kinematics', 
                               xbins=30,xmin=-3.0,xmax=3.0,ybins=32,ymin=-3.15,ymax=3.15)



	### 14 ###



    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)

    return cfgHelper.result()
    
    # # Otherwise, merge with result object and return
    # acc = cfgHelper.result()
    # result.merge(acc)
    # return result

if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'tauRegions.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    exampleMonitorAcc = tauMonitoringConfig(ConfigFlags)   # calls the main function above
    cfg.merge(exampleMonitorAcc)

    # If you want to turn on more detailed messages ...
    exampleMonitorAcc.getEventAlgo('tauMonAlgBA').OutputLevel = 2 # DEBUG
    exampleMonitorAcc.getEventAlgo('tauMonAlgCR').OutputLevel = 2 # DEBUG
    exampleMonitorAcc.getEventAlgo('tauMonAlgEC').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events
