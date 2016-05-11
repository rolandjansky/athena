# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#**********************************************************************
# python: L2 Factory
# Authors:
#      Ryan Mackenzie White <ryan.white@cern.ch>
#      Joao Victor Pinto    <jodafons@cern.ch>
# Date: Feb 2015
# Description:
#
#**********************************************************************/
from egammaRec.Factories import ToolFactory

class TrigEgammaL2EmulationFactory:

  _chainDict = list()
  _factory   = dict()

  def __init__( self, trigList, **kw):

    self._level          = kw.pop('OutputLevel'     , 0   )
    self._ringerLocation = kw.pop('RingerConfigFile', None)

    from AthenaCommon.Logging import logging
    self._logger = logging.getLogger("TrigEgammaL2EmulationFactory")

    for chainName in trigList:
      self._chainDict.append( self.__genChainDict( chainName ) )

    #Create list tools
    self._factory['Trig__TrigEgammaL2CaloSelectorTool']       = self.__t2calo_factory("TrigEgammaL2CaloEmulationTool")
    self._factory['Trig__TrigEgammaL2CaloRingerSelectorTool'] = self.__ringer_factory("TrigEgammaL2CaloRingerEmulationTool")

  def retrieve(self):
    return self._factory

  def __t2calo_factory(self, name):
    from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaL2CaloSelectorTool
    from TrigEgammaHypo.TrigL2CaloHypoCutDefs                import L2CaloCutMaps
    from AthenaCommon.SystemOfUnits                          import GeV

    theTools     = list()
    #protection for duplicate tools
    theTools_shortName = list()
  
    for chainDict in self._chainDict:
      if not (chainDict['signature'] and chainDict['IDinfo']):  
        self._logger.warning(('skipping %s because this is not a good egamma trigger')%(chainDict['chainName']))
        continue
 
      trigType     = chainDict['trigType']
      IDinfo       = chainDict['IDinfo']
      threshold    = chainDict['threshold']
      #Protection for duplicate tools
      if not chainDict['shortName'] in theTools_shortName:

        self._logger.info(('creating tool with name: %s...')%(chainDict['shortName']))
        theCutMaps = L2CaloCutMaps(threshold)
        # create L2Calo tool    
        tool = ToolFactory(Trig__TrigEgammaL2CaloSelectorTool, 
                           name           = name+'_'+chainDict['shortName'], 
                           Pidname        = IDinfo,
                           Signature      = chainDict['signature'],
                           OutputLevel    = self._level,
                           HADETthr       = theCutMaps.MapsHADETthr[IDinfo],
                           CARCOREthr     = theCutMaps.MapsCARCOREthr[IDinfo],
                           CAERATIOthr    = theCutMaps.MapsCAERATIOthr[IDinfo],
                           EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47],
                           dETACLUSTERthr = 0.1,
                           dPHICLUSTERthr = 0.1,
                           ETthr          = 9*[threshold*GeV],
                           ET2thr         = 9*[90.0*GeV],
                           HADET2thr      = 9*[999.0],
                           F1thr          = 9*[0.005],
                           WETA2thr       = 9*[99999.],
                           WSTOTthr       = 9*[99999.],
                           F3thr          = 9*[99999.])
  
        #protection because duplicate tools
        theTools_shortName.append(chainDict['shortName'])
        theTools.append( tool() )

    return theTools


  def __ringer_factory(self, name): 
    from TrigMultiVarHypo.TrigL2CaloRingerCutDefs import TrigL2CaloRingerCutDefs
    from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaL2CaloRingerSelectorTool
    from AthenaCommon.SystemOfUnits                 import GeV

    theTools     = list()
    #protection because duplicate tools
    theTools_shortName = list()

    for chainDict in self._chainDict:
      if not (chainDict['signature'] and chainDict['IDinfo']):  
        self._logger.warning(('skipping %s because this is not a good egamma trigger')%(chainDict['chainName']))
        continue

      trigType  = chainDict['trigType']
      IDinfo    = chainDict['IDinfo']
      threshold = chainDict['threshold']

      if not chainDict['shortName'] in theTools_shortName:

        cutDefs  = TrigL2CaloRingerCutDefs( threshold, IDinfo, chainDict['chainName'])
        #loop over neural networks
        tool = ToolFactory(Trig__TrigEgammaL2CaloRingerSelectorTool,
                           name              = name+'_'+chainDict['shortName'],
                           EtCut             = float(threshold)*GeV,
                           Pidname           = IDinfo,
                           Signature         = chainDict['signature'],
                           OutputLevel       = self._level,
                           Nodes             = cutDefs.Nodes,
                           Weights           = cutDefs.Weights,
                           Bias              = cutDefs.Bias,
                           Thresholds        = cutDefs.Thresholds,
                           EtBins            = cutDefs.EtBins,
                           EtaBins           = cutDefs.EtaBins,
                           NormalisationRings= cutDefs.NormalisationRings,
                           SectionRings      = cutDefs.SectionRings,
                           NRings            = cutDefs.NRings) 
        
        theTools_shortName.append(chainDict['shortName'])
        theTools.append(tool)

    return theTools

  #Helper function to extract the dictonary
  def __genChainDict(self, chainName):

    from TriggerMenu.menu.SignatureDicts import getSignatureInformation
    if 'HLT' in chainName: chainName = chainName[4::]
    chainDict = {'chainName':chainName,'signature':None,'IDinfo':None}
    trigType               = chainName[0]
    chainSplit             = chainName.split('_')
    chainDict['threshold'] = int(chainSplit[0][1:])
    chainDict['trigType']  = trigType

    if trigType is 'e':
      information = getSignatureInformation('Electron')[1]
      chainDict['signature']='Electron'
    elif trigType is 'g':
      information = getSignatureInformation('Photon')[1]
      chainDict['signature']='Photon'
    else:
      logger.warning('Signaure not accept for egamma emulation')
      return chainDict

    for partName in chainSplit[1:]:
      for key in information.keys():
        if partName in information[key]:
          chainDict[key]=partName

    print chainDict
    chainDict['shortName'] = trigType+str(chainDict['threshold']) + \
             ( ('_'+chainDict['IDinfo']) if chainDict['IDinfo'] else chainDict['addInfo'])
    return chainDict
















