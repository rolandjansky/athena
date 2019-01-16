# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

#
#--- define base classes
#

class SctTrigPrepRawDataFormatMonitorBase(TrigGenericMonitoringToolConfig):
  def __init__(self, name="SctTrigPrepRawDataFormatMonitorBase", type="electron"):
    super (SctTrigPrepRawDataFormatMonitorBase, self).__init__(name)

    maxclu = 200
    maxid = 400
    maxerrbins = 45
    from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
    deta = InDetTrigSliceSettings[('etaHalfWidth',type)]
    if deta>3.:
      maxclu = 2000
      maxid = 3000
    elif deta>0.7:
      maxclu = 1000
      maxid = 1500
    elif deta>=0.2:
      maxclu = 400
      maxid = 600

    self.Histograms += [ defineHistogram('numSctClusters',
                                         type='TH1F',
                                         title="Number of SCT Clusters",
                                         xbins = 100, xmin=0., xmax=maxclu)]

    self.Histograms += [ defineHistogram('numSctIds',
                                         type='TH1F',
                                         title="Number Det Elements in ROI",
                                         xbins = 100, xmin=0., xmax=maxid)]
        
    self.Histograms += [ defineHistogram('SctBSErr',
                                         type='TH1F',
                                         title="SCT BS decoding errors", 
                                         xbins = maxerrbins, xmin=0.5, xmax=maxerrbins+0.5,
                                         labels='ByteStreamParseError: TimeOutError: BCIDError: LVL1IDError: PreambleError: FormatterError: TrailerError: TrailerOverflowError: HeaderTrailerLimitError: ABCDError: RawError: MaskedLink: RODClockError: TruncatedROD: ROBFragmentError: MissingLinkHeaderError: MaskedROD: ABCDError_Chip0:xxxxx ABCDError_Chip1: ABCDError_Chip2: ABCDError_Chip3: ABCDError_Chip4: ABCDError_Chip5: ABCDError_Error1: ABCDError_Error2: ABCDError_Error4: TempMaskedChip0: TempMaskedChip1: TempMaskedChip2: TempMaskedChip3: TempMaskedChip4: TempMaskedChip5: ABCDError_Error7: ABCDError_Invalid'
                                         ) ]

    self.Histograms += [ defineHistogram('SctHashId',
                                         type='TH1F',
                                         title="Cluster hash ID in SCT", 
                                         xbins = 100, xmin=0., xmax=8300.) ]

    self.Histograms += [ defineHistogram('SctOccupancyHashId',
                                         type='TH1F',
                                         title="Hash ID of modules flagged by occupancy cut SCT", 
                                         xbins = 100, xmin=0., xmax=8300.) ]


class PixelTrigPrepRawDataFormatMonitorBase(TrigGenericMonitoringToolConfig):
  def __init__(self, name="PixelTrigPrepRawDataFormatMonitorBase", type="electron"):
    super (PixelTrigPrepRawDataFormatMonitorBase, self).__init__(name)

    maxclu = 200
    maxid = 400
    from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
    deta = InDetTrigSliceSettings[('etaHalfWidth',type)]
    if deta>3.:
      maxclu = 2000
      maxid = 3000
    elif deta>0.7:
      maxclu = 1000
      maxid = 1500
    elif deta>=0.2:
      maxclu = 400
      maxid = 600

    self.Histograms += [ defineHistogram('numPixClusters',
                                         type='TH1F',
                                         title="Number of Pixel Clusters",
                                         xbins = 100, xmin=0., xmax=maxclu)]

    self.Histograms += [ defineHistogram('numPixIds',
                                         type='TH1F',
                                         title="Number Det Elements in ROI",
                                         xbins = 100, xmin=0., xmax=maxid)]
        
    self.Histograms += [ defineHistogram('PixBSErr',
                                         type='TH1F',
                                         title="Pix BS decoding errors", 
                                         xbins = 10, xmin=0., xmax=20,
                                         labels='TimeOut=0: BCID=1: LVL1ID=2: Preamble=3: Trailer=4: Flagged=5: DisableFE=6: ROD=7: Decoding=8: Invalid=9'
                                         )]

    self.Histograms += [ defineHistogram('PixHashId',
                                         type='TH1F',
                                         title="Cluster hash ID in Pixel", 
                                         xbins = 100, xmin=0., xmax=2000.)]

