## with new JiveXML_config_RecEx.py:
## can run with doJiveXML also from AOD
import os
#### remove db link (RecExCommon_links is executed
#### ahead of all RTT jOs):
os.system('rm -rf sqlite200')
#### input file link is being updated (no need to update jOs):
## Use AutoConfiguration
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/project/Atlantis/rttInputs/xAOD_May2014.pool.root"]
jp.Rec.doJiveXML=True
athenaCommonFlags.EvtMax.set_Value_and_Lock(10)
jp.Rec.doWriteTAG=False
jp.Rec.doESD=False
jp.Rec.doWriteTAG=False
jp.Rec.doCBNT=False
jp.Rec.doMonitoring=False
## The main topOptions, always needs along with AutoConfiguration:
include("RecExCommon/RecExCommon_topOptions.py")
## for track-vertex association
theVertexRetriever.TrackCollection = "TrackParticleCandidate"

# geometry version written intoxml-file (for upgrade)
from AthenaCommon.GlobalFlags import globalflags
theEventData2XML.GeometryVersion = globalflags.DetDescrVersion()
theEventData2XML.OutputLevel = DEBUG

