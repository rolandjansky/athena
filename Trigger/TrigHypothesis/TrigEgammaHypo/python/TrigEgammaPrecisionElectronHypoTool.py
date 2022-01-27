#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.SystemOfUnits import GeV
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.AllConfigFlags import ConfigFlags

from ROOT import egammaPID

def same( val , tool):
  return [val]*( len( tool.EtaBins ) - 1 )

#
# Create the hypo alg with all selectors
#
def createTrigEgammaPrecisionElectronHypoAlg(name, sequenceOut):
    acc = ComponentAccumulator()
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
    MonTool = GenericMonitoringTool("MonTool_"+name)
  
    acc_ElectronCBSelectorTools = TrigEgammaPrecisionElectronCBSelectorCfg()
    acc_ElectronLHSelectorTools = TrigEgammaPrecisionElectronLHSelectorCfg()
    acc_ElectronDNNSelectorTools = TrigEgammaPrecisionElectronDNNSelectorCfg()

    acc.merge(acc_ElectronCBSelectorTools)
    acc.merge(acc_ElectronLHSelectorTools)
    acc.merge(acc_ElectronDNNSelectorTools)
  
    thePrecisionElectronHypo = CompFactory.TrigEgammaPrecisionElectronHypoAlg(name)
    thePrecisionElectronHypo.Electrons = str(sequenceOut)
    thePrecisionElectronHypo.RunInView = True
    thePrecisionElectronHypo.ElectronCBSelectorTools = acc_ElectronCBSelectorTools.getPublicTools()
    thePrecisionElectronHypo.ElectronLHSelectorTools = acc_ElectronLHSelectorTools.getPublicTools()
    thePrecisionElectronHypo.ElectronDNNSelectorTools = acc_ElectronDNNSelectorTools.getPublicTools()
    thePrecisionElectronHypo.DNNNames = ["dnntight", "dnnmedium", "dnnloose"] # just like the pidnames
    thePrecisionElectronHypo.CBNames = ["medium", "loose", "mergedtight"] # just like the pidnames
    thePrecisionElectronHypo.LHNames = ["lhtight", "lhmedium", "lhloose", "lhvloose", 
                                        "lhtight_nopix", "lhmedium_nopix","lhloose_nopix","lhvloose_nopix"] # just like the pidnames
    MonTool.Histograms = [ 
                defineHistogram('TIME_exec', type='TH1F', path='EXPERT', title="Precision Electron Hypo Algtime; time [ us ] ; Nruns", xbins=80, xmin=0.0, xmax=8000.0),
                defineHistogram('TIME_LH_exec', type='TH1F', path='EXPERT', title="Precision Electron Hypo LH Algtime; time [ us ] ; Nruns", xbins=20, xmin=0.0, xmax=2000),
                defineHistogram('TIME_DNN_exec', type='TH1F', path='EXPERT', title="Precision Electron Hypo DNN Algtime; time [ us ] ; Nruns", xbins=20, xmin=0.0, xmax=2000),
    ]
    MonTool.HistPath = 'PrecisionElectronHypo/'+name
    thePrecisionElectronHypo.MonTool=MonTool
    #acc.addEventAlgo(thePrecisionElectronHypo)
    return thePrecisionElectronHypo, acc

def TrigEgammaPrecisionElectronHypoAlgCfg(flags, name, inputElectronCollection ):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    hypo_tuple = createTrigEgammaPrecisionElectronHypoAlg( name, inputElectronCollection )
    hypo_alg = hypo_tuple[0]
    hypo_acc = hypo_tuple[1]
    acc.addEventAlgo( hypo_alg )
    acc.merge(hypo_acc)
    acc.addService( CompFactory.AthONNX.ONNXRuntimeSvc())
    return acc

