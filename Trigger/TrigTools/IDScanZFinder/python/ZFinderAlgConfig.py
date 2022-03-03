# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def MinBiasZFinderCfg(flags):
    acc = ComponentAccumulator()
    TrigZFinder = CompFactory.TrigZFinder
    tools = [TrigZFinder("HMT_TripletPixOnly", TripletMode=1, MinZBinSize=3.5, PhiBinSize=0.50, UseOnlyPixels=True, MaxLayer=3)]


    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')

    monTool.defineHistogram( 'ZVertex', path='EXPERT', type='TH1F', title='Vertex Z distribution;z [mm];Entries',
                            xbins=400, xmin=-200, xmax=200 )
    monTool.defineHistogram( 'ZVertexWeight', path='EXPERT', type='TH1F', title='Vertex Weight;Weight;Entries',
                            xbins=100, xmin=0.0, xmax=100 )
                            

    MinBiasZFinderAlg = CompFactory.TrigZFinderAlg("TrigZFinderAlg", 
                                                   vertexKey=recordable("HLT_vtx_z"), 
                                                   MonTool = monTool,
                                                   ZFinderTools = tools)


    acc.addEventAlgo(MinBiasZFinderAlg)
    return acc