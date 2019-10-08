from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool

def InDetPriVxFinderMonitoringTool():
    newMonTool = GenericMonitoringTool("PriVxMonTool")
    newMonTool.defineHistogram('numTracks'  , type='TH1F',path='EXPERT',title="Number of Input Tracks", xbins=200, xmin=0., xmax=200)
    newMonTool.defineHistogram('numVertices', type='TH1F',path='EXPERT',title="Number of Vertices    ", xbins=100, xmin=0., xmax=100)

   #All vertices X vs Y position 
    newMonTool.defineHistogram('allVertexZ', type='TH1F',path='EXPERT',title="All Vertex Z position", xbins=400, xmin=-50., xmax=50)
    newMonTool.defineHistogram('allVertexChi2', type='TH1F',path='EXPERT',title="All Vertex Chi2 of the fit", xbins=100, xmin=0., xmax=10)
    newMonTool.defineHistogram('allVertexnDoF', type='TH1F',path='EXPERT',title="All Vertex nDoF of the fit", xbins=100, xmin=0., xmax=10)

    newMonTool.defineHistogram('allVertexX, allVertexY',path='EXPERT', type='TH2F',
                                                 title="All vertices Y vs X; Vertex X [mm]; Vertex Y [mm]",
                                                 xbins=200, xmin=-5, xmax=5, ybins=200, ymin=-5, ymax=5, opt='kLBN')



   #Just primary vertex
    newMonTool.defineHistogram('primVertexZ', type='TH1F',path='EXPERT',title="All Vertex Z position", xbins=400, xmin=-50., xmax=50)
    newMonTool.defineHistogram('primVertexChi2', type='TH1F',path='EXPERT',title="All Vertex Chi2 of the fit", xbins=100, xmin=0., xmax=10)
    newMonTool.defineHistogram('primVertexnDoF', type='TH1F',path='EXPERT',title="All Vertex nDoF of the fit", xbins=100, xmin=0., xmax=10)

    newMonTool.defineHistogram('primVertexX, primVertexY',path='EXPERT', type='TH2F',
                                                 title="Primary Vertex Y vs X; Vertex X [mm]; Vertex Y [mm]",
                                                 xbins=200, xmin=-5, xmax=5, ybins=200, ymin=-5, ymax=5, opt='kLBN')

    return newMonTool

