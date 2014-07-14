# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: commonD3PD.py 
#
# @file PhysicsAnalysis/D3PDMaker/D3PDMakerConfig/python/commonD3PD.py
# @author Lei Zhou lei.zhou@cern.ch
# @date Mar, 2013
# @brief Construct a Common D3PD which is a union of SMWZ, SUSY, TOP, TAU and JETMET D3PDs 


from D3PDMakerConfig.CoreCommonD3PD import *


def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw


def commonD3PD (alg = None,
              file = 'physics.root',
              tuplename = 'physics',
              streamname = 'd3pdstream',
              doThirdMuonChain = True,
              stdElectronContainer = D3PDMakerFlags.ElectronSGKey(),
              gsfElectronContainer = D3PDMakerFlags.GSFElectronSGKey(),
              muonContainer        = 'None', # can be 'Muons' for third muon chain
              stacoMuonContainer   = 'StacoMuonCollection',
              muidMuonContainer    = 'MuidMuonCollection',
              caloMuonContainer    = 'CaloMuonCollection',
              photonContainer      = D3PDMakerFlags.PhotonSGKey(),
              tauContainer         = D3PDMakerFlags.TauSGKey(),
              jetEM4Container      = 'AntiKt4TopoEMJets',
              jetEM6Container      = 'AntiKt6TopoEMJets',
              jetLC4Container      = 'AntiKt4LCTopoJets',
              jetLC6Container      = 'AntiKt6LCTopoJets',
              **kw):


    flags=CommonD3PDMakerFlags

    # define track and cluster filters

    preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

    if not hasattr( preseq, "GoodTracks" ):
        filter1 = makeTrackFilterAlg(TracksName = D3PDMakerFlags.TrackSGKey(),
                                     OutputTracksName='GoodTracks',
                                     ptCut=0.)
        preseq += filter1

    if not hasattr( preseq, "HighPtTracks" ):
        filter2 = makeTrackFilterAlg(TracksName = D3PDMakerFlags.TrackSGKey(),
                                     OutputTracksName='HighPtTracks',
                                     ptCut=4000.,nSCTPix=4)
        preseq += filter2
    
    if not hasattr( preseq, "HighPtClusters" ):
        filter3 = makeClusterFilter(InputName  = D3PDMakerFlags.ClusterSGKey(),
                                    OutputName = 'HighPtClusters',
                                    ptCut=10000.)
        preseq += filter3

    if not hasattr( preseq, "HighPtEMClusters" ):
        filter4 = makeClusterFilter(InputName  = D3PDMakerFlags.EMTopoClusterSGKey(),
                                    OutputName = 'HighPtEMClusters',
                                    ptCut=10000.)
        preseq += filter4

    if not hasattr( preseq, "SUSYTrackCandidate" ):
        filter5 = makeTrackFilterAlg(TracksName = D3PDMakerFlags.TrackSGKey(),
                                     OutputTracksName='SUSYTrackCandidate',
                                     ptCut=10000.)
        preseq += filter5

    # perform recoil calculation
    from HadronicRecoil.Configurables import add_hadronic_recoil_filters
    preseq = add_hadronic_recoil_filters(preseq)


    # beam background stuff
    from RecBackgroundAlgs.RecBackgroundAlgsConf import BeamBackgroundFiller
    if not hasattr( preseq, "BeamBackgroundFiller" ):
        BeamBackgroundFiller=BeamBackgroundFiller()
        preseq += BeamBackgroundFiller

    # now configure the D3PD
    if not alg:
        alg = MSMgr.NewRootStream(StreamName = streamname, FileName = file, TreeName = tuplename)

        
    alg += EventInfoD3PDObject        (**_args (10, 'EventInfo', kw))
    alg += LArCollisionTimeD3PDObject (**_args (10, 'LArCollisionTime', kw))
    addBunchStructureMetadata (alg)

    if flags.doEventShape():
        alg += EventShapeD3PDObject(**_args(10, 'rho', kw))
    if flags.doMuonSpShower2():
        alg += MuonSpShowerD3PDObject (**_args(10, 'MuonSpShower', kw))
    

    alg += BackgroundWordD3PDObject (**_args(0, 'BkgWord', kw))
    alg += BeamBackgroundD3PDObject(1)
    alg += DefaultV0D3PDObject(**_args (1, 'V0Objects', kw) )

    from D3PDMakerConfig.addTauD3PDInfo import addTauD3PDInfo
    addTauD3PDInfo( alg, file, tuplename, kw )

    # Eta rings of energy
    if not rec.readAOD:
        from CaloD3PDMaker.RapgapD3PDObject       import EtaRingsNonTileD3PDObject
        from CaloD3PDMaker.RapgapD3PDObject       import EtaRingsD3PDObject

        alg += EtaRingsNonTileD3PDObject  ( **_args ( 0, 'EtaRings', kw) )
        alg += EtaRingsD3PDObject         ( **_args ( 0, 'EtaRings', kw) )
    
    # Electron/Photon blocks
    if (stdElectronContainer != 'None' ):
        alg += ElectronD3PDObject         (**_args (10, 'Electron', kw, sgkey = stdElectronContainer,
                                                    include = flags.electronInclude()))
        
    if (gsfElectronContainer != 'None') and testSGKey ('ElectronContainer', gsfElectronContainer):
        alg += GSFElectronD3PDObject  (**_args (10, 'GSFElectron', kw,
                                                sgkey = gsfElectronContainer,
                                                prefix = 'el_gsf_',
                                                include = flags.electronInclude()))

    # define associations
    if not PhotonD3PDObject.allBlocknames().has_key('RecoveryMatch'):
        from PhotonD3PDMaker.PhotonRecoveryAssociation import PhotonRecoveryAssociation
        if (stdElectronContainer != 'None'):
            PhotonRecoveryAssoc = PhotonRecoveryAssociation(PhotonD3PDObject,
                                                            'ElectronContainer',
                                                            stdElectronContainer,
                                                            prefix = 'el_',
                                                            include = ['TrkFitQuality','Vertex'],
                                                            blockname = 'RecoveryMatch',
                                                            target = 'ph_ElectronRecovered_')

    if (photonContainer != 'None' ):
        alg += PhotonD3PDObject           (**_args (10, 'Photon', kw, sgkey = photonContainer,
                                                    include = flags.photonInclude(),
                                                    ConversionVertexTrackIndex_target = "GSF_trk"))
    
    # Muon blocks
    ### Third muon chain variables!
    if doThirdMuonChain and muonContainer == 'None':
        muonContainer = 'Muons'
        
    if (muonContainer != 'None' ):
        alg += myMuonD3PDObject             (**_args (10, 'Muon', kw,
                                                    sgkey=muonContainer, prefix='mu_',
                                                    include = flags.muonInclude()+["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex","L2CBInfoIndex"],
                                                    allowMissing = True ))

    if (muidMuonContainer != 'None' ):
        alg += myMuonD3PDObject             (**_args (10, 'MuidMuon', kw,
                                                    sgkey=muidMuonContainer, prefix='mu_muid_',
                                                    include = flags.muonInclude()+["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex","L2CBInfoIndex"],
                                                    allowMissing = True ))

    if (stacoMuonContainer != 'None' ):
        alg += myMuonD3PDObject             (**_args (10, 'StacoMuon', kw,
                                                    sgkey=stacoMuonContainer, prefix='mu_staco_',
                                                    include = flags.muonInclude()+["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex","L2CBInfoIndex"],
                                                    allowMissing = True ))

    if (caloMuonContainer != 'None' ):
        alg += myMuonD3PDObject             (**_args (10, 'CaloMuon', kw,
                                                    sgkey=caloMuonContainer, prefix='mu_calo_',
                                                    include = flags.muonInclude() + ["MuonHitSummary", "Authors", "Likelihood",
                                                               "CaloEnergyLoss", "Quality"],
                                                    #exclude = "Isolation"))
                                                    allowMissing = True ))

    # add missingEt info
    from D3PDMakerConfig.addMissingEtInfo import addMissingEtInfo
    addMissingEtInfo( alg, file, tuplename, kw )

    # add Jets info
    from D3PDMakerConfig.addJetsInfo  import addJetsInfo
    addJetsInfo( alg, file, tuplename, kw )

    from HiggsD3PDMaker.HSG2VertexReconstruction  import  addHSG2VertexReconstruction
    addHSG2VertexReconstruction(alg, muon_target = "mu_")


    alg += HforD3PDObject(**_args (0, 'HforInfo', kw))

    # HadronicRecoil blocks
    alg += ElectronD3PDObject(0,  sgkey = "HR_selectedElectrons",       prefix = "hr_el_")
    alg += MuonD3PDObject( 0,     sgkey = "HR_selectedMuons",           prefix = "hr_mu_" )


    # track and cluster blocks
    #alg += ClusterD3PDObject          (**_args ( 0, 'Clusters1', kw,
    #                                             include=['CenterMagMoments']))
                                                 #exclude=['SamplingBasics']))

    # ... higher LOD for pt>10 GeV
    #alg += ClusterD3PDObject          (**_args ( 2, 'Clusters2', kw,
    #                                             sgkey  = 'HighPtClusters',
    #                                             prefix = 'clpt10_'))
                     
    
    #alg += ClusterD3PDObject          (**_args ( 3, 'Clusters3', kw,
    #                                             sgkey  = 'HighPtEMClusters',
    #                                             prefix = 'emclpt10_'))


    if flags.doClusterHad():
        alg += ClusterD3PDObject (**_args(0, 'CaloCalTopoCluster', kw, prefix='cl_lc_', exclude='SamplingBasics',
                                          Kinematics_WriteE=False))

        alg += ClusterD3PDObject (**_args(2, 'CaloCalTopoCluster', kw, prefix='cl_',
                                          exclude=['Kinematics','SamplingBasics'],
                                          Kinematics_WriteE=False))

    if flags.doClusterEM():
        alg += ClusterD3PDObject (**_args(0, 'CaloCalTopoCluster', kw, prefix='cl_em_', exclude='SamplingBasics',
                                          Kinematics_WriteE=False, Kinematics_SignalState=0 ))


    # turn on detailed track info if writing SoftQCD version of D3PD
    if flags.doSoftQCD() :

        alg += TrackParticleD3PDObject(**_args (3, 'Tracks1', kw,
                                                sgkey  = 'GoodTracks',
                                                label  = 'trk',
                                                prefix = 'trk_',
                                                doTruth = True,
                                                TruthParticleTarget='mc_',
                                                storeDiagonalCovarianceAsErrors = True,
                                                storeHitTruthMatching = True,
                                                storePixelHitsOnTrack = False,
                                                storePixelHolesOnTrack = False,
                                                storePixelOutliersOnTrack = False,
                                                storeSCTHitsOnTrack = False,
                                                storeTrackFitQuality = True,
                                                storeTrackMomentum = True,
                                                storeTrackSummary = True,
                                                trackParametersAtBeamSpotLevelOfDetails = 0,
                                                trackParametersAtGlobalPerigeeLevelOfDetails = 1,
                                                trackParametersAtPrimaryVertexLevelOfDetails = 2 ))

    else :

        alg += TrackParticleD3PDObject(**_args ( 10, 'Tracks3', kw,
                                                 sgkey  = 'SUSYTrackCandidate',
                                                 label  = 'trk10',
                                                 prefix = 'trk10_',
                                                 include = ["Isolation","Isolation_40_1GeV","Isolation_40_2GeV","Isolation_40_3GeV","Isolation_40_3GeV_hitschi","Isolation_40_4GeV","Isolation_50_5GeV"],
                                                 TruthParticleTarget='mc_',
                                                 trackParametersAtBeamSpotLevelOfDetails=2,
                                                 trackParametersAtPrimaryVertexLevelOfDetails=2,
                                                 #trackParametersAtBeamLineLevelOfDetails=2,  # does not work??
                                                 trk_TrackSummary_FullInfo = False,
                                                 doTruth=True ))

        alg += TrackParticleD3PDObject(**_args ( 10, 'Tracks1', kw,
                                                 sgkey  = 'GoodTracks',
                                                 label  = 'trk',
                                                 prefix = 'trk_',
                                                 exclude = ['trk_IPEstimate'],
                                                 TruthParticleTarget='mc_',
                                                 trackParametersAtBeamSpotLevelOfDetails=1,
                                                 trackParametersAtGlobalPerigeeLevelOfDetails = 1,
                                                 trackParametersAtPrimaryVertexLevelOfDetails=2,
                                                 storeTrackParametersAtCalo=True,
                                                 storeTrackParametersAtCalo2ndLayer=True,
                                                 storeDiagonalCovarianceAsErrors=False,
                                                 storeTrackInfo=True,
                                                 storeHitTruthMatching=True,
                                                 doTruth = True ))
                            

        alg += TrackParticleD3PDObject(**_args ( 3, 'Tracks2', kw,
                                                 sgkey  = 'HighPtTracks',
                                                 label  = 'trk4',
                                                 prefix = 'trkpt4_',
                                                 exclude = ['trk_IPEstimate'],
                                                 doTruth = True,
                                                 TruthParticleTarget='mc_',
                                                 storeDiagonalCovarianceAsErrors = True,
                                                 storeHitTruthMatching = True,
                                                 storePixelHitsOnTrack = False,
                                                 storePixelHolesOnTrack = False,
                                                 storePixelOutliersOnTrack = False,
                                                 storeSCTHitsOnTrack = False,
                                                 storeSCTHolesOnTrack = False,
                                                 storeSCTOutliersOnTrack = False,
                                                 storeTRTHitsOnTrack = False,
                                                 storeTRTHolesOnTrack = False,
                                                 storeTRTOutliersOnTrack = False,
                                                 storeTrackFitQuality = True,
                                                 storeTrackMomentum = True,
                                                 storeTrackSummary = True,
                                                 trk_TrackSummary_IDOutliers = True,
                                                 trk_TrackSummary_PixelInfoPlus = True,
                                                 trk_TrackSummary_SCTInfoPlus = True,
                                                 trk_TrackSummary_TRTInfoPlus = True,
                                                 trk_TrackSummary_InfoPlus = True,
                                                 trk_TrackSummary_MuonHits = True,
                                                 trackParametersAtBeamSpotLevelOfDetails = 0,
                                                 trackParametersAtGlobalPerigeeLevelOfDetails = 2,
                                                 trackParametersAtPrimaryVertexLevelOfDetails = 2,
                                                 storeTrackParametersAtCalo=False ))

    
    
    # Primary vertex block - May be missing in single-beam data.
    alg += PrimaryVertexD3PDObject (**_args (3, 'PrimaryVertex', kw,
                                             allowMissing = True,
                                             sgkey = D3PDMakerFlags.VertexSGKey(),
                                             prefix = 'vxp_',
                                             include = ['Error']))
    
    # MBTS 

    alg += MBTSD3PDObject             (**_args (10, 'MBTS', kw))
    alg += MBTSTimeD3PDObject         (**_args (10, 'MBTSTime', kw))
    alg += MBTSTriggerBitsD3PDObject  (**_args (10, 'MBTSTriggerBits', kw))
    alg += CollisionDecisionD3PDObject(**_args (10, 'CollisionDecision', kw))
    
    # Truth
    if rec.doTruth():
        
        alg += TruthParticleD3PDObject (10, sgkey=D3PDMakerFlags.TruthParticlesSGKey())
        alg += TruthVertexD3PDObject(**_args (1, 'TruthVertex', kw, sgkey=D3PDMakerFlags.TruthSGKey()))

        alg += TruthMuonD3PDObject    (**_args ( 2, 'TruthMuon', kw))

        alg += GenEventD3PDObject (**_args (1, 'GenEvent', kw ) ) #, filter = CoreTruthFilterTool() ))

        # ... heavy flavor truth information
        #from TruthD3PDMaker.TruthD3PDMakerKeys import TruthD3PDKeys
        #TruthD3PDKeys.TruthTrackGetterLabel.set_Value("D3PDTruth_TruthTrack")
        alg += GenVertexD3PDObject( **_args(0, "hfgenvertex", kw, prefix='mchfvtx_',
                                            filter = smwzhfGenVtxFilterTool ))

        alg += GenTruthTrackD3PDObject( **_args(0, "TruthTracks", kw )) 

        alg += GenParticleD3PDObject( **_args(10, "hfgenparticle", kw, prefix='mchfpart_',
                                              filter = smwzhfGenPartFilterTool,
                                              GenParticle_WriteMotherType=False,
                                              GenParticle_WriteMotherBarcode=False))

        # ...  leptonic W/Z truth information
        alg += GenParticleD3PDObject( **_args(10, "lepwzgenparticle", kw, prefix='mclepwzpart_',
                                              filter = smwzlepwzGenPartFilterTool,
                                              label = "LepWZTruthD3PDObject",
                                              exclude = ["GenPartProdVertexAssoc","GenPartDecayVertexAssoc"],
                                              GenParticle_WriteMotherType=False,
                                              GenParticle_WriteMotherBarcode=False,
                                              GenPartMotherAssoc_target="LepWZTruthD3PDObject",
                                              GenPartChildAssoc_target="LepWZTruthD3PDObject"))
            
        alg += egammaTruthD3PDObject(0)
        
        if (SUSYD3PDFlags.includeSUSYHardProcess()) :
            alg += SUSYHardProcessD3PDObject (**_args (0, 'HardProcess', kw))

        # A dictionary of store gate keys and ntuple variable prefix
        truthParticleContainers = {'TruthTopVertexContainer':'mc_t_vx_',
                                   'TruthWVertexContainer':'mc_W_vx_',
                                   'TruthZVertexContainer':'mc_Z_vx_',
                                   'TruthTopDecayContainer':'mc_t_vx_end_',
                                   'TruthWDecayContainer':'mc_W_vx_end_',
                                   'TruthZDecayContainer':'mc_Z_vx_end_',
                                   'TruthWTauContainer':'mc_W_tau_',
                                   'TruthZTauContainer':'mc_Z_tau_'}
        #import D3PDMakerCoreComps
        from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
        from TruthD3PDMaker                import TruthParticleGenParticleAssociationTool
        for contKey, prefix in truthParticleContainers.iteritems():
            truthPartObj = make_SGDataVector_D3PDObject('TruthParticleContainer',
                                                        contKey,
                                                        prefix, "truthPartObj")
            genPart = SimpleAssociation(truthPartObj,TruthParticleGenParticleAssociationTool)
            trupart2 = IndexAssociation (genPart,
                                         TruthD3PDMaker.GenParticleTruthParticleAssociationTool,
                                         target = 'mc_',
                                         Target = 'mc_')
            alg += truthPartObj(0,prefix)

        

    #if not rec.doTruth():
    #    alg += BeamSpotD3PDObject(10)
        
    # Trigger
    if D3PDMakerFlags.DoTrigger():
        
        from PhysicsD3PDMaker.SMWZTriggerBitsD3PDObject        import SMWZTriggerBitsD3PDObject
        alg += SMWZTriggerBitsD3PDObject      (**_args (10, 'SMWZTriggerBits', kw))
 
        alg += GenericTriggerBitsD3PDObject      (**_args (10, 'GenericTriggerBits', kw))
        alg += InitTrigD3PDObject(10) 
        
        # Trigger Decision + metadata
        alg += TrigDecisionD3PDObject  (**_args(10, 'TrigDecision', kw)) 
        #addTrigConfMetadata(alg)

        ####### begin from TRIGBJET ########
        from TriggerD3PDMaker.JetETROID3PDObject        import JetETROID3PDObject
        from JetD3PDMaker.jetMETD3PDTrigger           import jetMETTriggerBitsD3PDObject
        alg += jetMETTriggerBitsD3PDObject(10)
        alg += JetETROID3PDObject (10)
        # The L2 JE information:
        from TrigMissingETD3PDMaker.TrigMETD3PDObject import TrigMETD3PDObject
        alg += TrigMETD3PDObject(level = 10, prefix = "trig_L2_je_",
                                 sgkey = "HLT_L2JetEtSum", allowMissing = True )
        ###### end from TRIGBJET ########
        
        
        # Bunch group info
        alg += BGCodeD3PDObject (**_args (2, 'BGCode', kw))

        # Egamma and Mu
        TrigEgammaD3PDObjects (alg, 10)
        TrigMuonD3PDObjects( alg, 10, addNaviInfo = True, addConfigInfo = True)
        alg += egammaTriggerBitsD3PDObject   (**_args (0, 'egammaTriggerBits', kw))
        alg += MuonTriggerBitsD3PDObject     (**_args (2, 'MuonTriggerBits', kw))

        alg += JetROID3PDObject           (**_args (10, 'JetROI', kw,
                                                    prefix = "trig_L1_jet_"))
        alg += TrigJetD3PDObject          (**_args (2, 'TrigJet', kw,
                                                    sgkey='HLT_TrigT2CaloJet',
                                                    prefix='trig_L2_jet_',
                                                    allowMissing = True))
        alg += EFJetD3PDObject            (**_args (2, 'EFJet', kw,
                                                    sgkey='HLT_TrigJetRec',
                                                    prefix='trig_EF_jet_',
                                                    allowMissing = True,
                                                    include=['Triggers']))

        # MET
        TrigMETD3PDObjects (alg)
        alg += METD3PDTriggerBitsObject (0)

        # The BJet information:
        TrigBJetD3PDObjects(alg, level = 10, doAddL2ID = True, doAddEFID = True,
                            addNaviInfo = True, addConfigInfo = True, addL1Jet = True) #, addOffTrack = True)

        alg += QcdTriggerBitsD3PDObject (**_args(10, 'TriggerBits', kw))

        from TrigTauD3PDMaker.TrigTauD3PD                   import TrigTauD3PDObjects 
        TrigTauD3PDObjects (alg, 2)
        

