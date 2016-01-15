# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


class TrigL2CaloRingerCutDefs():

  _signatureDict = {
      #'benchmark' : ['pd','sp','pf'],
      'chainName' : '',
      #L2+EF (Calo) combined. ringers is set like T2Calo benchmark
      #EFCalo: using only ringer calo like EFCalo benchmark step
      'caloInfo'  : ['L2EFCalo','EFCalo'],
      'signature' : ['e','g'],
      'threshold' : 0,
  }

  def __init__(self, threshold, IDinfo, chainName):
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
    logger.info(('CutDefs for chain %s')%(chainName))
   
    #local variable
    discrs = None

    self._chainPart['chainName'] = chainName
    self._chainPart['IDinfo']    = IDinfo
    #self._chainPart['benchmark'] = 'pd'
    self._chainPart['caloInfo']  = 'EFCalo'

    from TrigMultiVarHypo.TrigL2CaloRingerConstants import SignaturesMap
    self._signatures = SignaturesMap()

    if chainName[0] in self._signatureDict['signature']:
      self._chainPart['signature']=chainName[0]
    else:
      raise RuntimeError('Bad signature')

    for partName in chainName.split('_'):

      #default is Probabillity detection (PD) reference
      #if partName in self._signatureDict['benchmark']:
      #  self._chainPart['benchmark']=partName
      
      #default is EFCalo benchmark level    
      if partName in self._signatureDict['caloInfo']:
        self._chainPart['caloInfo']=partName

    #Select the correct threshold.
    if (float(threshold) < 10): 
      self._chainPart['threshold'] = 0
    elif(float(threshold) >= 10. and float(threshold) < 17):
      self._chainPart['threshold'] = 10
    elif(float(threshold) >= 17. and float(threshold) < 24):
      self._chainPart['threshold'] = 17
    elif(float(threshold) >= 24. and float(threshold) < 30):
      self._chainPart['threshold'] = 24
    elif(float(threshold) >= 30 and float(threshold) < 60):
      self._chainPart['threshold'] = 30
    elif(float(threshold) >= 60):
      self._chainPart['threshold'] = 60
    
    logger.info(('set discriminator for  %dGeV because the threshold is %d.')%(self._chainPart['threshold'],
                                                                               float(threshold)))
 
    key = ('%s%d_%s_%s')%(  self._chainPart['signature'],
                               self._chainPart['threshold'],
                               self._chainPart['IDinfo'],
                               self._chainPart['caloInfo'],
                               #self._chainPart['benchmark'],
                               )

    try:  
      #inport all the discriminators
      logger.info('discriminator: %s -> key: %s',self._chainPart['chainName'],key)
      discrs =  self._signatures[key] 
    except:
      logger.info(('INCORRECT key info: [%s] not configured.')%(key) )
    

    if discrs:
      #This is a dep region hypothesis: row is Et bin and col is eta bin
      from TrigMultiVarHypo.TrigRingerPreprocessorDefs import Norm1

      ppList =     [  Norm1(),Norm1(), Norm1(), Norm1(),
                      Norm1(),Norm1(), Norm1(), Norm1(),
                      Norm1(),Norm1(), Norm1(), Norm1() ]
      
      discrList  = [ discrs['eta0_et0'], discrs['eta1_et0'], discrs['eta2_et0'], discrs['eta3_et0'],
                     discrs['eta0_et1'], discrs['eta1_et1'], discrs['eta2_et1'], discrs['eta3_et1'],
                     discrs['eta0_et2'], discrs['eta1_et2'], discrs['eta2_et2'], discrs['eta3_et2'],]
      
      logger.info('parser discrimiator with key: [%s]',key)
      #prepare the parser to athena phase
      self.__parser(discrList, ppList)


  def __parser(self, discrList, ppList):

    for discr in discrList:
      self.Nodes += discr['nodes'] 
      self.Thresholds.append(   discr['threshold']   )
      #list of lists to std::vector<std::vector<double>>
      self.Weights.append(     discr['weights']      )
      self.Bias.append(        discr['bias']         )
      self.EtBins.append(      discr['etBin']        )
      self.EtaBins.append(     discr['etaBin']       )

    for pp in ppList:
      #Prepoc configuration
      self.NormalisationRings += pp.NormalisationRings
      self.SectionRings       += pp.SectionRings      
      self.NRings             += pp.NRings           






