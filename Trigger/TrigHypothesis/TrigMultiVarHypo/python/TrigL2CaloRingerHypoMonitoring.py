
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigL2CaloRingerHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
  __slots__ = []   
  def __init__(self, name = "TrigL2CaloRingerHypoValidationMonitoring"):
    super(TrigL2CaloRingerHypoValidationMonitoring, self).__init__(name)
    self.defineTarget( "Validation" )
    self.Histograms = [ defineHistogram('NeuralNetworkOutput', type='TH1F', 
                                        title="L2 Calo Neural Network Output; Output Neuron Value",
                                        xbins=220, xmin=-1.1, xmax=1.1) ]

class TrigL2CaloRingerHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
  __slots__ = []
  def __init__(self, name = "TrigL2CaloRingerHypoOnlineMonitoring"):
    super(TrigL2CaloRingerHypoOnlineMonitoring, self).__init__(name)

    self.defineTarget( "Online" )
    self.Histograms = [ defineHistogram('NeuralNetworkOutput', type='TH1F', 
                                  title="L2 Calo Neural Network Output; Output Neuron Value",
                                  xbins=220, xmin=-1.1, xmax=1.1) ]

class TrigL2CaloRingerHypoTimeMonitoring(TrigGenericMonitoringToolConfig):
  __slots__ = [] 
  def __init__(self, name = "TrigL2CaloRingerHypoTimeMonitoring"):
    super(TrigL2CaloRingerHypoTimeMonitoring,self).__init__(name)
    self.defineTarget("Time")
    self.Histograms = [defineHistogram('TotalTime',
                                        type='TH1F',title='TotalTime'
                                        ,xbins=220,xmin=0.0,xmax=0.2)]

class TrigL2CaloRingerFexTimeMonitoring(TrigGenericMonitoringToolConfig):
  __slots__ = []
 
  def __init__(self, name = "TrigL2CaloRingerFexTimeMonitoring"):
    super(TrigL2CaloRingerFexTimeMonitoring,self).__init__(name)
    self.defineTarget("Time")
    self.Histograms = [defineHistogram('TotalTime',
                                        type='TH1F',title='TotalTime'
                                        ,xbins=220,xmin=0.0,xmax=0.2)]
    self.Histograms += [defineHistogram('Decision',
                                        type='TH1F',title='Decision'
                                        ,xbins=220,xmin=0.0,xmax=0.2)]
    self.Histograms += [defineHistogram('Normalization',
                                        type='TH1F',title='Normalization'
                                        ,xbins=220,xmin=0.0,xmax=0.2)]
    self.Histograms += [defineHistogram('StoreOutput',
                                        type='TH1F',title='StoreOutput'
                                        ,xbins=220,xmin=0.0,xmax=0.2)]
