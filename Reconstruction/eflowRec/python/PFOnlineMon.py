# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def getMonTool_PFAlgorithm():
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')

    #monTool.HistPath = 'MyGroup/MySubDir'  # default is the parent name of MonTool
    monTool.defineHistogram( 'TIME_execute', path='EXPERT', type='TH1F', title='Counts',
                             xbins=100, xmin=0, xmax=1000 )
    monTool.defineHistogram( 'TIME_subtract', path='EXPERT', type='TH1F', title='Counts',
                             xbins=100, xmin=0, xmax=1000 )
    monTool.defineHistogram( 'N_efrTracks', path='EXPERT', type='TH1F', title='Counts',
                             xbins=100, xmin=0, xmax=1000 )
    monTool.defineHistogram( 'N_efrClusters', path='EXPERT', type='TH1F', title='Counts',
                             xbins=100, xmin=0, xmax=2000 )
    return monTool

def getMonTool_PFTrackSelector():
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')

    #monTool.HistPath = 'MyGroup/MySubDir'  # default is the parent name of MonTool
    monTool.defineHistogram( 'TIME_execute', path='EXPERT', type='TH1F', title='Counts',
                             xbins=100, xmin=0, xmax=1000 )
    monTool.defineHistogram( 'TIME_track', path='EXPERT', type='TH1F', title='Counts',
                             xbins=50, xmin=0, xmax=50 )
    monTool.defineHistogram( 'N_tracks', path='EXPERT', type='TH1F', title='Counts',
                             xbins=100, xmin=0, xmax=500 )
    return monTool
