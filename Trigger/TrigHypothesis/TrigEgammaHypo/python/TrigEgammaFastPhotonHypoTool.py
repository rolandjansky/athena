# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.SystemOfUnits import GeV


#
# For photons
#
class TrigEgammaFastPhotonHypoToolConfig:

  __operation_points  = [  'tight'    , 
                           'medium'   , 
                           'loose'    , 
                           ]

  def __init__(self, name, threshold, sel):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaFastPhotonHypoTool')
    self.__name       = name
    self.__threshold  = float(threshold) 
    self.__sel        = sel

    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.TrigEgammaFastPhotonHypoToolInc(name)
    tool.EtaBins = [0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47] 

    self.__tool = tool

    def same( val ):
      return [val]*( len( self.tool().EtaBins ) - 1 )
  
    tool.ETthr = same( 0.*GeV )
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

    # add mon tool
    self.addMonitoring()


  #
  # Monitoring code
  #
  def addMonitoring(self):
    
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool,defineHistogram

    monTool = GenericMonitoringTool("MonTool"+self.__name)
    monTool.Histograms = [
      defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="FastPhoton Hypo Cut Counter;Cut Counter", xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative"),
      defineHistogram('PtCalo', type='TH1F', path='EXPERT', title="FastPhoton Hypo p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000),
      defineHistogram('CaloEta', type='TH1F', path='EXPERT', title="FastPhoton Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5),
      defineHistogram('CaloPhi', type='TH1F', path='EXPERT', title="FastPhoton Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2),
    ]
    monTool.HistPath = 'FastPhotonHypo/'+self.__name
    self.__tool.MonTool = monTool




def _IncTool(name, threshold, sel):
  config = TrigEgammaFastPhotonHypoToolConfig(name, threshold, sel)
  config.compile()
  return config.tool()



def TrigEgammaFastPhotonHypoToolFromDict( d ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if i['signature']=='Photon' ]

    def __th(cpart):
        return cpart['threshold']

    def __sel(cpart):
        return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']

    name = d['chainName']

    return _IncTool( name,  __th( cparts[0]),  __sel( cparts[0]) )



def TrigEgammaFastPhotonHypoToolFromName( name, conf ):
    """ provides configuration of the hypo tool giben the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """

    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    decodedDict = dictFromChainName(conf)
    return TrigEgammaFastPhotonHypoToolFromDict( decodedDict )


if __name__ == "__main__":
    
    tool = TrigEgammaFastPhotonHypoToolFromName("HLT_g5_etcut_L1EM3", "HLT_g5_etcut_L1EM3")   
    assert tool, "Not configured simple tool"
