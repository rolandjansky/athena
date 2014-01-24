#--------------------------------------------------------------
#-- Job Options for HLTMonitoring
#--------------------------------------------------------------

#############################################

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

# in order to check available detectors
from RecExConfig.RecFlags  import rec

#### flags ####

# if not 'HLTMonFlags' in dir():
#   from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags
# 
# if not 'DQMonFlags' in dir():
#   from AthenaMonitoring.DQMonFlags import DQMonFlags

################ Mon Tools #################

# Bphys - Running JpsiFinder
if HLTMonFlags.doMonTier0:
    if HLTMonFlags.doBphys and rec.doInDet:
      include( "TrigBphysMonitoring/RunJpsiFinder.py" )

topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager

#Global HLTMonTool
if HLTMonFlags.doGeneral:
    try:
        from TrigHLTMonitoring.TrigHLTMonitoringConfig import HLTGeneralTool
        HLTMonManager.AthenaMonTools += HLTGeneralTool()
    except:
         print "Problems with the general HLTMonTool"

if HLTMonFlags.doMonTier0:

    # HLTEgammaMonTool - Calculates basic efficiencies
    if HLTMonFlags.doEgamma:
      try:
       if rec.doCalo and rec.doInDet:
        from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import HLTEgammaMonitoringTool
        HLTMonManager.AthenaMonTools += HLTEgammaMonitoringTool()
      except:
        print "Problems with HLTEgammaTool, tool not enabled"

    # HLTCaloTool - Makes basic comparisons between online/offline
    if HLTMonFlags.doCalo:
      try:
       if rec.doCalo and rec.doInDet:
        from TrigCaloMonitoring.TrigCaloMonitoringConfig import HLTCaloMonitoringTool
        HLTMonManager.AthenaMonTools+=HLTCaloMonitoringTool()
      except:
        print "Problems with HLTCaloTool, tool not enabled"
    
    # HLTMuonMonTool - Multiple Muon algorithms EDM Dumper
    if HLTMonFlags.doMuon:
      try:
        from TrigMuonMonitoring.TrigMuonMonitoringConfig import TrigMuonMonitoringTool
        HLTMonManager.AthenaMonTools += TrigMuonMonitoringTool()
      except:
        print "Problems with HLTMuonTool, tool not enabled"

    # IDtrk HLTMonTool
    if HLTMonFlags.doIDtrk:
      try:
        from TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig import TrigIDtrkMonitoringTool
        HLTMonManager.AthenaMonTools += TrigIDtrkMonitoringTool()
      except:
        print "Problems with HLTIDtrkTool, tool not enabled"

    # MET HLTMonTool
    if HLTMonFlags.doMET:
      try:
        from TrigMETMonitoring.TrigMETMonitoringConfig import HLTMETMonitoringTool
        HLTMonManager.AthenaMonTools += HLTMETMonitoringTool()
      except:
        print "Problems with HLTMETTool, tool not enabled"

    # Tau HLTMonTool
    if HLTMonFlags.doTau:
      try:
        from TrigTauMonitoring.TrigTauMonitoringConfig import TrigTauMonitoringTool
        HLTMonManager.AthenaMonTools += TrigTauMonitoringTool()
      except:
        print "Problems with HLTTauTool, tool not enabled"
        
    # Jet HLTMonTool
    if HLTMonFlags.doJet:
      try:
        from TrigJetMonitoring.TrigJetMonitoringConfig import TrigJetMonitoringTool
        HLTMonManager.AthenaMonTools += TrigJetMonitoringTool()
      except:
        print "Problems with HLTJetTool, tool not enabled"
    
    # b-jet HLTMonTool
    if HLTMonFlags.doBjet and rec.doInDet:
      try:
        from TrigBjetMonitoring.TrigBjetMonitoringConfig import TrigBjetMonitoringConfig
        HLTMonManager.AthenaMonTools += TrigBjetMonitoringConfig()
      except:
        print "Problems with HLTBjetTool, tool not enabled"
    
    # B-phys HLTMonTool
    if HLTMonFlags.doBphys:
      try:
        from TrigBphysMonitoring.TrigBphysMonitoringConfig import TrigBphysMonitoringTool
        HLTMonManager.AthenaMonTools += TrigBphysMonitoringTool()
      except:
        print "Problems with HLTBphysTool, tool not enabled"

    # MinBias HLTMonTool
    if HLTMonFlags.doMinBias:
      try:
        from TrigMinBiasMonitoring.TrigMinBiasMonitoringConfig import TrigMinBiasMonitoringTool
        HLTMonManager.AthenaMonTools += TrigMinBiasMonitoringTool()
      except:
        print "Problems with HLTMinBiasDumpTool, tool not enabled"

    # MinBias HLTMonTool
    if HLTMonFlags.doIDJpsiMon:
      try:
        from TrigIDJpsiMonitoring.TrigIDJpsiMonitoringConfig import TrigIDJpsiMonitoringTool
        HLTMonManager.AthenaMonTools += TrigIDJpsiMonitoringTool()
      except:
        print "Problems with, TrigIDJpsiMonTool, tool not enabled"


################ Dump Tools ################

if HLTMonFlags.doDump:

    # HLTEgammaDumpTool - HLT EDM Egamma Dumper
    if HLTMonFlags.doEgamma:
      try:
        from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import HLTEgammaMonitoringDumpTool
        HLTMonManager.AthenaMonTools += HLTEgammaMonitoringDumpTool()
      except:
        print "Problems with HLTEgammaDumpTool, tool not enabled"


    #Make the custom tau TTP ntuples.
    if HLTMonFlags.doOfflineTauTTP :      
     try:
      from TrigTauPerformAthena.TrigTauPerformAthenaConf import TrigTauPerformAthenaAlgo
      ttpalgo = TrigTauPerformAthenaAlgo()
      from AthenaCommon.AlgSequence import AlgSequence
      topSequence = AlgSequence()
      topSequence += ttpalgo          
     except:
      print "Problems with OfflineTauTTP, tool not enabled"
############################################

HLTMonManager.FileKey = "GLOBAL"

print HLTMonManager;

#---------------------------------------------------
#----   End
#---------------------------------------------------

