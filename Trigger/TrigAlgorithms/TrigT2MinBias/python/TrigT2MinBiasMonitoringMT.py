# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def SpCountMonitoring():

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')
    monTool.defineHistogram('pixCL_1', path='EXPERT', type='TH1I', title='pixCL_1',xbins=100, xmin=-0, xmax=60999.5)
    monTool.defineHistogram('pixCLBeforeCuts', path='EXPERT', type='TH1I', title='totPixBeforeCuts', xbins = 250, xmin=-0.5, xmax=6999.5)
    monTool.defineHistogram('pixCL_2', path='EXPERT', type='TH1I', title='pixCL_2',xbins=100, xmin=-0, xmax=60999.5)
    monTool.defineHistogram('pixCLmin3', path='EXPERT', type='TH1I', title='pixCLmin3',xbins=100, xmin=-0, xmax=60999.5)
    monTool.defineHistogram('pixCL', path='EXPERT', type='TH1I', title='pixCL', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('pixClBarrel', path='EXPERT', type='TH1I', title='pixClBarrel', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('pixClEndcapA', path='EXPERT', type='TH1I', title='pixClEndcapA', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('pixClEndcapC', path='EXPERT', type='TH1I', title='pixClEndcapC', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('pixModulesOverThreshold', path='EXPERT', type='TH1I', title='Pixels ModulesOverThreshold', xbins = 100, xmin=0, xmax=100)
    monTool.defineHistogram('sctSP', path='EXPERT', type='TH1I', title='sctSP', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('sctSPEndcapC', path='EXPERT', type='TH1I', title='sctSPEndcapC', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('sctSPBarrel', path='EXPERT', type='TH1I', title='sctSPBarrel', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('sctSPEndcapA', path='EXPERT', type='TH1I', title='sctSPEndcapA', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('pixCL, sctSP', path='EXPERT', type='TH2I', title='SP ; pix SP after ToT cut; sct SP', xbins = 500, xmin=-0.5, xmax=60999.5, ybins = 500, ymin=-0.5, ymax=60999.5)
    monTool.defineHistogram('sctModulesOverThreshold', path='EXPERT', type='TH1I', title='ACT ModulesOverThreshold', xbins = 100, xmin=0, xmax=100)

    return monTool


def MbtsFexMTMonitoring():

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')
    monTool.defineHistogram('triggerEnergies', path='EXPERT', type='TH1D', title='triggerEnergies',xbins=100, xmin=-0, xmax=60999.5)
    monTool.defineHistogram('triggerEta', path='EXPERT', type='TH1D', title='triggerEta', xbins = 250, xmin=-5, xmax=5)
    monTool.defineHistogram('triggerPhi', path='EXPERT', type='TH1I', title='triggerPhi',xbins=100, xmin=-3.2, xmax=3.2)
    monTool.defineHistogram('triggerTimes', path='EXPERT', type='TH1I', title='triggerTimes',xbins=100, xmin=-0, xmax=60999.5)
    return monTool
