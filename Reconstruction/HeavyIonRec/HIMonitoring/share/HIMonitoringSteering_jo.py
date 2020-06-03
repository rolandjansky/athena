from RecExConfig.RecFlags import rec

## Event Shape 
from HIMonitoring.HIMonitoringConf import HIMonitoringEventShapeTool

if rec.doHeavyIon():
    HIEventShapeMonTool = HIMonitoringEventShapeTool (
		    name = "HIEventShapeMonTool",
		    EnableLumi = False,
		    ZDCmon = True,
		    ESmon = True,
		    FCalEt_nbins = 120, 
		    lowFCalEt = -0.2,
		    highFCalEt = 5.8)
 
if rec.doHIP():	
    HIEventShapeMonTool = HIMonitoringEventShapeTool (
		    name = "HIEventShapeMonTool",
		    EnableLumi = False,
		    ZDCmon = True,
		    ESmon = True,
		    FCalEt_nbins = 96,
		    lowFCalEt = -0.16,
		    highFCalEt = 0.8)


#HIEventShapeMonTool.TriggerChain = "HLT_noalg_mb_L1ZDC_A_C,HLT_noalg_mb_L1MBTS_1_1,HLT_noalg_mb_L1TE5"
#if DQMonFlags.useTrigger():
#   HIEventShapeMonTool.TrigDecisionTool = monTrigDecTool #tool should already be in namespace

## Photons 
from HIMonitoring.HIMonitoringConf import HIMonitoringPhotonsTool
if rec.doHeavyIon():
    HIPhotonsMonTool = HIMonitoringPhotonsTool (
		    name           = "HIPhotonsMonTool",
		    FCalEt_nbins = 120,
		    lowFCalEt = -0.2,
		    highFCalEt = 5.8,
		    FCalEt_peripheral = 100.0,
		    FCalEt_central = 2000.0)

if rec.doHIP():
    HIPhotonsMonTool = HIMonitoringPhotonsTool (
		    name           = "HIPhotonsMonTool",
		    FCalEt_nbins = 96,
		    lowFCalEt = -0.16,   
		    highFCalEt = 0.8,
		    FCalEt_peripheral = 20.0,
		    FCalEt_central = 50.0)

#HIPhotonsMonTool.TriggerChain = "HLT_g15_loose"
#if DQMonFlags.useTrigger():
#    HIPhotonsMonTool.TrigDecisionTool = monTrigDecTool #tool should already be in namespace

## Muons 
from HIMonitoring.HIMonitoringConf import HIMonitoringMuonsTool
HIMuonsMonTool = HIMonitoringMuonsTool (
		name           = "HIMuonsMonTool",)
		

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

if rec.doHeavyIon():
    HIElectronsMonTool = HIMonitoringElectronsTool (
		    name           = "HIElectronsMonTool",
		    FCalEt_nbins = 120,
		    lowFCalEt = -0.2,
		    highFCalEt = 5.8, 
		    FCalEt_nbins_oneSide = 60,
		    lowFCalEt_oneSide = -0.1,
		    highFCalEt_oneSide = 3.9)

if rec.doHIP():
    HIElectronsMonTool = HIMonitoringElectronsTool (
		    name           = "HIElectronsMonTool",
		    FCalEt_nbins = 96,
		    lowFCalEt = -0.16,
		    highFCalEt = 0.8,
		    FCalEt_nbins_oneSide = 48,
		    lowFCalEt_oneSide = -0.08,
		    highFCalEt_oneSide = 0.4)

                                      
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
