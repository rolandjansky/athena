#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''@file PFOMonitorAlgorithm.py
@author L. Sawyer
@date 2020-07-15
@python configuration for the Run III PArticle Flow Object (PFO) Monitoring package
'''

def PFOMonitoringConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # If you need to set up special tools, etc., you will need your own ComponentAccumulator;
    # uncomment the following 2 lines and use the last three lines of this function instead of the ones
    # just before
    # from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    # result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'PFOAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    # This uses the new Configurables object system.
    from AthenaConfiguration.ComponentFactory import CompFactory
    PFOMonAlg = helper.addAlgorithm(CompFactory.PFOMonitorAlgorithm,'PFOMonAlg')


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # exampleMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    # PFOMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'

    # PFO energy threshold in GeV  TBD!
    # PFOMonAlg.lowEt = 5.0
    # PFOMonAlg.hiEt = 20.0


    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # Add monitor groups - All. 
    pfosAllGroup = helper.addGroup(
        alg=PFOMonAlg,
        name='PFOMonitorAllPFOs',
        topPath='PFOs/AllPFOs/'
    )
    # Add monitor groups - Charged. 
    pfosChargedGroup = helper.addGroup(
        alg=PFOMonAlg,
        name='PFOMonitorChargedPFOs',
        topPath='PFOs/ChargedPFOs/'
    )
    # Add monitor groups - Neutral. 
    pfosNeutralGroup = helper.addGroup(
        alg=PFOMonAlg,
        name='PFOMonitorNeutralPFOs',
        topPath='PFOs/NeutralPFOs/'
    )

    #############################################
    # Configure histograms - All PFOs
    pfosAllGroup.defineHistogram('nPFOs',  title='No. of PFOs;N_{PFOs};Events',xbins=200,xmin=0.0,xmax=1000.0)    
    pfosAllGroup.defineHistogram('pfopT',  title='PFO transverse momentum;pT;Events',xbins=100,xmin=0.0,xmax=2000)
    pfosAllGroup.defineHistogram('pfoEta', title='PFO eta;eta;Events',xbins=98,xmin=-4.9,xmax=4.9)
    pfosAllGroup.defineHistogram('pfoPhi', title='PFO phi;phi;Events',xbins=64,xmin=-3.15,xmax=3.15)
    pfosAllGroup.defineHistogram('pfoMass',title='PFO energy;Mass;Events',xbins=100,xmin=0.0,xmax=500)
    pfosAllGroup.defineHistogram('pfoE',   title='PFO energy;E;Events',xbins=300,xmin=0.0,xmax=3000)
    pfosAllGroup.defineHistogram('pfoRapidity', title='PFO rapidity;y;Events',xbins=98,xmin=-4.9,xmax=4.9)

    pfosAllGroup.defineHistogram('pfoEta,pfoPhi,pfoE', type='TProfile2D', title='Average PFO Energy;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    pfosAllGroup.defineHistogram('pfoEta,pfoPhi,pfopT', type='TProfile2D', title='Average PFO Transverse Momentum;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    pfosAllGroup.defineHistogram('pfoEta,pfoPhi', type='TH2F',title='PFO Occupancy - No Threshold;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
  
    #############################################
    # Configure histograms - Charged PFOs
    pfosChargedGroup.defineHistogram('nChPFOs',  title='No. of Charged PFOs;N_{PFOs};Events',xbins=200,xmin=0.0,xmax=1000.0)    
    pfosChargedGroup.defineHistogram('ChpfopT',  title='Charged PFO transverse momentum;pT;Events',xbins=100,xmin=0.0,xmax=2000)
    pfosChargedGroup.defineHistogram('ChpfoEta', title='Charged PFO eta;eta;Events',xbins=98,xmin=-4.9,xmax=4.9)
    pfosChargedGroup.defineHistogram('ChpfoPhi', title='Charged PFO phi;phi;Events',xbins=64,xmin=-3.15,xmax=3.15)
    pfosChargedGroup.defineHistogram('ChpfoMass',title='Charged PFO energy;Mass;Events',xbins=100,xmin=0.0,xmax=500)
    pfosChargedGroup.defineHistogram('ChpfoE',   title='Charged PFO energy;E;Events',xbins=300,xmin=0.0,xmax=3000)
    pfosChargedGroup.defineHistogram('ChpfoDenseEnv',title='Charged PFO isInDenseEnvironment Flag',xbins=3,xmin=-1,xmax=2)
    pfosChargedGroup.defineHistogram('ChpfoExpE', title='Charged PFO Expected Energy From Tracks; E_{exp};Events', xbins=300,xmin=0.0,xmax=3000.0)
    pfosChargedGroup.defineHistogram('ChpfoExpE;DenseEnvFlagE', title='Charged PFO Expected Energy From Tracks - IsInDenseEvironmet=TRUE; E_{exp};Events',
                            xbins=200,xmin=0.0,xmax=2000.0, cutmask='DenseEnvFlag')

    pfosChargedGroup.defineHistogram('ChpfoEta,ChpfoPhi,ChpfoE', type='TProfile2D', title='Average Charged PFO Energy;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    pfosChargedGroup.defineHistogram('ChpfoExpE,ChpfoPhi,ChpfoE', type='TProfile2D', title='Average Charged PFO Expected Energy From Tracks;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    pfosChargedGroup.defineHistogram('ChpfoEta,ChpfoPhi,ChpfopT', type='TProfile2D', title='Average Charged PFO Transverse Monmentum;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    pfosChargedGroup.defineHistogram('ChpfoEta,ChpfoPhi', type='TH2F',title='Charged PFO Occupancy - No Threshold;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    pfosChargedGroup.defineHistogram('ChpfoEta,ChpfoPhi;DenseEnvFlagEtaPhi', type='TH2F',title='Charged PFO Occupancy - IsInDenseEvironmet=TRUE;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15, cutmask='DenseEnvFlag' )

    #############################################
    # Configure histograms - Neutral PFOs
    pfosNeutralGroup.defineHistogram('nNuPFOs',  title='No. of Neutral PFOs;N_{PFOs};Events',xbins=200,xmin=0.0,xmax=1000.0)    
    pfosNeutralGroup.defineHistogram('NupfopT',  title='Neutral PFO transverse momentum;pT;Events',xbins=100,xmin=0.0,xmax=2000)
    pfosNeutralGroup.defineHistogram('NupfoEta', title='Neutral PFO eta;eta;Events',xbins=98,xmin=-4.9,xmax=4.9)
    pfosNeutralGroup.defineHistogram('NupfoPhi', title='Neutral PFO phi;phi;Events',xbins=64,xmin=-3.15,xmax=3.15)
    pfosNeutralGroup.defineHistogram('NupfoMass',title='Neutral PFO energy;Mass;Events',xbins=100,xmin=0.0,xmax=500)
    pfosNeutralGroup.defineHistogram('NupfoE',   title='Neutral PFO energy;E;Events',xbins=300,xmin=0.0,xmax=3000)
    pfosNeutralGroup.defineHistogram('NupfoRapidity', title='Neutral PFO rapidity;y;Events',xbins=98,xmin=-4.9,xmax=4.9)
    pfosNeutralGroup.defineHistogram('NupfoIsolation', title='Neutral PFO Isolation;Iso;Events',xbins=100,xmin=-1.5,xmax=2.0)
    pfosNeutralGroup.defineHistogram('NupfoEPos',title='Neutral PFO Energy From Positive Cells;E_{pos};Events',xbins=500,xmin=0.0,xmax=10000.0)
    pfosNeutralGroup.defineHistogram('NupfoSecondR',title='Neutral PFO SECOND_R',xbins=60,xmin=-1.0,xmax=50.0)
    pfosNeutralGroup.defineHistogram('NupfoSecondLambda',title='Neutral PFO SECOND_LAMBDA',xbins=60,xmin=-1.0,xmax=3000.0)
    pfosNeutralGroup.defineHistogram('NupfoCenterLambda',title='Neutral PFO CENTER_LAMBDA',xbins=60,xmin=-50.0,xmax=3000.0)
    
    pfosNeutralGroup.defineHistogram('NupfoBadLarQFrac',title='Neutral PFO BADLARQ_FRAC',xbins=25,xmin=-1.0,xmax=1.5)
    pfosNeutralGroup.defineHistogram('NupfoAvgLarQ',title='Neutral PFO AVG_LAR_Q',xbins=31,xmin=-1000.0,xmax=30000.0)
    pfosNeutralGroup.defineHistogram('NupfoAvgTileQ',title='Neutral PFO _AVG_TILE_Q',xbins=21,xmin=-10.0,xmax=200.0)
    pfosNeutralGroup.defineHistogram('NupfoEMProb',title='Neutral PFO EM_PROBABILITY',xbins=21,xmin=-1.0,xmax=1.0)

    pfosNeutralGroup.defineHistogram('NupfoNBadCells',title='Neutral PFO N_BAD_CELLS',xbins=30,xmin=-1.0,xmax=2.0)
    pfosNeutralGroup.defineHistogram('NupfoEBadCells',title='Neutral PFO Energy From Bad Cells;E_{bad cells};Events',xbins=210,xmin=-10.0,xmax=100.0)

    # These quantities do not appear to be filled currently
    #pfosNeutralGroup.defineHistogram('NupfoSignificance', title='Neutral PFO Significance;Signif;Events',xbins=100,xmin=-100.0,xmax=100.0)
    #pfosNeutralGroup.defineHistogram('NupfoEFracMax',title='Neutral PFO Energy Fraction of Highest Cell;E_{frac};Events',xbins=200,xmin=-1.0,xmax=1.5)
    
    pfosNeutralGroup.defineHistogram('NupfoEta,NupfoPhi,NupfoE', type='TProfile2D', title='Average Neutral PFO Energy;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    pfosNeutralGroup.defineHistogram('NupfoEta,NupfoPhi,NupfopT', type='TProfile2D', title='Average Neutral PFO Transverse Momentum;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )
    pfosNeutralGroup.defineHistogram('NupfoEta,NupfoPhi', type='TH2F',title='Neutral PFO Occupancy - No Threshold;eta;phi',
                            xbins=98,xmin=-4.9,xmax=4.9,ybins=64,ymin=-3.15,ymax=3.15 )


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
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    import sys
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'PFOMonitorOutput.root'
    ConfigFlags.fillFromArgs(sys.argv[1:])
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesThreadedCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    pfoMonitorAcc = PFOMonitoringConfig(ConfigFlags)
    cfg.merge(pfoMonitorAcc)

    # If you want to turn on more detailed messages ...
    # pfoMonitorAcc.getEventAlgo('PFOMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=False) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events
