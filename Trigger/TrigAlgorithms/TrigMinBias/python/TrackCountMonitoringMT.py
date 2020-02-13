def TrackCountMonitoring():

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')
    monTool.defineHistogram('ntrks', path='EXPERT', type='TH1I', title='ntrks',xbins=100, xmin=-0, xmax=2000)
    monTool.defineHistogram('counts', path='EXPERT', type='TH1I', title='counts',xbins=50, xmin=-0.5, xmax=4.5)
    return monTool
