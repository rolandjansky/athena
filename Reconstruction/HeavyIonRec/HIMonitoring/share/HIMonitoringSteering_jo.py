## Event Shape 
from HIMonitoring.HIMonitoringConf import HIMonitoringEventShapeTool

HIEventShapeMonTool = HIMonitoringEventShapeTool (
name = "HIEventShapeMonTool",
EnableLumi = False,
ZDCmon = True,
ESmon = True,
)

#HIEventShapeMonTool.TriggerChain = "HLT_noalg_mb_L1ZDC_A_C,HLT_noalg_mb_L1MBTS_1_1,HLT_noalg_mb_L1TE5"
#if DQMonFlags.useTrigger():
#   HIEventShapeMonTool.TrigDecisionTool = monTrigDecTool #tool should already be in namespace

## Photons 
from HIMonitoring.HIMonitoringConf import HIMonitoringPhotonsTool
HIPhotonsMonTool = HIMonitoringPhotonsTool (
name           = "HIPhotonsMonTool",
)

#HIPhotonsMonTool.TriggerChain = "HLT_g15_loose"
#if DQMonFlags.useTrigger():
#    HIPhotonsMonTool.TrigDecisionTool = monTrigDecTool #tool should already be in namespace

## Muons 
from HIMonitoring.HIMonitoringConf import HIMonitoringMuonsTool
HIMuonsMonTool = HIMonitoringMuonsTool (
name           = "HIMuonsMonTool",
)

#HIMuonsMonTool.TriggerChain = "HLT_mu4,HLT_mu10,HLT_mu14"
#if DQMonFlags.useTrigger():
#    HIMuonsMonTool.TrigDecisionTool = monTrigDecTool #tool should already be in namespace

## ZDC  
from HIMonitoring.HIMonitoringConf import HIMonitoringZdcTool
HIZdcMonTool = HIMonitoringZdcTool (
name           = "HIZdcMonTool",
)

#HIZdcMonTool.TriggerChain = "HLT_noalg_mb_L1ZDC_A_C"
#if DQMonFlags.useTrigger():
#    HIZdcMonTool.TrigDecisionTool = monTrigDecTool #tool should already be in namespace

## Electrons  
from HIMonitoring.HIMonitoringConf import HIMonitoringElectronsTool
HIElectronsMonTool = HIMonitoringElectronsTool (
name           = "HIElectronsMonTool",
)
                                      
#HIElectronsMonTool.TriggerChain = "HLT_e15_etcut_ion,HLT_e15_loose_ion,HLT_e15_medium_ion"
#if DQMonFlags.useTrigger():
#    HIElectronsMonTool.TrigDecisionTool = monTrigDecTool #tool should already be in namespace



if DQMonFlags.monManEnvironment == 'tier0ESD' or DQMonFlags.monManEnvironment == 'tier0': 
    ToolSvc += HIEventShapeMonTool
    HIEventShapeMonMan = AthenaMonManager( "HIEventShapeManager",
                                   FileKey             = DQMonFlags.monManFileKey(),             #"GLOBAL" #"stat"
                                   ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(), #True
                                   DataType            = DQMonFlags.monManDataType(),            #"cosmics"
                                   Environment         = DQMonFlags.monManEnvironment(),         #"user"
                                   ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),    #True
                                   Run                 = DQMonFlags.monManRun(),                 #1
                                   LumiBlock           = DQMonFlags.monManLumiBlock(),           #1
                                   AthenaMonTools      = [ HIEventShapeMonTool ])
    topSequence += HIEventShapeMonMan
    
    ToolSvc += HIPhotonsMonTool
    HIPhotonsMonMan = AthenaMonManager( "HIPhotonsMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),             #"GLOBAL" #"stat"
                                   ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(), #True
                                   DataType            = DQMonFlags.monManDataType(),            #"cosmics"
                                   Environment         = DQMonFlags.monManEnvironment(),         #"user"
                                   ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),    #True
                                   Run                 = DQMonFlags.monManRun(),                 #1
                                   LumiBlock           = DQMonFlags.monManLumiBlock(),           #1
                                   AthenaMonTools      = [ HIPhotonsMonTool ])
    topSequence += HIPhotonsMonMan

    ToolSvc += HIMuonsMonTool
    HIMuonsMonMan = AthenaMonManager( "HIMuonsMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),             #"GLOBAL" #"stat"
                                   ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(), #True
                                   DataType            = DQMonFlags.monManDataType(),            #"cosmics"
                                   Environment         = DQMonFlags.monManEnvironment(),         #"user"
                                   ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),    #True
                                   Run                 = DQMonFlags.monManRun(),                 #1
                                   LumiBlock           = DQMonFlags.monManLumiBlock(),           #1
                                   AthenaMonTools      = [ HIMuonsMonTool ])
    topSequence += HIMuonsMonMan
        
    ToolSvc += HIElectronsMonTool
    HIElectronsMonMan = AthenaMonManager( "HIElectronsMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),             #"GLOBAL" #"stat"
                                   ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(), #True
                                   DataType            = DQMonFlags.monManDataType(),            #"cosmics"
                                   Environment         = DQMonFlags.monManEnvironment(),         #"user"
                                   ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),    #True
                                   Run                 = DQMonFlags.monManRun(),                 #1
                                   LumiBlock           = DQMonFlags.monManLumiBlock(),           #1
                                   AthenaMonTools      = [ HIElectronsMonTool ])
    topSequence += HIElectronsMonMan

if DQMonFlags.monManEnvironment == 'tier0ESD' or DQMonFlags.monManEnvironment == 'tier0' or DQMonFlags.monManEnvironment == 'online':
    ToolSvc += HIZdcMonTool
    HIZdcMonMan = AthenaMonManager( "HIZdcMonManager",
                                   FileKey             = DQMonFlags.monManFileKey(),             #"GLOBAL" #"stat"
                                   ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(), #True
                                   DataType            = DQMonFlags.monManDataType(),            #"cosmics"
                                   Environment         = DQMonFlags.monManEnvironment(),         #"user"
                                   ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),    #True
                                   Run                 = DQMonFlags.monManRun(),                 #1
                                   LumiBlock           = DQMonFlags.monManLumiBlock(),           #1
                                   AthenaMonTools      = [ HIZdcMonTool ])
    topSequence += HIZdcMonMan
