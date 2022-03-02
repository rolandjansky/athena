#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

class PlotBinnings:
    def __init__(self,
                 xbins: int,
                 xmin: float,
                 xmax: float):
        self.xbins = xbins
        self.xmin = xmin
        self.xmax = xmax

def defineHistogramWithBinning(monitoringGroup,
                               name: str,
                               type: str,
                               title: str,
                               binningX: PlotBinnings,
                               binningY: PlotBinnings = None) -> None:
    assert isinstance(name, str)
    assert isinstance(type, str)
    assert isinstance(title, str)
    assert isinstance(binningX, PlotBinnings)
    if binningY is not None:
        assert isinstance(binningY, PlotBinnings)

    if binningY is None:
        monitoringGroup.defineHistogram(name,
                                        type=type,
                                        title=title,
                                        xbins=binningX.xbins,
                                        xmin=binningX.xmin,
                                        xmax=binningX.xmax)
    else:
        monitoringGroup.defineHistogram(name, 
                                        type=type, 
                                        title=title,
                                        xbins=binningX.xbins,
                                        xmin=binningX.xmin,
                                        xmax=binningX.xmax,
                                        ybins=binningY.xbins,
                                        ymin=binningY.xmin,
                                        ymax=binningY.xmax)

class ITkPixelBinnings:
    def __init__(self):
        self.Nsp = PlotBinnings(xbins=10000, xmin=0, xmax=10000)
        self.Nseeds = PlotBinnings(xbins=10000, xmin=0, xmax=10000)

        self.Pos_X = PlotBinnings(xbins=400, xmin=-400, xmax=400)
        self.Pos_Y = PlotBinnings(xbins=400, xmin=-400, xmax=400)
        self.Pos_Z = PlotBinnings(xbins=1000, xmin=-3000, xmax=3000)
        self.Pos_R = PlotBinnings(xbins=3500, xmin=0, xmax=350)
        self.Pos_Signed_R = PlotBinnings(xbins=2*self.Pos_R.xbins,
                                         xmin=-self.Pos_R.xmax,
                                         xmax=self.Pos_R.xmax)

        self.Cov_R = PlotBinnings(xbins=20, xmin=0, xmax=6)
        self.Cov_Z = PlotBinnings(xbins=20, xmin=0, xmax=6)

class ITkStripBinnings:
    def __init__(self):
        self.Nsp = PlotBinnings(xbins=10000, xmin=0, xmax=10000)
        self.Nseeds = PlotBinnings(xbins=10000, xmin=0, xmax=10000)

        self.Pos_X = PlotBinnings(xbins=1200, xmin=-1200, xmax=1200)
        self.Pos_Y = PlotBinnings(xbins=1200, xmin=-1200, xmax=1200)
        self.Pos_Z = PlotBinnings(xbins=1000, xmin=-3000, xmax=3000)
        self.Pos_R = PlotBinnings(xbins=12000, xmin=300, xmax=1100)
        self.Pos_Signed_R = PlotBinnings(xbins=2*self.Pos_R.xbins,
                                         xmin=-self.Pos_R.xmax,
                                         xmax=self.Pos_R.xmax)

        self.Cov_R = PlotBinnings(xbins=20, xmin=0, xmax=6)
        self.Cov_Z = PlotBinnings(xbins=20, xmin=0, xmax=6)

class InDetPixelBinnings:
    def __init__(self):
        self.Nsp = PlotBinnings(xbins=10000, xmin=0, xmax=10000)
        self.Nseeds = PlotBinnings(xbins=10000, xmin=0, xmax=10000)

        self.Pos_X = PlotBinnings(xbins=160, xmin=-160, xmax=160)
        self.Pos_Y = PlotBinnings(xbins=160, xmin=-160, xmax=160)
        self.Pos_Z = PlotBinnings(xbins=1100, xmin=-1100, xmax=1100)
        self.Pos_R = PlotBinnings(xbins=1600, xmin=0, xmax=160)
        self.Pos_Signed_R = PlotBinnings(xbins=2*self.Pos_R.xbins,
                                         xmin=-self.Pos_R.xmax,
                                         xmax=self.Pos_R.xmax)
                                    
        self.Cov_R = PlotBinnings(xbins=20, xmin=0, xmax=6)
        self.Cov_Z = PlotBinnings(xbins=20, xmin=0, xmax=6)
                                    

class InDetSCTBinnings:
    def __init__(self):
        self.Nsp = PlotBinnings(xbins=10000, xmin=0, xmax=10000)
        self.Nseeds = PlotBinnings(xbins=10000, xmin=0, xmax=10000)
                                    
        self.Pos_X = PlotBinnings(xbins=600, xmin=-600, xmax=600)
        self.Pos_Y = PlotBinnings(xbins=600, xmin=-600, xmax=600)
        self.Pos_Z = PlotBinnings(xbins=1000, xmin=-3000, xmax=3000)
        self.Pos_R = PlotBinnings(xbins=5700, xmin=250, xmax=570)
        self.Pos_Signed_R = PlotBinnings(xbins=2*self.Pos_R.xbins,
                                         xmin=-self.Pos_R.xmax,
                                         xmax=self.Pos_R.xmax)

        self.Cov_R = PlotBinnings(xbins=20, xmin=0, xmax=6)
        self.Cov_Z = PlotBinnings(xbins=20, xmin=0, xmax=6)


