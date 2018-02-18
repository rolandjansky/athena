# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

import HIGG5Common
ExtraContent=HIGG5Common.getHIGG5Common()
ExtraContent+=[
   "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.pt.eta.phi.m.constituentLinks.JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m.Angularity.Aplanarity.DetectorEta.ECF1.ECF2.ECF3.FoxWolfram0.FoxWolfram2.GhostMuonSegmentCount.GhostTrackCount.KtDR.Parent.PlanarFlow.Qw.Split12.Split23.Tau1_wta.Tau2_wta.Tau3_wta.ZCut12",
    "Photons.f3core"
   ]

ExtraContentTruth=HIGG5Common.getHIGG5CommonTruth()

ExtraContainers=[]

ExtraContainersTruth=[]
