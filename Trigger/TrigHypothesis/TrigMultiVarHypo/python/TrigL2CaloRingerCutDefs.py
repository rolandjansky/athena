# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


class TrigL2CaloRingerCutDefs():

  _signatureDict = {
      'signature' : ['e','g'],
  }
  _pidMap = {
      'tight': 'Tight', 'medium':'Medium', 'loose':'Loose', 'vloose':'VeryLoose',
      'lhtight': 'Tight', 'lhmedium': 'Medium', 'lhloose':'Loose', 'lhvloose': 'VeryLoose'
      }

  def __init__(self, threshold, IDinfo, trigType):
    self._chainPart           = dict()
    self.Nodes                = list()
    self.Weights              = list()
    self.Bias                 = list()
    self.Thresholds           = list()
    self.EtBins               = list()
    self.EtaBins              = list()
    self.NormalisationRings   = list()
    self.SectionRings         = list()
    self.NRings               = list()

    from AthenaCommon.Logging import logging
    logging.getLogger().info("Importing %s",__name__)
    logger = logging.getLogger("TrigMultiVarHypo.TrigL2CaloRingerCutDefs")
    logger.info(('CutDefs for chain %s')%(trigType))
   
    #local variable
    discrs = None

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



    try:  
      #inport all the discriminators
      from TrigMultiVarHypo.TrigL2CaloRingerConstants import SignaturesMap
      logger.info('discriminator: %s -> key: %s', trigType, key)
      discrs =  SignaturesMap()[key]
    except:
      logger.info(('INCORRECT key info: [%s] not configured.')%(key) )
    

    if discrs:
      #This is a dep region hypothesis: row is Et bin and col is eta bin
      from TrigMultiVarHypo.TrigRingerPreprocessorDefs import Norm1

      ppList=list();  discrList=list()
      for key_, o in discrs.iteritems():
        ppList.append( Norm1() )
        discrList.append( o )
      
      logger.info('parser discrimiator with key: [%s]',key)
      #prepare the parser to athena phase
      self.__parser(discrList, ppList)


  def __parser(self, discrList, ppList):

    for discr in discrList:
      self.Nodes += discr['discriminator']['nodes'] 
      self.Thresholds.append(  discr['discriminator']['threshold']   )
      self.Weights.append(     discr['discriminator']['weights']      )
      self.Bias.append(        discr['discriminator']['bias']         )
      self.EtBins.append(      discr['configuration']['etBin']        )
      self.EtaBins.append(     discr['configuration']['etaBin']       )

    for pp in ppList:
      #Prepoc configuration
      self.NormalisationRings += pp.NormalisationRings
      self.SectionRings       += pp.SectionRings      
      self.NRings             += pp.NRings           