# Event metadata
    alg.MetadataTools += [LBMetadataConfig()]
    alg.MetadataTools += [CutFlowMetadataConfig(alg)]

# begin for add CutflowSvc
#EventBookkeepers
    #from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    #from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    #if not hasattr(svcMgr,"CutFlowSvc"):
    #    from EventBookkeeperTools.EventBookkeeperToolsConf import CutFlowSvc
    #    svcMgr+=CutFlowSvc()
    #    pass
    #if rec.readAOD() or rec.readESD():
        #force CutFlowSvc execution (necessary for file merging)
    #    theApp.CreateSvc+=['CutFlowSvc']
    #    pass

    # Determine current skimming cycle and input stream name
    #from RecExConfig.InputFilePeeker import inputFileSummary
    #from RecExConfig.RecoFunctions import GetSkimCycle
    #inputCycle=GetSkimCycle(inputFileSummary)
    #if inputCycle<0:
    #    currentCycle=1
    #else:
    #    currentCycle=inputCycle+1
    #    pass
    #svcMgr.CutFlowSvc.SkimmingCycle=currentCycle
    #svcMgr.CutFlowSvc.InputStream=rec.mergingStreamName()
    #Exception for DPD pass-through mode
    #if rec.doDPD.passThroughMode:
    #    svcMgr.CutFlowSvc.SkimmingCycle=0
    #    svcMgr.CutFlowSvc.InputStream="Virtual"
    #    pass
    #if rec.DPDMakerScripts()!=[] and not rec.doDPD.passThroughMode :
    #    #Create a separate EventBookkeeper list to persistify skimming cycle info
    #    from EventBookkeeperTools.BookkeepingInfoWriter import EventBookkeepersWriter
    #    SkimmingCycleDefiner=EventBookkeepersWriter("SkimmingCycleDefiner")
    #    SkimmingCycleDefiner.OutputCollectionName="cycle"+str(currentCycle)
    #    SkimmingCycleDefiner.ParentStreamName=rec.mergingStreamName()
    #    topSequence = AlgSequence()
    #    topSequence+=SkimmingCycleDefiner #

        #Explicitely add file metadata from input and from transient store
    #    MSMgr.AddMetaDataItemToAllStreams(inputFileSummary['metadata_itemsList'])
    #    MSMgr.AddMetaDataItemToAllStreams( "LumiBlockCollection#*" )
    #    MSMgr.AddMetaDataItemToAllStreams( "EventBookkeeperCollection#*" )
    #    MSMgr.AddMetaDataItemToAllStreams( "IOVMetaDataContainer#*" )
    #    pass
