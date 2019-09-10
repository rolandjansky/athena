from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool

def InDetMonitoringTool():
    newMonTool = GenericMonitoringTool("MonTool")
    newMonTool.defineHistogram('numSctClusters',    type='TH1F',path='EXPERT',title="Number of SCT Clusters",xbins=250,xmin=0.,xmax=10000)
    newMonTool.defineHistogram('numPixClusters',    type='TH1F',path='EXPERT',title="Number of PIXEL Clusters",     xbins=250, xmin=0., xmax=3000)
    newMonTool.defineHistogram('numSctSpacePoints', type='TH1F',path='EXPERT',title="Number of SCT Space Points",   xbins=250, xmin=0., xmax=500)
    newMonTool.defineHistogram('numPixSpacePoints', type='TH1F',path='EXPERT',title="Number of PIXEL Space Points", xbins=250, xmin=0., xmax=500)

    return newMonTool