class TrtTrigPrepRawDataFormatMonitorBase(TrigGenericMonitoringToolConfig):
  def __init__(self, name="TrtTrigPrepRawDataFormatMonitorBase", type="electron"):
    super (TrtTrigPrepRawDataFormatMonitorBase, self).__init__(name)

    maxcirc = 1500
    maxid = 400
    from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
    deta = InDetTrigSliceSettings[('etaHalfWidth',type)]
    if deta>3.:
      maxcirc = 15000
      maxid = 1000
    elif deta>0.7:
      maxcirc = 3000
      maxid = 700
    elif deta>=0.2:
      maxid = 600

    self.Histograms += [ defineHistogram('numTrtDriftCircles',
                                         type='TH1F',
                                         title="Number of TRT Drift Circles",
                                         xbins = 100, xmin=0., xmax=maxcirc)]
    
    self.Histograms += [ defineHistogram('numTrtIds',
                                         type='TH1F',
                                         title="Number Det Elements in ROI",
                                         xbins = 100, xmin=0., xmax=maxid)]
    
    self.Histograms += [ defineHistogram('TrtBsErr',
                                         type='TH1F',
                                         title="TRT BS decoding errors", 
                                         xbins = 6, xmin=0., xmax=20,
                                         labels='BCIDError: L1IDError: MISSINGError: ERRORError: SIDError: RobStatusError'
                                         )]

    self.Histograms += [ defineHistogram('TrtHashId',
                                         type='TH1F',
                                         title="Cluster hash ID in TRT",
                                         xbins = 100, xmin=0., xmax=16000.)]

    
#
#-- online
#

class SctTrigPrepRawDataFormatOnlineMonitor(SctTrigPrepRawDataFormatMonitorBase):
  def __init__(self, name="SctTrigPrepRawDataFormatOnlineMonitor", type="electron"):
    super (SctTrigPrepRawDataFormatOnlineMonitor, self).__init__(name,type)
    self.defineTarget("Online")


class PixelTrigPrepRawDataFormatOnlineMonitor(PixelTrigPrepRawDataFormatMonitorBase):
  def __init__(self, name="PixelTrigPrepRawDataFormatOnlineMonitor", type="electron"):
    super (PixelTrigPrepRawDataFormatOnlineMonitor, self).__init__(name,type)
    self.defineTarget("Online")


class TrtTrigPrepRawDataFormatOnlineMonitor(TrtTrigPrepRawDataFormatMonitorBase):
  def __init__(self, name="TrtTrigPrepRawDataFormatOnlineMonitor", type="electron"):
    super (TrtTrigPrepRawDataFormatOnlineMonitor, self).__init__(name,type)
    self.defineTarget("Online")

        
#        
#--- validation
#

class SctTrigPrepRawDataFormatValidationMonitor(SctTrigPrepRawDataFormatMonitorBase):
  def __init__(self, name="SctTrigPrepRawDataFormatValidationMonitor", type="electron"):
    super (SctTrigPrepRawDataFormatValidationMonitor, self).__init__(name,type)
    self.defineTarget("Validation")

class PixelTrigPrepRawDataFormatValidationMonitor(PixelTrigPrepRawDataFormatMonitorBase):
  def __init__(self, name="PixelTrigPrepRawDataFormatValidationMonitor", type="electron"):
    super (PixelTrigPrepRawDataFormatValidationMonitor, self).__init__(name,type)
    self.defineTarget("Validation")


class TrtTrigPrepRawDataFormatValidationMonitor(TrtTrigPrepRawDataFormatMonitorBase):
  def __init__(self, name="TrtTrigPrepRawDataFormatValidationMonitor", type="electron"):
    super (TrtTrigPrepRawDataFormatValidationMonitor, self).__init__(name,type)
    self.defineTarget("Validation")
        
