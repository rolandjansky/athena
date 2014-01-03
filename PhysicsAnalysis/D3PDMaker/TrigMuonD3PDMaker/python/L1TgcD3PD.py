# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: L1TgcD3PD.py 452967 2011-08-09 15:37:43Z takashi $
#
# @file D3PDMakerConfig/python/physicsD3PD.py
# @author maarten boonekamp <maarten.boonekamp@cea.fr>
# @date Sep, 2009
# @brief Construct a physics D3PD.
#


import D3PDMakerCoreComps

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from EventCommonD3PDMaker.EventInfoD3PDObject          import EventInfoD3PDObject
from EventCommonD3PDMaker.LBMetadataConfig             import LBMetadataConfig
from EventCommonD3PDMaker.GenEventD3PDObject           import GenEventD3PDObject

from MuonD3PDMaker.MuonD3PDObject                      import MuonD3PDObject
from MuonD3PDMaker.MuonSegmentD3PDObject               import MuonSegmentD3PDObject

from TrackD3PDMaker.VertexD3PDObject                   import PrimaryVertexD3PDObject
from TrackD3PDMaker.TrackD3PDObject                    import TrackParticleD3PDObject


from RecExConfig.RecFlags                              import rec

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw


def L1TgcD3PD ( alg = None,
                file = "l1tgc.root",
                tuplename = 'physics',
                **kw ):

    # Create the algorithm if it was not given to the function already:
    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream( tuplename, file )
        pass

    ### event information
    alg += EventInfoD3PDObject        (**_args (10, 'EventInfo', kw))

    ### Primary vertex block - May be missing in single-beam data.
    alg += PrimaryVertexD3PDObject (**_args (1, 'PrimaryVertex', kw,
                                             allowMissing = True,
                                             sgkey = D3PDMakerFlags.VertexSGKey(),
                                             prefix = 'vxp_'))

    ### ID track block
    alg += TrackParticleD3PDObject(1)

    ### Muon blocks
    alg += MuonD3PDObject             (**_args (10, 'MuidMuon', kw,
                                                sgkey='MuidMuonCollection', prefix='mu_muid_'))
    alg += MuonD3PDObject             (**_args (10, 'StacoMuon', kw,
                                                sgkey='StacoMuonCollection', prefix='mu_staco_'))
    alg += MuonD3PDObject             (**_args (10, 'CaloMuon', kw,
                                                sgkey='CaloMuonCollection', prefix='mu_calo_'))


    ### Muon segment blocks
    alg += MuonSegmentD3PDObject      (**_args (10, 'MooreSeg', kw,
                                                sgkey='MooreSegments', prefix='mooreseg_',
                                                include=['MooreSegmentT0'], exclude = ['MuonboySegmentT0']))
    alg += MuonSegmentD3PDObject      (**_args (10, 'MboySeg', kw,
                                                sgkey='ConvertedMBoySegments', prefix='mboyseg_',
                                                include=['MuonboySegmentT0'], exclude = ['MooreSegmentT0']))
    alg += MuonSegmentD3PDObject      (**_args (10, 'MuGirlSegment', kw,
                                                sgkey='MuGirlSegments', prefix='mgseg_',
                                                include=['MooreSegmentT0'], exclude = ['MuonboySegmentT0']))


    ### Truth
    if rec.doTruth():
        from MuonD3PDMaker.TruthMuonD3PDObject  import TruthMuonD3PDObject
        from EventCommonD3PDMaker.TruthParticleD3PDObject import TruthParticleD3PDObject
        alg += TruthMuonD3PDObject    (**_args ( 2, 'TruthMuon', kw))
        alg += GenEventD3PDObject     (**_args ( 1, 'GenEvent', kw))
        alg += TruthParticleD3PDObject(**_args ( 1, 'TruthParticle', kw))


    ### Trigger
    #if D3PDMakerFlags.DoTrigger():
    if 1:
        from TriggerD3PDMaker.BGCodeD3PDObject import BGCodeD3PDObject
        alg += BGCodeD3PDObject (**_args (2, 'BGCode', kw))

        from TriggerD3PDMaker.MuonROID3PDObject import MuonROID3PDObject
        alg += MuonROID3PDObject (**_args (10, 'MuonROI', kw))

        from TriggerD3PDMaker.MuCTPID3PDObject import MuCTPID3PDObject
        alg += MuCTPID3PDObject (**_args (10, 'MuCTPI', kw))


        from AthenaCommon.AppMgr import ToolSvc
        from AthenaCommon.AppMgr import ServiceMgr
        from AthenaCommon.AppMgr import theApp

        #from AthenaCommon.Constants import *
        #ServiceMgr.MessageSvc.OutputLevel = DEBUG

        #-------------------------------------------------------------
        # TGCcablingServerSvc
        #-------------------------------------------------------------
        from MuonCablingServers.MuonCablingServersConf import TGCcablingServerSvc
        ServiceMgr += TGCcablingServerSvc()
        theApp.CreateSvc += ["TGCcablingServerSvc"]
        ServiceMgr.TGCcablingServerSvc.Atlas = True
        ServiceMgr.TGCcablingServerSvc.forcedUse = True
        ServiceMgr.TGCcablingServerSvc.useMuonTGC_CablingSvc = True
        from TGC_CondCabling.TGC_CondCablingConf import TGCCablingDbTool
        ToolSvc += TGCCablingDbTool()
        from IOVDbSvc.CondDB import conddb
        conddb.addFolderSplitMC('TGC','/TGC/CABLING/MAP_SCHEMA','/TGC/CABLING/MAP_SCHEMA')
        import MuonCnvExample.MuonCablingConfig

        #-------------------------------------------------------------
        # RPCcablingServerSvc
        #-------------------------------------------------------------
        from MuonCablingServers.MuonCablingServersConf import RPCcablingServerSvc
        ServiceMgr += RPCcablingServerSvc()
        theApp.CreateSvc += [ "RPCcablingServerSvc" ]
        ServiceMgr.RPCcablingServerSvc.Atlas = True
        ServiceMgr.RPCcablingServerSvc.forcedUse = True
        ServiceMgr.RPCcablingServerSvc.useMuonRPC_CablingSvc = True
        from RPCcablingSim.RPCcablingSimConf import RPCcablingSimSvc
        from MuonRPC_Cabling.MuonRPC_CablingConf import MuonRPC_CablingSvc
        import MuonRPC_Cabling.MuonRPC_CablingConfig
        ServiceMgr.MuonRPC_CablingSvc.RPCTriggerRoadsfromCool = True
        conddb.addFolderSplitMC("RPC","/RPC/CABLING/MAP_SCHEMA"     , "/RPC/CABLING/MAP_SCHEMA")
        conddb.addFolderSplitMC("RPC","/RPC/CABLING/MAP_SCHEMA_CORR", "/RPC/CABLING/MAP_SCHEMA_CORR")
        conddb.addFolderSplitMC("RPC","/RPC/TRIGGER/CM_THR_ETA", "/RPC/TRIGGER/CM_THR_ETA")
        conddb.addFolderSplitMC("RPC","/RPC/TRIGGER/CM_THR_PHI", "/RPC/TRIGGER/CM_THR_PHI")
        from RPC_CondCabling.RPC_CondCablingConf import RPCCablingDbTool
        ToolSvc += RPCCablingDbTool()
        ToolSvc.RPCCablingDbTool.MapConfigurationFolder = "/RPC/CABLING/MAP_SCHEMA"
        ToolSvc.RPCCablingDbTool.MapCorrectionFolder = "/RPC/CABLING/MAP_SCHEMA_CORR"

        #-------------------------------------------------------------
        # TrigEffJpsiTools
        #-------------------------------------------------------------
        from TrigEffJpsiTools.TrigEffJpsiToolsConf import TrigEffJpsiTools
        ToolSvc += TrigEffJpsiTools("TrigEffJpsiTools")

        #-------------------------------------------------------------
        # TrigMuonCoinHierarchy
        #-------------------------------------------------------------
        from TrigMuonCoinHierarchy.TrigMuonCoinHierarchyConf import Trigger__MuonRoiChainFindTool
        MuonRoiChainFindTool = Trigger__MuonRoiChainFindTool("Trigger::MuonRoiChainFindTool")
        MuonRoiChainFindTool.skipRoiThresholdComparison = True
        ToolSvc += MuonRoiChainFindTool


        #-------------------------------------------------------------
        # L1TGC D3PD
        #-------------------------------------------------------------
        from TrigMuonD3PDMaker.L1TgcPrepData_D3PDObject import L1TgcPrepData_D3PDObject
        alg += L1TgcPrepData_D3PDObject(level=10)

        from TrigMuonD3PDMaker.L1TgcCoinData_D3PDObject import L1TgcCoinData_D3PDObject
        alg += L1TgcCoinData_D3PDObject(level=10)

        from TrigMuonD3PDMaker.L1TgcCoinHierarchy_D3PDObject import L1TgcCoinHierarchy_D3PDObject
        alg += L1TgcCoinHierarchy_D3PDObject(level=10)

        from TrigMuonD3PDMaker.L1TgcRpcPrepData_D3PDObject import L1TgcRpcPrepData_D3PDObject
        alg += L1TgcRpcPrepData_D3PDObject(level=10)

        from TrigMuonD3PDMaker.L1TgcMdtPrepData_D3PDObject import L1TgcMdtPrepData_D3PDObject
        alg += L1TgcMdtPrepData_D3PDObject(level=10)

        from TrigMuonD3PDMaker.L1TgcTrkExtrapolation_D3PDObject import L1TgcStacoExtrapolation_D3PDObject
        alg += L1TgcStacoExtrapolation_D3PDObject(level=10)
        from TrigMuonD3PDMaker.L1TgcTrkExtrapolation_D3PDObject import L1TgcStacoBiasedExtrapolation_D3PDObject
        alg += L1TgcStacoBiasedExtrapolation_D3PDObject(level=10)
        from TrigMuonD3PDMaker.L1TgcTrkExtrapolation_D3PDObject import L1TgcMuidExtrapolation_D3PDObject
        alg += L1TgcMuidExtrapolation_D3PDObject(level=10)
        from TrigMuonD3PDMaker.L1TgcTrkExtrapolation_D3PDObject import L1TgcMuidBiasedExtrapolation_D3PDObject
        alg += L1TgcMuidBiasedExtrapolation_D3PDObject(level=10)
        from TrigMuonD3PDMaker.L1TgcTrkExtrapolation_D3PDObject import L1TgcCaloExtrapolation_D3PDObject
        alg += L1TgcCaloExtrapolation_D3PDObject(level=10)

        from TrigMuonD3PDMaker.L1TgcTriggerInfo_D3PDObject import L1TgcTriggerInfo_D3PDObject
        alg += L1TgcTriggerInfo_D3PDObject(level=10)

        # TandP info
        from TrigMuonD3PDMaker.L1TgcTandp_D3PDObject import L1TgcStacoTandp_D3PDObject
        alg += L1TgcStacoTandp_D3PDObject(level=10)
        from TrigMuonD3PDMaker.L1TgcTandp_D3PDObject import L1TgcMuidTandp_D3PDObject
        alg += L1TgcMuidTandp_D3PDObject(level=10)


        from TrkMagFieldTools.TrkMagFieldToolsConf import Trk__MagneticFieldTool
        InDetMagField = Trk__MagneticFieldTool('InDetMagField')
        ToolSvc += InDetMagField

        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        InDetExtrapolator = AtlasExtrapolator()

        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        TrkVKalVrtFitter = Trk__TrkVKalVrtFitter(Extrapolator = InDetExtrapolator,
                                                 MagFieldSvc = InDetMagField,
                                                 FirstMeasuredPoint = True,
                                                 #FirstMeasuredPointLimit = True,
                                                 MakeExtendedVertex  = True)
        ToolSvc += TrkVKalVrtFitter

    # Event metadata
    alg.MetadataTools += [LBMetadataConfig()]

    # Event Filter
    from TrigMuonD3PDMaker.TrigMuonD3PDMakerConf import L1TgcSkim
    skim = L1TgcSkim()
    #skim.DumpAll = True
    skim.PassThroughFilterPattern = "EF_L1MU[0-9]+_NoAlg"

    # for Muons stream
    if (rec.triggerStream() == "Muons"):
        skim.PreScaleTandp = 100 # events with Jpsi/Z candidates are prescaled by the factor specified
        skim.PreScaleNotTandp = 750 # events without Jpsi/Z candidates are prescaled by the factor specified

    # for JetTauEtmiss stream
    if (rec.triggerStream() == "JetTauEtmiss"):
        skim.JetFilterPattern = ("(EF_te[0-9]+$)|" +
                                 "(EF_xs[0-9]+((_loose)|(_medium)|(_tight))?_noMu$)|" +
                                 "(EF_xe[0-9]+((_loose)|(_medium)|(_tight))?_noMu$)|" +
                                 "(EF_je[0-9]+(_NoEF)?$)|" +
                                 "(EF_L1[F]?J[0-9]+(_NoAlg)?$)|" +
                                 "(EF_[f]?j[0-9]+_[f]?j[0-9]+_a[0-9]+tc_EFFS(_anymct[0-9]+)?$)|" +
                                 "(EF_tau[0-9]+((_loose)|(_medium)|(_tight))?$)|" +
                                 "(EF_[0-9]?[f]?j[0-9]+_a[0-9]+tc_EFFS((_deta[0-9]+_F(B|C))|(_xe[0-9]+((_loose)|(_medium)|(_tight))?_noMu))?$)")
        print skim.JetFilterPattern
        if (len(skim.JetFilterPattern)):
            skim.SingleMuon = True
            skim.PreScaleNotTandp = 50 # events without Jpsi/Z candidates are prescaled by the factor specified

    alg.filterSeq += skim

    return alg
# eof
