# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class InDetTrigTrackingxAODCnvMonitorBase(TrigGenericMonitoringToolConfig):
  def __init__(self, name="InDetTrigTrackingxAODCnvMonitorBase", type="electron"):
    super (InDetTrigTrackingxAODCnvMonitorBase, self).__init__(name)

    d0max=5.
    z0max=250.
    qoverpmax=4e-3
    ntrkmax=100
    nvertexmax = 25
    phimax = 3.2
    phibin = 64
    etamax = 3.
    etabin = 60

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
    self.Histograms += [ defineHistogram('trk_d0sig',
                                         type='TH1F',
                                         title="d0sig",
                                         xbins = 100, xmin=-d0max, xmax=d0max)]
    self.Histograms += [ defineHistogram('trk_z0',
                                         type='TH1F',
                                         title="z0",
                                         xbins = 100, xmin=-z0max, xmax=z0max)]
    self.Histograms += [ defineHistogram('trk_phi',
                                         type='TH1F',
                                         title="phi",
                                         xbins = phibin, xmin=-phimax, xmax=phimax)]
    self.Histograms += [ defineHistogram('trk_eta',
                                         type='TH1F',
                                         title="eta",
                                         xbins = etabin, xmin=-etamax, xmax=etamax)]
    self.Histograms += [ defineHistogram('trk_qoverp',
                                         type='TH1F',
                                         title="qoverp",
                                         xbins = 100, xmin=-qoverpmax, xmax=qoverpmax)]
    self.Histograms += [ defineHistogram('trk_nPIXHits',
                                         type='TH1F',
                                         title="npix_hits",
                                         xbins = 20, xmin=0., xmax=20.)]
    self.Histograms += [ defineHistogram('trk_nSCTHits',
                                         type='TH1F',
                                         title="nsct_hits",
                                         xbins = 20, xmin=0., xmax=20.)]
    self.Histograms += [ defineHistogram('trk_nTRTHits',
                                         type='TH1F',
                                         title="ntrt_hits",
                                         xbins = 50, xmin=0., xmax=50.)]
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
    self.Histograms += [ defineHistogram('roi_eta',
                                         type='TH1F',
                                         title="Eta of the input RoI",
                                         xbins = 100, xmin=-5, xmax=5)]
    self.Histograms += [ defineHistogram('roi_phi',
                                         type='TH1F',
                                         title="Phi of the input RoI",
                                         xbins = phibin, xmin=-phimax, xmax=phimax)]
    self.Histograms += [ defineHistogram('roi_z',
                                         type='TH1F',
                                         title="z of the input RoI",
                                         xbins = 100, xmin=-300, xmax=300)]
    self.Histograms += [ defineHistogram('roi_etaWidth',
                                         type='TH1F',
                                         title="Eta width of the input RoI",
                                         xbins = 100, xmin=0, xmax=5)]
    self.Histograms += [ defineHistogram('roi_phiWidth',
                                         type='TH1F',
                                         title="Phi width of the input RoI",
                                         xbins = 2*phibin, xmin=0, xmax=6.4)]
    self.Histograms += [ defineHistogram('roi_zWidth',
                                         type='TH1F',
                                         title="z width of the input RoI",
                                         xbins = 100, xmin=0, xmax=500)]                           
              
    self.Histograms += [ defineHistogram('IBLz, IBLresx',
                                         type='TProfile',
                                         title="IBLresx vs z (profile)",
                                         xbins=70, xmin=-350., xmax=350.,
                                         ymin=-0.1, ymax=0.1 ) ]

    self.Histograms += [ defineHistogram('IBLz, IBLresy',
                                         type='TProfile',
                                         title="IBLresy vs z (profile)",
                                         xbins=70, xmin=-350., xmax=350.,
                                         ymin=-0.1, ymax=+0.1 ) ]

    self.Histograms += [ defineHistogram('trk_z0, IBLHitExpectedAndFound',
                                         type='TProfile',
                                         title="IBLHitExpectedAndFound vs z (profile)",
                                         xbins=50, xmin=-250., xmax=250.,
                                         ymin=-0.1, ymax=+1.1 ) ]


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
                                           xbins = etabin, xmin=-etamax, xmax=etamax)]
      self.Histograms += [ defineHistogram('mu_phi',
                                           type='TH1F',
                                           title="mu_phi",
                                           xbins = phibin, xmin=-phimax, xmax=phimax)]
      self.Histograms += [ defineHistogram('mu_d0, mu_phi',
                                           type='TH2F',
                                           title="reco: d0 vs phi",
                                           xbins=100, xmin=-10., xmax=10.,
                                           ybins=phibin, ymin=-phimax, ymax=phimax ) ]
      self.Histograms += [ defineHistogram('mu_eta, mu_phi',
                                           type='TH2F',
                                           title="reco: eta vs phi",
                                           xbins=etabin, xmin=-etamax, xmax=etamax,
                                           ybins=phibin, ymin=-phimax, ymax=phimax ) ]
      self.Histograms += [ defineHistogram('mu_etaRoi, mu_phiRoi',
                                           type='TH2F',
                                           title="roi: eta vs phi",
                                           xbins=etabin, xmin=-etamax, xmax=etamax,
                                           ybins=phibin, ymin=-phimax, ymax=phimax ) ]
      self.Histograms += [ defineHistogram('mu_eta, mu_etaRoi',
                                           type='TH2F',
                                           title="reco vs roi: eta",
                                           xbins=etabin/2, xmin=-etamax, xmax=etamax,
                                           ybins=etabin/2, ymin=-etamax, ymax=etamax ) ]
      self.Histograms += [ defineHistogram('mu_phi, mu_phiRoi',
                                           type='TH2F',
                                           title="reco vs roi: phi",
                                           xbins=phibin/2, xmin=-phimax, xmax=phimax,
                                           ybins=phibin/2, ymin=-phimax, ymax=phimax ) ]
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

class InDetTrigTrackingxAODCnvOnlineMonitor(InDetTrigTrackingxAODCnvMonitorBase):
  def __init__(self, name="InDetTrigTrackingxAODCnvOnlineMonitor", type="electron"):
    super (InDetTrigTrackingxAODCnvOnlineMonitor, self).__init__(name,type)
    self.defineTarget("Online")

#
#-- validation
#

class InDetTrigTrackingxAODCnvValidationMonitor(InDetTrigTrackingxAODCnvMonitorBase):
  def __init__(self, name="InDetTrigTrackingxAODCnvValidationMonitor", type="electron"):
    super (InDetTrigTrackingxAODCnvValidationMonitor, self).__init__(name,type)
    self.defineTarget("Validation")
