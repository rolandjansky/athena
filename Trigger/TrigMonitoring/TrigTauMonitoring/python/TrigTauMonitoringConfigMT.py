# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

if 'DQMonFlags' not in dir():
    from AthenaMonitoring.DQMonFlags import DQMonFlags as dqflags

class TrigTauMonAlgBuilder:

  _configured = False
  _get_monitoring_mode_success = False

  data_type = ''
  pp_mode = False
  cosmic_mode = False
  mc_mode = False
  activate_tau = False
  tauList = []

  # Add a flag to enable emulation
  __acceptable_keys_list=['derivation','emulation','detailedHistograms','basePath']
  emulation = False
  derivation = False
  detailedHistograms = False
  basePath = 'HLT/TauMon'


  def __init__(self, helper, runflag, **kwargs):
 
    from AthenaCommon.Logging import logging
    self.__logger = logging.getLogger( 'TrigTauMonAlgBuilder' )
    self.runFlag = runflag
    self.helper = helper
    if not self._configured:
      for key,value in kwargs.items():
        if key in self.__acceptable_keys_list:
          setattr(self,key,value)
      self.configureMode()

  def configureMode(self):

    self.__logger.info("TrigTauMonToolBuilder.configureMode()")
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
        self.activate_tau = True
      else:
        self.activate_tau=True
    else:
        self.activate_tau=True
    """    self.activate_jpsiee=True 
        self.activate_electron=True
        self.activate_photon=True
    elif self.HI_mode is True or self.pPb_mode is True or self.cosmic_mode is True:
      self.activate_electron=True
      self.activate_photon=True
    else:
      self.activate_zee=True
      self.activate_jpsiee=True
      self.activate_electron=True
      self.activate_photon=True"""

    
  def configure(self):
    self.setProperties()
    self.configureMonitor()
    self.configureHistograms()


  def getTrigInfo( self, trigger ):

    class TrigTauInfo(object):

      def __init__(self, trigger):
        self.__chain = trigger

      def chain(self):
        return self.__chain

      def isL1Item(self):
        return True if self.chain().startswith('L1') else False

      def isRNN(self):
        return True if "RNN" in self.chain() else False

    return TrigTauInfo(trigger)


  def get_monitoring_mode(self):

    self.__logger.info("TrigTauMonToolBuilder.get_monitoring_mode()")
    self.data_type = dqflags.monManDataType()
    if self.data_type == 'monteCarlo': 
      self.mc_mode = True
      return True
    elif self.data_type == 'collisions': 
      self.pp_mode = True
      return True
    elif self.data_type == 'cosmics':
      self.cosmic_mode = True
      return True
    else:
      return False


  def setProperties(self):

    self.__logger.info("TrigTauMonToolBuilder.setProperties()")
    self.basePath = 'HLT/TauMon'
   
    if self.pp_mode is True:
      self.setDefaultProperties()
    elif self.cosmic_mode is True:
      # This should be change in future
      self.setDefaultProperties()
    elif self.mc_mode is True:
      # This should be change in future
      self.setDefaultProperties()
    else:
      self.__logger.info('No monitoring mode configured, use default')
      self.setDefaultProperties()


    self.__logger.info('Configuring tau chains %s',self.tauList)


  def setDefaultProperties(self):
    
    # This will be removed for future.
    monitoring_tau = [
    'HLT_tau25_mediumRNN_tracktwoMVA_L1TAU12IM',
    'HLT_tau80_medium1_tracktwo_L1TAU60',
    'HLT_tau0_perf_ptonly_L1TAU100',
    'HLT_tau160_idperf_track_L1TAU100',
    'HLT_tau160_idperf_tracktwo_L1TAU100',
    'HLT_tau160_perf_tracktwo_L1TAU100',
    'HLT_tau160_idperf_tracktwoMVA_L1TAU100',
    'HLT_tau160_perf_tracktwoMVA_L1TAU100',
    'HLT_tau160_mediumRNN_tracktwoMVA_L1TAU100',
    'HLT_tau160_medium1_tracktwo_L1TAU100',
    'HLT_tau160_medium1_tracktwoEF_L1TAU100',
    'HLT_tau200_medium1_tracktwoEF_L1TAU100',
    'HLT_tau200_mediumRNN_tracktwoMVA_L1TAU100',
    'HLT_tau80_mediumRNN_tracktwoMVA_tau60_mediumRNN_tracktwoMVA_L1TAU60_2TAU40',
    'HLT_tau80_mediumRNN_tracktwoMVA_tau35_mediumRNN_tracktwoMVA_L1TAU60_DR-TAU20ITAU12I',
    'HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1DR-TAU20ITAU12I-J25'
    ]

    self.tauList = monitoring_tau

  #
  # Create all minitor algorithms
  #
  def configureMonitor( self ):

    if self.activate_tau:

      self.__logger.info( "Creating the Tau monitor algorithm...")
      self.tauMonAlg = self.helper.addAlgorithm( CompFactory.TrigTauMonitorAlgorithm, "TrigTauMonAlg" )
      self.tauMonAlg.TriggerList=self.tauList    



  def configureHistograms(self):

    if self.activate_tau and self.tauMonAlg:
      self.bookExpertHistograms( self.tauMonAlg, self.tauMonAlg.TriggerList )

  #
  # Booking all histograms
  #
  def bookExpertHistograms( self, monAlg, triggers ):

    self.__logger.info( "Booking all histograms for alg: %s", monAlg.name )

    for trigger in triggers:
      info = self.getTrigInfo(trigger)

      if info.isRNN() is True:
        self.bookRNNInputVars( monAlg, trigger,nProng='1P', online=True )
        self.bookRNNInputVars( monAlg, trigger,nProng='MP', online=True )
        self.bookRNNInputVars( monAlg, trigger,nProng='1P', online=False )
        self.bookRNNInputVars( monAlg, trigger,nProng='MP', online=False )
        self.bookRNNTrack( monAlg, trigger, online=True )
        self.bookRNNTrack( monAlg, trigger, online=False )
        self.bookRNNCluster( monAlg, trigger, online=True )
        self.bookRNNCluster( monAlg, trigger, online=False )
  #
  # Book RNN Variables
  #
  def bookRNNInputVars( self, monAlg, trigger,nProng, online ):

    monGroupName = trigger+'/RNN/'+('HLT' if online else 'Offline')+'/InputScalar_'+nProng

    monGroup = self.helper.addGroup( monAlg, monGroupName, 
                              self.basePath+'/'+monGroupName )

    monGroup.defineHistogram('centFrac', title='Centrality Fraction ('+nProng+'); centFrac; Events',xbins=50,xmin=-0.05,xmax=1.2)
    monGroup.defineHistogram('etOverPtLeadTrk', title='etOverPtLeadTrk log ('+nProng+'); etOverPtLeadTrk_log; Events',xbins=60,xmin=-3.,xmax=3.)
    monGroup.defineHistogram('dRmax', title='max dR of associated tracks ('+nProng+'); dRmax; Events',xbins=50,xmin=-0.1,xmax=0.3)
    monGroup.defineHistogram('absipSigLeadTrk', title='AbsIpSigLeadTrk ('+nProng+'); absipSigLeadTrk; Events',xbins=25,xmin=0.0,xmax=20.0)
    monGroup.defineHistogram('sumPtTrkFrac', title='SumPtTrkFrac ('+nProng+'); SumPtTrkFrac; Events',xbins=50,xmin=-0.5,xmax=1.1)
    monGroup.defineHistogram('emPOverTrkSysP', title='EMPOverTrkSysP log ('+nProng+'); EMPOverTrkSysP_log; Events',xbins=50,xmin=-5.,xmax=3.)
    monGroup.defineHistogram('ptRatioEflowApprox', title='ptRatioEflowApprox ('+nProng+'); ptRatioEflowApprox; Events',xbins=50,xmin=0.0,xmax=2.0)
    monGroup.defineHistogram('mEflowApprox', title='mEflowApprox log ('+nProng+'); mEflowApprox_log; Events',xbins=50,xmin=0.,xmax=5.)
    monGroup.defineHistogram('ptDetectorAxis', title='ptDetectorAxis log ('+nProng+'); ptDetectorAxis_log; Events',xbins=50,xmin=0.,xmax=5.)
    if nProng=='MP':  monGroup.defineHistogram('massTrkSys', title='massTrkSys log ('+nProng+'); massTrkSys_log; Events',xbins=50,xmin=0.,xmax=3.)

  def bookRNNTrack( self, monAlg, trigger, online ):

    monGroupName = trigger+'/RNN/'+('HLT' if online else 'Offline')+'/InputTrack'

    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupName )

    monGroup.defineHistogram('track_pt_log',title='track_pt_log;track_pt_log;Events',xbins=20,xmin=2,xmax=7)
    monGroup.defineHistogram('track_pt_jetseed_log',title='track_pt_jetseed_log;track_pt_jetseed_log;Events',xbins=50,xmin=2,xmax=7)
    monGroup.defineHistogram('track_dEta',title='track_dEta;track_dEta;Events',xbins=100,xmin=-0.5,xmax=0.5)
    monGroup.defineHistogram('track_dPhi',title='track_dPhi;track_dPhi;Events',xbins=100,xmin=-0.5,xmax=0.5)
    monGroup.defineHistogram('track_d0_abs_log',title='track_d0_abs_log;track_d0_abs_log;Events',xbins=50,xmin=-7,xmax=2)
    monGroup.defineHistogram('track_z0sinThetaTJVA_abs_log',title='track_z0sinThetaTJVA_abs_log;track_z0sinThetaTJVA_abs_log;Events',xbins=50,xmin=-10,xmax=4)
    monGroup.defineHistogram('track_nIBLHitsAndExp',title='track_nIBLHitsAndExp; track_nIBLHitsAndExp;Events',xbins=3,xmin=0,xmax=3)
    monGroup.defineHistogram('track_nPixelHitsPlusDeadSensors',title='track_nPixelHitsPlusDeadSensors;track_nPixelHitsPlusDeadSensors;Events',xbins=11,xmin=0,xmax=11)
    monGroup.defineHistogram('track_nSCTHitsPlusDeadSensors',title='track_nSCTHitsPlusDeadSensors;track_nSCTHitsPlusDeadSensors;Events',xbins=20,xmin=0,xmax=20)

  def bookRNNCluster( self, monAlg, trigger, online ):

    monGroupName = trigger+'/RNN/'+('HLT' if online else 'Offline')+'/InputCluster'

    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupName )
                              
    monGroup.defineHistogram('cluster_et_log',title='cluster_et_log; cluster_et_log;Events',xbins=30,xmin=0,xmax=6)
    monGroup.defineHistogram('cluster_pt_jetseed_log',title='cluster_pt_jetseed_log; cluster_pt_jetseed_log;Events',xbins=50,xmin=2,xmax=7)
    monGroup.defineHistogram('cluster_dEta',title='cluster_dEta; cluster_dEta;Events',xbins=100,xmin=-0.5,xmax=0.5)
    monGroup.defineHistogram('cluster_dPhi',title='cluster_dPhi; cluster_dPhi;Events',xbins=100,xmin=-0.5,xmax=0.5)
    monGroup.defineHistogram('cluster_SECOND_R_log10',title='cluster_SECOND_R_log10; cluster_SECOND_R_log10;Events',xbins=50,xmin=-3,xmax=7)
    monGroup.defineHistogram('cluster_SECOND_LAMBDA_log10',title='cluster_SECOND_LAMBDA_log10; cluster_SECOND_LAMBDA_log10;Events',xbins=50,xmin=-3,xmax=7)
    monGroup.defineHistogram('cluster_CENTER_LAMBDA_log10',title='cluster_CENTER_LAMBDA_log10; cluster_CENTER_LAMBDA_log10;Events',xbins=50,xmin=-2,xmax=5)

