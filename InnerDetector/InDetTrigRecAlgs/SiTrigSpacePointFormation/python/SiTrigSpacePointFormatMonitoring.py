# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class SiTrigSpacePointFormatMonitorBase(TrigGenericMonitoringToolConfig):
    def __init__(self, name="SiTrigSpacePointFormatMonitorBase", type="electron"):
        super (SiTrigSpacePointFormatMonitorBase, self).__init__(name)


        maxclu = 250
        from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
        deta = InDetTrigSliceSettings[('etaHalfWidth',type)]
        if deta>=3.:
            maxclu = 2000
        elif deta>=0.2:
            maxclu = 1000
            
        self.Histograms += [ defineHistogram('numSctClusters',
                                             type='TH1F',
                                             title="Number of SCT Clusters",
                                             xbins = 250, xmin=0., xmax=maxclu)]
        self.Histograms += [ defineHistogram('numPixClusters',
                                             type='TH1F',
                                             title="Number of PIXEL Clusters",
                                             xbins = 250, xmin=0., xmax=maxclu)]
        self.Histograms += [ defineHistogram('numSctSpacePoints',
                                             type='TH1F',
                                             title="Number of SCT Space Points",
                                             xbins = 250, xmin=0., xmax=maxclu)]
        self.Histograms += [ defineHistogram('numPixSpacePoints',
                                             type='TH1F',
                                             title="Number of PIXEL Space Points",
                                             xbins = 250, xmin=0., xmax=maxclu)]      
        self.Histograms += [ defineHistogram('PixSPHashId',
                                             type='TH1F',
                                             title="SP hash ID in PIXEL",
                                             xbins = 200, xmin=0., xmax=2000.)]
        self.Histograms += [ defineHistogram('SctSPHashId',
                                             type='TH1F',
                                             title="SP hash ID in SCT",
                                             xbins = 200, xmin=0., xmax=8300.)]

        if (deta>=0.2):
            self.Histograms += [ defineHistogram('numSctClusters,numSctSpacePoints',
                                                 type='TH2F',
                                                 title="SCT Clusters vs Space Points (Zero)",
                                                 xbins = 50, xmin=0., xmax=50,
                                                 ybins = 50, ymin=0., ymax=50)]
            self.Histograms += [ defineHistogram('numPixClusters,numPixSpacePoints',
                                                 type='TH2F',
                                                 title="PIXEL Clusters vs Space Points (Zero)",
                                                 xbins = 50, xmin=0., xmax=50,
                                                 ybins = 50, ymin=0., ymax=50)]
        
#
#-- online
#        
class SiTrigSpacePointFormatOnlineMonitor(SiTrigSpacePointFormatMonitorBase):
    def __init__(self, name="SiTrigSpacePointFormatOnlineMonitor", type="electron"):
        super (SiTrigSpacePointFormatOnlineMonitor, self).__init__(name,type)
        self.defineTarget("Online") 
#
#-- validation
#
class SiTrigSpacePointFormatValidationMonitor(SiTrigSpacePointFormatMonitorBase):
    def __init__(self, name="SiTrigSpacePointFormatValidationMonitor", type="electron"):
        super (SiTrigSpacePointFormatValidationMonitor, self).__init__(name,type)
        self.defineTarget("Validation") 

