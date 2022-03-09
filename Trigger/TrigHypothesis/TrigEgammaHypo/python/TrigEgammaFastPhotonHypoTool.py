# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.SystemOfUnits import GeV
from AthenaConfiguration.AllConfigFlags import ConfigFlags

#
# For photons
#
class TrigEgammaFastPhotonHypoToolConfig:

  __operation_points  = [  'tight'    , 
                           'medium'   , 
                           'loose'    , 
                           'etcut'    , 
                           ]

  def same( self, val ):
    return [val]*( len( self.tool().EtaBins ) - 1 )

  def __init__(self, name,monGroups, cpart, tool=None):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaFastPhotonHypoTool')
    self.__name       = name
    self.__threshold  = float(cpart['threshold']) 
    self.__sel        = 'ion' if 'ion' in cpart['extra'] else (cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo'])
    self.__monGroups  = monGroups

    if not tool:
      from AthenaConfiguration.ComponentFactory import CompFactory
      tool = CompFactory.TrigEgammaFastPhotonHypoTool(name)

    self.__tool = tool

    tool.EtaBins = [0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]  
    tool.ETthr = self.same( 0. )
    tool.CARCOREthr = self.same( 0.0 )
    tool.CAERATIOthr = self.same( 0.0)
    tool.F1thr = self.same( 0.005 )
    tool.ET2thr = self.same( 90.0 * GeV )
    tool.HADET2thr = self.same( 999. * GeV )
    tool.HADETthr =  self.same( 999. * GeV ) 

 

    
    self.__log.debug( 'Chain     :%s', self.__name )
    self.__log.debug( 'Threshold :%s', self.__threshold )
    self.__log.debug( 'Pidname   :%s', self.__sel )



  def chain(self):
    return self.__name
  
  def pidname( self ):
    return self.__sel

  def etthr(self):
    return self.__threshold

  def tool(self):
    return self.__tool
  
  def etcut(self):
    self.__log.debug( 'Chain     :%s configured with etcut selection', self.chain() )
    self.tool().ETthr = self.same( self.etthr() *GeV - 3.* GeV) # Set the eT cut 3 GeV below HLT threshold

  def nocut(self):
    self.__log.debug( 'Chain     :%s configured with nocut selection', self.chain() )
    self.tool().AcceptAll = True

  def nominal(self):
    from TrigEgammaHypo.TrigEgammaFastCutDefs import TrigFastPhotonCutMaps
    self.__log.debug( 'Chain     :%s configured with nominal selection', self.chain() )
    self.tool().ETthr = self.same( self.etthr() *GeV - 3.* GeV) # Set the eT cut 3 GeV below HLT threshold
    self.tool().CARCOREthr     = TrigFastPhotonCutMaps( self.etthr() ).MapsCARCOREthr[ self.pidname() ]
    self.tool().CAERATIOthr    = TrigFastPhotonCutMaps( self.etthr() ).MapsCAERATIOthr [ self.pidname() ]
    self.tool().HADETthr       = TrigFastPhotonCutMaps( self.etthr() ).MapsHADETthr[ self.pidname() ]
    self.tool().HADET2thr = self.same(999.0 * GeV)
    self.tool().F1thr = self.same(0.005)
    self.tool().ET2thr = self.same( 90.0*GeV )


  #
  # Compile the chain
  #
  def compile(self):
    if self.pidname() in ('etcut', 'ion'):
        self.etcut()
    elif 'noalg' == self.pidname():
        self.nocut()
    elif 0 == self.etthr():
        self.nocut()
    else:
        self.nominal()


    # add mon tool
    if hasattr(self.tool(), "MonTool"):
      
      doValidationMonitoring = ConfigFlags.Trigger.doValidationMonitoring # True to monitor all chains for validation purposes
      monGroups = self.__monGroups

      if (any('egammaMon:online' in group for group in monGroups) or doValidationMonitoring):
        self.addMonitoring()


  #
  # Monitoring code
  #
  def addMonitoring(self):
    
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool,defineHistogram

    monTool = GenericMonitoringTool("MonTool"+self.chain())
    monTool.Histograms = [
      defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="FastPhoton Hypo Cut Counter;Cut Counter", xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative"),
      defineHistogram('Et', type='TH1F', path='EXPERT', title="FastPhoton Hypo E_{T}^{EM};E_{T}^{EM} [MeV]",xbins=50, xmin=-2000, xmax=100000),
      defineHistogram('Eta', type='TH1F', path='EXPERT', title="FastPhoton Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5),
      defineHistogram('Phi', type='TH1F', path='EXPERT', title="FastPhoton Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2),
      defineHistogram('Rcore', type='TH1F', path='EXPERT', title="FastPhoton Hypo R_{core};E^{3x7}/E^{7x7} in sampling 2",xbins=48, xmin=-0.1, xmax=1.1),
      defineHistogram('Eratio', type='TH1F', path='EXPERT',title="FastPhoton Hypo E_{ratio};E^{max1}-E^{max2}/E^{max1}+E^{max2} in sampling 1 (excl.crack)",xbins=64, xmin=-0.1, xmax=1.5),
      defineHistogram('Et_had', type='TH1F', path='EXPERT', title="FastPhoton Hypo E_{T}^{had} in first layer;E_{T}^{had} [MeV]",xbins=50, xmin=-2000, xmax=100000),
      defineHistogram('F1', type='TH1F', path='EXPERT', title="FastPhoton Hypo f_{1};f_{1}", xbins=34, xmin=-0.5, xmax=1.2),

    ]
    monTool.HistPath = 'FastPhotonHypo/'+self.chain()
    self.__tool.MonTool = monTool




def _IncTool(name, monGroups, cpart, tool=None):
  config = TrigEgammaFastPhotonHypoToolConfig(name,monGroups, cpart, tool=tool)
  config.compile()
  return config.tool()



def TrigEgammaFastPhotonHypoToolFromDict( d , tool=None):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if i['signature']=='Photon' ]
    name = d['chainName']
    monGroups = d['monGroups']
    return _IncTool( name, monGroups, cparts[0] , tool=tool)


