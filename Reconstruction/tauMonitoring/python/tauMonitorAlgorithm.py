#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file tauMonitorAlgorithm.py
@author C. D. Burton
@author P. Onyisi
@date 2018-01-11
@brief tau python configuration for the Run III AthenaMonitoring package
'''

from math import pi
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
    from AthenaConfiguration.ComponentFactory import CompFactory
    tauMonitorAlgorithm = CompFactory.tauMonitorAlgorithm
    tauMonAlgBA = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgBA')
    tauMonAlgCR = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgCR')
    tauMonAlgEC = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgEC')
    tauMonAlgGlobal = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgGlobal')
    tauMonAlgTauTrig = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgTauTrig')
    tauMonAlgEleTrig = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgEleTrig')
    tauMonAlgJetTrig = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgJetTrig')
    tauMonAlgHighPt = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgHighPt')
    tauMonAlgHighPtBDTLoose = cfgHelper.addAlgorithm( tauMonitorAlgorithm, name='tauMonAlgHighPtBDTLoose')




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
    tauMonAlgGlobal.etaMin = -100
    tauMonAlgGlobal.etaMax = 100
    tauMonAlgTauTrig.etaMin = -100
    tauMonAlgTauTrig.etaMax = 100
    tauMonAlgEleTrig.etaMin = -100
    tauMonAlgEleTrig.etaMax = 100
    tauMonAlgJetTrig.etaMin = -100
    tauMonAlgJetTrig.etaMax = 100
    tauMonAlgHighPt.etaMin = -100
    tauMonAlgHighPt.etaMax = 100
    tauMonAlgHighPtBDTLoose.etaMin = -100
    tauMonAlgHighPtBDTLoose.etaMax = 100

    tauMonAlgBA.kinGroupName = 'tauMonKinGroupBA'
    tauMonAlgCR.kinGroupName = 'tauMonKinGroupCR'
    tauMonAlgEC.kinGroupName = 'tauMonKinGroupEC'
    tauMonAlgGlobal.kinGroupName = 'tauMonKinGroupGlobal'
    tauMonAlgTauTrig.kinGroupName = 'tauMonKinGroupTauTrig'
    tauMonAlgEleTrig.kinGroupName = 'tauMonKinGroupEleTrig'
    tauMonAlgJetTrig.kinGroupName = 'tauMonKinGroupJetTrig'
    tauMonAlgHighPt.kinGroupName = 'tauMonKinGroupHighPt'
    tauMonAlgHighPtBDTLoose.kinGroupName = 'tauMonKinGroupHighPtBDTLoose'

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    myKinGroupBA = cfgHelper.addGroup(alg=tauMonAlgBA, name='tauMonKinGroupBA', topPath='Tau/TauB/' )
    myKinGroupCR = cfgHelper.addGroup(alg=tauMonAlgCR, name='tauMonKinGroupCR', topPath='Tau/TauCR/' )
    myKinGroupEC = cfgHelper.addGroup(alg=tauMonAlgEC, name='tauMonKinGroupEC', topPath='Tau/TauE/' )
    myKinGroupGlobal = cfgHelper.addGroup(alg=tauMonAlgGlobal, name='tauMonKinGroupGlobal', topPath='Tau/' )
    myKinGroupTauTrig = cfgHelper.addGroup(alg=tauMonAlgTauTrig, name='tauMonKinGroupTauTrig', topPath='Tau/Trigger/TauTrig' )
    myKinGroupEleTrig = cfgHelper.addGroup(alg=tauMonAlgEleTrig, name='tauMonKinGroupEleTrig', topPath='Tau/Trigger/EleTrig' )
    myKinGroupJetTrig = cfgHelper.addGroup(alg=tauMonAlgJetTrig, name='tauMonKinGroupJetTrig', topPath='Tau/Trigger/JetTrig' )

    myKinGroupHighPt = cfgHelper.addGroup(alg=tauMonAlgHighPt, name='tauMonKinGroupHighPt', topPath='Tau/' )
    myKinGroupHighPtBDTLoose = cfgHelper.addGroup(alg=tauMonAlgHighPtBDTLoose, name='tauMonKinGroupHighPtBDTLoose', topPath='Tau/' )


    naming= {
            'BA': "Tau_TauB_",
            'CR': "Tau_TauCR_",
            'EC': "Tau_TauE_",
            'Global': "",
            'HighPt': "",
            'HighPtBDTLoose': "",
            'EleTrig': "emTriggered_",
            'JetTrig': "jetTriggered_",
            'TauTrig': "tauTriggered_",
            }

    def namer(variables,histogramName, folder, algorithm):
        if folder != "":
            #This wouldn't have been necessary except the calo folder in the naming convention:
            #This adds the Tau prefix to the histograms in the calo folder
            if postfix == "Global" and (folder =="Calo" or folder=="Track"):
                return variables+";"+"Tau_"+naming[algorithm]+folder+"_"+histogramName
            else:
                return variables+";"+naming[algorithm]+folder+"_"+histogramName
        else:
            return variables+";"+naming[algorithm]+folder+histogramName

    # Add a GMT for the other example monitor algorithm
    # anotherGroup = cfgHelper.addGroup(anotherTauMonAlg,'tauMonitor')

    ### STEP 5 ###
    # Configure histograms
    for itup in [(myKinGroupBA,'BA'),
                 (myKinGroupCR,'CR'),
                 (myKinGroupEC,'EC'),
                 (myKinGroupGlobal,'Global'),
                 (myKinGroupHighPt,'HighPt'),
                 (myKinGroupHighPtBDTLoose,'HighPtBDTLoose'),
                 (myKinGroupEleTrig,'EleTrig'),
                 (myKinGroupJetTrig,'JetTrig'),
                 (myKinGroupTauTrig, 'TauTrig')]:

        (igroup, postfix) = itup

        PHIMIN = -pi
        PHIMAX = pi

        folder = ""

        if(postfix =="BA" or postfix =="CR" or postfix=="EC" or postfix == "Global" or postfix=="TauTrig" or postfix=="EleTrig" or postfix =="JetTrig"):

            igroup.defineHistogram(namer('nTauCandidates', 'nTauCandidates',"",postfix), title='Number of tau candidates;Number of Taus per Event', 
                                   xbins=30, xmin=-0.5, xmax=30.5,path=folder)


            igroup.defineHistogram(namer('tauCharge','tauCharge','',postfix), title='Charge of tau candidates;Charge;Number of Candidates',
                                   xbins=11, xmin=-5.5, xmax=5.5,path=folder)
            

            igroup.defineHistogram(namer('tauEt','tauEt','',postfix), title='Et of tau candidates;Transverse Energy (GeV);Number of Candidates',
                                   xbins=60, xmin=0., xmax=300.,path=folder)

            
            igroup.defineHistogram(namer('tauEta','tauEta','',postfix), title='Eta of tau candidates;Eta;Number of Candidates',
                                   xbins=51, xmin=-2.55, xmax=2.55,path=folder)
            
            igroup.defineHistogram(namer('LB,tauEta','tauEtaVsLB','',postfix), type='TH2F', title='Tau Eta Vs Lumiblock;Lumiblock;eta', 
                                   xbins=1200,xmin=0,xmax=1200,ybins=51,ymin=-2.55,ymax=2.55,path=folder)
            
            igroup.defineHistogram(namer('tauPhi','tauPhi','',postfix), title='Phi of tau candidates;Phi;Number of Candidates',
                                   xbins=65, xmin=PHIMIN-0.098174/2., xmax=PHIMAX+0.098174/2.,path=folder)
            
            igroup.defineHistogram(namer('tauEta,tauPhi','tauPhiVsEta','',postfix), type='TH2F', title='PhiVsEtaTitle;Eta;Phi', 
                                   xbins=30,xmin=-2.55,xmax=2.55,ybins=32,ymin=PHIMIN,ymax=PHIMAX,path=folder)

            
            igroup.defineHistogram(namer('LB,tauPhi','tauPhiVsLB','',postfix), type='TH2F', title='Tau Phi vs Lumiblock;Lumiblock;Phi', 
                                   xbins=1200,xmin=0,xmax=1200,ybins=80,ymin=PHIMIN,ymax=PHIMAX,path=folder)

        if(postfix =="BA" or postfix =="CR" or postfix=="EC" or postfix=="TauTrig"):

            igroup.defineHistogram(namer('tauPhiBDTLoose','phi','Identification_BDTLoose',postfix), title='Phi of tau candidates ( BDTLoose) ;Phi;Number of Candidates',
                xbins=65, xmin=PHIMIN-0.098174/2., xmax=PHIMAX+0.098174/2., path=folder+"Identification/BDTLoose" )

            igroup.defineHistogram(namer('tauEtaBDTLoose','eta','Identification_BDTLoose',postfix), title='Eta of tau candidates ( BDTLoose) ;Eta;Number of Candidates',
                xbins=51, xmin=-2.55, xmax=2.55, path=folder+"Identification/BDTLoose")

            igroup.defineHistogram(namer('tauEtBDTLoose','et','Identification_BDTLoose',postfix), title='Et of tau candidates;Transverse Energy (GeV);Number of Candidates',
                xbins=60, xmin=0., xmax=300.,path=folder+"Identification/BDTLoose")

            igroup.defineHistogram(namer('NumTracksBDTLoose','NumTracks','Identification_BDTLoose',postfix), title='Number Of Tracks for Tau Candidates (BDTLoose);Number Of Tracks;Number Of Candidates',
                xbins=21, xmin=-0.5, xmax=20.5,path=folder+"Identification/BDTLoose")

            igroup.defineHistogram(namer('tauPhiBDTMedium','phi','Identification_BDTMedium',postfix), title='Phi of tau candidates ( BDTMedium) ;Phi;Number of Candidates',
                xbins=65, xmin=PHIMIN-0.098174/2., xmax=PHIMAX+0.098174/2., path=folder+"Identification/BDTMedium" )

            igroup.defineHistogram(namer('tauEtaBDTMedium','eta','Identification_BDTMedium',postfix), title='Eta of tau candidates ( BDTMedium) ;Eta;Number of Candidates',
                xbins=51, xmin=-2.55, xmax=2.55, path=folder+"Identification/BDTMedium")

            igroup.defineHistogram(namer('tauEtBDTMedium','et','Identification_BDTMedium',postfix), title='Et of tau candidates;Transverse Energy (GeV);Number of Candidates',
                xbins=60, xmin=0., xmax=300.,path=folder+"Identification/BDTMedium")

            igroup.defineHistogram(namer('NumTracksBDTMedium','NumTracks','Identification_BDTMedium',postfix), title='Number Of Tracks for Tau Candidates (BDTMedium);Number Of Tracks;Number Of Candidates',
                xbins=21, xmin=-0.5, xmax=20.5,path=folder+"Identification/BDTMedium")
            igroup.defineHistogram(namer('tauPhiEt15BDTLoose','phi','Identification_BDTLoose15GeV',postfix), title='Phi of tau candidates (Et>15, BDTLoose) ;Phi;Number of Candidates',
                xbins=65, xmin=PHIMIN-0.098174/2., xmax=PHIMAX+0.098174/2., path=folder+"Identification/BDTLoose15GeV" )

            igroup.defineHistogram(namer('tauEtaEt15BDTLoose','eta','Identification_BDTLoose15GeV',postfix), title='Eta of tau candidates (Et>15, BDTLoose) ;Eta;Number of Candidates',
                xbins=51, xmin=-2.55, xmax=2.55, path=folder+"Identification/BDTLoose15GeV")

            igroup.defineHistogram(namer('nClustersEt15BDTLoose','nCluster','Identification_BDTLoose15GeV',postfix), title='Number Of CaloTopoClusters (Et>15,BDTLoose);Number Of Clusters;Number Of Candidates',
                xbins=40, xmin=0., xmax=40.,path=folder+"Identification/BDTLoose15GeV" )

            igroup.defineHistogram(namer('NumTracksEt15BDTLoose','NumTracks','Identification_BDTLoose15GeV',postfix), title='Number Of Tracks for Tau Candidates (Et>15,BDTLoose);Number Of Tracks;Number Of Candidates',
                xbins=21, xmin=-0.5, xmax=20.5,path=folder+"Identification/BDTLoose15GeV")

            igroup.defineHistogram(namer('tauEtEt15BDTLoose','et','Identification_BDTLoose15GeV',postfix), title='Et of tau candidates;Transverse Energy (GeV);Number of Candidates',
                                   xbins=60, xmin=0., xmax=300.,path=folder+"Identification/BDTLoose15GeV")

            igroup.defineHistogram(namer('panModeEt15BDTLoose','panMode','Identification_BDTLoose15GeV',postfix), title='tau decay mode from panTau upon JetBDTSigMedium;mode',
                                   xbins=5, xmin=0., xmax=5., path=folder+"Identification/BDTLoose15GeV", xlabels=["1p0n","1p1n","1pXn","3p0n","3pXn"])

            igroup.defineHistogram(namer('jetSeedEta','jetSeedEta','Calo',postfix), title='Calorimeter eta of tau candidates;Eta;Numbers of Candidates',path=folder+"Calo",
            xbins=50, xmin=-2.5, xmax=2.5 )

            igroup.defineHistogram(namer('jetSeedPhi','jetSeedPhi','Calo',postfix), title='Calorimeter phi of tau candidates;Phi;Numbers of Candidates',path=folder+"Calo",
            xbins=64, xmin=PHIMIN, xmax=PHIMAX )

            igroup.defineHistogram(namer('jetSeedPt','jetSeedPt','Calo',postfix), title='Calorimeter pt of tau candidates;Et;Numbers of Candidates',path=folder+"Calo",
            xbins=100, xmin=0., xmax=300 )
            
            igroup.defineHistogram(namer('EMRadius','EMRadius','Calo',postfix), title='Uncalibrated EM Radius;EM Radius;Number Of Candidates',path=folder+"Calo",
            xbins=50, xmin=0., xmax=1.2 )

            igroup.defineHistogram(namer('hadRadius','hadRadius','Calo',postfix), title='Hadronic Radius of tau candidates;Hadronic Radius; Number Of Candidates',path=folder+"Calo",
            xbins=50, xmin=0., xmax=1. )

            igroup.defineHistogram(namer('stripWidth2','stripWidth2','Calo',postfix), title='Strip Width of tau candidates;Strip Width;Number of Candidates',path=folder+"Calo",
            xbins=50, xmin=-0.1, xmax=0.12 )

            igroup.defineHistogram(namer('isolFrac','isolFrac','Calo',postfix), title='Isolation Fraction;Et Isolation Fraction;Number of Candidates',path=folder+"Calo",
            xbins=51, xmin=0.0, xmax=1.02 )

            igroup.defineHistogram(namer('nStrip','nStrip','Calo',postfix), title='Number of strip cells of tau candidates;Number of Strip Cells;Number of Candidates',path=folder+"Calo",
            xbins=56, xmin=-0.5, xmax=55.5 )

            igroup.defineHistogram(namer('etEMAtEMScale','etEMAtEMScale','Calo',postfix), title='EM energy at the EM scale;EM Et (GeV) );Number of Candidates',path=folder+"Calo",
            xbins=50, xmin=0., xmax=200. )

            igroup.defineHistogram(namer('etHadAtEMScale','etHadAtEMScale','Calo',postfix), title='Hadronic Energy at the EM Scale;Had Et (GeV) );Number of Candidates',path=folder+"Calo",
            xbins=50, xmin=0., xmax=200. )
            
            igroup.defineHistogram(namer('centFrac,LB','CentFracVsLB','Calo',postfix), type='TH2F', title='Centrality Fraction vs Lumiblock;Centrality Fraction;Lumiblock', path=folder+"Calo", 
            xbins=51,xmin=0,xmax=1.02,ybins=1200,ymin=0.,ymax=1200.)

            igroup.defineHistogram('centFrac,LB', type='TH2F', title='Centrality Fraction vs Lumiblock;Centrality Fraction;Lumiblock', path=folder+"Calo", 
            xbins=51,xmin=0,xmax=1.02,ybins=1200,ymin=0.,ymax=1200.)

            igroup.defineHistogram('isolFrac,LB', type='TH2F', title='Isolation Fraction vs Lumiblock;Isolation Fraction;Lumiblock', path=folder+"Calo", 
            xbins=51,xmin=0,xmax=1.02,ybins=1200,ymin=0.,ymax=1200.)

            igroup.defineHistogram(namer('BDTJetScore','BDTJetScore','Identification',postfix), title='BDT Score for Jet Rejection;Boosted Decision Tree Score',path=folder+"Identification",
            xbins=48, xmin=-1.1, xmax=1.1 )

            igroup.defineHistogram(namer('JetBDTBkgMedium','JetBDTBkgMedium','Identification',postfix), title='Loose EleBDT',path=folder+"Identification",
            xbins=2, xmin=-0.5, xmax=1.5, xlabels=["False","True"])

            igroup.defineHistogram(namer('BDTEleScoreSigTrans','BDTEleScoreSigTrans','Identification',postfix), title=' Flattened Signal Transformed BDT  for Ele Rejection;Boosted Decision Tree Score',path=folder+"Identification",
            xbins=24, xmin=-0.1, xmax=1.1 )

            igroup.defineHistogram(namer('BDTJetScoreSigTrans','BDTJetScoreSigTrans','Identification',postfix), title='Flattened signal Transformed BDT Score for Jet Rejection;Boosted Decision Tree Score',path=folder+"Identification",
            xbins=48, xmin=0, xmax=1.1 )


            igroup.defineHistogram(namer('eleBDTMedium','eleBDTMedium','Identification',postfix), title='Medium EleBDT',path=folder+"Identification",
            xbins=2, xmin=-0.5, xmax=1.5, xlabels=["False","True"])

            igroup.defineHistogram(namer('eleBDTTight','eleBDTTight','Identification',postfix), title='Tight EleBDT',path=folder+"Identification",
            xbins=2, xmin=-0.5, xmax=1.5, xlabels=["False","True"])

            igroup.defineHistogram(namer('muonVeto','muonVeto','Identification',postfix), title='Muon Veto',path=folder+"Identification",
            xbins=2, xmin=-0.5, xmax=1.5, xlabels=["False","True"] )

            igroup.defineHistogram(namer('tauBDTLoose','tauBDTLoose','Identification',postfix), title='Identification Flag: tauBDTLoose',path=folder+"Identification",
            xbins=2, xmin=-0.5, xmax=1.5 , xlabels=["False","True"])

            igroup.defineHistogram(namer('tauBDTMedium','tauBDTMedium','Identification',postfix), title='Identification Flag: tauBDTMedium',path=folder+"Identification",
            xbins=2, xmin=-0.5, xmax=1.5 , xlabels=["False","True"])

            igroup.defineHistogram(namer('tauBDTTight','tauBDTTight','Identification',postfix), title='Identification Flag: tauBDTTight',path=folder+"Identification",
            xbins=2, xmin=-0.5, xmax=1.5, xlabels=["False","True"])


            #Substructure Histograms
            igroup.defineHistogram(namer('panModeSubstructure','panMode','SubStructure',postfix), title='tau decay mode from PanTau upon JetBDTSigMedium;mode',
            xbins=8, xmin=0., xmax=8., path=folder+"SubStructure")

            igroup.defineHistogram(namer('BDTScoreAsP0','BDTScoreAsP0','SubStructure',postfix), title='BDT score indentifying pion zero ; BDT score',path=folder+"SubStructure",
            xbins=15, xmin=-0, xmax=1.2 )

            igroup.defineHistogram(namer('EMFracTrk','EMFracTrk','SubStructure',postfix), title='Ratio of pt to shot electromagnetic energy for associated tracks; track pt ratio in EM',path=folder+"SubStructure",
            xbins=15, xmin=0, xmax=1.5 )

            igroup.defineHistogram(namer('EfracL2EffCluster','EfracL2EffCluster','SubStructure',postfix), title='Energy fraction of leading two effective clusters in shot; energy fraction',path=folder+"SubStructure",
            xbins=15, xmin=0, xmax=1.5 )

            igroup.defineHistogram(namer('EisoEffCluster','EisoEffCluster','SubStructure',postfix), title='Isolation Energy after correction in effective clusters ; isolation energy (GeV)',path=folder+"SubStructure",
            xbins=10, xmin=0, xmax=50. )

            igroup.defineHistogram(namer('InvMassEffClusters','InvMassEffClusters','SubStructure',postfix), title='Invariant mass of effective clusters in shot; invariant mass (GeV)',path=folder+"SubStructure",
            xbins=40, xmin=0, xmax=8. )

            igroup.defineHistogram(namer('nNeutPFO','nNeutPFO','SubStructure',postfix), title='_NumNeutPFO;Number of neutral ParticleFlow objects ; PFO number',path=folder+"SubStructure",
            xbins=20, xmin=0, xmax=20. )

            igroup.defineHistogram(namer('nShot','nShot','SubStructure',postfix), title='number of shots ; shot number ',path=folder+"SubStructure",
            xbins=20, xmin=0, xmax=20. )

            igroup.defineHistogram(namer('panEta','panEta','SubStructure',postfix), title='tau Eta from PanTau ; substructure Eta',path=folder+"SubStructure",
            xbins=16, xmin=-3.2, xmax=3.2 )

            igroup.defineHistogram(namer('panPhi','panPhi','SubStructure',postfix), title='tau Phi from PanTau ; substructure Phi',path=folder+"SubStructure",
            xbins=16, xmin=PHIMIN, xmax=PHIMAX )

            igroup.defineHistogram(namer('panPt','panPt','SubStructure',postfix), title='tau Pt from PanTau ; substructure pt (GeV)',path=folder+"SubStructure",
            xbins=20, xmin=0, xmax=200 )

            igroup.defineHistogram(namer('pt3','shots_pt3','SubStructure',postfix), title='weighted cell pt in 3x3 window in shots; pt3 (GeV) ',path=folder+"SubStructure",
            xbins=24, xmin=0, xmax=12 )

            igroup.defineHistogram(namer('PSSFrac','PSSFracEffCluster','SubStructure',postfix), title='Energy fraction for PreSampling and sampling layers in effective clusters in shot;Sampling Energy Fraction',path=folder+"SubStructure",
            xbins=10, xmin=0, xmax=1. )

            igroup.defineHistogram(namer('ptRatioEflowApprox','ptRatioApprox','SubStructure',postfix), title='Ratio of pt to shot total energy for associated tracks; track pt ratio',
                xbins=20, xmin=0., xmax=2., path=folder+"SubStructure" )





        
        if(postfix =="BA" or postfix =="CR" or postfix=="EC" or postfix=="TauTrig" or postfix=="Global"):
            #Calo Histograms
            igroup.defineHistogram(namer('EMRadius','EMRadius','Calo',postfix), title='Uncalibrated EM Radius;EM Radius;Number Of Candidates',path=folder+"Calo",
            xbins=50, xmin=0., xmax=1.2 )

            igroup.defineHistogram(namer('hadRadius','hadRadius','Calo',postfix), title='Hadronic Radius of tau candidates;Hadronic Radius; Number Of Candidates',path=folder+"Calo",
            xbins=50, xmin=0., xmax=1. )

            igroup.defineHistogram(namer('stripWidth2','stripWidth2','Calo',postfix), title='Strip Width of tau candidates;Strip Width;Number of Candidates',path=folder+"Calo",
            xbins=50, xmin=-0.1, xmax=0.12 )

            igroup.defineHistogram(namer('isolFrac','isolFrac','Calo',postfix), title='Isolation Fraction;Et Isolation Fraction;Number of Candidates',path=folder+"Calo",
            xbins=51, xmin=0.0, xmax=1.02 )

            igroup.defineHistogram(namer('nStrip','nStrip','Calo',postfix), title='Number of strip cells of tau candidates;Number of Strip Cells;Number of Candidates',path=folder+"Calo",
            xbins=56, xmin=-0.5, xmax=55.5 )

            igroup.defineHistogram(namer('etEMAtEMScale','etEMAtEMScale','Calo',postfix), title='EM energy at the EM scale;EM Et (GeV) );Number of Candidates',path=folder+"Calo",
            xbins=50, xmin=0., xmax=200. )

            igroup.defineHistogram(namer('etHadAtEMScale','etHadAtEMScale','Calo',postfix), title='Hadronic Energy at the EM Scale;Had Et (GeV) );Number of Candidates',path=folder+"Calo",
            xbins=50, xmin=0., xmax=200. )
            
            igroup.defineHistogram(namer('centFrac,LB','centFracVsLB','Calo',postfix), type='TH2F', title='Centrality Fraction vs Lumiblock;Centrality Fraction;Lumiblock', path=folder+"Calo", 
            xbins=51,xmin=0,xmax=1.02,ybins=1200,ymin=0.,ymax=1200.)

            igroup.defineHistogram(namer('isolFrac,LB','isolFracVsLB','Calo',postfix), type='TH2F', title='Isolation Fraction vs Lumiblock;Isolation Fraction;Lumiblock', path=folder+"Calo", 
            xbins=51,xmin=0,xmax=1.02,ybins=1200,ymin=0.,ymax=1200.)

            igroup.defineHistogram(namer('centFrac',"centralityFraction","Calo",postfix), title='Centrality Fraction;Centrality Fraction;Number Of Candidates',
                xbins=70, xmin=0., xmax=1.4, path=folder+"Calo")

            igroup.defineHistogram(namer('dRmax',"dRmax","Calo",postfix), title='Maximum delta R;delta R;Number of Candidates',
                xbins=42, xmin=0., xmax=0.21, path=folder+"Calo" )

            igroup.defineHistogram(namer('tauEta','eta','Calo',postfix), title='Eta of tau candidates;Eta;Number of Candidates',
                xbins=51, xmin=-2.55, xmax=2.55,path=folder+"Calo")

            igroup.defineHistogram(namer('tauPhi','phi','Calo',postfix), title='Phi of tau candidates;Phi;Number of Candidates',
                                   xbins=65, xmin=PHIMIN, xmax=PHIMAX,path=folder+"Calo")

            igroup.defineHistogram(namer('trkAvgDist','trkAvgDist','Calo',postfix), title='Average Track Distance from Calorimeter Seed;Distance (mm);Number of Candidates',
                xbins=20, xmin=0., xmax=1., path=folder+"Calo" )

            #Track Histograms!
            igroup.defineHistogram(namer('d0','d0','Track',postfix), title='Track d0;Transverse Impact Parameter (mm)',path=folder+"Track",
            xbins=50, xmin=-5.0, xmax=5.0 )

            igroup.defineHistogram(namer('z0PriVtx','z0PriVtx','Track',postfix), title=' z0 respected to primary vertex ; Z0 SinTheta (mm)',path=folder+"Track",
            xbins=50, xmin=-35.0, xmax=35.0 )

            igroup.defineHistogram(namer('etaTrack','eta','Track',postfix), title='Track Eta;Eta',path=folder+"Track",
            xbins=51, xmin=-2.55, xmax=2.55 )

            igroup.defineHistogram(namer('leadTrkPt','leadTrkPt','Track',postfix), title='Pt of Leading track;Transverse Momentum (GeV)',path=folder+"Track",
            xbins=50, xmin=0., xmax=200. )

            igroup.defineHistogram(namer('nHighPtTaus','nHighPtTaus','Track',postfix), title='Number of High Pt tau candidates;Number of Taus per Event', 
                    xbins=15, xmin=-0.5, xmax=14.5,path=folder+"Track")

            igroup.defineHistogram(namer('numberOfTRTHighThresholdHits','numberOfTRTHighThresholdHits','Track',postfix), title='Number of TRT High Threshold Hits;Number of High Threshold TRT Hits',path=folder+"Track",
            xbins=15, xmin=-0.5, xmax=14.5 )

            igroup.defineHistogram(namer('numberOfTRTHighThresholdOutliers','numberOfTRTHighThresholdOutliers','Track',postfix), title="Number of TRT High Threshold Outliers;Number of TRT High Threshold Outliers",path=folder+"Track",
            xbins=26, xmin=-0.5, xmax=25.5 )

            igroup.defineHistogram(namer('numberOfTRTHits','numberOfTRTHits','Track',postfix), title='Number of TRT Low Threshold Hits;Number of Low Threshold TRT Hits',path=folder+"Track",
            xbins=101, xmin=-0.5, xmax=100.5 )

            igroup.defineHistogram(namer('numberOfTRTOutliers','numberOfTRTOutliers','Track',postfix), title="Number of TRT Low Threshold Outliers;Number of TRT Low Threshold Outliers",path=folder+"Track",
            xbins=31, xmin=-0.5, xmax=25.5)
            
            igroup.defineHistogram(namer('ptTrack','pT','Track',postfix), title='Track pT;Transverse Momentum (GeV)',path=folder+"Track",
            xbins=60, xmin=0, xmax=300 )

            igroup.defineHistogram(namer('phiTrack','phi','Track',postfix), title='Track Phi;Phi',path=folder+"Track",
            xbins=64, xmin=PHIMIN, xmax=PHIMAX )

            igroup.defineHistogram(namer('trkWidth2','trkWidth2','Track',postfix), title='Weighted Track Width;Momentum-Weighted Width of Track System',path=folder+"Track",
            xbins=50, xmin=0.0, xmax=0.1 )

            igroup.defineHistogram(namer('ipZ0SinThetaSigLeadTrk','ipZ0SinThetaSigLeadTrk','Track',postfix), title='Impact Parameter z0 Sine Theta Significance of Leading Track;Z0SinTheta Significance',path=folder+"Track",
            xbins=50, xmin=-10.0, xmax=10.0 )

            igroup.defineHistogram(namer('numberOfPixelHits','numberOfPixelHits','Track',postfix), title='Number of Pixel Hits;Number of Pixel Hits',path=folder+"Track",
            xbins=25, xmin=-0.5, xmax=25.5 )

            igroup.defineHistogram(namer('numberOfSCTHits','numberOfSCTHits','Track',postfix), title='Number of SCT Hits;Number of SCT Hits',path=folder+"Track",
            xbins=26, xmin=0, xmax=26 )

            igroup.defineHistogram(namer('etOverPtLeadTrack','etOverPtLeadTrack','Track',postfix), title='Et over Pt of lead track of tau candidates;Et/Pt;Number of Candidates',
            xbins=50, xmin=0., xmax=12., path=folder+"Track" )

            igroup.defineHistogram(namer('ipSigLeadTrk','ipSigLeadTrk','Track',postfix), title='Impact Parameter Significance of Leading Track;Transverse Impact Parameter Significance',
            xbins=100, xmin=-5., xmax=5., path=folder+"Track" )

            igroup.defineHistogram(namer('massTrkSys','massTrkSys','Track',postfix), title='Mass of the Track System;Invariant Mass (GeV)',
                xbins=30, xmin=-1., xmax=5., path=folder+"Track" )

            igroup.defineHistogram(namer('trFlightPathSig','trFlightPathSig','Track',postfix),title='Track Transverse Flight Path Significance;Transverse Flight Path Significance',
                xbins=125, xmin=-4.5, xmax=8., path=folder+"Track" )

            igroup.defineHistogram(namer('z0','z0','Track',postfix), title='Track z0;Longitudinal Impact Parameter (mm)',path=folder+"Track",
            xbins=50, xmin=-35.0, xmax=35.0 )

        if postfix == 'Global' or postfix == "TauTrig" or postfix == "EleTrig" or postfix=="JetTrig":
            igroup.defineHistogram(namer('LB','nTauPerLB','',postfix), title='Total number of tau candidates per LB);Luminosity Block);Number of Candidates', 
                    xbins=1000, xmin=0., xmax=1000. ,path=folder)

            igroup.defineHistogram(namer('nHighPtTauCandidates','nHightPtTauCandidates','',postfix), title='Number of High Pt tau candidates;Number of Taus per Event', 
                    xbins=15, xmin=-0.5, xmax=14.5,path=folder)

            igroup.defineHistogram(namer('NumTracks','tauNumTracks','',postfix), title='Number Of Tracks for Tau Candidates;Number Of Tracks;Number Of Candidates',
                      xbins=21, xmin=-0.5, xmax=20.5,path=folder)

            igroup.defineHistogram(namer('nClusters','nCluster','',postfix), title='Number Of CaloTopoClusters;Number Of Clusters;Number Of Candidates',
                      xbins=40, xmin=0., xmax=40. ,path=folder)

            igroup.defineHistogram(namer('tauEta,tauEt','tauEtVsEta','',postfix), type='TH2F', title='Tau Et Vs Eta;#eta;Transverse Energy (Gev)',
                      xbins=40,xmin=-2.55,xmax=2.55 ,ybins=300,ymin=0,ymax=300,path=folder)

            igroup.defineHistogram(namer('tauPhi,tauEt','tauEtVsPhi','',postfix), type='TH2F', title='EtVsPhi;Phi;Transverse Energy (GeV);',
                      xbins=80,xmin=PHIMIN,xmax=PHIMAX,ybins=300,ymin=-0.,ymax=300.,path=folder)

            if postfix == 'Global':
                #Physics Histograms - I suspect these should be doing some kind of selection, not obvious in the old code.
                igroup.defineHistogram(namer('tauEta','tau_eta','',postfix), title='Eta of tau candidates;Eta;Number of Candidates',
                                       xbins=51, xmin=-2.55, xmax=2.55,path=folder+"Physics/Z")

                igroup.defineHistogram(namer('tauEta,tauEt','tau_pTVsEta','',postfix), type='TH2F', title='Tau Et Vs Eta;#eta;Transverse Energy (Gev)',
                          xbins=40,xmin=-2.55,xmax=2.55 ,ybins=300,ymin=0,ymax=300,path=folder+"Physics/Z")

                
                #will use different variable when not a placeholder
                igroup.defineHistogram(namer('tauEta','lepton_pTVsEta','',postfix), title='Placeholder for empty histogram;Eta;Number of Candidates',
                                       xbins=51, xmin=-2.55, xmax=2.55,path=folder+"Physics/Z")


                igroup.defineHistogram(namer('tauEta,tauEt','tau_pTVsEta','',postfix), type='TH2F', title='Tau Et Vs Eta;#eta;Transverse Energy (Gev)',
                          xbins=40,xmin=-2.55,xmax=2.55 ,ybins=300,ymin=0,ymax=300,path=folder+"Physics/W")

        if postfix == 'HighPt':
            igroup.defineHistogram(namer('tauEtaEt15,tauPhiEt15','tauPhiVsEta_et15','',postfix), type='TH2F', title='EtaVsEtTitle;Eta;Phi', 
               xbins=30,xmin=-2.55,xmax=2.55,ybins=32,ymin=PHIMIN,ymax=PHIMAX)

        if postfix == 'HighPtBDTLoose':

            igroup.defineHistogram(namer('tauEtaEt15BDTLoose,tauPhiEt15BDTLoose','tauPhiVsEta_et15_BDTLoose','',postfix), type='TH2F', title='Phi vs Eta (Et>15, BDTLoose) ;Eta;Phi', 
               xbins=30,xmin=-2.55,xmax=2.55,ybins=32,ymin=PHIMIN,ymax=PHIMAX)



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
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    exampleMonitorAcc = tauMonitoringConfig(ConfigFlags)   # calls the main function above
    cfg.merge(exampleMonitorAcc)

    # If you want to turn on more detailed messages ...
    exampleMonitorAcc.getEventAlgo('tauMonAlgBA').OutputLevel = 2 # DEBUG
    exampleMonitorAcc.getEventAlgo('tauMonAlgCR').OutputLevel = 2 # DEBUG
    exampleMonitorAcc.getEventAlgo('tauMonAlgEC').OutputLevel = 2 # DEBUG
    exampleMonitorAcc.getEventAlgo('tauMonAlgGlobal').OutputLevel = 2 # DEBUG
    exampleMonitorAcc.getEventAlgo('tauMonAlgTauTrig').OutputLevel = 2 # DEBUG
    exampleMonitorAcc.getEventAlgo('tauMonAlgEleTrig').OutputLevel = 2 # DEBUG
    exampleMonitorAcc.getEventAlgo('tauMonAlgJetTrig').OutputLevel = 2 # DEBUG
    exampleMonitorAcc.getEventAlgo('tauMonAlgHighPt').OutputLevel = 2 # DEBUG
    exampleMonitorAcc.getEventAlgo('tauMonAlgHighPtBDTLoose').OutputLevel = 2 # DEBUG

    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events
