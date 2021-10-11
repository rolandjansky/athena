# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def InDetMonitoringTool():
    newMonTool = GenericMonitoringTool("MonTool")
    newMonTool.defineHistogram('numSctClusters',    type='TH1F',path='EXPERT',title="Number of SCT Clusters",       xbins=50, xmin=0., xmax=100)
    newMonTool.defineHistogram('numPixClusters',    type='TH1F',path='EXPERT',title="Number of PIXEL Clusters",     xbins=50, xmin=0., xmax=100)

    return newMonTool



