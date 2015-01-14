## RTT always runs RecExCommon_links.sh,
## but hard-wired link sqlite200 causes trouble
## with new real data:
import os
os.system('rm -rf sqlite200')
## Use AutoConfiguration
#from RecExConfig.RecFlags  import rec #obsolete
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/project/Atlantis/rttInputs/ESD_7Jan15.pool.root"]
jp.Rec.doJiveXML.set_Value_and_Lock(True)
athenaCommonFlags.EvtMax.set_Value_and_Lock(10)
jp.Rec.doWriteTAG=False
jp.Rec.doWriteAOD=False
jp.Rec.doCBNT=False
jp.Rec.doMonitoring=False
## The main topOptions, always needs along with AutoConfiguration:
include("RecExCommon/RecExCommon_topOptions.py")
##
## default is ExtendedTracks, not present in Cosmics
theTrackRetriever.PriorityTrackCollection = "Tracks"

# geometry version written intoxml-file (for upgrade)
from AthenaCommon.GlobalFlags import globalflags
theEventData2XML.GeometryVersion = globalflags.DetDescrVersion()
theEventData2XML.OutputLevel = DEBUG

