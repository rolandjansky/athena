#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
 
from AthenaCommon.SystemOfUnits import GeV

def same( val , tool):
  return [val]*( len( tool.EtaBins ) - 1 )


#
# Create the hypo alg with all selectors
#
def createTrigEgammaPrecisionElectronHypoAlg(name, sequenceOut):
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
    MonTool = GenericMonitoringTool("MonTool_"+name)
    
    # make the Hypo
    from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import createTrigEgammaPrecisionElectronCBSelectors
    from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import createTrigEgammaPrecisionElectronLHSelectors
    from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import createTrigEgammaPrecisionElectronDNNSelectors
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionElectronHypoAlg
   
    thePrecisionElectronHypo = TrigEgammaPrecisionElectronHypoAlg(name)
    thePrecisionElectronHypo.Electrons = sequenceOut
    thePrecisionElectronHypo.RunInView = True
    thePrecisionElectronHypo.DNNNames = ["dnntight", "dnnmedium", "dnnloose"] # just like the pidnames
    thePrecisionElectronHypo.ElectronCBSelectorTools = createTrigEgammaPrecisionElectronCBSelectors()
    thePrecisionElectronHypo.ElectronLHSelectorTools = createTrigEgammaPrecisionElectronLHSelectors()
    thePrecisionElectronHypo.ElectronDNNSelectorTools = createTrigEgammaPrecisionElectronDNNSelectors()
    thePrecisionElectronHypo.CBNames = ["medium", "loose", "mergedtight"] # just like the pidnames
    thePrecisionElectronHypo.LHNames = ["lhtight", "lhmedium", "lhloose", "lhvloose", "lhtightnopix", "lhmediumnopix","lhloosenopix","lhvloosenopix"] # just like the pidnames
    MonTool.Histograms = [ 
                defineHistogram('TIME_exec', type='TH1F', path='EXPERT', title="Precision Electron Hypo Algtime; time [ us ] ; Nruns", xbins=80, xmin=0.0, xmax=8000.0),
                defineHistogram('TIME_LH_exec', type='TH1F', path='EXPERT', title="Precision Electron Hypo LH Algtime; time [ us ] ; Nruns", xbins=30, xmin=0.0, xmax=3000.0),
                defineHistogram('TIME_DNN_exec', type='TH1F', path='EXPERT', title="Precision Electron Hypo DNN Algtime; time [ us ] ; Nruns", xbins=80, xmin=0.0, xmax=8000.0),
    ]
    MonTool.HistPath = 'PrecisionElectronHypo/'+name
    thePrecisionElectronHypo.MonTool=MonTool


    return thePrecisionElectronHypo



