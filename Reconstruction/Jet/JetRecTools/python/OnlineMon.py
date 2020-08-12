def getMonTool_Algorithm(path):
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')

    #monTool.HistPath = 'MyGroup/MySubDir'  # default is the parent name of MonTool
    monTool.HistPath = path

    monTool.defineHistogram( 'TIME_execute', path='EXPERT', type='TH1F', title='Counts',
                             xbins=400, xmin=0, xmax=200 )
    monTool.defineHistogram( 'TIME_subtract', path='EXPERT', type='TH1F', title='Counts',
                             xbins=400, xmin=0, xmax=200 )
    return monTool