class TrigEgammaPrecisionElectronHypoToolConfig:


  __operation_points  = [  'tight'    ,
                           'medium'   ,
                           'loose'    ,
                           'vloose'   ,
                           'lhtight'  ,
                           'lhmedium' ,
                           'lhloose'  ,
                           'lhvloose' ,
                           'lhtight_nopix'  ,
                           'lhmedium_nopix' ,
                           'lhloose_nopix'  ,
                           'lhvloose_nopix' ,
                           'dnntight' ,
                           'dnnmedium',
                           'dnnloose' ,
                           'mergedtight'
                           ]

  __operation_points_lhInfo = [
        'nopix'
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


  def __init__(self, name, monGroups, cpart, tool=None):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaPrecisionElectronHypoTool')
    self.__name = name
    self.__threshold = float(cpart['threshold'])
    self.__sel = cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']
    self.__iso = cpart['isoInfo']
    self.__d0  = cpart['lrtInfo']
    self.__gsfInfo = cpart['gsfInfo']
    self.__lhInfo = cpart['lhInfo']
    self.__monGroups = monGroups
    
    if not tool:
      from AthenaConfiguration.ComponentFactory import CompFactory
      tool = CompFactory.TrigEgammaPrecisionElectronHypoTool( name )

    tool.EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    tool.ETthr          = same( self.__threshold*GeV, tool )
    tool.dETACLUSTERthr = 0.1
    tool.dPHICLUSTERthr = 0.1
    tool.RelPtConeCut   = -999
    tool.PidName        = ""
    tool.d0Cut          = -1
    tool.AcceptAll      = False
    self.__tool         = tool    

    self.__log.debug( 'Electron_Chain     :%s', self.__name )
    self.__log.debug( 'Electron_Threshold :%s', self.__threshold )
    self.__log.debug( 'Electron_Pidname   :%s', self.__sel )
    self.__log.debug( 'Electron_iso       :%s', self.__iso )
    self.__log.debug( 'Electron_d0        :%s', self.__d0 )

  def chain(self):
    return self.__name

  #
  # Get the pidname
  #
  def pidname( self ):
    # if LLH, we should append the LH extra information if exist
    if 'lh' in self.__sel and self.__lhInfo and self.__lhInfo in self.__operation_points_lhInfo:
      return self.__sel + '_' + self.__lhInfo
    else:
      return self.__sel

  def etthr(self):
    return self.__threshold

  def isoInfo(self):
    return self.__iso

  def d0Info(self):
    return self.__d0

  def gsfInfo(self):
    return self.__gsfInfo

  def tool(self):
    return self.__tool
  
  def nocut(self):

    self.__log.debug( 'Configure nocut' )
    self.tool().ETthr          = same( self.etthr()*GeV, self.tool())
    self.tool().dETACLUSTERthr = 9999.
    self.tool().dPHICLUSTERthr = 9999.

  def etcut(self):

    self.__log.debug( 'Configure etcut' )
    self.tool().ETthr          = same( ( self.etthr() -  3 )*GeV )
    # No other cuts applied
    self.tool().dETACLUSTERthr = 9999.
    self.tool().dPHICLUSTERthr = 9999.

  #
  # LRT extra cut
  #
  def addLRTCut(self):
    if not self.d0Info() in self.__lrtD0Cut:
      self.__log.fatal(f"Bad LRT selection name: {self.d0Info()}")
    self.__tool.d0Cut = self.__lrtD0Cut[self.d0Info()]

  def acceptAll(self):
     self.tool().AcceptAll = True
  #
  # Isolation extra cut
  #
  def addIsoCut(self):
    if not self.isoInfo() in self.__isolationCut:
      self.__log.fatal(f"Bad Iso selection name: {self.isoInfo()}")
    self.tool().RelPtConeCut = self.__isolationCut[self.isoInfo()]

 

  def nominal(self):
    if not self.pidname() in self.__operation_points:
      self.__log.fatal("Bad selection name: %s" % self.pidname())
    self.tool().PidName = self.pidname()

  
  #
  # Compile the chain
  #
  def compile(self):

    if 'nocut' == self.pidname():
      self.nocut()

    else: # nominal chain using pid selection
      self.nominal()


    # secundary cut configurations
    if self.isoInfo() and self.isoInfo()!="":
      self.addIsoCut()
    if self.d0Info() and self.d0Info()!="":
      self.addLRTCut()
    

    if hasattr(self.tool(), "MonTool"):
      from TrigEgammaMonitoring.TrigEgammaMonitoringMTConfig import doOnlineMonForceCfg
      doOnlineMonAllChains = doOnlineMonForceCfg()
      monGroups = self.__monGroups

      if (any('egammaMon:online' in group for group in monGroups) or doOnlineMonAllChains):
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


def _IncTool( name, monGroups, cpart, tool=None):
    config = TrigEgammaPrecisionElectronHypoToolConfig(name, monGroups, cpart, tool=tool)
    config.compile()
    return config.tool()



def TrigEgammaPrecisionElectronHypoToolFromDict( d , tool=None):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if ((i['signature']=='Electron') or (i['signature']=='Electron'))]
    name = d['chainName']
    monGroups = d['monGroups']
    return _IncTool( name, monGroups, cparts[0] , tool=tool )

                   
    
