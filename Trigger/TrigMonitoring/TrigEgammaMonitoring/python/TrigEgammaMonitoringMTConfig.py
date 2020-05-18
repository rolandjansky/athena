# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

'''@file TrigEgammaMonitoringConfigRun3.py
@author D. Maximov (histograms), Joao victor Pinto (core)
@date 2019-07-08
@brief Run 3 configuration builder. Histograms definitions taken from TrigEgammaPlotTool
'''


#from TrigEgammaAnalysisTools.TrigEgammaProbelist import monitoring_electron, monitoring_photon, monitoringTP_electronJpsiee, monitoringTP_electron
from TrigEgammaHypo.TrigEgammaPidTools import ElectronPidTools
from TrigEgammaHypo.TrigEgammaPidTools import PhotonPidTools
import cppyy
 
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentFactory import CompFactory as CfgMgr


if 'DQMonFlags' not in dir():
    from AthenaMonitoring.DQMonFlags import DQMonFlags as dqflags




class TrigEgammaMonAlgBuilder:

  _configured = False
  _get_monitoring_mode_success = False
  
  data_type = ''
  pp_mode = False
  pPb_mode = False
  HI_mode = False
  cosmic_mode = False
  mc_mode = False
  activate_electron = False
  activate_photon = False
  activate_zee = False
  activate_jpsiee = False
  tagItems = []
  jpsitagItems = []
  electronList = []
  photonList = []
  tpList = []
  jpsiList = []

  # Monitoring algorithms
  zeeMonAlg = None
  jpsieeMonAlg = None
  elMonAlg = None
  phMonAlg = None

  
  # Add a flag to enable emulation
  __acceptable_keys_list=['derivation','emulation','detailedHistograms','basePath']
  emulation = False
  derivation = False
  detailedHistograms = False
  basePath = 'HLT/EgammaMon'


  isemnames = ["Tight", "Medium", "Loose"]
  lhnames   = ["LHTight", "LHMedium", "LHLoose"]
 

  def __init__(self, helper, runflag, **kwargs):
 
    from AthenaCommon.Logging import logging
    self.__logger = logging.getLogger( 'TrigEgammaMonAlgBuilder' )
    self.runFlag = runflag
    self.helper = helper
    if not self._configured:
      for key,value in kwargs.items():
        if key in self.__acceptable_keys_list:
          setattr(self,key,value)
      self.configureMode()



  def configureMode(self):

    self.__logger.info("TrigEgammaMonToolBuilder.configureMode()")
    self._get_monitoring_mode_success = self.get_monitoring_mode()
    if self._get_monitoring_mode_success is False:
      self.__logger.warning("HLTMonTriggerList: Error getting monitoring mode, default monitoring lists will be used.")
    else:
      self.__logger.info("Configuring for %s", self.data_type)

    # Since we load the tools by name below 
    # Need to ensure the correct tools are configured 
    # for each monitoring mode
    if self.mc_mode is True or self.pp_mode is True:
      if(self.derivation is True or self.emulation is True):
        self.activate_zee = True
      else:
        self.activate_zee=True
        self.activate_jpsiee=True 
        self.activate_electron=True
        self.activate_photon=True
    elif self.HI_mode is True or self.pPb_mode is True or self.cosmic_mode is True:
      self.activate_electron=True
      self.activate_photon=True
    else:
      self.activate_zee=True
      self.activate_jpsiee=True
      self.activate_electron=True
      self.activate_photon=True

    
  def configure(self):
    self.setProperties()
    self.configureMonitor()
    self.configureHistograms()





  def get_monitoring_mode(self):

    # Implementation of https://its.cern.ch/jira/browse/ATR-13200
    self.__logger.info("TrigEgammaMonToolBuilder.get_monitoring_mode()")
    self.data_type = dqflags.monManDataType()
    if self.data_type == 'monteCarlo': 
      self.mc_mode = True
      return True
    elif self.data_type == 'collisions': 
      self.pp_mode = True
      return True
    elif self.data_type == 'heavyioncollisions':
      self.HI_mode = True
      self.pPb_mode = True
      return True
    elif self.data_type == 'cosmics':
      self.cosmic_mode = True
      return True
    else:
      return False


  def setProperties(self):

    self.__logger.info("TrigEgammaMonToolBuilder.setProperties()")
    self.basePath = 'HLT/EgammaMon'
   
    if self.pp_mode is True:
      self.setDefaultProperties()
    elif self.cosmic_mode is True:
      # This should be change in future
      self.setDefaultProperties()
    elif self.HI_mode is True or self.pPb_mode is True:
      # This should be change in future
      self.setDefaultProperties()
    elif self.mc_mode is True:
      # This should be change in future
      self.setDefaultProperties()
    else:
      self.__logger.info('No monitoring mode configured, use default')
      self.setDefaultProperties()


    self.__logger.info('Configuring TP electron chains %s',self.tpList)
    self.__logger.info('Configuring electron chains %s',self.electronList)
    self.__logger.info('Configuring photon chains %s',self.photonList)



  def setDefaultProperties(self):
    
    #from TrigEgammaMonitoring.TrigEgammaMonitCategory import validation_electron, validation_photon
    #from TrigEgammaMonitoring.TrigEgammaMonitCategory import validationTP_electron, validationTP_Jpsiee
    #from TrigEgammaMonitoring.TrigEgammaMonitCategory import monitoring_tags, monitoring_jpsitags
    
    # This will be removed for future.
    monitoring_electron = ['HLT_e3_etcut_L1EM3','HLT_e5_etcut_L1EM3','HLT_e7_etcut_L1EM3','HLT_e300_etcut_L1EM24VHI']
    monitoring_photon = ['HLT_g5_etcut_L1EM3','HLT_g5_loose_L1EM3','HLT_g5_medium_L1EM3','HLT_g5_tight_L1EM3','HLT_g140_loose_L1EM24VH']
    monitoringTP_electron = ['HLT_e26_lhtight_L1EM24VHI','HLT_e60_lhmedium_L1EM24VHI','HLT_e140_lhloose_L1EM24VHI']


    #self.electronList = validation_electron
    #self.photonList   = validation_photon
    #self.tpList       = validationTP_electron
    #self.jpsiList     = validationTP_Jpsiee
    #self.tagItems     = [] #monitoring_tags 
    #self.jpsitagItems = [] #monitoring_jpsitags

    self.electronList = monitoring_electron
    self.photonList   = monitoring_photon
    self.tpList       = monitoringTP_electron
    self.jpsiList     = []
    self.tagItems     = [] #monitoring_tags 
    self.jpsitagItems = [] #monitoring_jpsitags

  



  #
  # Create all minitor algorithms
  #
  def configureMonitor( self ):

    acc = self.helper.resobj
    EgammaMatchTool = CompFactory.TrigEgammaMatchingToolMT()
    EgammaMatchTool.DeltaR=0.4
    acc.addPublicTool(EgammaMatchTool)
    cppyy.loadDictionary('ElectronPhotonSelectorToolsDict')
    # Following loads the online selectors
  
    # setup all pid tools
    ElectronPidTools()
    PhotonPidTools()
  
    # Offline selectors -- taken from latest conf
    LooseElectronSelector             = CfgMgr.AsgElectronIsEMSelector("T0HLTLooseElectronSelector")
    MediumElectronSelector            = CfgMgr.AsgElectronIsEMSelector("T0HLTMediumElectronSelector")
    TightElectronSelector             = CfgMgr.AsgElectronIsEMSelector("T0HLTTightElectronSelector")
    LooseLHSelector                   = CfgMgr.AsgElectronLikelihoodTool("T0HLTLooseLHSelector")
    MediumLHSelector                  = CfgMgr.AsgElectronLikelihoodTool("T0HLTMediumLHSelector")
    TightLHSelector                   = CfgMgr.AsgElectronLikelihoodTool("T0HLTTightLHSelector")
    VeryLooseLHSelector               = CfgMgr.AsgElectronLikelihoodTool("T0HLTVeryLooseLHSelector")
  
    acc.addPublicTool(LooseElectronSelector)
    acc.addPublicTool(MediumElectronSelector)
    acc.addPublicTool(TightElectronSelector)
    acc.addPublicTool(LooseLHSelector)
    acc.addPublicTool(MediumLHSelector)
    acc.addPublicTool(TightLHSelector)
    acc.addPublicTool(VeryLooseLHSelector)
  
    if self.runFlag == '2018':
      # cut based
      LooseElectronSelector.ConfigFile  = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMLooseSelectorCutDefs.conf"
      MediumElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMMediumSelectorCutDefs.conf"
      TightElectronSelector.ConfigFile  = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMTightSelectorCutDefs.conf"
      # 2018 (vtest)
      LooseLHSelector.ConfigFile        = "ElectronPhotonSelectorTools/offline/mc16_20170828/ElectronLikelihoodLooseOfflineConfig2017_CutBL_Smooth.conf"
      MediumLHSelector.ConfigFile       = "ElectronPhotonSelectorTools/offline/mc16_20170828/ElectronLikelihoodMediumOfflineConfig2017_Smooth.conf"
      TightLHSelector.ConfigFile        = "ElectronPhotonSelectorTools/offline/mc16_20170828/ElectronLikelihoodTightOfflineConfig2017_Smooth.conf"
      VeryLooseLHSelector.ConfigFile    = "ElectronPhotonSelectorTools/offline/mc16_20170828/ElectronLikelihoodVeryLooseOfflineConfig2017_Smooth.conf"
    elif self.runFlag == '2017':
      # cut based
      LooseElectronSelector.ConfigFile  = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMLooseSelectorCutDefs.conf"
      MediumElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMMediumSelectorCutDefs.conf"
      TightElectronSelector.ConfigFile  = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMTightSelectorCutDefs.conf"
      # 2017 (v11)
      LooseLHSelector.ConfigFile        = "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodLooseOfflineConfig2016_CutBL_Smooth.conf"
      MediumLHSelector.ConfigFile       = "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodMediumOfflineConfig2016_Smooth.conf"
      TightLHSelector.ConfigFile        = "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodTightOfflineConfig2016_Smooth.conf"
      VeryLooseLHSelector.ConfigFile    = "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodVeryLooseOfflineConfig2016_Smooth.conf"
    else:
      # raise since the configuration its not defined
      raise RuntimeError( 'Wrong run flag configuration' )
    

    if self.activate_zee:

      self.__logger.info( "Creating the Zee monitor algorithm...")
      self.zeeMonAlg = self.helper.addAlgorithm( CompFactory.TrigEgammaMonitorTagAndProbeAlgorithm, "TrigEgammaMonitorTagAndProbeAlgorithm_Zee" )
      self.zeeMonAlg.Analysis='Zee'
      self.zeeMonAlg.MatchTool = EgammaMatchTool
      self.zeeMonAlg.TPTrigger=False
      self.zeeMonAlg.ElectronKey = 'Electrons'
      self.zeeMonAlg.isEMResultNames=self.isemnames
      self.zeeMonAlg.LHResultNames=self.lhnames
      self.zeeMonAlg.ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector]
      self.zeeMonAlg.ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector]
      self.zeeMonAlg.ZeeLowerMass=80
      self.zeeMonAlg.ZeeUpperMass=100
      self.zeeMonAlg.OfflineTagMinEt=25
      self.zeeMonAlg.OfflineTagSelector='LHTight'
      self.zeeMonAlg.OfflineProbeSelector='LHLoose'
      self.zeeMonAlg.OppositeCharge=True
      self.zeeMonAlg.RemoveCrack=False
      self.zeeMonAlg.TagTriggerList=self.tagItems
      self.zeeMonAlg.TriggerList=self.tpList
      self.zeeMonAlg.DetailedHistograms=self.detailedHistograms


    if self.activate_jpsiee:

      self.__logger.info( "Creating the Jpsiee monitor algorithm...")
      self.jpsieeMonAlg = self.helper.addAlgorithm( CompFactory.TrigEgammaMonitorTagAndProbeAlgorithm, "TrigEgammaMonitorTagAndProbeAlgorithm_Jpsiee" )
      self.jpsieeMonAlg.DoJpsiee=True
      self.jpsieeMonAlg.Analysis='Jpsiee'
      self.jpsieeMonAlg.MatchTool = EgammaMatchTool
      self.jpsieeMonAlg.TPTrigger=False
      self.jpsieeMonAlg.ElectronKey = 'Electrons'
      self.jpsieeMonAlg.isEMResultNames=self.isemnames
      self.jpsieeMonAlg.LHResultNames=self.lhnames
      self.jpsieeMonAlg.ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector]
      self.jpsieeMonAlg.ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector]
      self.jpsieeMonAlg.ZeeLowerMass=2
      self.jpsieeMonAlg.ZeeUpperMass=5
      self.jpsieeMonAlg.OfflineTagMinEt=5
      self.jpsieeMonAlg.OfflineTagSelector='LHTight'
      self.jpsieeMonAlg.OfflineProbeSelector='LHLoose'
      self.jpsieeMonAlg.OppositeCharge=True
      self.jpsieeMonAlg.RemoveCrack=False
      self.jpsieeMonAlg.TagTriggerList=self.jpsitagItems
      self.jpsieeMonAlg.TriggerList=self.jpsiList
      self.jpsieeMonAlg.DetailedHistograms=self.detailedHistograms


    if self.activate_electron:

      self.__logger.info( "Creating the Electron monitor algorithm...")
      self.elMonAlg = self.helper.addAlgorithm( CompFactory.TrigEgammaMonitorElectronAlgorithm, "TrigEgammaMonitorElectronAlgorithm" )
      self.elMonAlg.MatchTool = EgammaMatchTool
      self.elMonAlg.ElectronKey = 'Electrons'
      self.elMonAlg.isEMResultNames=self.isemnames
      self.elMonAlg.LHResultNames=self.lhnames
      self.elMonAlg.ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector]
      self.elMonAlg.ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector]
      self.elMonAlg.ForcePidSelection=False
      self.elMonAlg.ForceProbeIsolation=False
      self.elMonAlg.ForceEtThreshold=True
      self.elMonAlg.TriggerList=self.electronList
      self.elMonAlg.DetailedHistograms=self.detailedHistograms

    if self.activate_photon:

      self.__logger.info( "Creating the Photon monitor algorithm...")
      self.phMonAlg = self.helper.addAlgorithm( CompFactory.TrigEgammaMonitorPhotonAlgorithm, "TrigEgammaMonitorPhotonAlgorithm" )
      self.phMonAlg.MatchTool = EgammaMatchTool
      self.phMonAlg.PhotonKey = 'Photons'
      self.phMonAlg.isEMResultNames=self.isemnames
      self.phMonAlg.LHResultNames=self.lhnames
      self.phMonAlg.ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector]
      self.phMonAlg.ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector]
      self.phMonAlg.TriggerList=self.photonList
      self.phMonAlg.DetailedHistograms=self.detailedHistograms



  
  def configureHistograms(self):

    if self.activate_zee and self.zeeMonAlg:
      self.setBinning()
      self.bookEvent( self.zeeMonAlg, self.zeeMonAlg.Analysis )
      triggers = self.zeeMonAlg.TriggerList; triggers.extend( self.zeeMonAlg.TagTriggerList )
      self.bookExpertHistograms( self.zeeMonAlg, triggers )
    if self.activate_jpsiee and self.jpsieeMonAlg:
      self.setBinning(True)
      self.bookEvent( self.jpsieeMonAlg, self.jpsieeMonAlg.Analysis )
      triggers = self.jpsieeMonAlg.TriggerList; triggers.extend( self.jpsieeMonAlg.TagTriggerList )
      self.bookExpertHistograms( self.jpsieeMonAlg, triggers )
    
    self.setBinning()
    if self.activate_electron and self.elMonAlg:
      self.bookExpertHistograms( self.elMonAlg, self.elMonAlg.TriggerList )
    if self.activate_photon and self.phMonAlg:
      self.bookExpertHistograms( self.phMonAlg, self.phMonAlg.TriggerList )
  

 
  def addGroup( self, monAlg, name, path ):
    return self.helper.addGroup( monAlg, name, path )

  def addHistogram(self, monGroup, hist):
      monGroup.defineHistogram(hist.name, **hist.kwargs)


  #
  # Booking all histograms
  #
  def bookExpertHistograms( self, monAlg, triggers ):

    self.__logger.info( "Booking all histograms for alg: %s", monAlg.name )

    for trigger in triggers:
      self.bookL1CaloDistributions( monAlg, trigger )
      self.bookL2CaloDistributions( monAlg, trigger )
      self.bookL2ElectronDistributions( monAlg, trigger )
      self.bookEFCaloDistributions( monAlg, trigger )
      self.bookShowerShapesAndTrackingDistributions( monAlg, trigger, online=True )
      self.bookShowerShapesAndTrackingDistributions( monAlg, trigger, online=False )
      self.bookEfficiencies( monAlg, trigger, "L1Calo" )
      self.bookEfficiencies( monAlg, trigger, "L2Calo" )
      self.bookEfficiencies( monAlg, trigger, "L2"     )
      self.bookEfficiencies( monAlg, trigger, "EFCalo" )
      self.bookEfficiencies( monAlg, trigger, "HLT"    )
      
      if self.detailedHistograms:
        for pid in self.isemnames + self.lhnames:
          self.bookEfficiencies( monAlg, trigger, "HLT", pid )
          self.bookEfficiencies( monAlg, trigger, "HLT", pid+"Iso" )

      

  def bookEvent(self, monAlg, analysis):

    #cutLabels = ["Events","LAr","RetrieveElectrons","TwoElectrons","PassTrigger","EventWise","Success"]
    #probeLabels=["Electrons","NotTag","OS","SS","ZMass","HasTrack","HasCluster","Eta","Et","IsGoodOQ","GoodPid","NearbyJet","Isolated"]
    # Create mon group.  The group name should be the path name for map
    monGroup = self.addGroup( monAlg, 'Event', self.basePath+'/Expert/Event' )
    monGroup.defineHistogram(analysis+"_CutCounter", type='TH1I', path='/', title="Event Selection; Cut ; Count",xbins=6, xmin=0, xmax=6)
    monGroup.defineHistogram(analysis+"_TagCutCounter", type='TH1F', path='', title="Number of Probes; Cut ; Count",xbins=12, xmin=0, xmax=12)
    monGroup.defineHistogram(analysis+"_ProbeCutCounter", type='TH1F', path='', title="Number of Probes; Cut ; Count",xbins=12, xmin=0, xmax=12)
    monGroup.defineHistogram(analysis+"_Mee", type='TH1F', path='/', title="Offline M(ee); m_ee [GeV] ; Count",xbins=50, xmin=monAlg.ZeeLowerMass, xmax=monAlg.ZeeUpperMass)


  #
  # Book L1Calo distributions
  #
  def bookL1CaloDistributions( self , monAlg, trigger ):

    from TrigEgammaMonitoring.TrigEgammaMonitorHelper import TH1F
    monGroup = self.addGroup( monAlg, trigger+'_Distributions_L1Calo', self.basePath+'/'+trigger+'/Distributions/L1Calo' )
    
    self.addHistogram(monGroup, TH1F("energy", "Cluster Energy; E [GeV] ; Count", 100, 0., 200.))
    self.addHistogram(monGroup, TH1F("roi_eta", "RoI word Cluster Energy; E [GeV] ; Count", 100, 0, 200))
    self.addHistogram(monGroup, TH1F("emIso", "EM Isolation; E [GeV] ; Count", 50, -1., 20.))
    self.addHistogram(monGroup, TH1F("hadCore", "HAD Isolation; E [GeV] ; Count", 50, -1., 20.))
    self.addHistogram(monGroup, TH1F("eta", "eta; eta ; Count", 50, -2.5, 2.5))
    self.addHistogram(monGroup, TH1F("phi", "phi; phi ; Count", 20, -3.2, 3.2))



  #
  # Book L2Calo distributions
  #
  def bookL2CaloDistributions( self , monAlg, trigger ):

    from TrigEgammaMonitoring.TrigEgammaMonitorHelper import TH1F
    monGroup = self.addGroup( monAlg, trigger+'_Distributions_L2Calo', self.basePath+'/'+trigger+'/Distributions/L2Calo' )
    
    self.addHistogram(monGroup, TH1F("et", "ET; ET [GeV] ; Count", 100, 0., 100.))
    self.addHistogram(monGroup, TH1F("eta", "eta; eta ; Count", self._nEtabins, self._etabins))
    self.addHistogram(monGroup, TH1F("phi", "phi; phi ; Count", 20, -3.2, 3.2))


  #
  # book L2 Electron distributions
  #
  def bookL2ElectronDistributions( self, monAlg, trigger ):

    from TrigEgammaMonitoring.TrigEgammaMonitorHelper import TH1F
    monGroup = self.addGroup( monAlg, trigger+'_Distributions_L2Electron', self.basePath+'/'+trigger+'/Distributions/L2' )
    
    self.addHistogram(monGroup, TH1F("et", "ET; ET [GeV] ; Count", 100, 0., 100.))
    self.addHistogram(monGroup, TH1F("eta", "eta; eta ; Count", self._nEtabins, self._etabins))
    self.addHistogram(monGroup, TH1F("phi", "phi; phi ; Count", 20, -3.2, 3.2))


  #
  # Book EFCalo distributions
  #
  def bookEFCaloDistributions( self, monAlg, trigger ):
    
    from TrigEgammaMonitoring.TrigEgammaMonitorHelper import TH1F
    monGroup = self.addGroup( monAlg, trigger+'_Distributions_EFCalo', self.basePath+'/'+trigger+'/Distributions/EFCalo' )
    
    self.addHistogram(monGroup, TH1F("et", "ET; ET [GeV] ; Count", 100, 0., 100.))
    self.addHistogram(monGroup, TH1F("eta", "eta; eta ; Count", self._nEtabins, self._etabins))
    self.addHistogram(monGroup, TH1F("phi", "phi; phi ; Count", 20, -3.2, 3.2))
    self.addHistogram(monGroup, TH1F("energy", "Cluster Energy; E [GeV] ; Count", 50, 0., 100.))
    self.addHistogram(monGroup, TH1F("eta_calo", "eta_calo; eta_calo ; Count", 50, -2.47, 2.47))
    self.addHistogram(monGroup, TH1F("phi_calo", "phi_calo; phi_calo ; Count", 50, -3.14, 3.14))
    self.addHistogram(monGroup, TH1F("energyBE0", "Cluster Energy BE0; E [GeV] ; Count", 50, 0., 100.))
    self.addHistogram(monGroup, TH1F("energyBE1", "Cluster Energy BE1; E [GeV] ; Count", 50, 0., 100.))
    self.addHistogram(monGroup, TH1F("energyBE2", "Cluster Energy BE2; E [GeV] ; Count", 50, 0., 100.))
    self.addHistogram(monGroup, TH1F("energyBE3", "Cluster Energy BE3; E [GeV] ; Count", 50, 0., 100.))



  #
  # Book Shower shapes
  #
  def bookShowerShapesAndTrackingDistributions( self, monAlg, trigger, online=True ):
    
    from TrigEgammaMonitoring.TrigEgammaMonitorHelper import TH1F
    monGroup = self.addGroup( monAlg, trigger+'_Distributions_' + ("HLT" if online else "Offline"), 
                              self.basePath+'/'+trigger+'/Distributions/' + ("HLT" if online else "Offline") )

    self.addHistogram(monGroup, TH1F("ethad", "ethad; ethad ; Count", 20, -10, 10))
    self.addHistogram(monGroup, TH1F("ethad1", "ethad1; ehad1 ; Count", 20, -10, 10))
    self.addHistogram(monGroup, TH1F("Rhad", "Rhad; Rhad ; Count", 35, -0.3, 0.3))
    self.addHistogram(monGroup, TH1F("Rhad1", "Rhad1; Rhad1 ; Count", 30, -0.3, 0.3))
    self.addHistogram(monGroup, TH1F("Reta", "Reta; Reta ; Count", 15, 0., 1.5))
    self.addHistogram(monGroup, TH1F("Rphi", "Rphi; Rphi ; Count", 15, 0., 1.5))
    self.addHistogram(monGroup, TH1F("weta1", "weta1; weta1 ; Count", 12, 0.4, 1.))
    self.addHistogram(monGroup, TH1F("weta2", "weta2; weta2 ; Count", 20, 0., 0.02))
    self.addHistogram(monGroup, TH1F("f1", "f1; f1 ; Count", 11, -0.1, 1.))
    self.addHistogram(monGroup, TH1F("f3", "f3; f3 ; Count", 21, -0.1, 0.2))
    self.addHistogram(monGroup, TH1F("eratio","eratio; eratio; Count",20, 0, 2))
    self.addHistogram(monGroup, TH1F("et", "ET; ET [GeV] ; Count", 100, 0., 100.))
    self.addHistogram(monGroup, TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 100, 0., 2000.))
    self.addHistogram(monGroup, TH1F("eta", "eta; eta ; Count", self._nEtabins, self._etabins))
    self.addHistogram(monGroup, TH1F("phi", "phi; phi ; Count", 20, -3.2, 3.2))
    self.addHistogram(monGroup, TH1F("topoetcone20", "topoetcone20; topoetcone20 [GeV] ; Count", 100, -10.0, 10.0))
    self.addHistogram(monGroup, TH1F("topoetcone20_rel", "topoetcone20/pt; topoetcone20/pt ; Count", 100, -0.5, 0.5))
    self.addHistogram(monGroup, TH1F("topoetcone40_shift", "topoetcone40-2.45 GeV; topoetcone40-2.45 GeV [GeV] ; Count", 100, -10.0, 10.0))
    self.addHistogram(monGroup, TH1F("topoetcone40_shift_rel", "(topoetcone40-2.45 GeV)/pt; (topoetcone40-2.45 GeV)/pt ; Count", 100, -0.5, 0.5))


    # Tracking quantities
    self.addHistogram(monGroup, TH1F("deta1", "deta1; deta1 ; Count", 40, -0.01, 0.01))
    self.addHistogram(monGroup, TH1F("deta1_EMECA", "deta1 EMEC-A; deta1 ; Count", 40, -0.01, 0.01))
    self.addHistogram(monGroup, TH1F("deta1_EMECC", "deta1 EMEC-C; deta1 ; Count", 40, -0.01, 0.01))
    self.addHistogram(monGroup, TH1F("deta1_EMEBA", "deta1 EMEB-A; deta1 ; Count", 40, -0.01, 0.01))
    self.addHistogram(monGroup, TH1F("deta1_EMEBC", "deta1 EMEB-A; deta1 ; Count", 40, -0.01, 0.01))
    self.addHistogram(monGroup, TH1F("deta2", "deta2; deta2 ; Count", 40, -0.01, 0.01))
    self.addHistogram(monGroup, TH1F("dphi2", "dphi2; dphi2 ; Count", 40, -0.1, 0.1))
    self.addHistogram(monGroup, TH1F("dphiresc", "dphiresc; dphiresc ; Count", 40, -0.1, 0.1))
    self.addHistogram(monGroup, TH1F("eprobht","eProbHT; eProbHT; Count",20, 0, 1.))
    self.addHistogram(monGroup, TH1F("npixhits","nPixHit; nPixHits; Count",10, 0, 10))
    self.addHistogram(monGroup, TH1F("nscthits","nSCTHit; nSCTHits; Count",20, 0, 20))
    self.addHistogram(monGroup, TH1F("charge","charge; charge; Count", 4,-2,2))
    self.addHistogram(monGroup, TH1F("ptcone20", "ptcone20; ptcone20; Count", 50, 0.0, 5.0))
    self.addHistogram(monGroup, TH1F("ptvarcone20", "ptcone20; ptcone20; Count", 50, 0.0, 5.0))
    self.addHistogram(monGroup, TH1F("d0", "d0; d0 ; Count", 40, -1, 1))
    self.addHistogram(monGroup, TH1F("d0sig", "d0sig; d0sig ; Count", 40, -10, 10))
    self.addHistogram(monGroup, TH1F("pt", "p_{T}; p_{T} [GeV] ; Count", 100,0.,100.))
    self.addHistogram(monGroup, TH1F("ptcone20_rel", "ptcone20/pt; ptcone20/pt; Count", 50, 0.0, 1.0))
    self.addHistogram(monGroup, TH1F("ptvarcone20_rel", "ptcone20/pt; ptcone20/pt; Count", 50, 0.0, 1.0))
    
    
  #
  # Book efficiencies
  #
  def bookEfficiencies(self, monAlg, trigger, level, subgroup=None ):

    from TrigEgammaMonitoring.TrigEgammaMonitorHelper import TH1F, TProfile
    if subgroup:
      monGroup = self.addGroup( monAlg, trigger+'_Efficiency_'+level+'_'+subgroup, self.basePath+'/'+trigger+'/Efficiency/'+level+'/'+subgroup )
    else:
      monGroup = self.addGroup( monAlg, trigger+'_Efficiency_'+level, self.basePath+'/'+trigger+'/Efficiency/'+level )
    


    # Numerator
    self.addHistogram(monGroup, TH1F("match_pt", "Trigger Matched Offline p_{T}; p_{T} [GeV] ; Count", self._nEtbins, self._etbins))
    self.addHistogram(monGroup, TH1F("match_et", "Trigger Matched Offline E_{T}; E_{T} [GeV]; Count", self._nEtbins, self._etbins))
    self.addHistogram(monGroup, TH1F("match_highet", "Trigger Matched Offline E_{T}; E_{T} [GeV]; Count", 40, 0., 1000.))
    self.addHistogram(monGroup, TH1F("match_eta", "Trigger Matched Offline #eta; #eta ; Count", self._nEtabins, self._etabins))
    self.addHistogram(monGroup, TH1F("match_phi", "Trigger Matched #phi; #phi ; Count", 20, -3.2, 3.2))
    self.addHistogram(monGroup, TH1F("match_avgmu", "Trigger Matched <#mu>; <#mu> ; Count", 16, 0, 80))
    self.addHistogram(monGroup, TH1F("match_npvtx", "Trigger Matched npvtx; npvtx ; Count", 16, 0, 80))

    # Denominator
    self.addHistogram(monGroup, TH1F("pt", "Offline p_{T}; p_{T} [GeV] ; Count", self._nEtbins, self._etbins))
    self.addHistogram(monGroup, TH1F("et", "Offline E_{T}; E_{T} [GeV] ; Count", self._nEtbins, self._etbins))
    self.addHistogram(monGroup, TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 40, 0., 1000.))
    self.addHistogram(monGroup, TH1F("eta", "Offline #eta; #eta ; Count", self._nEtabins, self._etabins))
    self.addHistogram(monGroup, TH1F("phi", "Offline #phi; #phi ; Count", 20, -3.2, 3.2))
    self.addHistogram(monGroup, TH1F("avgmu", "<#mu>; <#mu> ; Count", 16, 0, 80))
    self.addHistogram(monGroup, TH1F("npvtx", "npvtx; npvtx ; Count", 16, 0, 80))

    # Efficiency
    self.addHistogram(monGroup, TProfile("pt,pt_passed", "#epsilon(p_T); p_{T} ; Efficiency", self._nEtbins, self._etbins))
    self.addHistogram(monGroup, TProfile("et,et_passed", "#epsilon(E_T); E_{T} [GeV] ; Efficiency", self._nEtbins, self._etbins))
    self.addHistogram(monGroup, TProfile("highet,highet_passed", "#epsilon(E_T); E_{T} [GeV] ; Efficiency", 40, 0., 1000.))
    self.addHistogram(monGroup, TProfile("eta,eta_passed", "#epsilon(#eta); #eta ; Efficiency", self._nEtabins, self._etabins))
    self.addHistogram(monGroup, TProfile("phi,phi_passed", "#epsilon(#phi); #phi ; Efficiency", 20, -3.2, 3.2))
    self.addHistogram(monGroup, TProfile("avgmu,avgmu_passed", "#epsilon(<#mu>); <#mu> ; Efficiency", 16, 0, 80))
    self.addHistogram(monGroup, TProfile("npvtx,npvtx_passed", "#epsilon(npvtx); npvtx ; Efficiency", 16, 0, 80))







  # 
  # Set binning
  #
  def setBinning(self, doJpsiee=False):

    """ Method to define all binning required that is variable"""

    # Binning as defined in TP framework
    coarse_eta_bins = [-2.47,-1.52,-1.37,-0.60,0.00,0.60,1.37,1.52,2.47] # 9 items

    coarse_et_bins = [4.,20.,30.,40.,50.,60.,150.] # 7 items

    default_eta_bins = [-2.47,-2.37,-2.01,-1.81,-1.52,-1.37,-1.15,-0.80,-0.60,-0.10,0.00,
        0.10, 0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47] # 21 items

    default_et_bins = [4.,7.,10.,15.,20.,25.,30.,35.,40.,45.,50.,60.,80.,150.] # 14 items

    etabins = [-2.47,-2.37,-2.01,-1.81,-1.52,-1.37,-1.15,-0.8,-0.6,-0.1,
        0.0,0.1,0.6,0.8,1.15,1.37,1.52,1.81,2.01,2.37,2.47] # 21 items

    # TH2 with variable bin x-Axis, but constant bin y-Axis takes only Double_t arrays
    etbins_Zee = [0.,2.,4.,6.,8.,10.,
        12.,14.,16.,18.,20.,22.,24.,26.,28.,
        30.,32.,34.,36.,38.,40.,42.,44.,46.,48.,50.,55.,60.,65.,70.,100.] # 31 items

    etbins_Jpsiee = [ 0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5,
        5, 5.5, 6, 6.5, 7, 7.5, 8, 8.5, 9, 9.5,
        10,10.5,11,11.5,12,12.5,13,13.5,14,14.5,
        15,15.5,16,16.5,17,17.5,18,18.5,19,19.5,
        20,20.5,21,21.5,22,22.5,23,23.5,24,24.5,
        25,25.5] # 52 items


    if doJpsiee:
        self._nEtbins=51
        self._etbins = etbins_Jpsiee[0:self._nEtbins+1]
    else:
        self._nEtbins=30
        self._etbins = etbins_Zee[0:self._nEtbins+1]

    # Define the binning
    self._nEtabins=20
    self._ndefaultEtbins=13
    self._ndefaultEtabins=20
    self._ncoarseEtbins=6
    self._ncoarseEtabins=8

    #Fill the arrays
    self._etabins = etabins[0:self._nEtabins+1]
    self._defaultEtbins = default_et_bins[0:self._ndefaultEtbins+1]
    self._defaultEtabins = default_eta_bins[0:self._ndefaultEtabins+1]
    self._coarseEtbins = coarse_et_bins[0:self._ncoarseEtbins+1]
    self._coarseEtabins = coarse_eta_bins[0:self._ncoarseEtabins+1]



  def getTrigInfo( self, trigger ):

    class TrigEgammaInfo(object):

      def __init__(self, trigger):
        self.__chain = trigger

      def chain(self):
        return self.__chain
      
      def isL1Item(self):
        return True if self.chain().startswith('L1') else False

      def isElectron(self):
        return True if (self.isL1Item() or self.chain().startswith('HLT_e')) else False
      
      def isPhoton(self):
        return True if (self.chain().startswith('HLT_g')) else False

      def pidname(self):
        return self.chain().split('_')[2]

    return TrigEgammaInfo(trigger)

