#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.SystemOfUnits import GeV
from AthenaConfiguration.ComponentFactory import CompFactory

#
# For photons
#
class TrigEgammaFastPhotonHypoToolConfig:

  __operation_points  = [  'tight'    , 
                           'medium'   , 
                           'loose'    , 
                           ]

  def __init__(self, name, chain, threshold, sel):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('Trig__TrigEgammaEmulationFastPhotonHypoTool')
    self.__chain      = chain
    self.__threshold  = float(threshold) 
    self.__sel        = sel

    tool = CompFactory.Trig.TrigEgammaEmulationFastPhotonHypoTool(name)
    tool.EtaBins = [0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47] 

    self.__tool = tool

    def same( val ):
      return [val]*( len( self.tool().EtaBins ) - 1 )
  
    tool.ETThr = same( 0.*GeV )
    tool.CARCOREthr = same( 0.0 )
    tool.CAERATIOthr = same( 0.0)
    tool.dETACLUSTERthr = 0.1
    tool.dPHICLUSTERthr = 0.1
    tool.F1thr = same(0.005)
    tool.ET2thr = same( 90.0*GeV )
    tool.HADET2thr = same(999.0)
    tool.HADETthr =  same(0.035) 
    
    self.__log.debug( 'Chain     :%s', name )
    self.__log.debug( 'Threshold :%s', threshold )
    self.__log.debug( 'Pidname   :%s', sel )


  def chain(self):
    return self.__name
  
  def pidname( self ):
    return self.__sel

  def etthr(self):
    return self.__threshold

  def tool(self):
    return self.__tool
  

  def nocut(self):
    self.tool().AcceptAll = True


  def nominal(self):
    # not implemented yet
    pass


  #
  # Compile the chain
  #
  def compile(self):
    self.nocut()


  

def _IncTool(name, chain, threshold, sel):
  config = TrigEgammaFastPhotonHypoToolConfig(name, chain, threshold, sel)
  config.compile()
  return config.tool()



def TrigEgammaFastPhotonHypoToolFromDict( name, d ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if i['signature']=='Photon' ]

    def __th(cpart):
        return cpart['threshold']

    def __sel(cpart):
        return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']

    chain = d['chainName']
    return _IncTool( name, chain, __th( cparts[0]),  __sel( cparts[0]) )

