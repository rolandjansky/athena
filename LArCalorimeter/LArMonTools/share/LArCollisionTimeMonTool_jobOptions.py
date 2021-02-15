###############################################################################
# Author : Vikas Bansal <Vikas.Bansal@cern.ch>
# Name   : LArCollisionTimeMonTool_jobOptions.py
# Purpose: To configure and schedule LArCollisionTime Monitoring tool.
#          This jobOptions tweaks Athena Algorithm Sequence so that the
#          LArCollisionTimeAlg is called before LArMonManager.
###############################################################################


#########
# Now configure and schedule the monitoring tool
from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
theBunchCrossingTool=BunchCrossingTool()
#ToolSvc+=theBunchCrossingTool

# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0

# --- LArCollisionTimeMon configuration ---
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from LArMonTools.LArMonToolsConf import LArCollisionTimeMonTool
LArCollisionTimeMon = LArCollisionTimeMonTool(
    # --- Configure mon tool ---
    name                      = "LArCollisionTimeMon",
    timeDiffCut               = 5000.0,#5.0
    nCells                    = 1,# Needs at least >=2 cells to get rid of electronic noise
    m_lumi_blocks             = 3000,
    BunchCrossingTool = theBunchCrossingTool,
    TrainFrontDistance        = 20,
    IsOnline                  = athenaCommonFlags.isOnline()
    )


LArCluCollTimeMonTool=LArCollisionTimeMonTool(Key="ClusterCollTime",
                                              histPath="LArClusterCollTimeOldTool",
                                              nCells=0,
                                              m_lumi_blocks=3000,
                                              BunchCrossingTool = theBunchCrossingTool,
                                              TrainFrontDistance = 20,
                                              IsOnline = athenaCommonFlags.isOnline()
                                              )


# --- register with toolservice ---
#ToolSvc += [LArCollisionTimeMon,LArCluCollTimeMonTool]
LArMon.AthenaMonTools+=[ LArCollisionTimeMon,LArCluCollTimeMonTool ]

