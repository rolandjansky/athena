# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def PixelClusterization_MonitoringTool():
     newMonTool = GenericMonitoringTool("MonTool")
     newMonTool.defineHistogram('TIME_Total', path='EXPERT',type='TH1F',title="Total time (ms)", xbins = 50, xmin=0.0, xmax=1000.0)
     return newMonTool

def SCT_Clusterization_MonitoringTool():
     newMonTool = GenericMonitoringTool("MonTool")
     newMonTool.defineHistogram('TIME_Total'        , path='EXPERT',type='TH1F',title="Total time (ms)"        , xbins = 100, xmin=0.0, xmax=1000.0)
     newMonTool.defineHistogram('TIME_SummaryTool'  , path='EXPERT',type='TH1F',title="SummaryTool time (ms)"  , xbins = 100, xmin=0.0, xmax=1000.0)
     newMonTool.defineHistogram('TIME_Clusterize'   , path='EXPERT',type='TH1F',title="Clusterize time (ms)"   , xbins = 100, xmin=0.0, xmax=1000.0)
     return newMonTool

