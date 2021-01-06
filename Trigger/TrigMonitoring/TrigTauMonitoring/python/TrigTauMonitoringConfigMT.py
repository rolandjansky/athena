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

      def L1seed(self):
        l1seed = ''
        splits = self.chain().split("_")
        for split in splits:
            if split.startswith('L1TAU'):
                l1seed = split
        return l1seed

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
    'HLT_tau25_idperf_tracktwoMVABDT_L1TAU12IM',
    'HLT_tau25_perf_tracktwoMVABDT_L1TAU12IM',
    'HLT_tau25_medium1_tracktwoMVABDT_L1TAU12IM',
    'HLT_tau25_verylooseRNN_tracktwoMVABDT_L1TAU12IM',
    'HLT_tau25_looseRNN_tracktwoMVABDT_L1TAU12IM',
    'HLT_tau25_mediumRNN_tracktwoMVABDT_L1TAU12IM',
    'HLT_tau25_tightRNN_tracktwoMVABDT_L1TAU12IM',
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
    'HLT_tau160_perf_tracktwoMVABDT_L1TAU100',
    'HLT_tau160_idperf_tracktwoMVABDT_L1TAU100',
    'HLT_tau160_mediumRNN_tracktwoMVABDT_L1TAU100',
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

    l1seeds = []

    for trigger in triggers:
      info = self.getTrigInfo(trigger)
  
      l1seeds.append(info.L1seed())

      if info.isRNN() is True:
        self.bookRNNInputVars( monAlg, trigger,nProng='1P', online=True )
        self.bookRNNInputVars( monAlg, trigger,nProng='MP', online=True )
        self.bookRNNInputVars( monAlg, trigger,nProng='1P', online=False )
        self.bookRNNInputVars( monAlg, trigger,nProng='MP', online=False )
        self.bookRNNTrack( monAlg, trigger, online=True )
        self.bookRNNTrack( monAlg, trigger, online=False )
        self.bookRNNCluster( monAlg, trigger, online=True )
        self.bookRNNCluster( monAlg, trigger, online=False )
        self.bookbasicVars( monAlg, trigger, online=True )
        self.bookbasicVars( monAlg, trigger, online=False )
        self.bookHLTEffHistograms( monAlg, trigger,nProng='1P')
        self.bookHLTEffHistograms( monAlg, trigger,nProng='MP')
      else:
        self.bookBDTOut( monAlg, trigger, nProng='1P')
        self.bookBDTOut( monAlg, trigger, nProng='MP')
        self.bookBDTNoCorr( monAlg, trigger, nProng='1P')
        self.bookBDTNoCorr( monAlg, trigger, nProng='MP')

    #remove duplicated from L1 seed list
    l1seeds = list(dict.fromkeys(l1seeds))
    for l1seed in l1seeds:
        if not l1seed : 
            continue
        self.bookL1( monAlg, l1seed )
        self.bookL1EffHistograms( monAlg, l1seed, nProng='1P')
        self.bookL1EffHistograms( monAlg, l1seed, nProng='MP') 
   

  #
  # Booking HLT efficiencies
  #

  def bookHLTEffHistograms( self, monAlg, trigger, nProng ):

    monGroupName = trigger+'_HLT_Efficiency_'+nProng
    monGroupPath = 'HLT_Efficiency/'+trigger+'/HLT_Efficiency_'+ nProng

    monGroup = self.helper.addGroup( monAlg, monGroupName, 
                              self.basePath+'/'+monGroupPath )

    def defineEachStepHistograms(xvariable, xlabel, xbins, xmin, xmax):

       monGroup.defineHistogram(monGroupName+'_HLTpass,'+monGroupName+'_'+xvariable+';EffHLT_'+xvariable+'_wrt_Offline',
                                title='HLT Efficiency ' +trigger+' '+nProng+';'+xlabel+';Efficiency',
                                type='TEfficiency',xbins=xbins,xmin=xmin,xmax=xmax)

    defineEachStepHistograms('tauPt', 'p_{T} [GeV]', 60, 0.0, 300.)
    defineEachStepHistograms('tauEta','#eta', 13, -2.6, 2.6)
    defineEachStepHistograms('tauPhi','#phi', 16, -3.2, 3.2) 
    defineEachStepHistograms('averageMu', 'average pileup', 10, 0., 80.)

  #
  # Booking L1 efficiencies
  #
 
  def bookL1EffHistograms( self, monAlg, L1seed, nProng ):

    monGroupName = L1seed+'_L1_Efficiency_'+nProng
    monGroupPath = 'L1_Efficiency/'+L1seed+'/L1_Efficiency_'+ nProng

    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )

    def defineEachStepHistograms(xvariable, xlabel, xbins, xmin, xmax):

       monGroup.defineHistogram(monGroupName+'_L1pass,'+monGroupName+'_'+xvariable+';EffL1_'+xvariable+'_wrt_Offline',
                                title='L1 Efficiency ' +L1seed+' '+nProng+';'+xlabel+';Efficiency',
                                type='TEfficiency',xbins=xbins,xmin=xmin,xmax=xmax)

    defineEachStepHistograms('tauPt', 'p_{T} [GeV]', 60, 0.0, 300.)
    defineEachStepHistograms('tauEta','#eta', 13, -2.6, 2.6)
    defineEachStepHistograms('tauPhi','#phi', 16, -3.2, 3.2)
    defineEachStepHistograms('averageMu', 'average pileup', 10, 0., 80.)

  #
  # Booking L1 Variables
  #

  def bookL1( self, monAlg, trigL1Item):

    monGroupName = trigL1Item+'_L1'
    monGroupPath = 'L1/'+trigL1Item+'/L1'

    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )

    monGroup.defineHistogram('L1RoIEt;L1RoIEta', type='TH2F', title='L1 RoI Et vs Eta; E_{T}[GeV]; #eta',
                            path=monGroupPath,
                            xbins=100,xmin=0,xmax=100,
                            ybins=100,ymin=-2.6,ymax=2.6)
    monGroup.defineHistogram('L1RoIEt;L1RoIPhi', type='TH2F', title='L1 RoI Et vs Phi; E_{T}[GeV]; #phi',
                            path=monGroupPath,
                            xbins=100,xmin=0,xmax=100,
                            ybins=100,ymin=-3.2,ymax=3.2)
    monGroup.defineHistogram('L1RoIEta;L1RoIPhi', type='TH2F', title='L1 RoI Eta vs Phi; #eta; #phi',
                            path=monGroupPath,
                            xbins=100,xmin=-2.6,xmax=2.6,
                            ybins=100,ymin=-3.2,ymax=3.2)
    monGroup.defineHistogram('L1RoIEMIso', title='L1 RoI EM Isol ; E_{T}^{EM Isol}[GeV]; N RoI',xbins=16,xmin=-2,xmax=30)
    monGroup.defineHistogram('L1RoIEta', title='L1 RoI Eta ; #eta; N RoI',xbins=100,xmin=-2.6,xmax=2.6)
    monGroup.defineHistogram('L1RoIHadCore', title='L1 RoI HAD Core ; E_{T}^{HAD}[GeV]; N RoI',xbins=16,xmin=-2,xmax=30)
    monGroup.defineHistogram('L1RoIHadIsol', title='L1 RoI HAD Isol ; E_{T}^{HAD Isol}[GeV]; N RoI',xbins=16,xmin=-2,xmax=30)
    monGroup.defineHistogram('L1RoIPhi', title='L1 RoI Phi ; #phi; N RoI',xbins=100,xmin=-3.2,xmax=3.2)
    monGroup.defineHistogram('L1RoITauClus', title='L1 RoI Tau Clust Energy; E_{T}[GeV]; N RoI',xbins=260,xmin=0,xmax=130)
    monGroup.defineHistogram('L1RoITauClus;L1RoIEMIso', type='TH2F', title='L1 RoI TauClus vs EMiso ; E_{T}[GeV]; E_{T}^{EM Isol}[GeV]',
                            path=monGroupPath,
                            xbins=140,xmin=10,xmax=80,
                            ybins=42,ymin=-1,ymax=20)
    monGroup.defineHistogram('L1RoIeT', title='L1 RoI Tau Clust Energy; E_{T}[GeV]; N RoI',xbins=260,xmin=0,xmax=130)

                             
  #
  # Book RNN Variables
  #
  def bookRNNInputVars( self, monAlg, trigger,nProng, online ):

    monGroupName = trigger+'_RNN_'+('HLT' if online else 'Offline')+'_InputScalar_'+nProng
    monGroupPath = 'RNN/InputScalar_'+nProng+'/'+trigger+('/HLT' if online else '/Offline')

    monGroup = self.helper.addGroup( monAlg, monGroupName, 
                              self.basePath+'/'+monGroupPath )

    monGroup.defineHistogram('centFrac', title='Centrality Fraction ('+nProng+'); centFrac; Events',xbins=50,xmin=-0.05,xmax=1.2)
    monGroup.defineHistogram('etOverPtLeadTrk', title='etOverPtLeadTrk log ('+nProng+'); etOverPtLeadTrk_log; Events',xbins=60,xmin=-3.,xmax=3.)
    monGroup.defineHistogram('dRmax', title='max dR of associated tracks ('+nProng+'); dRmax; Events',xbins=50,xmin=-0.1,xmax=0.3)
    monGroup.defineHistogram('absipSigLeadTrk', title='AbsIpSigLeadTrk ('+nProng+'); absipSigLeadTrk; Events',xbins=25,xmin=0.0,xmax=20.0)
    monGroup.defineHistogram('sumPtTrkFrac', title='SumPtTrkFrac ('+nProng+'); SumPtTrkFrac; Events',xbins=50,xmin=-0.5,xmax=1.1)
    monGroup.defineHistogram('emPOverTrkSysP', title='EMPOverTrkSysP log ('+nProng+'); EMPOverTrkSysP_log; Events',xbins=50,xmin=-5.,xmax=3.)
    monGroup.defineHistogram('ptRatioEflowApprox', title='ptRatioEflowApprox ('+nProng+'); ptRatioEflowApprox; Events',xbins=50,xmin=0.0,xmax=2.0)
    monGroup.defineHistogram('mEflowApprox', title='mEflowApprox log ('+nProng+'); mEflowApprox_log; Events',xbins=50,xmin=0.,xmax=5.)
    monGroup.defineHistogram('ptDetectorAxis', title='ptDetectorAxis log ('+nProng+'); ptDetectorAxis_log; Events',xbins=50,xmin=0.,xmax=5.)
    if nProng=='MP':  
      monGroup.defineHistogram('massTrkSys', title='massTrkSys log ('+nProng+'); massTrkSys_log; Events',xbins=50,xmin=0.,xmax=3.)

  def bookRNNTrack( self, monAlg, trigger, online ):

    monGroupName = trigger+'_RNN_'+('HLT' if online else 'Offline')+'_InputTrack'
    monGroupPath = 'RNN/InputTrack/'+trigger+('/HLT' if online else '/Offline')

    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )

    monGroup.defineHistogram('track_pt_log',title='track_pt_log;track_pt_log;Events',xbins=20,xmin=2,xmax=7)
    monGroup.defineHistogram('track_pt_jetseed_log',title='track_pt_jetseed_log;track_pt_jetseed_log;Events',xbins=50,xmin=2,xmax=7)
    monGroup.defineHistogram('track_dEta',title='track_dEta;track_dEta;Events',xbins=100,xmin=-0.5,xmax=0.5)
    monGroup.defineHistogram('track_dPhi',title='track_dPhi;track_dPhi;Events',xbins=100,xmin=-0.5,xmax=0.5)
    monGroup.defineHistogram('track_d0_abs_log',title='track_d0_abs_log;track_d0_abs_log;Events',xbins=50,xmin=-7,xmax=2)
    monGroup.defineHistogram('track_z0sinThetaTJVA_abs_log',title='track_z0sinThetaTJVA_abs_log;track_z0sinThetaTJVA_abs_log;Events',xbins=50,xmin=-10,xmax=4)
    monGroup.defineHistogram('track_nIBLHitsAndExp',title='track_nIBLHitsAndExp; track_nIBLHitsAndExp;Events',xbins=3,xmin=0,xmax=3)
    monGroup.defineHistogram('track_nPixelHitsPlusDeadSensors',title='track_nPixelHitsPlusDeadSensors;track_nPixelHitsPlusDeadSensors;Events',xbins=11,xmin=0,xmax=11)
    monGroup.defineHistogram('track_nSCTHitsPlusDeadSensors',title='track_nSCTHitsPlusDeadSensors;track_nSCTHitsPlusDeadSensors;Events',xbins=20,xmin=0,xmax=20)
        
  def bookRNNCluster( self, monAlg, trigger, online ):

    monGroupName = trigger+'_RNN_'+('HLT' if online else 'Offline')+'_InputCluster'
    monGroupPath = 'RNN/InputCluster/'+trigger+('/HLT' if online else '/Offline')

    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )
                              
    monGroup.defineHistogram('cluster_et_log',title='cluster_et_log; cluster_et_log;Events',xbins=30,xmin=0,xmax=6)
    monGroup.defineHistogram('cluster_pt_jetseed_log',title='cluster_pt_jetseed_log; cluster_pt_jetseed_log;Events',xbins=50,xmin=2,xmax=7)
    monGroup.defineHistogram('cluster_dEta',title='cluster_dEta; cluster_dEta;Events',xbins=100,xmin=-0.5,xmax=0.5)
    monGroup.defineHistogram('cluster_dPhi',title='cluster_dPhi; cluster_dPhi;Events',xbins=100,xmin=-0.5,xmax=0.5)
    monGroup.defineHistogram('cluster_SECOND_R_log10',title='cluster_SECOND_R_log10; cluster_SECOND_R_log10;Events',xbins=50,xmin=-3,xmax=7)
    monGroup.defineHistogram('cluster_SECOND_LAMBDA_log10',title='cluster_SECOND_LAMBDA_log10; cluster_SECOND_LAMBDA_log10;Events',xbins=50,xmin=-3,xmax=7)
    monGroup.defineHistogram('cluster_CENTER_LAMBDA_log10',title='cluster_CENTER_LAMBDA_log10; cluster_CENTER_LAMBDA_log10;Events',xbins=50,xmin=-2,xmax=5)
    
  def bookbasicVars( self, monAlg, trigger, online ):
  
    monGroupName = trigger+('HLT' if online else 'Offline')+'_basicVars'
    monGroupPath = 'basicVars/'+trigger+('/HLT' if online else '/Offline')

    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )
    
    monGroup.defineHistogram('hEFEt', title='EF Et;E_{T}[GeV];Nevents',xbins=50,xmin=0,xmax=250)
    monGroup.defineHistogram('hEFEta', title='EF TrigCaloCluster Eta; #eta ; Nevents',xbins=26,xmin=-2.6,xmax=2.6)
    monGroup.defineHistogram('hEFPhi', title='EF TrigCaloCluster Phi; #phi ; Nevents',xbins=16,xmin=-3.2,xmax=3.2)
    monGroup.defineHistogram('hEFnTrack', title='EF number of tracks;number of tracks;Nevents',xbins=10,xmin=0,xmax=10)

    monGroup.defineHistogram('hEFEta;hEFPhi', type='TH2F', title='EF TrigCaloCluster Eta vs Phi; #eta ; #phi',
                               path=monGroupPath,
                               xbins=26,xmin=-2.6,xmax=2.6,ybins=16,ymin=-3.2,ymax=3.2)
    monGroup.defineHistogram('hEFEt;hEFPhi', type='TH2F', title='Et from tau Jet vs #phi; #phi^{EF}; Raw E_{T} [GeV]',
                               path=monGroupPath,
                               xbins=16,xmin=-3.2,xmax=3.2,ybins=50,ymin=0,ymax=100)
   
    monGroup.defineHistogram('hEFEta;hEFEt', type='TH2F', title='Et from tau Jet vs #eta; #eta^{EF}; Raw E_{T}[GeV]',
                               path=monGroupPath,
                               xbins=26,xmin=-2.6,xmax=2.6,ybins=50,ymin=0,ymax=100)
   
    monGroup.defineHistogram('hEFEtRaw', title='EF Et Raw;Uncalibrated E_{T}[GeV];Nevents',xbins=50,xmin=0,xmax=100)
    monGroup.defineHistogram('hEFnWideTrack', title='EF number of wide tracks;number of tracks;Nevents',xbins=10,xmin=0,xmax=10)

  #                                                                                                                                                                                                                                   
  # Book BDT Variables                                                                                                                                                                                                                
  #                                                  
  def bookBDTOut( self, monAlg, trigger, nProng):

    monGroupName = trigger+'_BDT_HLT_Out_'+nProng
    monGroupPath = 'BDT/Out_'+nProng+'/'+trigger
    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )

    print('MonGroup name is python: ', monGroupName)

    monGroup.defineHistogram('BDTJetScore', title='BDT Score ('+nProng+') ; HLT BDT Score; Candidates',xbins=50,xmin=0,xmax=1)
    monGroup.defineHistogram('BDTJetScoreSigTrans', title='Flattened BDT Score ('+nProng+') ; HLT BDT Score; Candidates',xbins=50,xmin=0,xmax=1)


  def bookBDTNoCorr( self, monAlg, trigger, nProng ):

    monGroupName = trigger+'_BDT_HLT_NoCorr_'+nProng
    monGroupPath = 'BDT/NoCorr_'+nProng+'/'+trigger
    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )

    monGroup.defineHistogram('CentFrac', title='Centrality Fraction ('+nProng+') non-corrected; centFrac; Candidates',xbins=50,xmin=-0.05,xmax=1.2)
    monGroup.defineHistogram('ChPiEMEOverCaloEME', title='ChPiEMEOverCaloEME ('+nProng+') non-corrected; ChPiEMEOverCaloEME; Candidates',xbins=51,xmin=-20,xmax=20)
    monGroup.defineHistogram('EMPOverTrkSys', title='EMPOverTrkSys ('+nProng+') non-corrected; EMPOverTrkSys; Candidates',xbins=41,xmin=0,xmax=40)
    monGroup.defineHistogram('etOverPtLeadTrk', title='etOverPtLeadTrk ('+nProng+') non-corrected; etOverPtLeadTrk; Candidates',xbins=51,xmin=-0.1,xmax=25)
    monGroup.defineHistogram('innerTrkAvgDist', title='innerTrkAvgDist ('+nProng+') non-corrected; innerTrkAvgDist; Candidates',xbins=50,xmin=-0.05,xmax=0.5)
    monGroup.defineHistogram('ptRatioEflowApprox', title='ptRatioEflowApprox ('+nProng+') non-corrected; ptRatioEflowApprox; Candidates',xbins=50,xmin=0,xmax=2)
    monGroup.defineHistogram('massTrkSys', title='massTrkSys log ('+nProng+'); massTrkSys_log; Events',xbins=50,xmin=0.,xmax=3.)

    if nProng=='1P':
      monGroup.defineHistogram('SumPtTrkFrac', title='SumPtTrkFrac ('+nProng+') non-corrected; SumPtTrkFrac; Candidates',xbins=50,xmin=-0.5,xmax=1.1)

    if nProng=='MP':
      monGroup.defineHistogram('dRmax', title='dRmax ('+nProng+') non-corrected; dRmax; Candidates',xbins=50,xmin=-0.1,xmax=0.3)
      monGroup.defineHistogram('massTrkSys', title='massTrkSys ('+nProng+') non-corrected; massTrkSys; Candidates',xbins=50,xmin=-0.1,xmax=15)
      monGroup.defineHistogram('mEflowApprox', title='mEflowApprox ('+nProng+') non-corrected; mEflowApprox; Candidates',xbins=61,xmin=-0.2,xmax=60.2)
      monGroup.defineHistogram('trFlightPathSig', title='trFlightPathSig ('+nProng+') non-corrected; trFlightPathSig; Candidates',xbins=50,xmin=-20,xmax=20)
