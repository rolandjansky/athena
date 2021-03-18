#--------------------------------------------------------------
#-- Job Options for HLTMonitoring
#--------------------------------------------------------------

#############################################

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigHLTMonitoring/addMonTools' )

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

# in order to check available detectors
from RecExConfig.RecFlags  import rec

#### flags ####

# if not 'HLTMonFlags' in dir():
#   from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags
#
if not 'DQMonFlags' in dir():
    from AthenaMonitoring.DQMonFlags import DQMonFlags

################ Mon Tools #################

# Bphys - Running JpsiFinder
if HLTMonFlags.doMonTier0:
    if HLTMonFlags.doBphys and rec.doInDet:
      include( "TrigBphysMonitoring/RunJpsiFinder.py" )

topSequence += AthenaMonManager("HLTMonManager",
        Run=DQMonFlags.monManRun(),
        DataType=DQMonFlags.monManDataType(),
        Environment=DQMonFlags.monManEnvironment())
HLTMonManager = topSequence.HLTMonManager

#Global HLTMonTool
if HLTMonFlags.doGeneral:
    try:
        from TrigHLTMonitoring.TrigHLTMonitoringConfig import HLTGeneralTool
        HLTMonManager.AthenaMonTools += HLTGeneralTool()
    except:
         log.info("Problems with the general HLTMonTool, tool not enabled")
         import traceback
         log.info (traceback.format_exc())

if HLTMonFlags.doMonTier0:
    # HLTEgammaMonTool - Calculates basic efficiencies
    if HLTMonFlags.doEgamma:
      try:
       if rec.doCalo and rec.doInDet:
        from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import TrigEgammaMonitoringTool
        HLTMonManager.AthenaMonTools += TrigEgammaMonitoringTool()
      except:
        log.info("Problems with HLTEgammaTool, tool not enabled")
        import traceback
        log.info (traceback.format_exc())

    # HLTCaloTool - Makes basic comparisons between online/offline
    if HLTMonFlags.doCalo:
      try:
       if rec.doCalo and rec.doInDet:
        from TrigCaloMonitoring.TrigCaloMonitoringConfig import HLTCaloMonitoringTool
        HLTMonManager.AthenaMonTools+=HLTCaloMonitoringTool()
      except:
        log.info("Problems with HLTCaloTool, tool not enabled")
        import traceback
        log.info (traceback.format_exc())

    # HLTMuonMonTool - Multiple Muon algorithms EDM Dumper
    if HLTMonFlags.doMuon:
      try:
        from TrigMuonMonitoring.TrigMuonMonitoringConfig import TrigMuonMonitoringTool
        HLTMonManager.AthenaMonTools += TrigMuonMonitoringTool()
        HLTMonManager.ExtraInputs += [('xAOD::MuonContainer','StoreGateSvc+Muons')]
        HLTMonManager.ExtraInputs += [('xAOD::VertexContainer','StoreGateSvc+PrimaryVertices')]
        HLTMonManager.ExtraInputs += [('xAOD::EventInfo','StoreGateSvc+EventInfo')]
        HLTMonManager.ExtraInputs += [('TrigOperationalInfoCollection','StoreGateSvc+HLT_TrigOperationalInfoCollection_EXPRESS_OPI_HLT')]
        HLTMonManager.ExtraInputs += [('xAOD::MuonRoIContainer','StoreGateSvc+LVL1MuonRoIs')]
      except:
        log.info("Problems with HLTMuonTool, tool not enabled")
        import traceback
        log.info (traceback.format_exc())

    # IDtrk HLTMonTool
    if HLTMonFlags.doIDtrk:
      try:
        from TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig import TrigIDtrkMonitoringTool
        HLTMonManager.AthenaMonTools += TrigIDtrkMonitoringTool()
      except:
        log.info("Problems with HLTIDtrkTool, tool not enabled")
        import traceback
        log.info (traceback.format_exc())


    # MET HLTMonTool
    if HLTMonFlags.doMET:
      try:
        from TrigMETMonitoring.TrigMETMonitoringConfig import HLTMETMonitoringTool
        HLTMonManager.AthenaMonTools += HLTMETMonitoringTool()
      except:
        log.info("Problems with HLTMETTool, tool not enabled")
        import traceback
        log.info (traceback.format_exc())

    # Tau HLTMonTool
    if HLTMonFlags.doTau:
      try:
        from TrigTauMonitoring.TrigTauMonitoringConfig import TrigTauMonitoringTool
        HLTMonManager.AthenaMonTools += TrigTauMonitoringTool()
      except:
        log.info("Problems with HLTTauTool, tool not enabled")
        import traceback
        log.info (traceback.format_exc())

    # Jet HLTMonTool
    if HLTMonFlags.doJet:
      try:
        from TrigJetMonitoring.TrigJetMonitoringConfig import TrigJetMonitoringTool
        HLTMonManager.AthenaMonTools += TrigJetMonitoringTool()
      except:
        log.info("Problems with HLTJetTool, tool not enabled")
        import traceback
        log.info (traceback.format_exc())

    # b-jet HLTMonTool
    if HLTMonFlags.doBjet and rec.doInDet:
      try:
        from TrigBjetMonitoring.TrigBjetMonitoringConfig import TrigBjetMonitoringConfig
        HLTMonManager.AthenaMonTools += TrigBjetMonitoringConfig()
      except:
        log.info("Problems with HLTBjetTool, tool not enabled")
        import traceback
        log.info (traceback.format_exc())

    # B-phys HLTMonTool
    if HLTMonFlags.doBphys:
      try:
        from TrigBphysMonitoring.TrigBphysMonitoringConfig import TrigBphysMonitoringTool
        HLTMonManager.AthenaMonTools += TrigBphysMonitoringTool()
      except:
        log.info("Problems with HLTBphysTool, tool not enabled")
        import traceback
        log.info (traceback.format_exc())

    # MinBias HLTMonTool
    if HLTMonFlags.doMinBias:
      try:
        from TrigMinBiasMonitoring.TrigMinBiasMonitoringConfig import TrigMinBiasMonitoringTool
        HLTMonManager.AthenaMonTools += TrigMinBiasMonitoringTool()
      except:
        log.info("Problems with HLTMinBiasDumpTool, tool not enabled")
        import traceback
        log.info (traceback.format_exc())

################ Dump Tools ################

if HLTMonFlags.doDump:

    # HLTEgammaDumpTool - HLT EDM Egamma Dumper
    if HLTMonFlags.doEgamma:
      try:
        from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import HLTEgammaMonitoringDumpTool
        HLTMonManager.AthenaMonTools += HLTEgammaMonitoringDumpTool()
      except:
        log.info("Problems with HLTEgammaDumpTool, tool not enabled")
        import traceback
        log.info (traceback.format_exc())


    #Make the custom tau TTP ntuples.
    if HLTMonFlags.doOfflineTauTTP :
     try:
      from TrigTauPerformAthena.TrigTauPerformAthenaConf import TrigTauPerformAthenaAlgo
      ttpalgo = TrigTauPerformAthenaAlgo()
      from AthenaCommon.AlgSequence import AlgSequence
      topSequence = AlgSequence()
      topSequence += ttpalgo
     except:
      log.info("Problems with OfflineTauTTP, tool not enabled")
      import traceback
      log.info (traceback.format_exc())

########## Menu-aware Monitoring ###########

if HLTMonFlags.doMaM == True:
    include( "TrigHLTMonitoring/runMaM.py" )

############################################

HLTMonManager.FileKey = "GLOBAL"

printfunc (HLTMonManager)

#---------------------------------------------------
#----   End
#---------------------------------------------------
