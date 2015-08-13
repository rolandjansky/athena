monManTau = AthenaMonManager(
             name                = "tauMonManager",
             FileKey             = DQMonFlags.monManFileKey(),
             Environment         = DQMonFlags.monManEnvironment(),
             ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
             DataType            = DQMonFlags.monManDataType()
          )
topSequence += monManTau


from tauMonitoring.tauMonitoringConf import *
from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
monbadlb = GetLArBadLBFilterTool()

LowStat = True
if DQMonFlags.monManEnvironment() == 'online' :
    LowStat = False 
print "doLowStat " , LowStat

myTDT = None
if DQMonFlags.useTrigger() and LowStat :
    if hasattr(ToolSvc, DQMonFlags.nameTrigDecTool()) :
        myTDT = getattr(ToolSvc, DQMonFlags.nameTrigDecTool())
    else :
        from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
        myTDT = Trig__TrigDecisionTool( "tauDQ_TrigDecisionTool" )
        ToolSvc += myTDT
    
tauMonTool           = tauMonTool (name = "tauMonTool")
tauMonTool.doLowStat = LowStat

if myTDT is None :
    tauMonTool.doTrigger = False
else :
    tauMonTool.doTrigger = DQMonFlags.useTrigger()
    tauMonTool.TrigDecisionTool = myTDT

print "doTrigger " , tauMonTool.doTrigger

if tauMonTool.doTrigger :
    tauMonTool.TauEleJetMbtsTriggerItems = [ [ "L1_TAU1.*" , "L1_TAU2.*" , "L1_TAU60" , "L1_EM15VHI_2TAU.*" , "L1_EM15VHI_TAU40.*" , "L1_MU10_TAU.*" , "L1_J25_3J12_EM15-TAU.*" ] , [ "HLT_e2.*" , "L1_EM2.*" , "L1_2EM15" ] , [ "HLT_j55" , "HLT_j60" , "HLT_j85" , "HLT_j110" , "HLT_j175" , "HLT_4j85" , "HLT_4j10.*" ] , [ "HLT_mb_sp2000_trk70_hmt" ] ]

tauMonTool.FilterTools = [ monAtlasReadyFilterTool ]
tauMonTool.FilterTools.append(monbadlb)
ToolSvc += tauMonTool
monManTau.AthenaMonTools += [ tauMonTool ]

