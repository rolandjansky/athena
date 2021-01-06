# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, \
  TrigGenericMonitoringToolConfig

class SiTrigSPSeededTrackFinderMonitorBase(TrigGenericMonitoringToolConfig):
  def __init__(self, name="SiTrigSPSeededTrackFinderMonitorBase", type="electron"):
    super (SiTrigSPSeededTrackFinderMonitorBase, self).__init__(name)
    
    from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
    deta = InDetTrigSliceSettings[('etaHalfWidth',type)]

    maxtr = 1000
    maxid = 1000
    maxvtx = 15
    maxseeds = 30000
    maxgoodseeds=2000
    maxbadseeds=15000
    maxtimegoodseeds = 1000
    maxtimebadseeds = 3000
    avtimegoodseeds = 1
    avtimebadseeds = 1
    
    if deta>3:
      maxtr = 1000
      maxid = 3000
      maxseeds = 30000
    elif deta>0.7:
      maxtr = 400
      maxid = 2000

    #finally by slices
    if type=="cosmicsN":
      maxtr = 20
      maxseeds = 2000
    
    self.Histograms += [ defineHistogram('numSeeds',
                                         type='TH1F',
                                         title="Number of seeds",
                                         xbins = 200, xmin=0., xmax=maxseeds)]
    self.Histograms += [ defineHistogram('numtracks',
                                         type='TH1F',
                                         title="Number of Si Tracks",
                                         xbins = 1000, xmin=0., xmax=maxtr)]
    self.Histograms += [ defineHistogram('numZvtx',
                                         type='TH1F',
                                         title="Number of Z Vertices",
                                         xbins = 15, xmin=0., xmax=maxvtx)]
    
    if not InDetTrigSliceSettings[('doFullScan',type)]:
      self.Histograms += [ defineHistogram('numDetElPixel',
                                           type='TH1F',
                                           title="Number of Pixel Detector Elements in RoI",
                                           xbins = 100, xmin=0., xmax=maxid)]
      self.Histograms += [ defineHistogram('numDetElSCT',
                                           type='TH1F',
                                           title="Number of SCT Detector Elements in RoI",
                                           xbins = 100, xmin=0., xmax=maxid)]

      self.Histograms += [ defineHistogram('RoIEta',
                                           type='TH1F',
                                           title="RoI Eta",
                                           xbins = 100, xmin=-3., xmax=3.)]
      self.Histograms += [ defineHistogram('RoIPhi',
                                           type='TH1F',
                                           title="RoI Phi",                                                               
                                           xbins = 160, xmin=-3.2, xmax=3.2)]
      self.Histograms += [ defineHistogram('nGoodSeedsTotal',
                                           type='TH1F',
                                           title="m_nGoodSeedsTotal",
                                           xbins = 1000, xmin=0, xmax=maxgoodseeds)]
      self.Histograms += [ defineHistogram('nBadSeedsTotal',
                                           type='TH1F',
                                           title="m_nBadSeedsTotal",
                                           xbins = 1000, xmin=0, xmax=maxbadseeds)]
      self.Histograms += [ defineHistogram('time_GoodSeeds',
                                           type='TH1F',
                                           title="m_timeGoodSeeds",
                                           xbins = 1000, xmin=0, xmax=maxtimegoodseeds)]
      self.Histograms += [ defineHistogram('time_BadSeeds',
                                           type='TH1F',
                                           title="m_timeBadSeeds",
                                           xbins = 1000, xmin=0, xmax=maxtimebadseeds)]
      self.Histograms += [ defineHistogram('time_AveGoodSeeds',
                                           type='TH1F',
                                           title="m_AvtimeGoodSeeds",
                                           xbins = 100, xmin=0, xmax=avtimegoodseeds)]
      self.Histograms += [ defineHistogram('time_AveBadSeeds',
                                           type='TH1F',
                                           title="m_AvtimeBadSeeds",
                                           xbins = 100, xmin=0, xmax=avtimebadseeds)]
      self.Histograms += [ defineHistogram('numSeeds, time_AveGoodSeeds',
                                           type='TH2F',
                                           title="m_AvtimeGoodSeeds_numSeeds",
                                           xbins = 200, xmin=0., xmax=maxseeds,
                                           ybins = 100, ymin=0, ymax=avtimegoodseeds)]
      self.Histograms += [ defineHistogram('numSeeds, time_AveBadSeeds',
                                           type='TH2F',
                                           title="m_AvtimeBadSeeds_numSeeds",
                                           xbins = 200, xmin=0., xmax=maxseeds, 
                                           ybins = 100, ymin=0,  ymax=avtimebadseeds)]


class SiTrigSPSeededTrackFinderOnlineMonitor(SiTrigSPSeededTrackFinderMonitorBase):
  def __init__(self, name="SiTrigSPSeededTrackFinderOnlineMonitor", type="electron"):
    super (SiTrigSPSeededTrackFinderOnlineMonitor, self).__init__(name,type)
    self.defineTarget("Online")

class SiTrigSPSeededTrackFinderValidationMonitor(SiTrigSPSeededTrackFinderMonitorBase):
  def __init__(self, name="SiTrigSPSeededTrackFinderValidationMonitor", type="electron"):
    super (SiTrigSPSeededTrackFinderValidationMonitor, self).__init__(name,type)
    self.defineTarget("Validation")    

        