def TrigEgammaPrecisionElectronHypoToolFromName(name, conf, tool=None):
    from TriggerMenuMT.HLT.Menu.DictFromChainName import dictFromChainName
    decodedDict = dictFromChainName(conf)
    return  TrigEgammaPrecisionElectronHypoToolFromDict( decodedDict , tool=tool )





#
# Electron DNN Selectors
#
def TrigEgammaPrecisionElectronDNNSelectorCfg(name='TrigEgammaPrecisionElectronDNNSelector', ConfigFilePath=None):
    acc = ComponentAccumulator()
    # We should include the DNN here
    if not ConfigFilePath:
      ConfigFilePath = ConfigFlags.Trigger.egamma.dnnVersion
  
    import collections
    SelectorNames = collections.OrderedDict({
          'dnntight'  :'AsgElectronDNNTightSelector',
          'dnnmedium' :'AsgElectronDNNMediumSelector',
          'dnnloose'  :'AsgElectronDNNLooseSelector',
          })

    ElectronToolConfigFile = collections.OrderedDict({
          'dnntight'  :'ElectronDNNMulticlassTight.conf',
          'dnnmedium' :'ElectronDNNMulticlassMedium.conf',
          'dnnloose'  :'ElectronDNNMulticlassLoose.conf',
          })

    for dnnname, name in SelectorNames.items():
      SelectorTool = CompFactory.AsgElectronSelectorTool(name)
      SelectorTool.ConfigFile = ConfigFilePath + '/' + ElectronToolConfigFile[dnnname]
      SelectorTool.skipDeltaPoverP = True
      acc.addPublicTool(SelectorTool)

    return acc

