#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

  # Add a flag to enable emulation
  __acceptable_keys_list=['monitorL1Topo']
  monitorL1Topo = False
  
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
    monitoring_bphys=moniAccess.monitoredChains(signatures="bphysMon",monLevels=["shifter"]) # other are ["shifter","t0","val"]
  
    # if mon groups not found fall back to hard-coded trigger monitoring list
    if len(monitoring_bphys) == 0:
      self.__logger.warning('  No chains found in bphysMon groups')
      monitoring_bphys = [
      # MuMu
      'HLT_2mu4_bDimu_L12MU3V',
      'HLT_2mu4_bJpsimumu_L12MU3V',
      # MuMuX
      'HLT_2mu4_bBmumux_BpmumuKp_L12MU3V',
      'HLT_2mu4_bBmumux_BsmumuPhi_L12MU3V',
      'HLT_2mu4_bBmumux_BdmumuKst_L12MU3V',
      ]
    
    for chain in monitoring_bphys :
      info = self.getTrigInfo(chain)
      if info.getTopo() == "MuMu" :
        self.monitored_mumu_list.append(chain)
      elif info.getTopo() == "MuMuX" :
        self.monitored_mumux_list.append(chain)

    self.__logger.info('  Configured bphys MuMu chains: %s',self.monitored_mumu_list)
    self.__logger.info('  Configured bphys MuMuX chains: %s',self.monitored_mumux_list)
    
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
    #from AthenaCommon.Constants import DEBUG,INFO
    #self.bphysMonAlg.OutputLevel = DEBUG
    
    self.bphysMonAlg.ContainerNames = self.monitored_containers
    self.bphysMonAlg.ChainNames_MuMu = self.monitored_mumu_list
    self.bphysMonAlg.ChainNames_MuMuX = self.monitored_mumux_list


  def configureHistograms(self):
    self.__logger.info("TrigBphysMonToolBuilder.configureHistograms()")
    self.__logger.info("Booking histograms for alg: %s", self.bphysMonAlg.name)
    self.bookContainers()
    self.bookChains()
  
  
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
    fullChainList = self.monitored_mumu_list + self.monitored_mumux_list
    for chain in fullChainList :
      monGroupName = 'Chain_'+chain
      monGroupPath = 'Chains/'+chain
      
      monGroup = self.helper.addGroup( self.bphysMonAlg, monGroupName,
                                       self.basePath+'/'+monGroupPath )
      
      self.bookChainGenericHists(chain, monGroup)
      if chain in self.monitored_mumu_list :
        self.bookBphysObjectHists(chain, monGroup, "dimu")
        self.bookMuonHists(chain, monGroup)
      elif chain in self.monitored_mumux_list :
        self.bookBphysObjectHists(chain, monGroup, "B")
        self.bookBphysObjectHists(chain, monGroup, "dimu")
        self.bookMuonHists(chain, monGroup)
        
        
  def bookChainGenericHists(self, chain, currentMonGroup) :
    currentMonGroup.defineHistogram('ncandidates',title='Number of Bphys candidates;number of candidates;Entries',
                                xbins=10,xmin=-0.5,xmax=9.5)
      
  def bookBphysObjectHists(self, chain, currentMonGroup, objStr):
    currentMonGroup.defineHistogram(objStr+'_mass',title='Dimuon mass;m(#mu^{+}#mu^{-}) [GeV];Events / (0.1 GeV)',
                                xbins=150,xmin=0.0,xmax=15.0)
    currentMonGroup.defineHistogram(objStr+'_pt',title='Dimuon transverse momentum;p_{T}(#mu^{+}#mu^{-}) [GeV];Events / (1 GeV)',
                                xbins=40,xmin=0.0,xmax=40.0)
    currentMonGroup.defineHistogram(objStr+'_y',title='Dimuon rapidity;y(#mu^{+}#mu^{-}) [GeV];Events / (0.1)',
                                xbins=50,xmin=-2.5,xmax=2.5)
    currentMonGroup.defineHistogram(objStr+'_chi2',title='Dimuon #chi^{2};#chi^{2}(#mu^{+}#mu^{-});Events / (0.5)',
                                xbins=80,xmin=0.0,xmax=40.0)
    
  def bookMuonHists(self, chain, currentMonGroup):
    currentMonGroup.defineHistogram('mu1_pt',title='Mu1 transverse momentum;p_{T}(#mu) [GeV];Events / (1 GeV)',
                                xbins=40,xmin=0.0,xmax=40.0)
    currentMonGroup.defineHistogram('mu1_eta',title='Mu1 pseudorapidity;#eta(#mu);Events / (0.1)',
                                xbins=54,xmin=-2.7,xmax=2.7)
    currentMonGroup.defineHistogram('mu1_d0',title='Mu1 d0;d_{0}(#mu);Events / (0.2 mm)',
                                xbins=100,xmin=-10.,xmax=10.)
    currentMonGroup.defineHistogram('mu2_pt',title='Mu2 transverse momentum;p_{T}(#mu) [GeV];Events / (1 GeV)',
                                xbins=40,xmin=0.0,xmax=40.0)
    currentMonGroup.defineHistogram('mu2_eta',title='Mu2 pseudorapidity;#eta(#mu);Events / (0.1)',
                                xbins=54,xmin=-2.7,xmax=2.7)
    currentMonGroup.defineHistogram('mu2_d0',title='Mu2 d0;d_{0}(#mu);Events / (0.2 mm)',
                                xbins=100,xmin=-10.,xmax=10.)

