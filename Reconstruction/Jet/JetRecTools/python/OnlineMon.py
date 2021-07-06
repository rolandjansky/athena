# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def getMonTool_Algorithm(path):
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')

    #monTool.HistPath = 'MyGroup/MySubDir'  # default is the parent name of MonTool
    monTool.HistPath = path

    monTool.defineHistogram( 'TIME_constitmod', path='EXPERT', type='TH1F', title='Counts',
                             xbins=100, xmin=0, xmax=250 )

    return monTool
