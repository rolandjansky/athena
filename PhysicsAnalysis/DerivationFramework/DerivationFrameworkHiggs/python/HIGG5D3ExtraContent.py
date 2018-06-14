# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

import HIGG5Common
ExtraContent=HIGG5Common.filterContentList('egammaClusters|Muons|TauJets|AntiKt4EMTopoJets|AntiKtVR30Rmax4Rmin02TrackJets|AntiKt10LCTopoJets|AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets|BTagging_AntiKtVR30Rmax4Rmin02Track|Photons|BTagging_AntiKt4EMTopo',
                                                  HIGG5Common.getHIGG5Common() )
ExtraContent+=[
    "Photons.f3core"
    ]

ExtraContentTruth=HIGG5Common.getHIGG5CommonTruth()

ExtraContainers=[]

ExtraContainersTruth=[]
