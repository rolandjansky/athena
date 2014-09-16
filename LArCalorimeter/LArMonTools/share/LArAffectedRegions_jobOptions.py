# import stuff
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

# --- For athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0

###### LAr Affected Region Tool Configuration ###############
from LArMonTools.LArMonToolsConf import LArAffectedRegions
theLArAffectedRegions = LArAffectedRegions(name="LArAffectedRegions",
                             ProcessNEvents = EventBlockSize,
                             IsOnline       = athenaCommonFlags.isOnline()      
                         )
ToolSvc += theLArAffectedRegions
LArMon.AthenaMonTools+=[ theLArAffectedRegions ]
