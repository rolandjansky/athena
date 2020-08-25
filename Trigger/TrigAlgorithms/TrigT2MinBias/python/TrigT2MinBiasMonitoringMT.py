# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def SpCountMonitoring():

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')
    monTool.defineHistogram('totNumPixCL_1', path='EXPERT', type='TH1I', title='totNumPixCL_1',xbins=100, xmin=-0, xmax=60999.5)
    monTool.defineHistogram('totPixBeforeCuts', path='EXPERT', type='TH1I', title='totPixBeforeCuts', xbins = 250, xmin=-0.5, xmax=6999.5)
    monTool.defineHistogram('totNumPixCL_2', path='EXPERT', type='TH1I', title='totNumPixCL_2',xbins=100, xmin=-0, xmax=60999.5)
    monTool.defineHistogram('totNumPixCLmin3', path='EXPERT', type='TH1I', title='totNumPixCLmin3',xbins=100, xmin=-0, xmax=60999.5)
    monTool.defineHistogram('totNumPixSP', path='EXPERT', type='TH1I', title='totNumPixSP', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('pixClBarrel', path='EXPERT', type='TH1I', title='pixClBarrel', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('pixClEndcapA', path='EXPERT', type='TH1I', title='pixClEndcapA', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('pixClEndcapC', path='EXPERT', type='TH1I', title='pixClEndcapC', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('totNumSctSP', path='EXPERT', type='TH1I', title='totNumSctSP', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('SctSpEndcapC', path='EXPERT', type='TH1I', title='SctSpEndcapC', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('SctSpBarrel', path='EXPERT', type='TH1I', title='SctSpBarrel', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('SctSpEndcapA', path='EXPERT', type='TH1I', title='SctSpEndcapA', xbins = 500, xmin=-0.5, xmax=60999.5)
    monTool.defineHistogram('totNumPixSP, totNumSctSP', path='EXPERT', type='TH2I', title='SP ; pix SP after ToT cut; sct SP', xbins = 500, xmin=-0.5, xmax=60999.5, ybins = 500, ymin=-0.5, ymax=60999.5)
    return monTool


def MbtsFexMTMonitoring():

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')
    monTool.defineHistogram('triggerEnergies', path='EXPERT', type='TH1D', title='triggerEnergies',xbins=100, xmin=-0, xmax=60999.5)
    monTool.defineHistogram('triggerEta', path='EXPERT', type='TH1D', title='triggerEta', xbins = 250, xmin=-5, xmax=5)
    monTool.defineHistogram('triggerPhi', path='EXPERT', type='TH1I', title='triggerPhi',xbins=100, xmin=-3.2, xmax=3.2)
    monTool.defineHistogram('triggerTimes', path='EXPERT', type='TH1I', title='triggerTimes',xbins=100, xmin=-0, xmax=60999.5)
    return monTool
