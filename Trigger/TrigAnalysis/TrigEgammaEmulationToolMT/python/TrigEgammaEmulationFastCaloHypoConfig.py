#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.SystemOfUnits import GeV
from AthenaConfiguration.ComponentFactory import CompFactory

def same( val , tool):
  return [val]*( len( tool.EtaBins ) - 1 )
  
def treatPidName(pidname):
  if 'tight' in pidname:
    return 'tight'
  elif 'medium' in pidname:
    return 'medium'
  # this should be before loose to works
  elif 'vloose' in pidname:
    return 'vloose'
  else:
    return 'loose'

#
# For electron and photons
#
class TrigEgammaFastCaloHypoToolConfig:

  __operation_points  = [  'tight'    , 
                           'medium'   , 
                           'loose'    , 
                           'vloose'   , 
                           'lhtight'  , 
                           'lhmedium' , 
                           'lhloose'  , 
                           'lhvloose' ,
                           #'dnntight',
                           #'dnnmedium',
                           #'dnnloose',
                           #'dnnvloose',
                           ]


  def __init__(self, name, chain, cand, threshold, sel, trackinfo, noringerinfo):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('Trig__TrigEgammaEmulationFastCaloHypoTool')
     
    self.__useRun3 = False
    self.__chain = chain
    self.__cand = cand
    self.__threshold  = float(threshold)
    self.__sel        = sel
    self.__trackinfo  = trackinfo
    self.__noringerinfo = noringerinfo

    tool = CompFactory.Trig.TrigEgammaEmulationFastCaloHypoTool( name )
    tool.AcceptAll      = False
    tool.UseRinger      = False
    tool.EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    tool.ETthr          = same( self.__threshold*GeV, tool )
    tool.dETACLUSTERthr = 0.1
    tool.dPHICLUSTERthr = 0.1
    tool.F1thr          = same( 0.005 , tool)
    tool.ET2thr         = same( 90.0*GeV, tool )
    tool.HADET2thr      = same( 999.0  , tool)
    tool.HADETthr       = same( 0.058  , tool)
    tool.WETA2thr       = same( 99999. , tool)
    tool.WSTOTthr       = same( 99999. , tool)
    tool.F3thr          = same( 99999. , tool)
    tool.CARCOREthr     = same( -9999. , tool)
    tool.CAERATIOthr    = same( -9999. , tool)
    tool.PidName        = ""

    self.__tool = tool

    self.__log.debug( 'Chain     :%s', name )
    self.__log.debug( 'Signature :%s', cand )
    self.__log.debug( 'Threshold :%s', threshold )
    self.__log.debug( 'Pidname   :%s', sel )
    self.__log.debug( 'trackinfo  :%s', trackinfo )
    self.__log.debug( 'noringerinfo :%s', noringerinfo )

  def chain(self):
    return self.__chain
  
  def pidname( self ):
    return self.__sel

  def etthr(self):
    return self.__threshold

  def isElectron(self):
    return 'e' in self.__cand

  def isPhoton(self):
    return 'g' in self.__cand

  def noringerinfo(self):
    return self.__noringerinfo

  def trackinfo(self):
    return self.__trackinfo

  def tool(self):
    return self.__tool
  

  def nocut(self):
    
    self.__log.debug( 'Configure nocut' )
    self.tool().AcceptAll      = True
    self.tool().UseRinger      = False
    self.tool().ETthr          = same( self.etthr()*GeV , self.tool())
    self.tool().dETACLUSTERthr = 9999.
    self.tool().dPHICLUSTERthr = 9999.
    self.tool().F1thr          = same( 0.0    , self.tool())
    self.tool().HADETthr       = same( 9999.  , self.tool())
    self.tool().CARCOREthr     = same( -9999. , self.tool())
    self.tool().CAERATIOthr    = same( -9999. , self.tool())


  def etcut(self):

    self.__log.debug( 'Configure etcut' )
    self.tool().UseRinger      = False
    self.tool().ETthr          = same( ( self.etthr()  -  3 )*GeV, self.tool() )
    self.tool().dETACLUSTERthr = 9999.
    self.tool().dPHICLUSTERthr = 9999.
    self.tool().F1thr          = same( 0.0    ,self.tool())
    self.tool().HADETthr       = same( 9999.  ,self.tool())
    self.tool().CARCOREthr     = same( -9999. ,self.tool())
    self.tool().CAERATIOthr    = same( -9999. ,self.tool())


  def noringer(self):

    self.__log.debug( 'Configure noringer' )
    from TrigEgammaHypo.TrigEgammaFastCutDefs import TrigFastCaloElectronCutMaps
    self.tool().UseRinger   = False
    self.tool().ETthr       = same( ( self.etthr()  - 3 )*GeV , self.tool())
    self.tool().HADETthr    = TrigFastCaloElectronCutMaps( self.etthr() ).MapsHADETthr[self.pidname()]
    self.tool().CARCOREthr  = TrigFastCaloElectronCutMaps( self.etthr() ).MapsCARCOREthr[self.pidname()]
    self.tool().CAERATIOthr = TrigFastCaloElectronCutMaps( self.etthr() ).MapsCAERATIOthr[self.pidname()]


  def ringer(self):

    self.__log.debug( 'Configure ringer' )
    self.tool().UseRinger = True
    self.tool().EtCut     = (self.etthr()-3.)*GeV  
    if not self.pidname() in self.__operation_points:
      self.__log.fatal("Bad selection name: %s" % self.pidname())
    self.tool().PidName = treatPidName(self.pidname())


  #
  # compile the chain
  #
  def compile(self):

    if 'etcut' == self.pidname():
      self.etcut()

    elif self.pidname() in self.__operation_points and 'noringer' in self.noringerinfo() and self.isElectron():
      self.noringer()

    elif self.pidname() in self.__operation_points and 'noringer' not in self.noringerinfo() and self.isElectron():
      self.ringer()
  
    elif self.pidname() in self.__operation_points and self.isPhoton():
      self.noringer()

 


def _IncTool(name, chain, cand, threshold, sel, trackinfo, noringerinfo):
  config = TrigEgammaFastCaloHypoToolConfig(name, chain, cand, threshold, sel, trackinfo, noringerinfo )
  config.compile()
  return config.tool()


def TrigEgammaFastCaloHypoToolFromDict( name, d ):
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

    chain = d['chainName']

    return _IncTool( name, chain, __cand( cparts[0]), __th( cparts[0]),  __sel( cparts[0]), __trackinfo(cparts[0]), __noringer(cparts[0]))
