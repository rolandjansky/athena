from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## add an AthenaMonManager algorithm to the list of algorithms to be ran
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
AFPMonManager = AthenaMonManager( name = "AFPHitsMonitoringManager",
                                 FileKey             = DQMonFlags.monManFileKey(),
                                 Environment         = DQMonFlags.monManEnvironment(),
                                 ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                 DataType            = DQMonFlags.monManDataType()
                                 )

topSequence +=  AFPMonManager

# ===== Silicon hits monitor =====
from AFP_Monitoring.AFP_MonitoringConf import AFPHitsMonitorTool
AFPSiHitsMonTool = AFPHitsMonitorTool (
name           = "AFPSiHitsMonitoring",
PreScale       = 0,
EnableLumi = True
)

layersInStationN = 4
from AFP_Monitoring.AFP_MonitoringConf import AFPSiStationMonitor

# *** station 0 ***
from AFP_Monitoring.AFP_MonitoringConf import AFPSiLayerMonitor
layersStation0 = list()
for layerID in range(layersInStationN):
    toolName = "station0Layer"
    toolName += str(layerID)
    layersStation0.append(AFPSiLayerMonitor(name = toolName, pixelLayerID = layerID, stationID = 0))
    ToolSvc += layersStation0[layerID]

station_0 = AFPSiStationMonitor (name = "AFPStation0", stationID = 0, layersMonitors = layersStation0)
ToolSvc += station_0

# *** station 1 ***
from AFP_Monitoring.AFP_MonitoringConf import AFPSiLayerMonitor
layersStation1 = list()
for layerID in range(layersInStationN):
    toolName = "station1Layer"
    toolName += str(layerID)
    layersStation1.append(AFPSiLayerMonitor(name = toolName, pixelLayerID = layerID, stationID = 1))
    ToolSvc += layersStation1[layerID]

station_1 = AFPSiStationMonitor (name = "AFPStation1", stationID = 1, layersMonitors = layersStation1)
ToolSvc += station_1

# *** station 2 ***
from AFP_Monitoring.AFP_MonitoringConf import AFPSiLayerMonitor
layersStation2 = list()
for layerID in range(layersInStationN):
    toolName = "station2Layer"
    toolName += str(layerID)
    layersStation2.append(AFPSiLayerMonitor(name = toolName, pixelLayerID = layerID, stationID = 2))
    ToolSvc += layersStation2[layerID]

station_2 = AFPSiStationMonitor (name = "AFPStation2", stationID = 2, layersMonitors = layersStation2)
ToolSvc += station_2

# *** station 3 ***
from AFP_Monitoring.AFP_MonitoringConf import AFPSiLayerMonitor
layersStation3 = list()
for layerID in range(layersInStationN):
    toolName = "station3Layer"
    toolName += str(layerID)
    layersStation3.append(AFPSiLayerMonitor(name = toolName, pixelLayerID = layerID, stationID = 3))
    ToolSvc += layersStation3[layerID]

station_3 = AFPSiStationMonitor (name = "AFPStation3", stationID = 3, layersMonitors = layersStation3)
ToolSvc += station_3

AFPSiHitsMonTool.stationsMonitors = [station_0, station_1, station_2, station_3]

ToolSvc += AFPSiHitsMonTool
AFPMonManager.AthenaMonTools += [ AFPSiHitsMonTool ]


# ===== Technical monitor =====
from AFP_Monitoring.AFP_MonitoringConf import AFPTechnicalMonitorTool
AFPTechnicalMonTool = AFPTechnicalMonitorTool (
name           = "AFPTechnicalMonitoring",
PreScale       = 0,
EnableLumi = True
)

ToolSvc += AFPTechnicalMonTool
AFPMonManager.AthenaMonTools += [ AFPTechnicalMonTool ]
