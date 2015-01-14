# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig


class TrigSteeringTestValMonitoring(TrigGenericMonitoringToolConfig) :
   def __init__ (self, name="TrigSteeringTestValMonitoring"):
      super(TrigSteeringTestValMonitoring,self).__init__(name)
      self.defineTarget("Validation")
      
      self.Histograms = [defineHistogram('PT', type='TH1F', title="p_T of jet ROI; p_T; number of RoIs", xbins=50,xmin=0,xmax=100) ]
      self.Histograms += [defineHistogram('DEta',type='TH1F', title="#Delta #eta of jet ROI; #delta #eta; number of RoIs", xbins=64,xmin=-3.2,xmax=3.2) ]
