#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory

if 'DQMonFlags' not in dir():
    from AthenaMonitoring.DQMonFlags import DQMonFlags as dqflags

class TrigBphysMonAlgBuilder:

  _configured = False
  _get_monitoring_mode_success = False

  data_type = ''
  pp_mode = False
  hi_mode = False
  cosmic_mode = False
  mc_mode = False
  monitored_mumu_list = []
  monitored_mumux_list = []
  monitored_elel_list = []

  __acceptable_keys_list=['useMonGroups']
  useMonGroups = True
  
  basePath = 'HLT/BphysMon'


  def __init__(self, helper, **kwargs):
 
    from AthenaCommon.Logging import logging
    self.__logger = logging.getLogger( 'TrigBphysMonAlgBuilder' )
    self.helper = helper
    if not self._configured:
      for key,value in kwargs.items():
        if key in self.__acceptable_keys_list:
          setattr(self,key,value)
      self.configureMode()

  def configureMode(self):

    self.__logger.info("TrigBphysMonToolBuilder.configureMode()")
    self._get_monitoring_mode_success = self.get_monitoring_mode()
    if not self._get_monitoring_mode_success :
      self.__logger.warning("  HLTMonTriggerList: Error getting monitoring mode, default monitoring lists will be used.")
    else:
      self.__logger.info("  Configuring for %s", self.data_type)

    
  def configure(self):
    self.setProperties()
    self.configureMonitor()
    self.configureHistograms()


  def getTrigInfo( self, trigger ):

    class TrigBphysInfo(object):
      # TODO: rewrite using chainParts dictionaries

      def __init__(self, trigger):
        self.__chain = trigger

      def chain(self):
        return self.__chain

      def isL1Item(self):
        return True if self.chain().startswith('L1') else False
      
      def getTopo(self):
        if "_bBmumu_" in self.chain() or "_bDimu_" in self.chain() or "_bJpsimumu_" in self.chain() :
          return "MuMu"
        elif "_bBmumux_" in self.chain() :
          return "MuMuX"
        elif "_bBee" in self.chain() :
          return "ElEl"
        else :
          return "Unknown"
        
      def getTopoVariant(self):
        splits = self.chain().split("_")
        for i in range(len(splits)) :
          if splits[i] == "bBmumux" : 
            return splits[i+1]
        return ""
        
    return TrigBphysInfo(trigger)


  def get_monitoring_mode(self):

    self.__logger.info("TrigBphysMonToolBuilder.get_monitoring_mode()")
    self.data_type = dqflags.monManDataType()
    if self.data_type == 'monteCarlo': 
      self.mc_mode = True
      return True
    elif self.data_type == 'collisions': 
      self.pp_mode = True
      return True
    elif self.data_type == 'heavyioncollisions': 
      self.hi_mode = True
      return True
    elif self.data_type == 'cosmics':
      self.cosmic_mode = True
      return True
    else:
      return False


  def setProperties(self):

    self.__logger.info("TrigBphysMonToolBuilder.setProperties()")
    self.basePath = 'HLT/BphysMon'
   
    if self.pp_mode is True:
      self.setDefaultProperties()
    elif self.hi_mode is True:
      # This should be change in future
      self.setDefaultProperties()
    elif self.cosmic_mode is True:
      # This should be change in future
      self.setDefaultProperties()
    elif self.mc_mode is True:
      # This should be change in future
      self.setDefaultProperties()
    else:
      self.__logger.info('  No monitoring mode configured, use default')
      self.setDefaultProperties()
      


  def setDefaultProperties(self):

    ### monitorig groups
    from TrigConfigSvc.TriggerConfigAccess import getHLTMonitoringAccess
    moniAccess=getHLTMonitoringAccess(self.helper.inputFlags)
    monitoring_bphys=moniAccess.monitoredChains(signatures="bphysMon",monLevels=["shifter","t0","val"]) # other are ["shifter","t0","val"]
  
    # if mon groups not found fall back to hard-coded trigger monitoring list
    if self.useMonGroups and len(monitoring_bphys) == 0 :
      self.__logger.warning('  No chains found in bphysMon groups')
    if (not self.useMonGroups) or (len(monitoring_bphys) == 0) :
      monitoring_bphys = [
      # MuMu
      'HLT_2mu4_bDimu_L12MU3V',
      'HLT_2mu4_bJpsimumu_L12MU3V',
      # MuMuX
      'HLT_2mu4_bBmumux_BpmumuKp_L12MU3V',
      'HLT_2mu4_bBmumux_BsmumuPhi_L12MU3V',
      'HLT_2mu4_bBmumux_BdmumuKst_L12MU3V',
      # ElEl
      'HLT_2e5_lhvloose_bBeeM6000_L1BKeePrimary',
      'HLT_e5_lhvloose_bBeeM6000_L1BKeePrimary',
      ]
    
    for chain in monitoring_bphys :
      info = self.getTrigInfo(chain)
      if info.getTopo() == "MuMu" :
        self.monitored_mumu_list.append(chain)
      elif info.getTopo() == "MuMuX" :
        self.monitored_mumux_list.append(chain)
      elif info.getTopo() == "ElEl" :
        self.monitored_elel_list.append(chain)

    self.__logger.info('  Configured bphys MuMu chains: %s',self.monitored_mumu_list)
    self.__logger.info('  Configured bphys MuMuX chains: %s',self.monitored_mumux_list)
    self.__logger.info('  Configured bphys ElEl chains: %s',self.monitored_elel_list)
    
    self.monitored_containers = ['HLT_DimuEF',
                                 'HLT_Bmumux',
                                 'HLT_Bmutrk',
                                 #'HLT_DrellYan', #uncomment when the DY chains are implemented
                                 'HLT_DiElecPrecision',
                                 'HLT_NoMuonDiElecPrecision']
    
    self.__logger.info('  Configured bphys containers: %s',self.monitored_containers)

  #
  # Create all monitor algorithms
  #
  def configureMonitor( self ):
    self.__logger.info("TrigBphysMonToolBuilder.configureMonitor()")
    self.bphysMonAlg = self.helper.addAlgorithm( CompFactory.TrigBphysMonitorAlgorithm, "TrigBphysMonAlg" )
    
    # Uncomment for debugging purposes
    #from AthenaCommon.Constants import VERBOSE,DEBUG,INFO
    #self.bphysMonAlg.OutputLevel = VERBOSE
    
    ## Vertexing tools
    acc = self.helper.resobj
    
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    AtlasExtrapolator = acc.popToolsAndMerge( AtlasExtrapolatorCfg(self.helper.inputFlags) )
    acc.addPublicTool(AtlasExtrapolator)
    
    VertexPointEstimator = CompFactory.InDet.VertexPointEstimator(
                                                                  name = 'BphysMonVertexPointEstimator',
                                                                  MinDeltaR = [-10000., -10000., -10000.],
                                                                  MaxDeltaR = [ 10000.,  10000.,  10000.],
                                                                  MaxPhi    = [ 10000.,  10000.,  10000.],
                                                                  MaxChi2OfVtxEstimation = 2000.)
    acc.addPublicTool(VertexPointEstimator)
    self.bphysMonAlg.VertexPointEstimator = VertexPointEstimator
    
    VertexFitter = CompFactory.Trk.TrkVKalVrtFitter(
                                                    name = 'BphysMonTrkVKalVrtFitter',
                                                    FirstMeasuredPoint = True,
                                                    MakeExtendedVertex = True,
                                                    Extrapolator = AtlasExtrapolator)
    acc.addPublicTool(VertexFitter)
    self.bphysMonAlg.VertexFitter = VertexFitter
    
    V0Tools = CompFactory.Trk.V0Tools(name = "BphysMonV0Tools",
                                      Extrapolator = AtlasExtrapolator)
    acc.addPublicTool(V0Tools)
    self.bphysMonAlg.V0Tools = V0Tools
    
    self.bphysMonAlg.ContainerNames = self.monitored_containers
    self.bphysMonAlg.ChainNames_MuMu = self.monitored_mumu_list
    self.bphysMonAlg.ChainNames_MuMuX = self.monitored_mumux_list
    self.bphysMonAlg.ChainNames_ElEl = self.monitored_elel_list


  def configureHistograms(self):
    self.__logger.info("TrigBphysMonToolBuilder.configureHistograms()")
    self.__logger.info("Booking histograms for alg: %s", self.bphysMonAlg.name)
    self.bookContainers()
    self.bookChains()
    self.bookOfflineDimuons()
  
  
  def bookContainers(self):
    for container in self.monitored_containers :
      self.bookContainerHists(container)
  
  
  def bookContainerHists(self, container):
    monGroupName = 'Container_'+container
    monGroupPath = 'Containers/'+container

    monGroup = self.helper.addGroup( self.bphysMonAlg, monGroupName,
                                     self.basePath+'/'+monGroupPath )
    
    # number of events histograms will be shown in root Containers/ folder
    monGroup.defineHistogram('ncandidates',title='Number of Bphys candidates;number of candidates;Entries',
                             xbins=10,xmin=-0.5,xmax=9.5)
    # the others in per-container hists
    monGroup.defineHistogram('bphys_mass',title='Bphys object mass;m [GeV];Events / (0.1 GeV)',
                             xbins=150,xmin=0.0,xmax=15.0)


  def bookChains(self):
    fullChainList = self.monitored_mumu_list + self.monitored_mumux_list + self.monitored_elel_list
    for chain in fullChainList :
      monGroupName = 'Chain_'+chain
      monGroupPath = 'Chains/'+chain
      
      monGroup = self.helper.addGroup( self.bphysMonAlg, monGroupName,
                                       self.basePath+'/'+monGroupPath )
      
      self.bookChainGenericHists(chain, monGroup)
      if chain in self.monitored_mumu_list :
        self.bookBphysObjectHists(chain, monGroup, "dimu")
        self.bookLeptonHists(chain, monGroup, "mu")
      elif chain in self.monitored_mumux_list :
        self.bookBphysObjectHists(chain, monGroup, "B")
        self.bookBphysObjectHists(chain, monGroup, "dimu")
        self.bookLeptonHists(chain, monGroup, "mu")
        self.bookTrkHists(chain, monGroup)
      elif chain in self.monitored_elel_list :
        self.bookBphysObjectHists(chain, monGroup, "diel")
        self.bookLeptonHists(chain, monGroup, "el")
        
        
  def bookOfflineDimuons(self):
    fullOfflDimuList = self.monitored_mumu_list + self.monitored_mumux_list + ["Any"]
    for chain in fullOfflDimuList :
      monGroupName = 'OfflineDimu_'+chain
      monGroupPath = 'OfflineDimu/'+chain
      
      monGroup = self.helper.addGroup( self.bphysMonAlg, monGroupName,
                                       self.basePath+'/'+monGroupPath )
      
      self.bookChainGenericHists(chain, monGroup)
      self.bookBphysObjectHists(chain, monGroup, "dimu", offline=True)
      self.bookLeptonHists(chain, monGroup, "mu")
      
      
  def bookChainGenericHists(self, chain, currentMonGroup) :
    currentMonGroup.defineHistogram('ncandidates',title='Number of Bphys candidates;number of candidates;Entries',
                                xbins=10,xmin=-0.5,xmax=9.5)
      
  def bookBphysObjectHists(self, chain, currentMonGroup, objStr, offline=False):
    objTitle = ''
    objSign = ''
    if objStr == 'dimu' :
      objTitle = 'Dimuon'
      objSign = '#mu^{+}#mu^{-}'
    elif objStr == 'B' :
      objTitle = 'B'
      objSign = 'B'
    elif objStr == 'diel' :
      objTitle = 'Dielectron'
      objSign = 'e^{+}e^{-}'
    currentMonGroup.defineHistogram(objStr+'_mass',title=f'{objTitle} mass;m({objSign}) [GeV];Events / (0.1 GeV)',
                                xbins=150,xmin=0.0,xmax=15.0)
    currentMonGroup.defineHistogram(objStr+'_fitmass',title=f'{objTitle} fitted mass;m({objSign}) [GeV];Events / (0.1 GeV)',
                                xbins=150,xmin=0.0,xmax=15.0)
    currentMonGroup.defineHistogram(objStr+'_pt',title=f'{objTitle} transverse momentum;p_{{T}}({objSign}) [GeV];Events / (1 GeV)',
                                xbins=40,xmin=0.0,xmax=40.0)
    currentMonGroup.defineHistogram(objStr+'_y',title=f'{objTitle} rapidity;y({objSign}) [GeV];Events / (0.1)',
                                xbins=50,xmin=-2.5,xmax=2.5)
    currentMonGroup.defineHistogram(objStr+'_chi2',title=f'{objTitle} #chi^{{2}};#chi^{{2}}({objSign});Events / (0.5)',
                                xbins=80,xmin=0.0,xmax=40.0)
    if objStr == 'dimu' or objStr == 'diel' :
      currentMonGroup.defineHistogram(objStr+'_dR',title=f'{objTitle} dR;dR({objSign});Events / (0.01)',
                                xbins=100,xmin=0.0,xmax=1.0)
      currentMonGroup.defineHistogram(objStr+'_deta',title=f'{objTitle} d#eta;d#eta({objSign});Events / (0.01)',
                                xbins=100,xmin=0.0,xmax=1.0)
      currentMonGroup.defineHistogram(objStr+'_dphi',title=f'{objTitle} dEphi;d#phi({objSign});Events / (0.04)',
                                xbins=80,xmin=0.0,xmax=3.2)
    if offline :
      currentMonGroup.defineHistogram(objStr+'_Lxy',title=f'{objTitle} Lxy;L_{{xy}}({objSign}) [mm];Events / (0.2 mm)',
                                  xbins=125,xmin=-5.,xmax=20.0)
      currentMonGroup.defineHistogram(objStr+'_LxySig',title=f'{objTitle} Lxy significance;L_{{xy}}/#sigma(L_{{xy}})({objSign});Events / (0.5)',
                                  xbins=90,xmin=-5,xmax=40.)
    
  def bookLeptonHists(self, chain, currentMonGroup, lepStr):
    lepTitle = ''
    lepSign = ''
    if lepStr == 'mu' :
      lepTitle = 'Mu'
      lepSign = '#mu'
    elif lepStr == 'el' :
      lepTitle = 'El'
      lepSign = 'e'
    else :
      self.__logger.warning("TrigBphysMonToolBuilder.bookLeptonHists(): wrong lepStr passed: %s, while el or mu expected; won't book anything", lepStr)
      return
    currentMonGroup.defineHistogram(lepStr+'1_pt',title=f'{lepTitle}1 transverse momentum;p_{{T}}({lepSign}) [GeV];Events / (1 GeV)',
                                xbins=40,xmin=0.0,xmax=40.0)
    currentMonGroup.defineHistogram(lepStr+'1_eta',title=f'{lepTitle}1 pseudorapidity;#eta({lepSign});Events / (0.1)',
                                xbins=54,xmin=-2.7,xmax=2.7)
    currentMonGroup.defineHistogram(lepStr+'1_d0',title=f'{lepTitle}1 d0;d_{{0}}({lepSign});Events / (0.2 mm)',
                                xbins=100,xmin=-10.,xmax=10.)
    currentMonGroup.defineHistogram(lepStr+'2_pt',title=f'{lepTitle}2 transverse momentum;p_{{T}}({lepSign}) [GeV];Events / (1 GeV)',
                                xbins=40,xmin=0.0,xmax=40.0)
    currentMonGroup.defineHistogram(lepStr+'2_eta',title=f'{lepTitle}2 pseudorapidity;#eta({lepSign});Events / (0.1)',
                                xbins=54,xmin=-2.7,xmax=2.7)
    currentMonGroup.defineHistogram(lepStr+'2_d0',title=f'{lepTitle}2 d0;d_{{0}}({lepSign});Events / (0.2 mm)',
                                xbins=100,xmin=-10.,xmax=10.)
  
  def bookTrkHists(self, chain, currentMonGroup):
    currentMonGroup.defineHistogram('trk_pt',title='Track transverse momentum;p_{T}(trk) [GeV];Events / (0.5 GeV)',
                                xbins=40,xmin=0.0,xmax=20.0)
    currentMonGroup.defineHistogram('trk_eta',title='Track pseudorapidity;#eta(trk);Events / (0.1)',
                                xbins=54,xmin=-2.7,xmax=2.7)
    currentMonGroup.defineHistogram('trk_d0',title='Track d0;d_{0}(trk);Events / (0.2 mm)',
                                xbins=100,xmin=-10.,xmax=10.)

