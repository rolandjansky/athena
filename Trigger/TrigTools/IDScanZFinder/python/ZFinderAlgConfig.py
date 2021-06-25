# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigEDMConfig.TriggerEDMRun3 import recordable
from IDScanZFinder.IDScanZFinderConf import TrigZFinderAlg
from IDScanZFinder.IDScanZFinderConf import TrigZFinder

MinBiasZFinderAlg = TrigZFinderAlg("TrigZFinderAlg", vertexKey=recordable("HLT_vtx_z"))
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("default")]
MinBiasZFinderAlg.ZFinderTools += [TrigZFinder("BeamSpotLike", TripletMode=1, TripletDZ=1, PhiBinSize=0.1, UseOnlyPixels=True, MaxLayer=3)]


from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
monTool = GenericMonitoringTool('MonTool')
monTool.OutputLevel=1

monTool.defineHistogram( 'ZVertex', path='EXPERT', type='TH1F', title='Vertex Z distribution;z [mm];Entries',
                         xbins=400, xmin=-200, xmax=200 )
monTool.defineHistogram( 'ZVertexWeight', path='EXPERT', type='TH1F', title='Vertex Weight;Weight;Entries',
                         xbins=100, xmin=0.0, xmax=100 )
                         
MinBiasZFinderAlg.MonTool = monTool