class TrigEgammaPrecisionElectronHypoToolConfig:


  __operation_points  = [  'tight'    ,
                           'medium'   ,
                           'loose'    ,
                           'vloose'   ,
                           'lhtight'  ,
                           'lhmedium' ,
                           'lhloose'  ,
                           'lhvloose' ,
                           'lhtightnopix'  ,
                           'lhmediumnopix' ,
                           'lhloosenopix'  ,
                           'lhvloosenopix' ,
                           'dnntight',
                           'dnnmedium',
                           'dnnloose',
                           'mergedtight'
                           ]

  # isolation cuts:w
  __isolationCut = {
        None: None,
        'ivarloose': 0.1,
        'ivarmedium': 0.065,
        'ivartight': 0.05
        }

  # LRT d0 cuts
  __lrtD0Cut = {
      '': -1.,
      None: None,
      'lrtloose':2.0,
      'lrtmedium':3.0,
      'lrttight':5.
      }


  def __init__(self, name, threshold, sel, iso, d0, trkInfo, tool=None):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaPrecisionElectronHypoTool')
    self.__name = name
    self.__threshold = float(threshold)
    self.__sel = sel
    self.__iso = iso
    self.__d0  = d0
    self.__trkInfo = trkInfo
    
    if not tool:
      from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionElectronHypoTool
      tool = TrigEgammaPrecisionElectronHypoTool( name )

    tool.EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    tool.ETthr          = same( self.__threshold*GeV, tool )
    tool.dETACLUSTERthr = 0.1
    tool.dPHICLUSTERthr = 0.1
    tool.RelPtConeCut   = -999
    tool.PidName        = ""
    tool.d0Cut          = -1
    self.__tool         = tool    

    self.__log.debug( 'Electron_Chain     :%s', name )
    self.__log.debug( 'Electron_Threshold :%s', threshold )
    self.__log.debug( 'Electron_Pidname   :%s', sel )
    self.__log.debug( 'Electron_iso       :%s', iso )
    self.__log.debug( 'Electron_d0       :%s', d0 )

  def chain(self):
    return self.__name

  def pidname( self ):
    return self.__sel

  def etthr(self):
    return self.__threshold

  def isoInfo(self):
    return self.__iso

  def d0Info(self):
    return self.__d0

  def trkInfo(self):
    return self.__trkInfo

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
    self.tool().ETthr          = same( ( self.etthr() -  3 )*GeV )
    # No other cuts applied
    self.tool().dETACLUSTERthr = 9999.
    self.tool().dPHICLUSTERthr = 9999.

  def lrt(self):
    if not self.d0Info() in self.__lrtD0Cut:
      self.__log.fatal(f"Bad LRT selection name: {self.d0Info()}")
    self.__tool.d0Cut = self.__lrtD0Cut[self.d0Info()]

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

      if self.d0Info() and self.d0Info() != '' and 'idperf' not in self.trkInfo():
        self.lrt()

    else:
      self.nominal()

      if self.d0Info() and self.d0Info() != '' and 'idperf' not in self.trkInfo():
        self.lrt()

    if hasattr(self.tool(), "MonTool"):
      self.addMonitoring()



  #
  # Create the monitoring code
  #
  def addMonitoring(self):

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
    monTool = GenericMonitoringTool("MonTool_"+self.chain())
    monTool.Histograms = [ defineHistogram('dEta', type='TH1F', path='EXPERT', title="PrecisionElectron Hypo #Delta#eta_{EF L1}; #Delta#eta_{EF L1}", xbins=80, xmin=-0.01, xmax=0.01),
                           defineHistogram('dPhi', type='TH1F', path='EXPERT', title="PrecisionElectron Hypo #Delta#phi_{EF L1}; #Delta#phi_{EF L1}", xbins=80, xmin=-0.01, xmax=0.01),
                           defineHistogram('Et_em', type='TH1F', path='EXPERT', title="PrecisionElectron Hypo cluster E_{T}^{EM};E_{T}^{EM} [MeV]", xbins=50, xmin=-2000, xmax=100000),
                           defineHistogram('Eta', type='TH1F', path='EXPERT', title="PrecisionElectron Hypo entries per Eta;Eta", xbins=100, xmin=-2.5, xmax=2.5),
                           defineHistogram('Phi', type='TH1F', path='EXPERT', title="PrecisionElectron Hypo entries per Phi;Phi", xbins=128, xmin=-3.2, xmax=3.2),
                           defineHistogram('EtaBin', type='TH1I', path='EXPERT', title="PrecisionElectron Hypo entries per Eta bin;Eta bin no.", xbins=11, xmin=-0.5, xmax=10.5),
                           defineHistogram('LikelihoodRatio', type='TH1F', path='EXPERT', title="PrecisionElectron Hypo LH", xbins=100, xmin=-5, xmax=5),
                           defineHistogram('mu', type='TH1F', path='EXPERT', title="Average interaction per crossing", xbins=100, xmin=0, xmax=100)]

    cuts=['Input','#Delta #eta EF-L1', '#Delta #phi EF-L1','eta','E_{T}^{EM}']

    monTool.Histograms += [ defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="PrecisionElectron Hypo Passed Cuts;Cut",
                                            xbins=13, xmin=-1.5, xmax=12.5,  opt="kCumulative", xlabels=cuts) ]

    monTool.HistPath = 'PrecisionElectronHypo/'+self.chain()
    self.tool().MonTool = monTool


def _IncTool( name, threshold, sel, iso, d0, trkInfo, tool=None):
    config = TrigEgammaPrecisionElectronHypoToolConfig(name, threshold, sel, iso, d0, trkInfo, tool=tool)
    config.compile()
    return config.tool()



def TrigEgammaPrecisionElectronHypoToolFromDict( d , tool=None):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if ((i['signature']=='Electron') or (i['signature']=='Electron'))]

    def __mult(cpart):
        return int( cpart['multiplicity'] )

    def __th(cpart):
        return cpart['threshold']
    
    def __sel(cpart):
        return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']

    def __iso(cpart):
        return cpart['isoInfo']

    def __d0(cpart):
        return cpart['lrtInfo']

    def __trk(cpart):
        return cpart['trkInfo']

    name = d['chainName']
    return _IncTool( name, __th( cparts[0]),  __sel( cparts[0] ), __iso ( cparts[0]), __d0(cparts[0]), __trk(cparts[0]) , tool=tool )

                   
    
def TrigEgammaPrecisionElectronHypoToolFromName(name, conf, tool=None):
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    decodedDict = dictFromChainName(conf)
    return  TrigEgammaPrecisionElectronHypoToolFromDict( decodedDict , tool=tool )
