# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class InDetTrigExtensProcessorMonitorBase(TrigGenericMonitoringToolConfig):
  def __init__(self, name="InDetTrigExtensProcessorMonitorBase", type="electron"):
    super (InDetTrigExtensProcessorMonitorBase, self).__init__(name)

    self.Histograms += [ defineHistogram('numTracksOut',
                                         type='TH1F',
                                         title="Number of Tracks at End",
                                         xbins = 50, xmin=0., xmax=50.)]
    self.Histograms += [ defineHistogram('numTracksIn',
                                         type='TH1F',
                                         title="Number of Tracks on Input",
                                         xbins = 50, xmin=0., xmax=50.)]
    self.Histograms += [ defineHistogram('numExtenTracks',
                                         type='TH1F',
                                         title="Number of Extended Tracks saved",
                                         xbins = 50, xmin=0., xmax=50.)]
    self.Histograms += [ defineHistogram('numOrigTracks',
                                         type='TH1F',
                                         title="Number of Original Tracks saved",
                                         xbins = 50, xmin=0., xmax=50.)]
  

class InDetTrigExtensProcessorOnlineMonitor(InDetTrigExtensProcessorMonitorBase):
  def __init__(self, name="InDetTrigExtensProcessorOnlineMonitor", type="electron"):
    super (InDetTrigExtensProcessorOnlineMonitor, self).__init__(name,type)
    self.defineTarget("Online")

class InDetTrigExtensProcessorValidationMonitor(InDetTrigExtensProcessorMonitorBase):
  def __init__(self, name="InDetTrigExtensProcessorValidationMonitor", type="electron"):
    super (InDetTrigExtensProcessorValidationMonitor, self).__init__(name,type)
    self.defineTarget("Validation")
        
        
        
