# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigEDMConfig.TriggerEDMRun3 import recordable
from IDScanZFinder.IDScanZFinderConf import TrigZFinderAlg
from IDScanZFinder.IDScanZFinderConf import TrigZFinder

MinBiasZFinderAlg = TrigZFinderAlg("TrigZFinderAlg", vertexKey=recordable("HLT_vtx_z"))
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("default")]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike", TripletMode=1, TripletDZ=1, PhiBinSize=0.1, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike1", TripletMode=1, TripletDZ=1, PhiBinSize=0.05, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike2", TripletMode=1, TripletDZ=1, PhiBinSize=0.02, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike3", TripletMode=1, TripletDZ=1, PhiBinSize=0.01, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike4", TripletMode=1, TripletDZ=0.5, PhiBinSize=0.1, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike5", TripletMode=1, TripletDZ=0.5, PhiBinSize=0.05, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike6", TripletMode=1, TripletDZ=0.5, PhiBinSize=0.02, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike7", TripletMode=1, TripletDZ=0.5, PhiBinSize=0.01, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike8", TripletMode=1, TripletDZ=1.5, PhiBinSize=0.1, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike9", TripletMode=1, TripletDZ=1.5, PhiBinSize=0.05, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike10", TripletMode=1, TripletDZ=1.5, PhiBinSize=0.02, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike11", TripletMode=1, TripletDZ=1.5, PhiBinSize=0.01, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike12", TripletMode=1, TripletDZ=2, PhiBinSize=0.1, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike13", TripletMode=1, TripletDZ=2, PhiBinSize=0.05, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike14", TripletMode=1, TripletDZ=2, PhiBinSize=0.02, UseOnlyPixels=True, MaxLayer=3)]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike15", TripletMode=1, TripletDZ=2, PhiBinSize=0.01, UseOnlyPixels=True, MaxLayer=3)]


from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
monTool = GenericMonitoringTool('MonTool')
monTool.OutputLevel=1

monTool.defineHistogram( 'ZVertex', path='EXPERT', type='TH1F', title='Vertex Z distribution;z [mm];Entries',
                         xbins=400, xmin=-200, xmax=200 )
monTool.defineHistogram( 'ZVertexWeight', path='EXPERT', type='TH1F', title='Vertex Weight;Weight;Entries',
                         xbins=100, xmin=0.0, xmax=100 )
                         
MinBiasZFinderAlg.MonTool = monTool