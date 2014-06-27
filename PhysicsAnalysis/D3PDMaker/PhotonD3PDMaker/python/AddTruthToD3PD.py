# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import PhotonD3PDMaker
from PhotonD3PDMaker.PhotonD3PDMakerFlags            import PhotonD3PDMakerFlags
from AthenaCommon.AlgSequence                        import AlgSequence
from D3PDMakerConfig.D3PDMakerFlags                  import D3PDMakerFlags
from PhotonD3PDMaker.PhotonD3PD                      import _args
from JetD3PDMaker.JetD3PDObject                      import JetD3PDObject
from JetTagD3PDMaker.JetTagD3PDMakerKeys             import JetTagD3PDKeys

def AddTruthToD3PD(alg, **kw):
    from AthenaCommon.AppMgr      import ToolSvc
    preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())

    # first things first: fix the truth bug
    
    from McParticleAlgs.ReOrderMcEventCollection import  FixMcEventCollection
    topSequence = AlgSequence()
    topSequence.Members.insert(1, FixMcEventCollection() )

    D3PDMakerFlags.TruthDoPileup     = False
    D3PDMakerFlags.TruthWritePartons = True
    D3PDMakerFlags.TruthWriteHadrons = False
    D3PDMakerFlags.TruthWriteGeant   = False

    if PhotonD3PDMakerFlags.SaveAllTruthParticles():
      D3PDMakerFlags.TruthWriteHadrons = True

    # These give us a bit too much
    # from MuonD3PDMaker.TruthMuonD3PDObject               import TruthMuonD3PDObject
    # alg += TruthMuonD3PDObject    (**_args (1, 'TruthMuon', kw))
    # truth_info = TruthParticleD3PDObject(**_args (1, 'TruthParticle', kw))

    # ------------------------------------------
    # GenEvent dumping
    #
    import TruthD3PDAnalysis
    from TruthD3PDAnalysis.TruthD3PDAnalysisConf import D3PD__GenObjectsFilterTool
    SimpleGenEventFilterTool = D3PD__GenObjectsFilterTool( "SimpleGenEventFilterTool" )
    SimpleGenEventFilterTool.RemoveInTimePileUp = True
    SimpleGenEventFilterTool.Remove2BCPileUp    = True
    SimpleGenEventFilterTool.Remove800nsPileUp  = True
    SimpleGenEventFilterTool.RemoveCavernBkg    = True
    SimpleGenEventFilterTool.RemoveDummyEvents  = True
    ToolSvc += SimpleGenEventFilterTool

    from TruthD3PDMaker.GenEventD3PDObject      import GenEventD3PDObject
    alg += GenEventD3PDObject     (**_args (1, 'GenEvent', kw,
                                            filter=SimpleGenEventFilterTool))
    # ------------------------------------------

    PhotonTruthCutTool = PhotonD3PDMaker.TruthParticleFilterCutTool(name   = "PhotonTruthCutTool",
                                                                    PDG_IDs = [22],
                                                                    pT_low = 1000,
                                                                    eta_high = 2.5)
    
    ElectronTruthCutTool = PhotonD3PDMaker.TruthParticleFilterCutTool(name   = "ElectronTruthCutTool",
                                                                      PDG_IDs = [11],
                                                                      pT_low = 1000,
                                                                      eta_high = 2.5)
    
    MuonTruthCutTool = PhotonD3PDMaker.TruthParticleFilterCutTool(name   = "MuonTruthCutTool",
                                                                  PDG_IDs = [13])

    TauTruthCutTool = PhotonD3PDMaker.TruthParticleFilterCutTool(name   = "TauTruthCutTool",
                                                                 PDG_IDs = [15,17])

    BosonTruthCutTool = PhotonD3PDMaker.TruthParticleFilterCutTool(name = "BosonTruthCutTool",
                                                                   PDG_IDs = [23, 24, 25, 32, 33, 34, 35, 36, 37])

    PhotonD3PDMakerFlags.TruthParticleFilters = [PhotonTruthCutTool,
                                                 ElectronTruthCutTool,
                                                 MuonTruthCutTool,
                                                 TauTruthCutTool,
                                                 BosonTruthCutTool]

    for i in PhotonD3PDMakerFlags.TruthParticleFilters():
        if not i in ToolSvc:
            ToolSvc += i

    doSortTruthParticles = False
    TP_container_name = 'TruthParticle'
    if doSortTruthParticles:
        # sort the truth particles before dumping them - probably not necessary
        TP_container_name = 'SortedTruthParticles'
        sorter =  makeTruthParticleSorterAlg(D3PDMakerFlags.TruthSGKey(),
                                             'TruthParticleContainer',
                                             TP_container_name)
        preseq += sorter
        D3PDMakerFlags.TruthParticlesSGKey = 'SortedTruthParticles,'+D3PDMakerFlags.TruthParticlesSGKey()
        
    from PhotonD3PDMaker.PhotonTruthParticleD3PDObject   import PhotonTruthParticleD3PDObject
    truthdump = PhotonTruthParticleD3PDObject(**_args (2, TP_container_name, kw))
    alg += truthdump

    from TrackD3PDMaker.TruthVertexD3PDObject import TruthVertexD3PDObject
    alg += TruthVertexD3PDObject(**_args (0, 'TruthVertex', kw))

    from PhotonD3PDMaker.EgammaTruthParticleD3PDObject import EgammaTruthParticleD3PDObject
    egammatruthdump = EgammaTruthParticleD3PDObject(**_args (2, "FilteredEgammaTruthObject", kw,
                                                       sgkey = "FilteredEgammaTruthCollection",
                                                       prefix = "egammaTruth_"))
    alg += egammatruthdump
    # ---------------------------------------------------------------------------------

    # ------------------------------------------------
    # add the TruthJet D3PDObjects
    for truthjets in PhotonD3PDMakerFlags.alltruthjets():
        alg += JetD3PDObject(**_args (1, truthjets, kw,
                                      prefix="jet_"+truthjets+"_",
                                      sgkey=truthjets+PhotonD3PDMakerFlags.JetTagD3PD_CollectionPostfix(),
                                      exclude=['DQMoments',
                                               'JetSamplingsFrac',
                                               'JetQual',
                                               'EMFraction',
                                               'JES',
                                               'JESMoments',
                                               'EMScale',
                                               'L1Info',
                                               'L2Info',
                                               'EFInfo'],
                                      include=['Kinematics',
                                               JetTagD3PDKeys.TruthInfoBlockName(),
                                               'JetShape']))
    # ------------------------------------------------

    from TauD3PDMaker.TauD3PDMakerConf import TruthTausToSG
    preseq += TruthTausToSG()
    
    from EventCommonD3PDMaker.DRIndexAssociation import DRIndexAssociation
    from TauD3PDMaker.TruthTauD3PDObject         import TruthTauD3PDObject
    from TauD3PDMaker.TauD3PDObject              import TauD3PDObject
    TauTrueTauAssoc = DRIndexAssociation(
        parent = TauD3PDObject,
        type_name = 'TruthParticleContainer',
        default_sgkey = 'TrueHadronicTaus',
        default_drcut = 0.2,
        prefix = 'trueTauAssocSmall_',
        target = 'trueTau_' )
    
    TrueTauTauAssoc = DRIndexAssociation(
        parent = TruthTauD3PDObject,
        type_name = 'Analysis::TauJetContainer',
        default_sgkey = D3PDMakerFlags.TauSGKey(),
        default_drcut = 0.2,
        prefix = 'tauAssocSmall_',
        target = 'tau_' )
    alg += TruthTauD3PDObject(10, sgkey = 'TrueHadronicTaus')
