 # Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
def TrackCountMonitoring():
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')
    monTool.defineHistogram('ntrks', path='EXPERT', type='TH1I', title='ntrks',xbins=100, xmin=-0.5, xmax=4999.5)
    from TrigMinBias.TrigMinBiasConf import TrackCountHypoAlgMT
    alg=TrackCountHypoAlgMT()
    for i in range(len(alg.min_pt)):
        monTool.defineHistogram('counts_minpTcutIndex_{}_maxZ0cutIndex_{}'.format(i,i), path='EXPERT', type='TH1I', title='counts for min pT and max z0 cut',xbins=100, xmin=-0.5, xmax=4999.5)
    return monTool
