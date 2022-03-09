# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

class TrigTauMonAlgBuilder:

  _configured = False
  _get_monitoring_mode_success = False

  data_type = ''
  pp_mode = False
  cosmic_mode = False
  mc_mode = False
  activate_tau = False
  tauList = []

  basePath = 'HLT/TauMon'

  def __init__(self, helper):
 
    from AthenaCommon.Logging import logging
    self.__logger = logging.getLogger( 'TrigTauMonAlgBuilder' )
    self.helper = helper
    if not self._configured:
      self.configureMode()
  
  def configureMode(self):

    self.__logger.info("TrigTauMonToolBuilder.configureMode()")
    self._get_monitoring_mode_success = self.get_monitoring_mode()
    if self._get_monitoring_mode_success is False:
      self.__logger.warning("HLTMonTriggerList: Error getting monitoring mode, default monitoring lists will be used.")
    else:
      self.__logger.info("Configuring for %s", self.data_type)

    self.activate_tau=True 

    
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

      def isComboChain(self):
        isComboChain = False
        splits = self.chain().split("_")
        Npart = 0
        for split in splits:
            if split.startswith('tau'):
                Npart+=1
        if Npart > 1:
            isComboChain = True
        return isComboChain        

      # CAUTION: Works only for no combined chains
      def L1thresh(self):
        thresh = []
        splits = self.chain().split("_")
        for split in splits:
            if split.startswith('L1'):
                for word in split:
                   if word.isdigit():
                      thresh.append(int(word))
        return thresh

      def L1seed(self):
        l1seed = ''
        splits = self.chain().split("_")
        for split in splits:
            if split.startswith('L1TAU') or split.startswith('L1eTAU') or split.startswith('L1cTAU'):
                l1seed = split
        return l1seed

      def isDiTau(self):
        return True if ( self.chain().count('tau') == 2 and '03dRAB' in self.chain()) else False

      def isTAndP(self):
        return True if ( ("ivarloose" in self.chain() or "ivarmedium" in self.chain()) and '03dRAB' in self.chain()) else False

    return TrigTauInfo(trigger)


  def get_monitoring_mode(self):

    self.__logger.info("TrigTauMonToolBuilder.get_monitoring_mode()")
    self.data_type = self.helper.inputFlags.DQ.DataType
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

    ### monitorig groups
    from TrigConfigSvc.TriggerConfigAccess import getHLTMonitoringAccess
    moniAccess=getHLTMonitoringAccess(self.helper.inputFlags)
    monitoring_tau=moniAccess.monitoredChains(signatures="tauMon",monLevels=["shifter","t0","val"])
  
    # if mon groups not found fall back to hard-coded trigger monitoring list
    if len(monitoring_tau) == 0:
      monitoring_tau = [
       # tau0
       'HLT_tau0_ptonly_L1TAU8',
       'HLT_tau0_ptonly_L1TAU60',
       # tau25
       'HLT_tau25_idperf_tracktwoMVA_L1TAU12IM',
       'HLT_tau25_idperf_tracktwoMVABDT_L1TAU12IM',
       'HLT_tau25_perf_tracktwoMVA_L1TAU12IM',
       'HLT_tau25_perf_tracktwoMVABDT_L1TAU12IM',
       'HLT_tau25_looseRNN_tracktwoMVA_L1TAU12IM',
       'HLT_tau25_looseRNN_tracktwoMVABDT_L1TAU12IM',
       'HLT_tau25_mediumRNN_tracktwoMVA_L1TAU12IM',
       'HLT_tau25_mediumRNN_tracktwoMVABDT_L1TAU12IM',
       'HLT_tau25_tightRNN_tracktwoMVA_L1TAU12IM',
       'HLT_tau25_tightRNN_tracktwoMVABDT_L1TAU12IM',
       # tau35
       'HLT_tau35_idperf_tracktwoMVA_L1TAU20IM',
       'HLT_tau35_idperf_tracktwoMVABDT_L1TAU20IM',
       'HLT_tau35_perf_tracktwoMVA_L1TAU20IM',
       'HLT_tau35_perf_tracktwoMVABDT_L1TAU20IM',
       'HLT_tau35_looseRNN_tracktwoMVA_L1TAU20IM',
       'HLT_tau35_looseRNN_tracktwoMVABDT_L1TAU20IM',
       'HLT_tau35_mediumRNN_tracktwoMVA_L1TAU20IM',
       'HLT_tau35_mediumRNN_tracktwoMVABDT_L1TAU20IM',
       'HLT_tau35_tightRNN_tracktwoMVA_L1TAU20IM',
       'HLT_tau35_tightRNN_tracktwoMVABDT_L1TAU20IM',
       # tau60
       'HLT_tau60_mediumRNN_tracktwoMVABDT_L1TAU40',
       # tau80
       'HLT_tau80_mediumRNN_tracktwoMVABDT_L1TAU60', 
       # tau160
       'HLT_tau160_ptonly_L1TAU100',
       'HLT_tau160_idperf_tracktwoMVA_L1TAU100',
       'HLT_tau160_idperf_tracktwoMVABDT_L1TAU100',
       'HLT_tau160_perf_tracktwoMVA_L1TAU100',
       'HLT_tau160_perf_tracktwoMVABDT_L1TAU100',
       'HLT_tau160_mediumRNN_tracktwoMVA_L1TAU100',
       'HLT_tau160_mediumRNN_tracktwoMVABDT_L1TAU100',
       # tau180
       'HLT_tau180_mediumRNN_tracktwoLLP_L1TAU100',
       'HLT_tau180_tightRNN_tracktwoLLP_L1TAU100',
       # tau200
       'HLT_tau200_mediumRNN_tracktwoMVA_L1TAU100',
       'HLT_tau200_mediumRNN_tracktwoMVABDT_L1TAU100',
       'HLT_tau200_mediumRNN_tracktwoLLP_L1TAU100',
       'HLT_tau200_tightRNN_tracktwoLLP_L1TAU100',
       # ditau
       'HLT_tau80_mediumRNN_tracktwoMVA_tau60_mediumRNN_tracktwoMVA_03dRAB_L1TAU60_2TAU40',
       'HLT_tau80_mediumRNN_tracktwoMVA_tau35_mediumRNN_tracktwoMVA_03dRAB30_L1TAU60_DR-TAU20ITAU12I',
       'HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dRAB30_L1DR-TAU20ITAU12I-J25',
       'HLT_tau80_mediumRNN_tracktwoMVABDT_tau60_mediumRNN_tracktwoMVABDT_03dRAB_L1TAU60_2TAU40',
       'HLT_tau80_mediumRNN_tracktwoMVABDT_tau35_mediumRNN_tracktwoMVABDT_03dRAB30_L1TAU60_DR-TAU20ITAU12I',
       'HLT_tau35_mediumRNN_tracktwoMVABDT_tau25_mediumRNN_tracktwoMVABDT_03dRAB30_L1DR-TAU20ITAU12I-J25',
       'HLT_tau80_mediumRNN_tracktwoLLP_tau60_mediumRNN_tracktwoLLP_03dRAB_L1TAU60_2TAU40',
       'HLT_tau80_mediumRNN_tracktwoLLP_tau60_tightRNN_tracktwoLLP_03dRAB_L1TAU60_2TAU40',
       'HLT_tau80_tightRNN_tracktwoLLP_tau60_tightRNN_tracktwoLLP_03dRAB_L1TAU60_2TAU40',
       'HLT_tau100_mediumRNN_tracktwoLLP_tau80_mediumRNN_tracktwoLLP_03dRAB_L1TAU60_2TAU40',
       'HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dRAB_L1TAU20IM_2TAU12IM_4J12p0ETA25',
       'HLT_tau40_mediumRNN_tracktwoMVA_tau35_mediumRNN_tracktwoMVA_03dRAB_L1TAU25IM_2TAU20IM_2J25_3J20',
       'HLT_tau35_mediumRNN_tracktwoMVABDT_tau25_mediumRNN_tracktwoMVABDT_03dRAB_L1TAU20IM_2TAU12IM_4J12p0ETA25',
       'HLT_tau40_mediumRNN_tracktwoMVABDT_tau35_mediumRNN_tracktwoMVABDT_03dRAB_L1TAU25IM_2TAU20IM_2J25_3J20',
       # Tag and probe
       'HLT_e26_lhtight_ivarloose_tau20_mediumRNN_tracktwoMVA_03dRAB_L1EM22VHI',
       'HLT_mu26_ivarmedium_tau20_mediumRNN_tracktwoMVA_03dRAB_L1MU14FCH',
       # Phase-I
       'HLT_tau25_idperf_tracktwoMVABDT_L1eTAU20',
       'HLT_tau25_perf_tracktwoMVABDT_L1eTAU20', 
       'HLT_tau25_mediumRNN_tracktwoMVABDT_L1eTAU20',
       'HLT_tau25_idperf_tracktwoMVABDT_L1eTAU20M',
       'HLT_tau25_perf_tracktwoMVABDT_L1eTAU20M',
       'HLT_tau25_mediumRNN_tracktwoMVABDT_L1eTAU20M',     
       'HLT_tau35_mediumRNN_tracktwoMVABDT_L1eTAU30', 
       'HLT_tau160_mediumRNN_tracktwoMVABDT_L1eTAU140',
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
      isMC = False
      self.data_type = self.helper.inputFlags.DQ.DataType
      if self.data_type == 'monteCarlo':
         isMC = True
      self.tauMonAlg.isMC = isMC


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

      self.bookbasicVars( monAlg, trigger, '0P', online=True )
      self.bookbasicVars( monAlg, trigger, '1P', online=True )
      self.bookbasicVars( monAlg, trigger, 'MP', online=True )
      self.bookbasicVars( monAlg, trigger, '1P', online=False )
      self.bookbasicVars( monAlg, trigger, '3P', online=False )

      self.bookHLTEffHistograms( monAlg, trigger,nProng='1P')
      self.bookHLTEffHistograms( monAlg, trigger,nProng='3P')

      self.bookTruth( monAlg, trigger, nProng='1P')
      self.bookTruth( monAlg, trigger, nProng='3P')
      self.bookTruthEfficiency( monAlg, trigger, nProng='1P')
      self.bookTruthEfficiency( monAlg, trigger, nProng='3P')

      self.bookRNNInputVars( monAlg, trigger,nProng='0P', online=True ) 
      self.bookRNNInputVars( monAlg, trigger,nProng='1P', online=True )
      self.bookRNNInputVars( monAlg, trigger,nProng='MP', online=True )
      self.bookRNNInputVars( monAlg, trigger,nProng='1P', online=False )
      self.bookRNNInputVars( monAlg, trigger,nProng='3P', online=False )
      self.bookRNNTrack( monAlg, trigger, online=True )
      self.bookRNNCluster( monAlg, trigger, online=True )
      self.bookRNNTrack( monAlg, trigger, online=False )
      self.bookRNNCluster( monAlg, trigger, online=False )

      if(info.isDiTau()):
        self.bookDiTauVars(monAlg, trigger)   
        self.bookDiTauHLTEffHistograms(monAlg, trigger)

      if(info.isTAndP()):
        self.bookTAndPVars(monAlg, trigger)
        self.bookTAndPHLTEffHistograms(monAlg, trigger)

    #remove duplicated from L1 seed list
    l1seeds = list(dict.fromkeys(l1seeds))
    for l1seed in l1seeds:
        if not l1seed : 
            continue
        self.bookL1( monAlg, l1seed)
        self.bookL1EffHistograms( monAlg, l1seed, nProng='1P')
        self.bookL1EffHistograms( monAlg, l1seed, nProng='3P') 
           

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
  # Booking DiTau efficiencies
  #

  def bookDiTauHLTEffHistograms(self, monAlg, trigger):
  
    monGroupName = trigger+'_DiTauHLT_Efficiency'
    monGroupPath = 'DiTauHLT_Efficiency/'+trigger+'/DiTauHLT_Efficiency'

    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )

    def defineEachStepHistograms(xvariable, xlabel, xbins, xmin, xmax):

       monGroup.defineHistogram(monGroupName+'_DiTauHLTpass,'+monGroupName+'_'+xvariable+';EffDiTauHLT_'+xvariable+'_wrt_Offline',
                                title='DiTau HLT Efficiency ' +trigger+';'+xlabel+';Efficiency',
                                type='TEfficiency',xbins=xbins,xmin=xmin,xmax=xmax)

    defineEachStepHistograms('dR',' dR(#tau,#tau)',20,0,4)
    defineEachStepHistograms('dEta',' dEta(#tau,#tau)',20,0,4)
    defineEachStepHistograms('dPhi',' dPhi(#tau,#tau)',8, -3.2, 3.2)
    defineEachStepHistograms('averageMu', 'average pileup', 10, 0., 80.)

  #
  # Booking TAndP efficiencies
  #

  def bookTAndPHLTEffHistograms(self, monAlg, trigger):
  
    monGroupName = trigger+'_TAndPHLT_Efficiency'
    monGroupPath = 'TAndPHLT_Efficiency/'+trigger+'/TAndPHLT_Efficiency'

    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )

    def defineEachStepHistograms(xvariable, xlabel, xbins, xmin, xmax):

       monGroup.defineHistogram(monGroupName+'_TAndPHLTpass,'+monGroupName+'_'+xvariable+';EffTAndPHLT_'+xvariable+'_wrt_Offline',
                                title='TAndP HLT Efficiency ' +trigger+';'+xlabel+';Efficiency',
                                type='TEfficiency',xbins=xbins,xmin=xmin,xmax=xmax)

    defineEachStepHistograms('tauPt', 'p_{T} [GeV]', 60, 0.0, 300.)
    defineEachStepHistograms('dR',' dR(#tau,lep)',20,0,4)
    defineEachStepHistograms('dEta',' dEta(#tau,lep)',20,0,4)
    defineEachStepHistograms('dPhi',' dPhi(#tau,lep)',8, -3.2, 3.2)
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
    monGroupPath = 'L1/'+trigL1Item

    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )

    monGroup.defineHistogram('L1RoIEt,L1RoIEta', type='TH2F', title='L1 RoI Et vs Eta; E_{T}[GeV]; #eta',
                            xbins=100,xmin=0,xmax=100,
                            ybins=100,ymin=-2.6,ymax=2.6)
    monGroup.defineHistogram('L1RoIEt,L1RoIPhi', type='TH2F', title='L1 RoI Et vs Phi; E_{T}[GeV]; #phi',
                            xbins=100,xmin=0,xmax=100,
                            ybins=100,ymin=-3.2,ymax=3.2)
    monGroup.defineHistogram('L1RoIEta,L1RoIPhi', type='TH2F', title='L1 RoI Eta vs Phi; #eta; #phi',
                            xbins=100,xmin=-2.6,xmax=2.6,
                            ybins=100,ymin=-3.2,ymax=3.2)
    monGroup.defineHistogram('L1RoIEMIsol', title='L1 RoI EM Isol; E_{T}^{EM Isol}[GeV]; N RoI',xbins=16,xmin=-2,xmax=30)
    monGroup.defineHistogram('L1RoIEta', title='L1 RoI Eta; #eta; N RoI',xbins=100,xmin=-2.6,xmax=2.6)
    monGroup.defineHistogram('L1RoIHadCore', title='L1 RoI HAD Core; E_{T}^{HAD}[GeV]; N RoI',xbins=16,xmin=-2,xmax=30)
    monGroup.defineHistogram('L1RoIHadIsol', title='L1 RoI HAD Isol; E_{T}^{HAD Isol}[GeV]; N RoI',xbins=16,xmin=-2,xmax=30)
    monGroup.defineHistogram('L1RoIPhi', title='L1 RoI Phi; #phi; N RoI',xbins=100,xmin=-3.2,xmax=3.2)
    monGroup.defineHistogram('L1RoITauClus', title='L1 RoI Tau Clust Energy; E_{T}[GeV]; N RoI',xbins=260,xmin=0,xmax=130)
    monGroup.defineHistogram('L1RoITauClus,L1RoIEMIsol', type='TH2F', title='L1 RoI TauClus vs EMiso; E_{T}[GeV]; E_{T}^{EM Isol}[GeV]',
                            xbins=140,xmin=10,xmax=80,
                            ybins=42,ymin=-1,ymax=20)
    monGroup.defineHistogram('L1RoIEt', title='L1 RoI Tau Clust Energy; E_{T}[GeV]; N RoI',xbins=30,xmin=0,xmax=150)
    monGroup.defineHistogram('L1RoIRCore', title='L1 RoI RCore isolation; rCore isolation; N RoI',xbins=20,xmin=0,xmax=2)
    monGroup.defineHistogram('L1RoIRHad' , title='L1 RoI RHAD isolation; rHad isolation; N RoI'  ,xbins=20,xmin=0,xmax=2)

                             
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
    if nProng=='MP' or nProng=='3P':  
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

  def bookbasicVars( self, monAlg, trigger, nProng, online ):
  
    monGroupName = trigger+('HLT' if online else 'Offline')+'_basicVars_'+nProng
    monGroupPath = 'basicVars/'+trigger+('/HLT' if online else '/Offline')+'_'+nProng

    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )

    info = self.getTrigInfo(trigger) 
    l1th = []    
    if(not info.isComboChain()):
      l1th = info.L1thresh()

    thresh=''
    for i in range(1,len(l1th)):
       thresh=thresh+str(l1th[i])

    etmin=0.
    etmax=250.
    
    if len(l1th) !=  0:
       etmin=int(thresh)
       etmax=5*int(thresh)

    monGroup.defineHistogram('hEFEt', title='EF Et;E_{T}[GeV];Nevents',xbins=50,xmin=etmin,xmax=etmax)
    monGroup.defineHistogram('hEFEta', title='EF TrigCaloCluster Eta; #eta ; Nevents',xbins=26,xmin=-2.6,xmax=2.6)
    monGroup.defineHistogram('hEFPhi', title='EF TrigCaloCluster Phi; #phi ; Nevents',xbins=16,xmin=-3.2,xmax=3.2)
    monGroup.defineHistogram('hEFnTrack', title='EF number of tracks;number of tracks;Nevents',xbins=10,xmin=0,xmax=10)

    monGroup.defineHistogram('hEFEta,hEFPhi', type='TH2F', title='Eta vs Phi; #eta ; #phi',
                               xbins=26,xmin=-2.6,xmax=2.6,ybins=16,ymin=-3.2,ymax=3.2)
    monGroup.defineHistogram('hEFEt,hEFPhi', type='TH2F',  title='Et vs Phi; E_{T} [GeV]; #phi',
                               xbins=50,xmin=etmin,xmax=etmax,ybins=16,ymin=-3.2,ymax=3.2) 
    monGroup.defineHistogram('hEFEt,hEFEta', type='TH2F',  title='Et vs Eta; E_{T} [GeV]; #eta',
                               xbins=50,xmin=etmin,xmax=etmax,ybins=26,ymin=-2.6,ymax=2.6)
   
    monGroup.defineHistogram('hEFEtRaw', title='EF Et Raw;Uncalibrated E_{T}[GeV];Nevents',xbins=50,xmin=0,xmax=100)
    monGroup.defineHistogram('hEFnWideTrack', title='EF number of wide tracks;number of tracks;Nevents',xbins=10,xmin=0,xmax=10)

    monGroup.defineHistogram('hRNNScore', title='EF RNN score; RNN score;Nevents',xbins=20,xmin=0,xmax=1)
    monGroup.defineHistogram('hRNNScoreSigTrans', title='EF RNN trans score; RNN Trans score;Nevents',xbins=20,xmin=0,xmax=1)

  def bookDiTauVars(self, monAlg, trigger):
    
    monGroupName = trigger+"_DiTauVars"
    monGroupPath = 'DiTauVars/'+trigger

    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )    

    monGroup.defineHistogram('hleadEFEt,hsubleadEFEt', type='TH2F', title='lead Et vs sublead Et; lead E_{T} [GeV] ; sublead E_{T} [GeV]',
                               xbins=50,xmin=0,xmax=250,ybins=50,ymin=0,ymax=250)
    monGroup.defineHistogram('hleadEFEta,hsubleadEFEta', type='TH2F', title='lead Eta vs sublead Eta; lead #eta ; sublead #eta',
                               xbins=26,xmin=-2.6,xmax=2.6,ybins=26,ymin=-2.6,ymax=2.6)
    monGroup.defineHistogram('hleadEFPhi,hsubleadEFPhi', type='TH2F', title='lead Phi vs sublead Phi; lead #phi ; sublead #phi',
                               xbins=16,xmin=-3.2,xmax=3.2,ybins=16,ymin=-3.2,ymax=3.2) 
    monGroup.defineHistogram('hdR', title='EF dR(#tau,#tau);dR(#tau,#tau);Nevents',xbins=40,xmin=0,xmax=4)
    monGroup.defineHistogram('hdEta', title='EF dEta(#tau,#tau);dEta(#tau,#tau);Nevents',xbins=40,xmin=0,xmax=4)
    monGroup.defineHistogram('hdPhi', title='EF dPhi(#tau,#tau);dPhi(#tau,#tau);Nevents',xbins=16,xmin=-3.2,xmax=3.2)

    monGroup.defineHistogram('Pt', title='Pt DiTau; P_{t}; Nevents', xbins=50,xmin=0,xmax=250)
    monGroup.defineHistogram('Eta', title='Eta(#tau,#tau);Eta(#tau,#tau);Nevents',xbins=26,xmin=-2.6,xmax=2.6)
    monGroup.defineHistogram('Phi', title='Phi(#tau,#tau);Phi(#tau,#tau);Nevents',xbins=16,xmin=-3.2,xmax=3.2)
    monGroup.defineHistogram('M', title='M(#tau,#tau);M_{#tau,#tau};Nevents',xbins=50,xmin=0,xmax=250)
    monGroup.defineHistogram('dPt', title='dPt |leading-subleading|; P_{t}; Nevents', xbins=20,xmin=0,xmax=200)  

  def bookTAndPVars(self, monAlg, trigger):

    monGroupName = trigger+"_TAndPVars"
    monGroupPath = 'TAndPVars/'+trigger

    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )

    monGroup.defineHistogram('hdR', title='EF dR(#tau,lep);dR(#tau,lep);Nevents',xbins=40,xmin=0,xmax=4)
    monGroup.defineHistogram('hdEta', title='EF dEta(#tau,lep);dEta(#tau,lep);Nevents',xbins=40,xmin=0,xmax=4)
    monGroup.defineHistogram('hdPhi', title='EF dPhi(#tau,lep);dPhi(#tau,lep);Nevents',xbins=16,xmin=-3.2,xmax=3.2)

    monGroup.defineHistogram('Pt', title='Pt DiTau; P_{t}; Nevents', xbins=50,xmin=0,xmax=250)
    monGroup.defineHistogram('Eta', title='Eta DiTau;Eta;Nevents',xbins=26,xmin=-2.6,xmax=2.6)
    monGroup.defineHistogram('Phi', title='Phi DiTau;Phi;Nevents',xbins=16,xmin=-3.2,xmax=3.2)
    monGroup.defineHistogram('M', title='M(#tau,lep) DiTau;M_{#tau,lep};Nevents',xbins=50,xmin=0,xmax=250)
    monGroup.defineHistogram('dPt', title='dPt |lep-#tau|; P_{t}; Nevents', xbins=20,xmin=0,xmax=200)

  def bookTruth( self, monAlg, trigger, nProng):

    monGroupName = trigger+'_EFVsTruth_'+nProng
    monGroupPath = 'EFVsTruth/'+trigger+'/EFVsTruth_'+nProng 
    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )

    monGroup.defineHistogram('pt_vis,Etratio',title='Etratiovis vs Pt_{vis}; Pt_{vis};(reco pt - truth vis pt)/truth vis pt ',type='TProfile',xbins=21,xmin=20,xmax=250)
    monGroup.defineHistogram('eta_vis,Etratio',title='Etratiovis vs #eta_{vis}; #eta_{vis}; (reco pt - truth vis pt)/truth vis pt ', type='TProfile', xbins=21,xmin=-3,xmax=3)
    monGroup.defineHistogram('phi_vis,Etratio',title='Etratiovis vs #phi_{vis}; #phi_{vis}; (reco pt - truth vis pt)/truth vis pt ', type='TProfile', xbins=21,xmin=-3,xmax=3)

    monGroup.defineHistogram('pt_vis', title='Pt_vis Value; P_{tvis}; Nevents', xbins=50,xmin=0,xmax=250)
    monGroup.defineHistogram('eta_vis', title='Eta_vis Value; #eta_{vis};Nevents', xbins=26,xmin=-2.6,xmax=2.6)
    monGroup.defineHistogram('phi_vis', title='Phi_vis Value; #phi_{vis}; Nevents', xbins=16,xmin=-3.2,xmax=3.2)

  def bookTruthEfficiency( self, monAlg, trigger, nProng):
  
    monGroupName = trigger+'_Truth_Efficiency_'+nProng
    monGroupPath = 'Truth_Efficiency/'+trigger+'/Truth_Efficiency_'+nProng
    monGroup = self.helper.addGroup( monAlg, monGroupName,
                              self.basePath+'/'+monGroupPath )
  
    def defineEachStepHistograms(xvariable, xlabel, xbins, xmin, xmax):

       monGroup.defineHistogram(monGroupName+'_HLTpass,'+monGroupName+'_'+xvariable+';EffHLT_'+xvariable+'_wrt_Truth',
                                title='HLT Efficiency ' +trigger+' ' +nProng+ ';'+xlabel+';Efficiency',
                                type='TEfficiency',xbins=xbins,xmin=xmin,xmax=xmax)

    defineEachStepHistograms('pt_vis', 'Pt_{vis} [GeV]', 60, 0.0, 300.)
    defineEachStepHistograms('eta_vis','#eta_{vis}', 13, -2.6, 2.6)
    defineEachStepHistograms('phi_vis','#phi_{vis}', 16, -3.2, 3.2)

