# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def xAODTrackingCnvMonTool():
    montool = GenericMonitoringTool("xAODTrackingCnvMonTool")

    montool.defineHistogram('TIME_Total', path='EXPERT',type='TH1F',title="Total time (ms)", xbins = 100, xmin=0.0, xmax=200)
    return montool
