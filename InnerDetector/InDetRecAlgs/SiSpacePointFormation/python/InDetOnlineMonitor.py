from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def InDetMonitoringTool():
    newMonTool = GenericMonitoringTool("MonTool")
    newMonTool.defineHistogram('numSctClusters',    type='TH1F',path='EXPERT',title="Number of SCT Clusters",       xbins=50, xmin=0., xmax=100)
    newMonTool.defineHistogram('numPixClusters',    type='TH1F',path='EXPERT',title="Number of PIXEL Clusters",     xbins=50, xmin=0., xmax=100)
    newMonTool.defineHistogram('numSctSpacePoints', type='TH1F',path='EXPERT',title="Number of SCT Space Points",   xbins=50, xmin=0., xmax=100)
    newMonTool.defineHistogram('numPixSpacePoints', type='TH1F',path='EXPERT',title="Number of PIXEL Space Points", xbins=50, xmin=0., xmax=100)

    return newMonTool



