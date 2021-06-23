# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration



from AthenaCommon.SystemOfUnits import GeV


#
# For electrons only
#
def createTrigEgammaFastElectronHypoAlg(name, sequenceOut):
  
  # make the Hypo
  #rom TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import createTrigEgammaFastCaloSelectors
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
                           #'dnntight',
                           #'dnnmedium',
                           #'dnnloose',
                           #'dnnvloose',
                           ]

  __trigElectronLrtd0Cut = { 'lrtloose' :2.0,
                             'lrtmedium':3.0,
                             'lrttight' :5.0
                           }

  def __init__(self, name, threshold, sel, trkinfo, lrtinfo):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaFastElectronHypoTool')
    self.__name       = name
    self.__threshold  = float(threshold) 
    self.__sel        = sel
    self.__trkInfo    = trkinfo
    self.__lrtInfo    = lrtinfo

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
    tool.PidName              = sel
    
    self.__log.debug( 'Chain     :%s', name )
    self.__log.debug( 'Threshold :%s', threshold )
    self.__log.debug( 'Pidname   :%s', sel )


  def chain(self):
    return self.__name
  
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
  # Apply NN ringer selection
  #
  def ringer(self):
    self.tool().DoRinger = True
    self.tool().PidName = self.pidname()

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

    # add mon tool
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




def _IncTool(name, threshold, sel, trk, lrt):
  config = TrigEgammaFastElectronHypoToolConfig(name, threshold, sel, trk, lrt)
  config.compile()
  return config.tool()



def TrigEgammaFastElectronHypoToolFromDict( d ):
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

    name = d['chainName']

    return _IncTool( name, __th(cparts[0]), __sel(cparts[0]), __trk(cparts[0]) , __lrt(cparts[0]) )



def TrigEgammaFastElectronHypoToolFromName( name, conf ):
    """ provides configuration of the hypo tool giben the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """

    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    decodedDict = dictFromChainName(conf)
    return TrigEgammaFastElectronHypoToolFromDict( decodedDict )


if __name__ == "__main__":
    pass


