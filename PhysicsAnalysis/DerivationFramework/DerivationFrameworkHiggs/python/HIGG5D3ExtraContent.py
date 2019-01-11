# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

import HIGG5Common
ExtraContent=HIGG5Common.filterContentList('egammaClusters|Muons|TauJets|AntiKt4EMTopoJets|AntiKt4EMPFlowJets|AntiKtVR30Rmax4Rmin02TrackJets|AntiKt10LCTopoJets|AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets|BTagging_AntiKtVR30Rmax4Rmin02Track|Photons|BTagging_AntiKt4EMTopo|BTagging_AntiKt4EMPFlow',
                                                  HIGG5Common.getHIGG5Common() )
ExtraContent+=[
    "Photons.f3core",
#    "AntiKt4EMPFlowJets.btaggingLink",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.HbbScore.KtDR.Dip12.ZCut12.ThrustMin.ThrustMaj.Angularity.Aplanarity.Sphericity.PlanarFlow.FoxWolfram2.FoxWolfram0",
    "AntiKt4EMTopoJets.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1"
    ]

ExtraContentTruth=HIGG5Common.getHIGG5CommonTruth()

ExtraContainers=[]

ExtraContainersTruth=[]
