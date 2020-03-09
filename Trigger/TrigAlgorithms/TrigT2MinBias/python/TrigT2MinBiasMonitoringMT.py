
def SpCountMonitoring():

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool('MonTool')
    monTool.defineHistogram('totNumPixCL_1', path='EXPERT', type='TH1I', title='totNumPixCL_1',xbins=100, xmin=-0, xmax=2000)
    monTool.defineHistogram('totPixBeforeCuts', path='EXPERT', type='TH1I', title='totPixBeforeCuts', xbins = 250, xmin=-0.5, xmax=4999.5)
    monTool.defineHistogram('totNumPixCL_2', path='EXPERT', type='TH1I', title='totNumPixCL_2',xbins=100, xmin=-0, xmax=2000)
    monTool.defineHistogram('totNumPixCLmin3', path='EXPERT', type='TH1I', title='totNumPixCLmin3',xbins=100, xmin=-0, xmax=2000)
    monTool.defineHistogram('totNumPixSP', path='EXPERT', type='TH1I', title='totNumPixSP', xbins = 250, xmin=-0.5, xmax=4999.5)
    monTool.defineHistogram('pixClBarrel', path='EXPERT', type='TH1I', title='pixClBarrel', xbins = 200, xmin=-0.5, xmax=1999.5)
    monTool.defineHistogram('pixClEndcapA', path='EXPERT', type='TH1I', title='pixClEndcapA', xbins = 200, xmin=-0.5, xmax=1999.5)
    monTool.defineHistogram('pixClEndcapC', path='EXPERT', type='TH1I', title='pixClEndcapC', xbins = 200, xmin=-0.5, xmax=1999.5)
    monTool.defineHistogram('totNumSctSP', path='EXPERT', type='TH1I', title='totNumSctSP', xbins = 250, xmin=-0.5, xmax=4999.5)
    monTool.defineHistogram('SctSpEndcapC', path='EXPERT', type='TH1I', title='SctSpEndcapC', xbins = 200, xmin=-0.5, xmax=1999.5)
    monTool.defineHistogram('SctSpBarrel', path='EXPERT', type='TH1I', title='SctSpBarrel', xbins = 200, xmin=-0.5, xmax=1999.5)
    monTool.defineHistogram('SctSpEndcapA', path='EXPERT', type='TH1I', title='SctSpEndcapA', xbins = 200, xmin=-0.5, xmax=1999.5)
    monTool.defineHistogram('totNumPixSP, totNumSctSP', path='EXPERT', type='TH2I', title='SP ; pix SP after ToT cut; sct SP', xbins = 50, xmin=-0.5, xmax=49.5, ybins = 50, ymin=-0.5, ymax=49.5)
    return monTool
