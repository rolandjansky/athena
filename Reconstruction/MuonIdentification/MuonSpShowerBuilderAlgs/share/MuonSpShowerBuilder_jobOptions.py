# MuonSpShowerBuilder jobOptions
# Frank Paige

from MuonSpShowerBuilderAlgs.MuonSpShowerBuilderAlgsConf import MuonSpShowerBuilder

MuonSpShowerBuilder=MuonSpShowerBuilder(
#    JetCollectionKey = "Cone7H1TowerJets",
    JetCollectionKey = "AntiKt4EMTopoJets",
    jetPtCut = 50000,
    RpcPrepDataKey = "RPC_Measurements",
    TgcPrepDataKey = "TGC_Measurements",
    MdtPrepDataKey = "MDT_DriftCircles",
    MuonSegmentKey = "MuonSegments",
    triggerHitCut = 0,
    mdtHitCut = 0,
    muonSegmentCut = 0)
topSequence += MuonSpShowerBuilder
#print          MuonSpShowerBuilder