# end for add CutflowSvc
    
    if D3PDMakerFlags.FilterCollCand():
        from CaloD3PDMaker.CollisionFilterAlg import CollisionFilterAlg
        alg.filterSeq += CollisionFilterAlg (tuplename + '_CollCandFilter')

    # Add BeamSpot information
    from TrackD3PDMaker.BeamSpotD3PDObject import BeamSpotD3PDObject
    alg += BeamSpotD3PDObject(10)

    #-------------------------------------------------------------------------------------
    # Diphoton information...  in order to run this, need to have setup the diphoton
    # analysis algorithm.  See CommonD3PD_prodJobOFragment.py.
    #
    from PhotonD3PDMaker.DiPhotonD3PDObject              import DiPhotonD3PDObject
    alg += DiPhotonD3PDObject         (**_args (10, 'DiPhoton'         , kw))
    #-------------------------------------------------------------------------------------


    #from D3PDMakerCoreComps.resolveSGKey                 import testSGKey
    GSFTrackParticlesInSample = testSGKey ('Rec::TrackParticleContainer', "GSFTrackParticleCandidate")
    if GSFTrackParticlesInSample:
        GSFTrackParticleD3PDObject = TrackD3PDObject(_label='trk',
                                                     _prefix='trk_',
                                                     _sgkey=D3PDMakerFlags.TrackSGKey(),
                                                     _object_name='GSFTrackParticleD3PDObject',
                                                     typeName='Rec::TrackParticleContainer',
                                                     vertexTarget='PV_',
                                                     vertexPrefix='PV_',
                                                     vertexSGKey='VxPrimaryCandidate',
                                                     truthTarget='mc',
                                                     truthPrefix='mc_',
                                                     detailedTruthPrefix='detailed_mc_',
                                                     truthMapKey='GSFTrackParticleTruthCollection',
                                                     SGKeyForTruth="GSFTrackParticleCandidate",
                                                     detailedTruthMapKey='DetailedTrackTruth',
                                                     SGKeyForDetailedTruth='Tracks',
                                                     flags=TrackD3PDFlags)
    # add a flag for conversion tracks
    from TrackD3PDMaker import TrackParticleVertexAssociationTool
    from PhotonD3PDMaker import GSFOriginalTrackParticleAssociationTool
    convTrackAssociation = SimpleAssociation \
                           (GSFTrackParticleD3PDObject,
                            TrackParticleVertexAssociationTool,
                            prefix = 'GSF_conv_',
                            blockname='ConversionTrackParticle',
                            VxSGKey='AllPhotonsVxCandidates')
    origTrackAssociation = IndexAssociation \
                               (GSFTrackParticleD3PDObject,
                                #PhotonD3PDMaker.GSFOriginalTrackParticleAssociationTool,
                                GSFOriginalTrackParticleAssociationTool,
                                target="trk",
                                prefix="trk_",
                                level=0,
                                blockname="GSFOriginalTPAssoc",
                                allowMissing=False,
                                AssocGetter = D3PDMakerCoreComps.SGObjGetterTool
                                ('GSFAssocGetter',
                                 SGKey = D3PDMakerFlags.GSFTrackAssocSGKey(),
                                 TypeName = 'TrackParticleAssocs'))
    alg += GSFTrackParticleD3PDObject (0, sgkey="GSFTrackParticleCandidate",
                                       label="GSF_trk",
                                       prefix="GSF_trk_")
                                    
    from PhotonD3PDMaker.PhotonPVD3PDObject import PVD3PDObject
    alg += PVD3PDObject (**_args (0, 'PrimaryVertex', kw,
                                  allowMissing = True,
                                  sgkey = D3PDMakerFlags.VertexSGKey(),
                                  prefix = 'PhotonPV_'))
         

    return alg
