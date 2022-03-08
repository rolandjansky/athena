# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration



from AthenaCommon.SystemOfUnits import GeV


#
# For electrons only
#
def createTrigEgammaFastElectronHypoAlg(name, sequenceOut):
  
  # make the Hypo
  #rom TriggerMenuMT.HLT.Egamma.TrigEgammaDefs import createTrigEgammaFastCaloSelectors
  from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaFastElectronHypoAlg
  theFastElectronHypo = TrigEgammaFastElectronHypoAlg(name)
  theFastElectronHypo.Electrons = sequenceOut
  theFastElectronHypo.RunInView = True

  # this should be uncommented when ringer is ready
  #theFastElectronHypo.PidNames = ["tight", "medium", "loose", "vloose"]
  #theFastElectronHypo.RingerNNSelectorTools = createTrigEgammaFastCaloSelectors()

  from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
  MonTool = GenericMonitoringTool("MonTool_"+name)
  MonTool.Histograms = [ 
        defineHistogram('TIME_exec', type='TH1F', path='EXPERT', title="Fast Calo Hypo Algtime; time [ us ] ; Nruns", xbins=80, xmin=0.0, xmax=8000.0),
        defineHistogram('TIME_NN_exec', type='TH1F', path='EXPERT', title="Fast Calo Hypo NN Algtime; time [ us ] ; Nruns", xbins=100, xmin=0.0, xmax=100),
  ]
  MonTool.HistPath = 'FastElectronHypo/'+name
  theFastElectronHypo.MonTool=MonTool
  return theFastElectronHypo



#
# For electrons
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
                           'mergedtight',
                           'dnntight' ,  
                           'dnnmedium',
                           'dnnloose' ,
                           'dnnvloose',
                           ]

  __trigElectronLrtd0Cut = { 'lrtloose' :2.0,
                             'lrtmedium':3.0,
                             'lrttight' :5.0
                           }

  def __init__(self, name, monGroups, cpart, tool=None):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaFastElectronHypoTool')
    self.__name       = name
    self.__threshold  = float(cpart['threshold']) 
    self.__sel        = cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']
    self.__idperfInfo    = cpart['idperfInfo']
    self.__lrtInfo    = cpart['lrtInfo']
    self.__monGroups       = monGroups

    if not tool:
      from AthenaConfiguration.ComponentFactory import CompFactory
      tool = CompFactory.TrigEgammaFastElectronHypoTool(name)
    
    self.__tool = tool
    tool.AcceptAll            = False
    tool.DoRinger             = False
    tool.TrackPt              = 0.0
    tool.CaloTrackdETA        = 0.2
    tool.CaloTrackdPHI        = 990.
    tool.CaloTrackdEoverPLow  = 0.0
    tool.CaloTrackdEoverPHigh = 999.0
    tool.TRTRatio             = -999.
    tool.PidName              = ""
    
    self.__log.debug( 'Chain     :%s', self.__name )
    self.__log.debug( 'Threshold :%s', self.__threshold )
    self.__log.debug( 'Pidname   :%s', self.__sel )


  def chain(self):
    return self.__name
  
  def pidname( self ):
    return self.__sel

  def etthr(self):
    return self.__threshold

  def lrtInfo(self):
    return self.__lrtInfo
  
  def idperfInfo(self):
    return self.__idperfInfo

  def tool(self):
    return self.__tool
  

  def nocut(self):
    self.tool().AcceptAll = True

  #
  # Apply NN ringer selection
  #
  def ringer(self):
    self.tool().DoRinger = True
    self.tool().PidName = self.pidname()


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


  def addLRTCut(self):
    self.tool().DoLRT = True
    self.tool().d0Cut=self.__trigElectronLrtd0Cut[self.lrtInfo()]


  #
  # Compile the chain
  #
  def compile(self):
    
    if 'idperf' in self.idperfInfo():
      self.nocut()
    else:
      self.nominal()

    # secondary extra cut
    if self.lrtInfo() in self.__trigElectronLrtd0Cut.keys():
      self.addLRTCut()


    # add mon tool
    if hasattr(self.tool(), "MonTool"):
      from TrigEgammaMonitoring.TrigEgammaMonitoringMTConfig import doOnlineMonForceCfg
      doOnlineMonAllChains = doOnlineMonForceCfg()
      monGroups = self.__monGroups

      if (any('egammaMon:online' in group for group in monGroups) or doOnlineMonAllChains):
        self.addMonitoring()


  #
  # Monitoring code
  #
  def addMonitoring(self):
    
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    monTool = GenericMonitoringTool("MonTool"+self.__name)
    monTool.defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="FastElectron Hypo Cut Counter;Cut Counter", xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative")
    monTool.defineHistogram('CaloTrackdEta', type='TH1F', path='EXPERT', title="FastElectron Hypo #Delta #eta between cluster and track;#Delta #eta;Nevents", xbins=80, xmin=-0.4, xmax=0.4)
    monTool.defineHistogram('CaloTrackdPhi', type='TH1F', path='EXPERT', title="FastElectron Hypo #Delta #phi between cluster and track;#Delta #phi;Nevents", xbins=80, xmin=-0.4, xmax=0.4)
    monTool.defineHistogram('CaloTrackEoverP', type='TH1F', path='EXPERT', title="FastElectron Hypo E/p;E/p;Nevents", xbins=120, xmin=0, xmax=12)
    monTool.defineHistogram('PtTrack', type='TH1F', path='EXPERT', title="FastElectron Hypo p_{T}^{track} [MeV];p_{T}^{track} [MeV];Nevents", xbins=50, xmin=0, xmax=100000)
    monTool.defineHistogram('PtCalo', type='TH1F', path='EXPERT', title="FastElectron Hypo p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000)
    monTool.defineHistogram('CaloEta', type='TH1F', path='EXPERT', title="FastElectron Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5)
    monTool.defineHistogram('CaloPhi', type='TH1F', path='EXPERT', title="FastElectron Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2)
    if self.tool().DoRinger:
      monTool.defineHistogram('NNOutput',type='TH1F', path='EXPERT',title="NN Output; NN; Count", xbins=17,xmin=-8,xmax=+8),

    monTool.HistPath = 'FastElectronHypo/'+self.__name
    self.tool().MonTool = monTool




def _IncTool(name, monGroups, cpart, tool=None):
  config = TrigEgammaFastElectronHypoToolConfig(name,monGroups, cpart, tool=tool)
  config.compile()
  return config.tool()



def TrigEgammaFastElectronHypoToolFromDict( d , tool=None):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if (i['signature']=='Electron')]
    name = d['chainName']
    monGroups = d['monGroups']
    return _IncTool( name, monGroups, cparts[0] , tool=tool)


if __name__ == "__main__":
    pass


