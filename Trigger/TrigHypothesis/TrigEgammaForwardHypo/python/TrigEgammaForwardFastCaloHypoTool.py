# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV

#
# For electrons only
#
def createTrigEgammaForwardFastCaloHypoAlg(name, sequenceOut):
  
  # make the Hypo alg
  from TrigEgammaForwardHypo.TrigEgammaForwardHypoConf import TrigEgammaForwardFastCaloHypoAlg
  theFastCaloHypo = TrigEgammaForwardFastCaloHypoAlg(name)
  theFastCaloHypo.CaloClusters = sequenceOut
  return theFastCaloHypo



class TrigEgammaForwardFastCaloHypoToolConfig:

 
  __operation_points  = [  'tight'    , 
                           'medium'   , 
                           'loose'    , 
                           'vloose'   , 
                           'lhtight'  , 
                           'lhmedium' , 
                           'lhloose'  , 
                           'lhvloose' ]



  def __init__(self, name, cand, threshold, sel, trackinfo, noringerinfo):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaForwardFastCaloHypoTool')
     
    self.__useRun3 = False
    self.__name = name
    self.__cand = cand
    self.__threshold = float(threshold)
    self.__sel = sel
    self.__trackinfo = trackinfo
    self.__noringerinfo = noringerinfo

    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.TrigEgammaForwardFastCaloHypoTool( name )
    tool.AcceptAll      = False
    tool.EtCut          = self.__threshold*GeV
  
    self.__tool = tool

    self.__log.debug( 'Chain     :%s', name )
    self.__log.debug( 'Signature :%s', cand )
    self.__log.debug( 'Threshold :%s', threshold )
    self.__log.debug( 'Pidname   :%s', sel )
    self.__log.debug( 'trackinfo  :%s', trackinfo )
    self.__log.debug( 'noringerinfo :%s', noringerinfo )

  def chain(self):
    return self.__name
  
  def pidname( self ):
    return self.__sel

  def etthr(self):
    return self.__threshold

  def isElectron(self):
    return 'e' in self.__cand

  def isPhoton(self):
    return 'g' in self.__cand

  def tool(self):
    return self.__tool
  

  def nocut(self):
    
    self.__log.debug( 'Configure nocut' )
    self.tool().AcceptAll      = True


  def compile(self):
    
    self.nocut()



def _IncTool(name, cand, threshold, sel, trackinfo, noringerinfo):
  config = TrigEgammaForwardFastCaloHypoToolConfig(name, cand, threshold, sel, trackinfo, noringerinfo )
  config.compile()
  return config.tool()


def TrigEgammaForwardFastCaloHypoToolFromDict( d ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if ((i['signature']=='Electron') or (i['signature']=='Photon'))]

    def __th(cpart):
        return cpart['threshold']

    def __sel(cpart):
        return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']

    def __cand(cpart):
        return cpart['trigType']

    def __trackinfo(cpart):
        return cpart['trkInfo'] if cpart['trkInfo'] else ''

    def __noringer(cpart):    
        return cpart['L2IDAlg'] if cpart['trigType']=='e' else ''

    name = d['chainName']

    return _IncTool( name, __cand( cparts[0]), __th( cparts[0]),  __sel( cparts[0]), __trackinfo(cparts[0]), __noringer(cparts[0]))


def TrigEgammaForwardFastCaloHypoToolFromName( name, conf ):
    """ To be phased out """
    """ set the name of the HypoTool (name=chain) and figure out the threshold and selection from conf """

    from TriggerMenuMT.HLT.Menu.DictFromChainName import dictFromChainName
    decodedDict = dictFromChainName(conf)
    return TrigEgammaForwardFastCaloHypoToolFromDict( decodedDict )

