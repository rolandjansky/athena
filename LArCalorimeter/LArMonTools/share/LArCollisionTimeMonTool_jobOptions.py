###############################################################################
# Author : Vikas Bansal <Vikas.Bansal@cern.ch>
# Name   : LArCollisionTimeMonTool_jobOptions.py
# Purpose: To configure and schedule LArCollisionTime Monitoring tool.
#          This jobOptions tweaks Athena Algorithm Sequence so that the
#          LArCollisionTimeAlg is called before LArMonManager.
###############################################################################

## Include LArCollisionTimeAlg
include ("LArCellRec/LArCollisionTime_jobOptions.py")

#########
# Now configure and schedule the monitoring tool
# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0

# --- LArCollisionTimeMon configuration ---
from LArMonTools.LArMonToolsConf import LArCollisionTimeMonTool
LArCollisionTimeMon = LArCollisionTimeMonTool(
    # --- Configure mon tool ---
    name                      = "LArCollisionTimeMon",
    timeDiffCut               = 5000.0,#5.0
    nCells                    = 1,# Needs at least >=2 cells to get rid of electronic noise
    OutputLevel               = 3,
    )

# --- register with toolservice ---
ToolSvc += LArCollisionTimeMon
LArMon.AthenaMonTools+=[ LArCollisionTimeMon ]
