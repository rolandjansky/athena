# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class InDetTrigParticleCreationMonitorBase(TrigGenericMonitoringToolConfig):
  def __init__(self, name="InDetTrigParticleCreationMonitorBase", type="electron"):
    super (InDetTrigParticleCreationMonitorBase, self).__init__(name)

    d0max=5.
    z0max=40.
    qoverpmax=4e-3
    ntrkmax=100
    nvertexmax = 25
        
    if type=="cosmicsN":
      d0max=1000.
      z0max=1400.
      qoverpmax=0.01
      ntrkmax=20
      nvertexmax=10


    ntrkbins = min(ntrkmax,100)
    nvertexbins = min(nvertexmax,25)

    # Common Histograms
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
    self.Histograms += [ defineHistogram('trk_nPIXHits',
                                         type='TH1F',
                                         title="npix_hits",
                                         xbins = 10, xmin=0., xmax=10.)]
    self.Histograms += [ defineHistogram('trk_nSCTHits',
                                         type='TH1F',
                                         title="nsct_hits",
                                         xbins = 20, xmin=0., xmax=20.)]
    self.Histograms += [ defineHistogram('trk_nTRTHits',
                                         type='TH1F',
                                         title="ntrt_hits",
                                         xbins = 76, xmin=-1., xmax=75.)]
    self.Histograms += [ defineHistogram('trk_chi2dof',
                                         type='TH1F',
                                         title="chi2dof",
                                         xbins = 100, xmin=0., xmax=10.)]
    self.Histograms += [ defineHistogram('roi_nTrkPart',
                                         type='TH1F',
                                         title="ntrkpart",
                                         xbins = ntrkbins, xmin=0., xmax=ntrkmax)]
    self.Histograms += [ defineHistogram('roi_nTracks',
                                         type='TH1F',
                                         title="ntrk",
                                         xbins = ntrkbins, xmin=0., xmax=ntrkmax)]
    self.Histograms += [ defineHistogram('roi_nVertices',
                                         type='TH1F',
                                         title="nvertex",
                                         xbins = nvertexbins, xmin=0., xmax=nvertexmax)]
    # Slice Specific Histograms
    if (type=="bjet"):
      self.Histograms += [ defineHistogram('roi_nTrkPart, bj_sumPt',
                                           type='TH2F',
                                           title="roi_nTrkPart vs bj_sumPt",
                                           xbins=100, xmin=0., xmax=100.,
                                           ybins=100, ymin=0, ymax=5000 ) ]
    if (type=="muon"):
      self.Histograms += [ defineHistogram('mu_pt',
                                           type='TH1F',
                                           title="mu_pt",
                                           xbins = 100, xmin=0., xmax=2000.)]
      self.Histograms += [ defineHistogram('mu_eta',
                                           type='TH1F',
                                           title="mu_eta",
                                           xbins = 100, xmin=-3., xmax=3.)]
      self.Histograms += [ defineHistogram('mu_phi',
                                           type='TH1F',
                                           title="mu_phi",
                                           xbins = 100, xmin=-3.1416, xmax=3.1416)]
      self.Histograms += [ defineHistogram('mu_d0, mu_phi',
                                           type='TH2F',
                                           title="reco: d0 vs phi",
                                           xbins=100, xmin=-5., xmax=5.,
                                           ybins=100, ymin=-3.1416, ymax=3.1416 ) ]
      self.Histograms += [ defineHistogram('mu_eta, mu_phi',
                                           type='TH2F',
                                           title="reco: eta vs phi",
                                           xbins=100, xmin=-3, xmax=3,
                                           ybins=100, ymin=-3.1416, ymax=3.1416 ) ]
      self.Histograms += [ defineHistogram('mu_etaRoi, mu_phiRoi',
                                           type='TH2F',
                                           title="roi: eta vs phi",
                                           xbins=100, xmin=-3, xmax=3,
                                           ybins=100, ymin=-3.1416, ymax=3.1416 ) ]
      self.Histograms += [ defineHistogram('mu_eta, mu_etaRoi',
                                           type='TH2F',
                                           title="reco vs roi: eta",
                                           xbins=100, xmin=-3, xmax=3,
                                           ybins=100, ymin=-3, ymax=3 ) ]
      self.Histograms += [ defineHistogram('mu_phi, mu_phiRoi',
                                           type='TH2F',
                                           title="reco vs roi: phi",
                                           xbins=100, xmin=-3.1416, xmax=3.1416,
                                           ybins=100, ymin=-3.1416, ymax=3.1416 ) ]
    if (type=="tau"):
      self.Histograms += [ defineHistogram('ta_roiDR',
                                           type='TH1F',
                                           title="ta_roiDR",
                                           xbins = 100, xmin=0., xmax=1.)]
      self.Histograms += [ defineHistogram('ta_lPt',
                                           type='TH1F',
                                           title="ta_lPt",
                                           xbins = 100, xmin=0., xmax=1000.)]
      self.Histograms += [ defineHistogram('ta_fracPt',
                                           type='TH1F',
                                           title="ta_fracPt",
                                           xbins = 100, xmin=0., xmax=1.0)]
      self.Histograms += [ defineHistogram('ta_sumCh',
                                           type='TH1F',
                                           title="ta_sumCh",
                                           xbins = 100, xmin=-50., xmax=50.)]
      self.Histograms += [ defineHistogram('ta_asyCh',
                                           type='TH1F',
                                           title="ta_asyCh",
                                           xbins = 24, xmin=-1.2, xmax=1.2)]
      self.Histograms += [ defineHistogram('ta_m',
                                           type='TH1F',
                                           title="ta_m",
                                           xbins = 301, xmin=-1., xmax=300.)]
      
#
#-- online
#

class InDetTrigParticleCreationOnlineMonitor(InDetTrigParticleCreationMonitorBase):
  def __init__(self, name="InDetTrigParticleCreationOnlineMonitor", type="electron"):
    super (InDetTrigParticleCreationOnlineMonitor, self).__init__(name,type)
    self.defineTarget("Online")

#
#-- validation
#

class InDetTrigParticleCreationValidationMonitor(InDetTrigParticleCreationMonitorBase):
  def __init__(self, name="InDetTrigParticleCreationValidationMonitor", type="electron"):
    super (InDetTrigParticleCreationValidationMonitor, self).__init__(name,type)
    self.defineTarget("Validation")
