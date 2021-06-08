#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.SystemOfUnits import GeV
from AthenaConfiguration.ComponentFactory import CompFactory

#
# For photons
#
class TrigEgammaFastElectronHypoToolConfig:

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

  __trigElectronLrtd0Cut = { 'lrtloose' :2.0,
                             'lrtmedium':3.0,
                             'lrttight' :5.0
                           }

  def __init__(self, name, chain, threshold, sel, trkinfo, lrtinfo):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaFastElectronHypoTool')
    self.__chain      = chain
    self.__threshold  = float(threshold) 
    self.__sel        = sel
    self.__trkInfo    = trkinfo
    self.__lrtInfo    = lrtinfo

    tool = CompFactory.Trig.TrigEgammaEmulationFastElectronHypoTool(name)
    self.__tool = tool

    tool.AcceptAll      = False
    tool.TrackPt        = 0.0
    tool.CaloTrackdETA  = 0.2
    tool.CaloTrackdPHI  = 990.
    tool.CaloTrackdEoverPLow  = 0.0
    tool.CaloTrackdEoverPHigh = 999.0
    tool.TRTRatio = -999.
    
    self.__log.debug( 'Chain     :%s', name )
    self.__log.debug( 'Threshold :%s', threshold )
    self.__log.debug( 'Pidname   :%s', sel )


  def chain(self):
    return self.__chain
  
  def pidname( self ):
    return self.__sel

  def etthr(self):
    return self.__threshold

  def lrtInfo(self):
    return self.__lrtInfo
  
  def trkInfo(self):
    return self.__trkInfo

  def tool(self):
    return self.__tool
  

  def nocut(self):
    self.tool().AcceptAll = True


  def nominal(self):
    if self.etthr() < 15:
      self.tool().TrackPt = 1.0 * GeV 
    elif self.etthr() >= 15 and self.etthr() < 20:
      self.tool().TrackPt = 2.0 * GeV 
    elif self.etthr() >= 20 and self.etthr() < 50:
      self.tool().TrackPt =  3.0 * GeV 
    elif self.etthr() >= 50:
      self.tool().TrackPt =  5.0 * GeV 
      self.tool().CaloTrackdETA =  999. 
      self.tool().CaloTrackdPHI =  999.

  def lrt(self):
    self.tool().DoLRT = True
    self.tool().d0Cut=self.__trigElectronLrtd0Cut[self.lrtInfo()]


  #
  # Compile the chain
  #
  def compile(self):
    
    if 'idperf' in self.trkInfo():
      self.nocut()
    elif self.lrtInfo() in self.__trigElectronLrtd0Cut.keys():
      self.nominal()
      self.lrt()
    else:
      self.nominal()



def _IncTool(name, chain, threshold, sel, trk, lrt):
  config = TrigEgammaFastElectronHypoToolConfig(name, chain, threshold, sel, trk, lrt)
  config.compile()
  return config.tool()



def TrigEgammaFastElectronHypoToolFromDict( name, d ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if (i['signature']=='Electron')]

    def __th(cpart):
        return cpart['threshold']

    def __sel(cpart):
        return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']

    def __lrt(cpart):
        return cpart['lrtInfo']

    def __trk(cpart):
        return cpart['trkInfo']

    chain = d['chainName']

    return _IncTool( name, chain, __th(cparts[0]), __sel(cparts[0]), __trk(cparts[0]) , __lrt(cparts[0]) )

