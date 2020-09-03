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
        self.bookRNNInputVars( monAlg, trigger, online=True )
        self.bookRNNInputVars( monAlg, trigger, online=False )


  #
  # Book Shower shapes
  #
  def bookRNNInputVars( self, monAlg, trigger, online=True ):

    monGroup = self.helper.addGroup( monAlg, trigger+'_RNNInputVars_' + ("HLT" if online else "Offline"), 
                              self.basePath+'/'+trigger+'/RNNInputVars/' + ("HLT" if online else "Offline") )

    monGroup.defineHistogram('centFrac', title='Centrality Fraction (1Prong); centFrac; Events',xbins=50,xmin=-0.05,xmax=1.2)
    monGroup.defineHistogram('etOverPtLeadTrk', title='etOverPtLeadTrk log (1Prong); etOverPtLeadTrk_log; Events',xbins=60,xmin=-3.,xmax=3.)
    monGroup.defineHistogram('dRmax', title='max dR of associated tracks (1Prong); dRmax; Events',xbins=50,xmin=-0.1,xmax=0.3)
    monGroup.defineHistogram('absipSigLeadTrk', title='AbsIpSigLeadTrk (1Prong); absipSigLeadTrk; Events',xbins=25,xmin=0.0,xmax=20.0)
    monGroup.defineHistogram('sumPtTrkFrac', title='SumPtTrkFrac (1Prong); SumPtTrkFrac; Events',xbins=50,xmin=-0.5,xmax=1.1)
    monGroup.defineHistogram('emPOverTrkSysP', title='EMPOverTrkSysP log (1Prong); EMPOverTrkSysP_log; Events',xbins=50,xmin=-5.,xmax=3.)
    monGroup.defineHistogram('ptRatioEflowApprox', title='ptRatioEflowApprox (1Prong); ptRatioEflowApprox; Events',xbins=50,xmin=0.0,xmax=2.0)
    monGroup.defineHistogram('mEflowApprox', title='mEflowApprox log (1Prong); mEflowApprox_log; Events',xbins=50,xmin=0.,xmax=5.)
    monGroup.defineHistogram('ptDetectorAxis', title='ptDetectorAxis log (1Prong); ptDetectorAxis_log; Events',xbins=50,xmin=0.,xmax=5.)
