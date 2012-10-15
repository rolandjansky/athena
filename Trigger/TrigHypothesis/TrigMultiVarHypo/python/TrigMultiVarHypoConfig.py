# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
from TrigMultiVarHypo.TrigMultiVarHypoConf import TrigRingerNeuralHypo, TrigRingerNeuralFex

# Networks generated at Matlab
from TrigMultiVarHypo.Networks import Net20090728

class TrigRingerNeuralFexConfig( TrigRingerNeuralFex ):
  __slots__ = []
	
  def __init__(self, name = "TrigRingerNeuralFexConfig"):
    super(TrigRingerNeuralFexConfig, self).__init__(name)
		
    self.HltFeature = "TrigRingerNeuralFex"
    self.Feature = "TrigT2CaloEgamma"

    from TrigMultiVarHypo.TrigMultiVarHypoMonitoring import TrigRingerNeuralFexTimeMonitoring
    time = TrigRingerNeuralFexTimeMonitoring()

    self.AthenaMonTools = [ time ]
    self.doTiming = True

    self.NodesVector = Net20090728.NodesVector
    self.WeightVector = Net20090728.WeightVector
    self.BiasVector = Net20090728.BiasVector

    from TrigT2CaloEgamma.RingerConstants import Section, Normalisation
    from TrigT2CaloEgamma import TrigT2CaloEgammaConfig

    self.NormalisationRings = [Normalisation.SEQUENTIAL,
                               Normalisation.SEQUENTIAL,
                               Normalisation.SEQUENTIAL,
                               Normalisation.SEQUENTIAL,
                               Normalisation.SEQUENTIAL,
                               Normalisation.SEQUENTIAL,
                               Normalisation.SEQUENTIAL];
    self.SectionRings = [Section.EM,
                         Section.EM,
                         Section.EM,
                         Section.EM,
                         Section.HAD,
                         Section.HAD,
                         Section.HAD];
    # Only change this if you know what you are doing
    self.NRings = [8, 64, 8, 8, 4, 4, 4]


class TrigRingerNeuralHypoConfig( TrigRingerNeuralHypo ):
  __slots__ = []
	
  def __init__(self, name = "TrigRingerNeuralHypoConfig"):
    super(TrigRingerNeuralHypoConfig, self).__init__(name)
    
    self.HltFeature = "TrigRingerNeuralFex"
    
    self.Threshold = 0.0
    self.AcceptAll = False
    self.EmEtCut = 0
    
    from TrigMultiVarHypo.TrigMultiVarHypoMonitoring import TrigRingerNeuralHypoValidationMonitoring, TrigRingerNeuralHypoOnlineMonitoring, TrigRingerNeuralHypoTimeMonitoring
    validation = TrigRingerNeuralHypoValidationMonitoring()        
    online     = TrigRingerNeuralHypoOnlineMonitoring()
    time = TrigRingerNeuralHypoTimeMonitoring()

    self.AthenaMonTools = [ time, validation, online ]
    self.doTiming = True


class TrigRingerNeuralFexConfig_e5( TrigRingerNeuralFexConfig ):
  __slots__ = []
	
  def __init__(self, name = "TrigRingerNeuralFexConfig_e5"):
    super(TrigRingerNeuralFexConfig_e5, self).__init__(name)

class TrigRingerNeuralFexConfig_e5_NoCut( TrigRingerNeuralFexConfig ):
  __slots__ = []
	
  def __init__(self, name = "TrigRingerNeuralFexConfig_e5_NoCut"):
    super(TrigRingerNeuralFexConfig_e5_NoCut, self).__init__(name)

class TrigRingerNeuralFexConfig_e10( TrigRingerNeuralFexConfig ):
  __slots__ = []
	
  def __init__(self, name = "TrigRingerNeuralFexConfig_e10"):
    super(TrigRingerNeuralFexConfig_e10, self).__init__(name)

class TrigRingerNeuralFexConfig_e15i( TrigRingerNeuralFexConfig ):
  __slots__ = []
	
  def __init__(self, name = "TrigRingerNeuralFexConfig_e15i"):
    super(TrigRingerNeuralFexConfig_e15i, self).__init__(name)

class TrigRingerNeuralHypoConfig_e5( TrigRingerNeuralHypoConfig ):
  __slots__ = []
	
  def __init__(self, name = "TrigRingerNeuralHypoConfig_e5"):
    super(TrigRingerNeuralHypoConfig_e5, self).__init__(name)

class TrigRingerNeuralHypoConfig_e5_NoCut( TrigRingerNeuralHypoConfig ):
  __slots__ = []
	
  def __init__(self, name = "TrigRingerNeuralHypoConfig_e5_NoCut"):
    super(TrigRingerNeuralHypoConfig_e5_NoCut, self).__init__(name)
    self.AcceptAll = True

class TrigRingerNeuralHypoConfig_e10( TrigRingerNeuralHypoConfig ):
  __slots__ = []
	
  def __init__(self, name = "TrigRingerNeuralHypoConfig_e10"):
    super(TrigRingerNeuralHypoConfig_e10, self).__init__(name)

class TrigRingerNeuralHypoConfig_e15i( TrigRingerNeuralHypoConfig ):
  __slots__ = []
	
  def __init__(self, name = "TrigRingerNeuralHypoConfig_e15i"):
    super(TrigRingerNeuralHypoConfig_e15i, self).__init__(name)

