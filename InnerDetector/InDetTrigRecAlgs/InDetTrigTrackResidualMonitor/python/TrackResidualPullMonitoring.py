# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

#
#--- define base classes
#

class SctTrackResidualPullMonitorBase(TrigGenericMonitoringToolConfig):
  def __init__(self, name="SctTrackResidualPullMonitorBase", type="electron"):
    super (SctTrackResidualPullMonitorBase, self).__init__(name)

  #  maxclu = 200
  #  maxid = 400
  #  from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
 #   deta = InDetTrigSliceSettings[('etaHalfWidth',type)]
 #   if deta>3.:
 #     maxclu = 2000
 #     maxid = 3000
 #   elif deta>0.7:
 #     maxclu = 1000
 #     maxid = 1500
 #   elif deta>=0.2:
 #     maxclu = 400
 #     maxid = 600


#Biased residual and pull for SCT

    self.Histograms += [ defineHistogram('ResidualSCTBiasedBarrel',
                                         type='TH1F',
                                         title="Residual calculated for the SCT Barrel (Biased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]


    self.Histograms += [ defineHistogram('ResidualSCTBiasedEndCapA',
                                         type='TH1F',
                                         title="Residual calculated for the SCT EndCapA (Biased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]


    self.Histograms += [ defineHistogram('ResidualSCTBiasedEndCapC',
                                         type='TH1F',
                                         title="Residual calculated for the SCT EndCapC (Biased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]


    self.Histograms += [ defineHistogram('PullSCTBiasedBarrel',
                                         type='TH1F',
                                         title="Pull calculated for the SCT Barrel (Biased)",
                                         xbins = 100, xmin=-10, xmax=10)]


    self.Histograms += [ defineHistogram('PullSCTBiasedEndCapA',
                                         type='TH1F',
                                         title="Pull calculated for the SCT EndCapA (Biased)",
                                         xbins = 100, xmin=-10, xmax=10)]


    self.Histograms += [ defineHistogram('PullSCTBiasedEndCapC',
                                         type='TH1F',
                                         title="Pull calculated for the SCT EndCapC (Biased)",
                                         xbins = 100, xmin=-10, xmax=10)]

    

#Unbiased Residual and pull for SCT removing hit from track
        
    self.Histograms += [ defineHistogram('ResidualSCTUnbiasedBarrel',
                                         type='TH1F',
                                         title="Residual calculated for the SCT Barrel (Unbiased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]

    self.Histograms += [ defineHistogram('ResidualSCTUnbiasedEndCapA',
                                         type='TH1F',
                                         title="Residual calculated for the SCT EndCapA (Unbiased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]


    self.Histograms += [ defineHistogram('ResidualSCTUnbiasedEndCapC',
                                         type='TH1F',
                                         title="Residual calculated for the SCT EndCapC (Unbiased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]


    self.Histograms += [ defineHistogram('PullSCTUnbiasedBarrel',
                                         type='TH1F',
                                         title="Pull calculated for the SCT Barrel (Unbiased)",
                                         xbins = 100, xmin=-10, xmax=10)]


    self.Histograms += [ defineHistogram('PullSCTUnbiasedEndCapA',
                                         type='TH1F',
                                         title="Pull calculated for the SCT EndCapA (Unbiased)",
                                         xbins = 100, xmin=-10, xmax=10)]



    self.Histograms += [ defineHistogram('PullSCTUnbiasedEndCapC',
                                         type='TH1F',
                                         title="Pull calculated for the SCT EndCapC (Unbiased)",
                                         xbins = 100, xmin=-10, xmax=10)]


    self.Histograms += [ defineHistogram('TRIALResidualSCTUnbiased',
                                         type='TH1F',
                                         title="Residual calculated for the SCT (Unbiased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]



                                     
    self.Histograms += [ defineHistogram('TRIALPullSCTUnbiased',
                                         type='TH1F',
                                         title="Pull calculated for the SCT (Unbiased)",
                                         xbins = 100, xmin=-10, xmax=10)]




#Biased Residual and pull for SCT removing hit from track
   
    self.Histograms += [ defineHistogram('TRIALResidualSCTBiased',
                                         type='TH1F',
                                         title="Residual calculated for the SCT (Biased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]
    
    self.Histograms += [ defineHistogram('TRIALPullSCTBiased',
                                         type='TH1F',
                                         title="Pull calculated for the SCT (Biased)",
                                         xbins = 100, xmin=-10, xmax=10)]

#Unbiased Residual and pull for SCT using ResType = Trk::ResidualPull::Unbiased

 #   self.Histograms += [ defineHistogram('ResidualSCTResTypeUnbiased',
 #                                        type='TH1F',
  #                                       title="Residual calculated for the SCT (Trk::ResidualPull::Unbiased)",
   #                                      xbins = 100, xmin=-0.5, xmax=0.5)]

   # self.Histograms += [ defineHistogram('PullSCTResTypeUnbiased',
    #                                     type='TH1F',
     #                                    title="Pull calculated for the SCT (Trk::ResidualPull::Unbiased)",
      #                                   xbins = 100, xmin=-10, xmax=10)]




class PixelTrackResidualPullMonitorBase(TrigGenericMonitoringToolConfig):
  def __init__(self, name="PixelTrackResidualPullMonitorBase", type="electron"):
    super (PixelTrackResidualPullMonitorBase, self).__init__(name)

  #  maxclu = 200
  #  maxid = 400
  #  from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
  #  deta = InDetTrigSliceSettings[('etaHalfWidth',type)]
  #  if deta>3.:
  #    maxclu = 2000
  #    maxid = 3000
  #  elif deta>0.7:
  #    maxclu = 1000
  #    maxid = 1500
  #  elif deta>=0.2:
  #    maxclu = 400
 #     maxid = 600


# Biased Residual and Pull (locX and locY) for Pixel

    self.Histograms += [ defineHistogram('ResidualPixellocXBiasedBarrel',
                                         type='TH1F',
                                         title="Residual Calculated for the Pixel Detector Barrel (dist locX)(Biased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]


    self.Histograms += [ defineHistogram('ResidualPixellocXBiasedEndCapA',
                                         type='TH1F',
                                         title="Residual Calculated for the Pixel Detector EndCapA (dist locX)(Biased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]

    self.Histograms += [ defineHistogram('ResidualPixellocXBiasedBarrelEndCapC',
                                         type='TH1F',
                                         title="Residual Calculated for the Pixel Detector EndCapC  (dist locX)(Biased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]





    self.Histograms += [ defineHistogram('ResidualPixellocYBiasedBarrel',
                                         type='TH1F',
                                         title="Residual Calculated for the Pixel Detector Barrel (dist locY)(Biased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]

    self.Histograms += [ defineHistogram('ResidualPixellocYBiasedEndCapA',
                                         type='TH1F',
                                         title="Residual Calculated for the Pixel Detector EndCapA (dist locY)(Biased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]


    self.Histograms += [ defineHistogram('ResidualPixellocYBiasedEndCapC',
                                         type='TH1F',
                                         title="Residual Calculated for the Pixel Detector EndCapC (dist locY)(Biased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]   


  

    self.Histograms += [ defineHistogram('PullPixellocXBiasedBarrel',
                                         type='TH1F',
                                         title="Pull Calculated for the Pixel Detector Barrel (dist locX)(Biased)",
                                         xbins = 100, xmin=-10, xmax=10)]

    self.Histograms += [ defineHistogram('PullPixellocXBiasedEndCapA',
                                         type='TH1F',
                                         title="Pull Calculated for the Pixel Detector EndCapA (dist locX)(Biased)",
                                         xbins = 100, xmin=-10, xmax=10)]


    self.Histograms += [ defineHistogram('PullPixellocXBiasedEndCapC',
                                         type='TH1F',
                                         title="Pull Calculated for the Pixel Detector EndCapC  (dist locX)(Biased)",
                                         xbins = 100, xmin=-10, xmax=10)]
      



    self.Histograms += [ defineHistogram('PullPixellocYBiasedBarrel',
                                         type='TH1F',
                                         title="Pull Calculated for the Pixel Detector Barrel (dist locY)(Biased)",
                                         xbins = 100, xmin=-10, xmax=10)]


    self.Histograms += [ defineHistogram('PullPixellocYBiasedEndCapA',
                                         type='TH1F',
                                         title="Pull Calculated for the Pixel Detector EndCapA (dist locY)(Biased)",
                                         xbins = 100, xmin=-10, xmax=10)]
    
    self.Histograms += [ defineHistogram('PullPixellocYBiasedEndCapC',
                                         type='TH1F',
                                         title="Pull Calculated for the Pixel Detector EndCapC (dist locY)(Biased)",
                                         xbins = 100, xmin=-10, xmax=10)]

        

    # Unbiased Residual and Pull (locX and locY) for Pixel calculated by removing hit from track

    self.Histograms += [ defineHistogram('ResidualPixellocXUnbiasedBarrel',
                                         type='TH1F',
                                         title="Residual Calculated for the Pixel Detector Barrel (dist locX)(Unbiased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]

    self.Histograms += [ defineHistogram('ResidualPixellocXUnbiasedEndCapA',
                                         type='TH1F',
                                         title="Residual Calculated for the Pixel Detector EndCapA (dist locX)(Unbiased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]

    self.Histograms += [ defineHistogram('ResidualPixellocXUnbiasedEndCapC',
                                         type='TH1F',
                                         title="Residual Calculated for the Pixel Detector EndCapC (dist locX)(Unbiased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]

 

    self.Histograms += [ defineHistogram('ResidualPixellocYUnbiasedBarrel',
                                         type='TH1F',
                                         title="Residual Calculated for the Pixel Detector Barrel (dist locY)(Unbiased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]
        
    self.Histograms += [ defineHistogram('ResidualPixellocYUnbiasedEndCapA',
                                         type='TH1F',
                                         title="Residual Calculated for the Pixel Detector EndCapA (dist locY)(Unbiased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]


    self.Histograms += [ defineHistogram('ResidualPixellocYUnbiasedEndCapC',
                                         type='TH1F',
                                         title="Residual Calculated for the Pixel Detector EndCapC (dist locY)(Unbiased)",
                                         xbins = 100, xmin=-0.5, xmax=0.5)]


   

    self.Histograms += [ defineHistogram('PullPixellocXUnbiasedBarrel',
                                         type='TH1F',
                                         title="Pull Calculated for the Pixel Detector Barrel (dist locX)(Unbiased)",
                                         xbins = 100, xmin=-10, xmax=10)]

    self.Histograms += [ defineHistogram('PullPixellocXUnbiasedEndCapA',
                                         type='TH1F',
                                         title="Pull Calculated for the Pixel Detector EndCapA (dist locX)(Unbiased)",
                                         xbins = 100, xmin=-10, xmax=10)]

    self.Histograms += [ defineHistogram('PullPixellocXUnbiasedEndCapC',
                                         type='TH1F',
                                         title="Pull Calculated for the Pixel Detector EndCapC (dist locX)(Unbiased)",
                                         xbins = 100, xmin=-10, xmax=10)]


    self.Histograms += [ defineHistogram('PullPixellocYUnbiasedBarrel',
                                         type='TH1F',
                                         title="Pull Calculated for the Pixel Detector Barrel (dist locY)(Unbiased)",
                                         xbins = 100, xmin=-10, xmax=10)]
        

    self.Histograms += [ defineHistogram('PullPixellocYUnbiasedEndCapA',
                                         type='TH1F',
                                         title="Pull Calculated for the Pixel Detector EndCapA (dist locY)(Unbiased)",
                                         xbins = 100, xmin=-10, xmax=10)]

    self.Histograms += [ defineHistogram('PullPixellocYUnbiasedEndCapC',
                                         type='TH1F',
                                         title="Pull Calculated for the Pixel Detector EndCapC (dist locY)(Unbiased)",
                                         xbins = 100, xmin=-10, xmax=10)]


 
  # Unbiased Residual and Pull (locX and locY) for Pixel calculated using Trk::ResidualPull::Unbiased)


  #  self.Histograms += [ defineHistogram('ResidualPixellocXResTypeUnbiased',
   #                                      type='TH1F',
    #                                     title="Residual Calculated for the Pixel Detector (dist locX)(Trk::ResidualPull::Unbiased)",
     #                                    xbins = 100, xmin=-0.5, xmax=0.5)]

  #  self.Histograms += [ defineHistogram('ResidualPixellocYResTypeUnbiased',
   #                                      type='TH1F',
    #                                     title="Residual Calculated for the Pixel Detector (dist locY)(Trk::ResidualPull::Unbiased)",
     #                                    xbins = 100, xmin=-0.5, xmax=0.5)]
        
    

  #  self.Histograms += [ defineHistogram('PullPixellocXResTypeUnbiased',
   #                                      type='TH1F',
    #                                     title="Pull Calculated for the Pixel Detector (dist locX)(Trk::ResidualPull::Unbiased)",
     #                                    xbins = 100, xmin=-10, xmax=10)]
    
  #  self.Histograms += [ defineHistogram('PullPixellocYResTypeUnbiased',
   #                                      type='TH1F',
    #                                     title="Pull Calculated for the Pixel Detector (dist locY)(Trk::ResidualPull::Unbiased)",
     #                                    xbins = 100, xmin=-10, xmax=10)]
        


# Track parameters 


class TrackParamsResidualPullMonitorBase(TrigGenericMonitoringToolConfig):
  def __init__(self, name="TrackParamsResidualPullMonitorBase", type="electron"):
    super (TrackParamsResidualPullMonitorBase, self).__init__(name)



    self.Histograms += [ defineHistogram('TrackPt',
                                         type='TH1F',
                                         title="Track Pt",
                                         xbins = 100, xmin=0., xmax=25000.)]
    
    
    
    self.Histograms += [ defineHistogram('npix',
                                         type='TH1F',
                                         title="Number of Pixel Hits",
                                         xbins = 100, xmin=0., xmax=15.)]
    
    
    self.Histograms += [ defineHistogram('nsct',
                                         type='TH1F',
                                         title="Number of SCT Hits",
                                         xbins = 100, xmin=0., xmax=15.)]
    
    self.Histograms += [ defineHistogram('npixh',
                                         type='TH1F',
                                         title="Number of Pixel Holes",
                                         xbins = 100, xmin=0., xmax=10.)]
    
    
    self.Histograms += [ defineHistogram('nscth',
                                         type='TH1F',
                                         title="Number of Pixel Holes",
                                         xbins = 100, xmin=0., xmax=10.)]

    
#
#-- online
#

class SctTrackResidualPullOnlineMonitor(SctTrackResidualPullMonitorBase):
  def __init__(self, name="SctTrackResidualPullOnlineMonitor", type="electron"):
    super (SctTrackResidualPullOnlineMonitor, self).__init__(name,type)
    self.defineTarget("Online")


class PixelTrackResidualPullOnlineMonitor(PixelTrackResidualPullMonitorBase):
  def __init__(self, name="PixelTrackResidualPullOnlineMonitor", type="electron"):
    super (PixelTrackResidualPullOnlineMonitor, self).__init__(name,type)
    self.defineTarget("Online")

class TrackParamsResidualPullOnlineMonitor(TrackParamsResidualPullMonitorBase):
  def __init__(self, name="TrackParamsResidualPullOnlineMonitor", type="electron"):
    super (TrackParamsResidualPullOnlineMonitor, self).__init__(name,type)
    self.defineTarget("Online")



        
#        
#--- validation
#

class SctTrackResidualPullValidationMonitor(SctTrackResidualPullMonitorBase):
  def __init__(self, name="SctTrackResidualPullValidationMonitor", type="electron"):
    super (SctTrackResidualPullValidationMonitor, self).__init__(name,type)
    self.defineTarget("Validation")

class PixelTrackResidualPullValidationMonitor(PixelTrackResidualPullMonitorBase):
  def __init__(self, name="PixelTrackResidualPullValidationMonitor", type="electron"):
    super (PixelTrackResidualPullValidationMonitor, self).__init__(name,type)
    self.defineTarget("Validation")

class TrackParamsResidualPullValidationMonitor(TrackParamsResidualPullMonitorBase):
  def __init__(self, name="TrackParamsResidualPullValidationMonitor", type="electron"):
    super (TrackParamsResidualPullValidationMonitor, self).__init__(name,type)
    self.defineTarget("Validation")
