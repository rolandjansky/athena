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
  _pidMap    = dict()
  _factory   = dict()

  def __init__( self, trigList, **kw):

    self._level          = kw.pop('OutputLevel'     , 0   )
    self._ringerLocation = kw.pop('RingerConfigFile', None)

    #Initialize pid map
    self._pidMap["vloose"]  = "Loose";
    self._pidMap["loose"]   = "Loose";
    self._pidMap["medium"]  = "Medium";
    self._pidMap["tight"]   = "Tight";
    self._pidMap["loose1"]  = "Loose";
    self._pidMap["medium1"] = "Medium";
    self._pidMap["tight1"]  = "Tight";
    self._pidMap["lhvloose"]= "LHLoose";
    self._pidMap["lhloose"] = "LHLoose";
    self._pidMap["lhmedium"]= "LHMedium";
    self._pidMap["lhtight"] = "LHTight";

    from AthenaCommon.Logging import logging
    self._logger = logging.getLogger("TrigEgammaL2EmulationFactory")

    for chainName in trigList:
      self._chainDict.append( self.__genChainDict( chainName ) )

    #Create list tools
    self._factory['Trig__TrigEgammaL2CaloSelectorTool']       = self.__t2calo_factory("TrigEgammaL2CaloEmulationTool")
    self._factory['Trig__TrigEgammaL2CaloRingerSelectorTool'] = self.__ringer_factory("TrigEgammaL2CaloRingerEmulationTool")
    self._factory['Trig__TrigEgammaL2ElectronSelectorTool']   = self.__electron_factory("TrigEgammaL2ElectronEmulationTool")


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
                           Pidname        = self.__getPid(IDinfo),
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

  def __electron_factory(self, name):

    from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaL2ElectronSelectorTool
    from AthenaCommon.SystemOfUnits                          import GeV

    theTools = list()
    #protection because duplicate tools
    theTools_shortName = list()

    L2IDAlgMap = {'TRT':3,'L2StarA':5,'L2StarB':6,'L2StarC':7,'FTK':8}
    for chainDict in self._chainDict:
        
      print chainDict
      if not (chainDict['signature'] and chainDict['IDinfo']):  
        self._logger.warning(('skipping %s because this is not a good egamma trigger')%(chainDict['chainName']))
        continue

      trigType  = chainDict['trigType']
      IDinfo    = chainDict['IDinfo']
      threshold = chainDict['threshold']

      if not chainDict['shortName'] in theTools_shortName:
        print 'Adding :' , chainDict['shortName']
        #Set default values
        TrackPt       = 1.0 * GeV
        CaloTrackdETA = 9*[ 0.2 ]
        CaloTrackdPHI = 9*[ 999.]

        if (threshold > 15):
            TrackPt = 2.0 * GeV
        elif (threshold >= 20):
            TrackPt = 3.0 * GeV
        elif (threshold >= 50):
            TrackPt = 5.0 * GeV
            CaloTrackdETA = 9*[ 999.]
            CaloTrackdPHI = 9*[ 999.]


        if 'L2IDAlgo' in chainDict.keys():
          # [0=All, 3=TRT, 5=SiTrack or IdScan, 6=L2StarB, 7=L2StarC, 8=FTK, 9=Fast]
          TrackAlgoId = L2IDAlgoMap[chainDict['L2IDAlgo']] 
        else:
          TrackAlgoId   = 9
    

        tool = ToolFactory(Trig__TrigEgammaL2ElectronSelectorTool,
                           name                     = name+'_'+chainDict['shortName'],
                           EtCut                    = float(threshold)*GeV,
                           Pidname                  = self.__getPid(IDinfo),
                           Signature                = chainDict['signature'],
                           OutputLevel              = self._level,
                           EtaBins                  = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47] ,
                           CaloTrackdEoverPLow      = 9*[0.0],
                           CaloTrackdEoverPHigh     = 9*[999.0],
                           TRTRatio                 = 9*[-999.],
                           EtaBinsTRT               = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47],
                           CaloTrackdEoverPLowTRT   = 9*[0.0],
                           CaloTrackdEoverPHighTRT  = 9*[999.0],
                           TRTRatioTRT              = 9*[-999.],
                           CaloTrackdETATRT         = 9*[999.],
                           TrackAlgoId              = TrackAlgoId,
                           TrackPt                  = TrackPt,
                           CaloTrackdETA             = CaloTrackdETA,
                           CaloTrackdPHI             = CaloTrackdPHI)
        theTools_shortName.append(chainDict['shortName'])
        theTools.append(tool)
    #Loop over chainDict
    return theTools

  def __ringer_factory(self, name): 
    from TrigMultiVarHypo.TrigL2CaloRingerCutDefs            import TrigL2CaloRingerCutDefs
    from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaL2CaloRingerSelectorTool
    from AthenaCommon.SystemOfUnits                          import GeV

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
                           Pidname           = self.__getPid(IDinfo),
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

  def __getPid(self, pidtype):
    if pidtype in self._pidMap.keys():
      return self._pidMap[pidtype]
    else:#Some Protection
      return None


  #Helper function to extract the dictonary
  def __genChainDict(self, chainName):

    from TriggerMenu.menu.SignatureDicts import getSignatureInformation
    #Remove HLT_* from chainName. 
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

    chainDict['shortName'] = trigType+str(chainDict['threshold']) + \
             ( ('_'+chainDict['IDinfo']) if chainDict['IDinfo'] else chainDict['addInfo'])
    return chainDict
















