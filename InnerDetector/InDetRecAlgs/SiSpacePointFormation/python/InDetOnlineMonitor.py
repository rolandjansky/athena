# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def InDetMonitoringTool():
    newMonTool = GenericMonitoringTool("MonTool")
    newMonTool.defineHistogram('numSctSpacePoints',    type='TH1F',path='EXPERT',title="Number of SCT SpacePoints",       xbins=50, xmin=0., xmax=5000)
    newMonTool.defineHistogram('numPixSpacePoints',    type='TH1F',path='EXPERT',title="Number of PIXEL SpacePoints",     xbins=50, xmin=0., xmax=5000)

    return newMonTool



