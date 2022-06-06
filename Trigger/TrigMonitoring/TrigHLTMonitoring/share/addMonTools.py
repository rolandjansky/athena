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
      log.warning('B-Physics JpsiFinder is no longer supported')

topSequence += AthenaMonManager("HLTMonManager",
        Run=DQMonFlags.monManRun(),
        DataType=DQMonFlags.monManDataType(),
        Environment=DQMonFlags.monManEnvironment())
HLTMonManager = topSequence.HLTMonManager

#Global HLTMonTool
if HLTMonFlags.doGeneral:
    log.warning("The HLT legacy monitoring is no longer supported")

if HLTMonFlags.doMonTier0:
    # HLTEgammaMonTool - Calculates basic efficiencies
    if HLTMonFlags.doEgamma:
        log.warning("The HLT legacy e/gamma monitoring is no longer supported")

    # HLTCaloTool - Makes basic comparisons between online/offline
    if HLTMonFlags.doCalo:
        log.warning("The HLT legacy Calo monitoring is no longer supported")

    # HLTMuonMonTool - Multiple Muon algorithms EDM Dumper
    if HLTMonFlags.doMuon:
      log.warning("The HLT legacy muon monitoring is no longer supported")

    # TrigInDet HLTMonTool
    if HLTMonFlags.doInDet:
      try:
        from TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig import TrigIDtrkMonitoringTool
        HLTMonManager.AthenaMonTools += TrigIDtrkMonitoringTool()
      except:
        log.info("Problems with HLTIDtrkTool, tool not enabled")
        import traceback
        log.info (traceback.format_exc())


    # MET HLTMonTool
    if HLTMonFlags.doMET:
        log.warning("The HLT legacy MET monitoring is no longer supported")

    # Tau HLTMonTool
    if HLTMonFlags.doTau:
        log.warning("The HLT legacy Tau monitoring is no longer supported")

    # Jet HLTMonTool
    if HLTMonFlags.doJet:
        log.warning("The HLT legacy jet monitoring is no longer supported")

    # b-jet HLTMonTool
    if HLTMonFlags.doBjet and rec.doInDet:
        log.warning("The HLT legacy b-jet monitoring is no longer supported")

    # B-phys HLTMonTool
    if HLTMonFlags.doBphys:
        log.warning("The HLT legacy B-Physics monitoring is no longer supported")

    # MinBias HLTMonTool
    if HLTMonFlags.doMinBias:
        log.warning("The HLT legacy MinBias monitoring is no longer supported")

################ Dump Tools ################

if HLTMonFlags.doDump:

    # HLTEgammaDumpTool - HLT EDM Egamma Dumper
    if HLTMonFlags.doEgamma:
        log.warning("The HLT legacy e/gamma dumper is no longer supported")

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
    log.warning("The legacy MaM is no longer supported")


############################################

HLTMonManager.FileKey = "GLOBAL"

printfunc (HLTMonManager)

#---------------------------------------------------
#----   End
#---------------------------------------------------