#
# Electron LH Selectors
#
def TrigEgammaPrecisionElectronLHSelectorCfg( name='TrigEgammaPrecisionElectronLHSelector', ConfigFilePath=None):

    # Configure the LH selectors
    acc = ComponentAccumulator()
    if not ConfigFilePath:
        ConfigFilePath = ConfigFlags.Trigger.egamma.pidVersion

    import collections
    SelectorNames = collections.OrderedDict({
          'lhtight'       :'AsgElectronLHTightSelector',
          'lhmedium'      :'AsgElectronLHMediumSelector',
          'lhloose'       :'AsgElectronLHLooseSelector',
          'lhvloose'      :'AsgElectronLHVLooseSelector',
          'lhtight_nopix' :'AsgElectronLHTightSelectorNoPix',
          'lhmedium_nopix':'AsgElectronLHMediumSelectorNoPix',
          'lhloose_nopix' :'AsgElectronLHLooseSelectorNoPix',
          'lhvloose_nopix':'AsgElectronLHVLooseSelectorNoPix',
          })
     
    ElectronToolConfigFile = collections.OrderedDict({
          'lhtight'         :'ElectronLikelihoodTightTriggerConfig.conf',
          'lhmedium'        :'ElectronLikelihoodMediumTriggerConfig.conf',
          'lhloose'         :'ElectronLikelihoodLooseTriggerConfig.conf',
          'lhvloose'        :'ElectronLikelihoodVeryLooseTriggerConfig.conf',
          'lhtight_nopix'   :'ElectronLikelihoodTightTriggerConfig_NoPix.conf',
          'lhmedium_nopix'  :'ElectronLikelihoodMediumTriggerConfig_NoPix.conf',
          'lhloose_nopix'   :'ElectronLikelihoodLooseTriggerConfig_NoPix.conf',
          'lhvloose_nopix'  :'ElectronLikelihoodVeryLooseTriggerConfig_NoPix.conf',
          })

    for pidname, name in SelectorNames.items():
      SelectorTool = CompFactory.AsgElectronLikelihoodTool(name)
      SelectorTool.ConfigFile = ConfigFilePath + '/' + ElectronToolConfigFile[pidname]
      SelectorTool.usePVContainer = False 
      SelectorTool.skipDeltaPoverP = True
      acc.addPublicTool(SelectorTool)
    return acc


#
# Electron CB Selectors
#

def TrigEgammaPrecisionElectronCBSelectorCfg(name='TrigEgammaPrecisionElectronCBSelector', ConfigFilePath=None):
    acc = ComponentAccumulator()
    from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import BitDefElectron

    ElectronLooseHI = (0
            | 1 << BitDefElectron.ClusterEtaRange_Electron
            | 1 << BitDefElectron.ClusterHadronicLeakage_Electron
            | 1 << BitDefElectron.ClusterMiddleEnergy_Electron
            | 1 << BitDefElectron.ClusterMiddleEratio37_Electron
            | 1 << BitDefElectron.ClusterMiddleWidth_Electron
            | 1 << BitDefElectron.ClusterStripsWtot_Electron
    )

    ElectronMediumHI = (ElectronLooseHI
            | 1 << BitDefElectron.ClusterMiddleEratio33_Electron
            | 1 << BitDefElectron.ClusterBackEnergyFraction_Electron
            | 1 << BitDefElectron.ClusterStripsEratio_Electron
            | 1 << BitDefElectron.ClusterStripsDeltaEmax2_Electron
            | 1 << BitDefElectron.ClusterStripsDeltaE_Electron
            | 1 << BitDefElectron.ClusterStripsFracm_Electron
            | 1 << BitDefElectron.ClusterStripsWeta1c_Electron
    )

    if not ConfigFilePath:
        ConfigFilePath = ConfigFlags.Trigger.egamma.pidVersion

    from collections import OrderedDict
    SelectorNames = OrderedDict({
          'medium': 'AsgElectronIsEMSelectorHIMedium',
          'loose': 'AsgElectronIsEMSelectorHILoose',
          'mergedtight'  : 'AsgElectronIsEMSelectorMergedTight',
    })

    ElectronToolConfigFile = {
          'medium': 'ElectronIsEMMediumSelectorCutDefs.conf',
          'loose': 'ElectronIsEMLooseSelectorCutDefs.conf',
          'mergedtight'  : 'ElectronIsEMMergedTightSelectorCutDefs.conf',
    }

    ElectronMaskBits = {
          'medium': ElectronMediumHI,
          'loose': ElectronLooseHI,
          'mergedtight'  : egammaPID.ElectronTightHLT,
    }

    for sel, name in SelectorNames.items():
        SelectorTool = CompFactory.AsgElectronIsEMSelector(name)
        SelectorTool.ConfigFile = ConfigFilePath + '/' + ElectronToolConfigFile[sel]
        SelectorTool.isEMMask = ElectronMaskBits[sel]
        acc.addPublicTool(SelectorTool)
    
    return acc

