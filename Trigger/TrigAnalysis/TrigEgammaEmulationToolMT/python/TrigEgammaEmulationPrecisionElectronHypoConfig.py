#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
 
from AthenaCommon.SystemOfUnits import GeV

def same( val , tool):
  return [val]*( len( tool.EtaBins ) - 1 )



class TrigEgammaEmulationPrecisionElectronHypoToolConfig:


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

  # isolation cuts:w
  __isolationCut = {
        None: None,
        'ivarloose': 0.1,
        'ivarmedium': 0.065,
        'ivartight': 0.05
        }

  def __init__(self, name, chain, threshold, sel, iso):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaEmulationPrecisionElectronHypoTool')
    self.__chain = chain
    self.__threshold = float(threshold)
    self.__sel = sel
    self.__iso = iso
    
    from TrigEgammaEmulationToolMT.TrigEgammaEmulationToolMTConf import Trig__TrigEgammaEmulationPrecisionElectronHypoTool
    tool = Trig__TrigEgammaEmulationPrecisionElectronHypoTool( name )

    tool.EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    tool.ETthr          = same( self.__threshold*GeV, tool )
    tool.dETACLUSTERthr = 0.1
    tool.dPHICLUSTERthr = 0.1
    tool.RelPtConeCut   = -999
    tool.PidName        = ""
    self.__tool         = tool    
    self.__log.debug( 'Electron_Chain     :%s', chain )
    self.__log.debug( 'Electron_Threshold :%s', threshold )
    self.__log.debug( 'Electron_Pidname   :%s', sel )
    self.__log.debug( 'Electron_iso       :%s', iso )

  def chain(self):
    return self.__chain

  def pidname( self ):
    return self.__sel

  def etthr(self):
    return self.__threshold

  def isoInfo(self):
    return self.__iso

  def tool(self):
    return self.__tool
  
  def nocut(self):

    self.__log.debug( 'Configure nocut' )
    #self.tool().AcceptAll = True
    self.tool().ETthr          = same( self.etthr()*GeV, self.tool())
    self.tool().dETACLUSTERthr = 9999.
    self.tool().dPHICLUSTERthr = 9999.


  def etcut(self):

    self.__log.debug( 'Configure etcut' )
    self.tool().ETthr          = same( ( self.etthr() -  3 )*GeV, self.tool() )
    # No other cuts applied
    self.tool().dETACLUSTERthr = 9999.
    self.tool().dPHICLUSTERthr = 9999.


  #
  # Isolation and nominal cut
  #
  def isoCut(self):
    self.tool().RelPtConeCut = self.__isolationCut[self.isoInfo()]
    self.nominal()
 

  def nominal(self):
    if not self.pidname() in self.__operation_points:
      self.__log.fatal("Bad selection name: %s" % self.pidname())
    self.tool().PidName = self.pidname()


  #
  # Compile the chain
  #
  def compile(self):

    if 'etcut' == self.pidname():
      self.etcut()

    elif 'nocut' == self.pidname():
      self.nocut()

    elif self.isoInfo() and self.isoInfo() != '':

      if self.isoInfo() not in self.__isolationCut.keys():
        self.__log.error('Isolation cut %s not defined!', self.isoInfo())
      self.__log.debug('Configuring Isolation cut %s with value %d',self.isoInfo(),self.__isolationCut[self.isoInfo()])
      self.isoCut()

    else:
      self.nominal()




#
# create and configure the precision electron hypo tool
#
def createPrecisionElectron( name, info ):

    cpart = info['chainParts'][0]
    sel = cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']
    etthr = int(cpart['threshold'])
    isoinfo = cpart['isoInfo']
    chain = info['chainName']

    hypo = TrigEgammaEmulationPrecisionElectronHypoToolConfig( name, chain, etthr, sel, isoinfo )
    hypo.compile()
    return hypo.tool()