# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class InDetTrigAmbiguitySolverMonitorBase(TrigGenericMonitoringToolConfig):
  def __init__(self, name="InDetTrigAmbiguitySolverMonitorBase"):
    super (InDetTrigAmbiguitySolverMonitorBase, self).__init__(name)

    d0max=120.
    z0max=300.
    qoverpmax=3e-3

    self.Histograms += [ defineHistogram('numInTracks',
                                         type='TH1F',
                                         title="Number of Processed Tracks",
                                         xbins = 200, xmin=0., xmax=200.)]
    self.Histograms += [ defineHistogram('numOutTracks',
                                         type='TH1F',
                                         title="Number of Saved Tracks",
                                         xbins = 100, xmin=0., xmax=100.)]
    self.Histograms += [ defineHistogram('chi2ndof',
                                         type='TH1F',
                                         title="chi2ndof",
                                         xbins = 100, xmin=0., xmax=10.)]
    self.Histograms += [ defineHistogram('trk_d0',
                                         type='TH1F',
                                         title="d0",
                                         xbins = 100, xmin=-d0max, xmax=d0max)]
    self.Histograms += [ defineHistogram('trk_z0',
                                         type='TH1F',
                                         title="z0",
                                         xbins = 100, xmin=-z0max, xmax=z0max)]
    self.Histograms += [ defineHistogram('trk_phi',
                                         type='TH1F',
                                         title="phi",
                                         xbins = 100, xmin=-3.142, xmax=3.142)]
    self.Histograms += [ defineHistogram('trk_eta',
                                         type='TH1F',
                                         title="eta",
                                         xbins = 100, xmin=-5., xmax=5)]
    self.Histograms += [ defineHistogram('trk_qoverp',
                                         type='TH1F',
                                         title="qoverp",
                                         xbins = 100, xmin=-qoverpmax, xmax=qoverpmax)]



class InDetTrigAmbiguitySolverValidationMonitor(InDetTrigAmbiguitySolverMonitorBase):
  def __init__(self, name="InDetTrigAmbiguitySolverValidationMonitor"):
    super (InDetTrigAmbiguitySolverValidationMonitor, self).__init__(name)
    self.defineTarget("Validation")

class InDetTrigAmbiguitySolverOnlineMonitor(InDetTrigAmbiguitySolverMonitorBase):
  def __init__(self, name="InDetTrigAmbiguitySolverOnlineMonitor"):
    super (InDetTrigAmbiguitySolverOnlineMonitor, self).__init__(name)
    self.defineTarget("Online")
