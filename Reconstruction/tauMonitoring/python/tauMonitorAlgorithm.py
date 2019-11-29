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
    tauMonAlgGlobal = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgGlobal')


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
    tauMonAlgGlobal.kinGroupName = 'tauMonKinGroupGlobal'


    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.


    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    myKinGroupBA = cfgHelper.addGroup(alg=tauMonAlgBA, name='tauMonKinGroupBA', topPath='Tau/BA/' )
    myKinGroupCR = cfgHelper.addGroup(alg=tauMonAlgCR, name='tauMonKinGroupCR', topPath='Tau/CR/' )
    myKinGroupEC = cfgHelper.addGroup(alg=tauMonAlgEC, name='tauMonKinGroupEC', topPath='Tau/EC/' )
    myKinGroupGlobal = cfgHelper.addGroup(alg=tauMonAlgGlobal, name='tauMonKinGroupGlobal', topPath='Tau/' )

    # Add a GMT for the other example monitor algorithm
    # anotherGroup = cfgHelper.addGroup(anotherTauMonAlg,'tauMonitor')


    ### STEP 5 ###
    # Configure histograms
    for itup in [(myKinGroupBA,'BA'),(myKinGroupCR,'CR'),(myKinGroupEC,'EC'),(myKinGroupGlobal,'Global')]:
        (igroup, postfix) = itup

        folder = ""

        igroup.defineHistogram('nTauCandidates', title='Number of tau candidates;Number of Taus per Event', 
                               xbins=40, xmin=0., xmax=40.)

        igroup.defineHistogram('tauCharge', title='Charge of tau candidates;Charge;Number of Candidates',
                               xbins=11, xmin=-5.5, xmax=5.5)

        igroup.defineHistogram('tauEt', title='Pt of tau candidates;Transverse Energy (GeV);Number of Candidates',
                               xbins=60, xmin=0., xmax=300.)

        igroup.defineHistogram('tauEta', title='Eta of tau candidates);Eta);Number of Candidates',
                               xbins=51, xmin=-2.55, xmax=2.55)

        igroup.defineHistogram('LB,tauEta', type='TH2F', title='tauEtaVsLB;LumiBlock;eta', 
                               xbins=1200,xmin=0,xmax=1200,ybins=51,ymin=-2.55,ymax=2.55)

        igroup.defineHistogram('tauPhi', title='Phi of tau candidates);Phi);Number of Candidates',
                               xbins=65, xmin=-3.1415926-0.098174/2., xmax=3.1415926+0.098174/2.)

        igroup.defineHistogram('tauEta,tauPhi', type='TH2F', title='PhiVsEtaTitle;#eta;#phi', 
                               xbins=30,xmin=-2.55,xmax=2.55,ybins=32,ymin=-3.15,ymax=3.15)

        igroup.defineHistogram('LB,tauPhi', type='TH2F', title='tauEtaVsLB;LumiBlock;#phi', 
                               xbins=1200,xmin=0,xmax=1200,ybins=80.,ymin=-4.,ymax=4.)

        igroup.defineHistogram('LB,tauEt', type='TH2F', title='tauEtaVsLB;LumiBlock;#Transverse Energy (GeV)', 
                               xbins=1200,xmin=0,xmax=1200,ybins=300.,ymin=0.,ymax=300.)



        if postfix != 'Global':

            igroup.defineHistogram('EMRadius', title='Uncalibrated EM Radius;EM Radius;Number Of Candidates',path=folder+"Calo",
            xbins=50, xmin=0., xmax=1.2 )

            igroup.defineHistogram('hadRadius', title='Hadronic Radius of tau candidates;Hadronic Radius; Number Of Candidates',path=folder+"Calo",
            xbins=50, xmin=0., xmax=1. )

            igroup.defineHistogram('stripWidth2', title='Strip Width of tau candidates;Strip Width;Number of Candidates',path=folder+"Calo",
            xbins=50, xmin=-0.1, xmax=0.12 )

            igroup.defineHistogram('isolFrac', title='Isolation Fraction;Et Isolation Fraction;Number of Candidates',path=folder+"Calo",
            xbins=51, xmin=0.0, xmax=1.02 )

            igroup.defineHistogram('nStrip', title='Number of strip cells of tau candidates;Number of Strip Cells;Number of Candidates',path=folder+"Calo",
            xbins=56, xmin=-0.5, xmax=55.5 )

            igroup.defineHistogram('etEMAtEMScale', title='EM energy at the EM scale;EM Et (GeV) );Number of Candidates',path=folder+"Calo",
            xbins=50, xmin=0., xmax=200. )

            igroup.defineHistogram('etHadAtEMScale', title='Hadronic Energy at the EM Scale;Had Et (GeV) );Number of Candidates',path=folder+"Calo",
            xbins=50, xmin=0., xmax=200. )


            igroup.defineHistogram('centFrac,LB', type='TH2F', title='Centrality Fraction vs Lumiblock;Centrality Fraction;Lumiblock', path=folder+"Calo", 
            xbins=51,xmin=0,xmax=1.02,ybins=1200.,ymin=0.,ymax=1200.)

            igroup.defineHistogram('isolFrac,LB', type='TH2F', title='Isolation Fraction vs Lumiblock;Isolation Fraction;Lumiblock', path=folder+"Calo", 
            xbins=51,xmin=0,xmax=1.02,ybins=1200.,ymin=0.,ymax=1200.)


            igroup.defineHistogram('BDTJetScore', title='BDT Score for Jet Rejection;Boosted Decision Tree Score',path=folder+"Identification",
            xbins=48, xmin=-1.1, xmax=1.1 )

            igroup.defineHistogram('BDTJetScoreSigTrans', title='Signal Transformed BDT Score for Jet Rejection;Boosted Decision Tree Score',path=folder+"Identification",
            xbins=48, xmin=0, xmax=1.1 )

            igroup.defineHistogram('eleBDTMedium', title='Medium EleBDT',path=folder+"Identification",
            xbins=2, xmin=-0.5, xmax=1.5, labels=["False","True"])

            igroup.defineHistogram('eleBDTTight', title='Tight EleBDT',path=folder+"Identification",
            xbins=2, xmin=-0.5, xmax=1.5, labels=["False","True"])

            igroup.defineHistogram('muonVeto', title='Muon Veto',path=folder+"Identification",
            xbins=2, xmin=-0.5, xmax=1.5, labels=["False","True"] )

            igroup.defineHistogram('tauBDTLoose', title='Identification Flag: tauBDTLoose',path=folder+"Identification",
            xbins=2, xmin=-0.5, xmax=1.5 , labels=["False","True"])

            igroup.defineHistogram('tauBDTMedium', title='Identification Flag: tauBDTMedium',path=folder+"Identification",
            xbins=2, xmin=-0.5, xmax=1.5 ,labels=["False","True"])

            igroup.defineHistogram('tauBDTTight', title='Identification Flag: tauBDTTight',path=folder+"Identification",
            xbins=2, xmin=-0.5, xmax=1.5, labels=["False","True"])


            #Not in the monTool...but in histos I'm meant to be making
            igroup.defineHistogram('BDTJetScore', title='test',path=folder+"Identification/BDTLoose15GeV",
            xbins=48, xmin=-1.1, xmax=1.1 )


            #Not in the monTool...but in histos I'm meant to be making
            igroup.defineHistogram('PSSFrac', title='test',path=folder+"Identification/EleVetoBDTinputs",
            xbins=10, xmin=0, xmax=1. )

            #Substructure time!
            igroup.defineHistogram('pi0bdt', title='test',path=folder+"SubStructure",
            xbins=120, xmin=-0, xmax=1.2 )


            igroup.defineHistogram('EMFracTrk', title='Ratio of pt to shot electromagnetic energy for associated tracks; track pt ratio in EM',path=folder+"SubStructure",
            xbins=15, xmin=0, xmax=1.5 )

            igroup.defineHistogram('EfracL2EffCluster', title='Energy fraction of leading two effective clusters in shot; energy fraction',path=folder+"SubStructure",
            xbins=15, xmin=0, xmax=1.5 )

            igroup.defineHistogram('EisoEffCluster', title='Isolation Energy after correction in effective clusters ; isolation energy (GeV)',path=folder+"SubStructure",
            xbins=10, xmin=0, xmax=50. )

            igroup.defineHistogram('InvMassEffClusters', title='Invariant mass of effective clusters in shot; invariant mass (GeV)',path=folder+"SubStructure",
            xbins=40, xmin=0, xmax=8. )

            igroup.defineHistogram('nNeutPFO', title='_NumNeutPFO;Number of neutral ParticleFlow objects ; PFO number',path=folder+"SubStructure",
            xbins=20, xmin=0, xmax=20. )

            igroup.defineHistogram('nShot', title='number of shots ; shot number ',path=folder+"SubStructure",
            xbins=20, xmin=0, xmax=20. )





        if postfix == 'Global':

            igroup.defineHistogram('NumTracks', title='NumTracks;nTracks;Events',
                      xbins=21, xmin=-0.5, xmax=20.5)

            igroup.defineHistogram('nClusters', title='nClusters;nClusters;Events',
                      xbins=40, xmin=0., xmax=40. )

            igroup.defineHistogram('tauEta,tauEt', type='TH2F', title='EtVsEta;eta;Transverse Energy (Gev)',
                      xbins=40.,xmin=-2.55,xmax=2.55 ,ybins=300.,ymin=0,ymax=300)

            igroup.defineHistogram('tauPhi,tauEt', type='TH2F', title='EtVsPhi;#phi;#eta;',
                      xbins=80.,xmin=-4.,xmax=4.,ybins=300.,ymin=-0.,ymax=300.)

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
    exampleMonitorAcc.getEventAlgo('tauMonAlgGlobal').OutputLevel = 2 # DEBUG

    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events
