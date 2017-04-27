# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Decrepted class. Remove this for future
class TrigL2CaloRingerCutDefs:
  _signatureDict = {
      'signature' : ['e','g'],
  }
  _pidMap = {
      'tight': 'Tight', 'medium':'Medium', 'loose':'Loose', 'vloose':'VeryLoose',
      'lhtight': 'Tight', 'lhmedium': 'Medium', 'lhloose':'Loose', 'lhvloose': 'VeryLoose'
      }
  def __init__(self, threshold, IDinfo, trigType):
    
    from AthenaCommon.Logging import logging
    logging.getLogger().info("Importing %s",__name__)
    self._logger = logging.getLogger("TrigMultiVarHypo.TrigL2CaloRingerCutDefs")
    self._logger.info(('CutDefs for chain %s')%(trigType))
    
    # Feature Extraction
    self.Nodes                = list()
    self.Weights              = list()
    self.Bias                 = list()
    self.EtBins               = list()
    self.EtaBins              = list()
    self.NormalisationRings   = list()
    self.SectionRings         = list()
    self.NRings               = list()
    # Hypothesis
    self.Thresholds           = list()
    self.EtBinsFromThreshold  = list()
    self.EtaBinsFromThreshold = list()
    # Metadata
    self.UseEtaVar            = False 
    self.UseLumiVar           = False 
    self.UseLumiTool          = False 
    self.DoPileupCorrection   = False 
    self.LumiCut              = 60
    self.UseNoActivationFunctionInTheLastLayer = False
  
    #local variable
    discrs               = None
    thresholds           = None
    metadataFromDiscr    = None
    metadataFromThr      = None
    key                  = self.__get_tuning_key( threshold, trigType, IDinfo )
    discrDict, thrDict   = self.__try_to_find_my_calib_file()
    if discrDict['version'] is 1:
      metadataFromDiscr       = discrDict['metadata']
      self.UseEtaVar          = metadataFromDiscr['UseEtaVar']
      self.UseLumiVar         = metadataFromDiscr['UseLumiVar']
      self.UseLumiTool        = metadataFromDiscr['UseLumiTool']
      self.LumiCut            = metadataFromDiscr['LumiCut']
      self._logger.info('Fast Ringer Tuning    name is: %s',discrDict['name'][0])
      self._logger.info('Fast Ringer Threshold name is: %s',thrDict['name'][0]  )
      try:
        discrs = discrDict['tuning'][key]
      except KeyError:
        self._logger.warning(('INCORRECT key info: [%s] not configured.')%(key) )
    else:
      self._logger.warning('Incorrect Discriminator file version...')
    if thrDict['version'] is 1:
      metadataFromThr = thrDict['metadata']
      self.DoPileupCorrection = metadataFromThr['DoPileupCorrection']
      self.UseNoActivationFunctionInTheLastLayer = metadataFromThr['UseNoActivationFunctionInTheLastLayer']
      try:
        thresholds = thrDict['tuning'][key]
      except KeyError:
        self._logger.warning(('INCORRECT key info: [%s] not configured.')%(key) )
    else:
      self._logger.warning('Incorrect Threshold file version...')
    if discrs:
      #This is a dep region hypothesis: row is Et bin and col is eta bin
      from TrigMultiVarHypo.TrigRingerPreprocessorDefs import Norm1
      ppList=list();  discrList=list()
     
      # Sorted keys
      for key_ in sorted(discrs.keys()):
        ppList.append( Norm1() )
        discrList.append( discrs[key_] )
      self._logger.debug('parser Discriminators with key: [%s]',key)
      #prepare the parser to athena c++ phase
      self.__parserDiscriminators(discrList, ppList)
    if thresholds:
      thrList=list()
      for key_ in sorted(thresholds):
        thrList.append( thresholds[key_] )
      self._logger.debug('parser Thresholdls with key: [%s]',key)
      self.__parserThresholds( thrList )
    # Some outputs to check!
    if self.UseLumiTool:
      self._logger.debug('Using the LumiBlockTool!')
    else:
      self._logger.warning('The LumiBlockTool was disable because this tuning does not support this configuration!')
    if self.UseNoActivationFunctionInTheLastLayer:
      self._logger.debug('Using no activation function in the last layer.')
    else:
      self._logger.debug('Using activation function in the last layer')
  

  def __try_to_find_my_calib_file(self):
    self._logger.debug('Try with the python import...')
    from TrigMultiVarHypo.TrigL2CaloRingerConstants import SignaturesMap
    from TrigMultiVarHypo.TrigL2CaloRingerThresholds import ThresholdsMap
    return SignaturesMap(), ThresholdsMap()
 
  def __get_tuning_key(self, threshold, trigType, IDinfo):
      if not (trigType[0] in self._signatureDict['signature']):
        raise RuntimeError('Bad signature')
      # Select the correct threshold.
      if (float(threshold) < 15): 
        if self._signatureDict['signature'][0] in trigType:
          # ElectronLowEnergyTightConf
          # ElectronLowEnergyMediumConf
          # ElectronLowEnergyLooseConf
          # ElectronLowEnergyVeryLooseConf
          key = ('ElectronLowEnergy%sConf')%(IDinfo)
        else:
          # PhotonLowEnergyTightConf
          # PhotonLowEnergyMediumConf
          # PhotonLowEnergyLooseConf
          # PhotonLowEnergyVeryLooseConf
          key = ('PhotonLowEnergy%sConf')%(IDinfo)
      else: #(float(threshold) >= 20):
        if self._signatureDict['signature'][0] in trigType:
          # ElectronHighEnergyTightConf
          # ElectronHighEnergyMediumConf
          # ElectronHighEnergyLooseConf
          # ElectronHighEnergyVeryLooseConf
          key = ('ElectronHighEnergy%sConf')%(self._pidMap[IDinfo])
        else:
          # PhotonHighEnergyTightConf
          # PhotonHighEnergyMediumConf
          # PhotonHighEnergyLooseConf
          # PhotonHighEnergyVeryLooseConf
          key = ('PhotonHighEnergy%sConf')%(self._pidMap[IDinfo])
      return key
  def __parserDiscriminators(self, discrList, ppList):
    for discr in discrList:
      self.Nodes += discr['discriminator']['nodes'] 
      self.Weights.append(     discr['discriminator']['weights']      )
      self.Bias.append(        discr['discriminator']['bias']         )
      self.EtBins.append(      discr['configuration']['etBin']        )
      self.EtaBins.append(     discr['configuration']['etaBin']       )
    for pp in ppList:
      #Prepoc configuration
      self.NormalisationRings += pp.NormalisationRings
      self.SectionRings       += pp.SectionRings      
      self.NRings             += pp.NRings           
  def __parserThresholds(self, thrList):
    for thr in thrList:
      self.Thresholds.append(thr['threshold'])
      self.EtBinsFromThreshold.append(thr['configuration']['etBin'])
      self.EtaBinsFromThreshold.append(thr['configuration']['etaBin'])



