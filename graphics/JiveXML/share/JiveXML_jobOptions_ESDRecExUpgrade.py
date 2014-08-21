## RTT always runs RecExCommon_links.sh,
## but hard-wired link sqlite200 causes trouble
## with new real data:
import os
os.system('rm -rf sqlite200')
## Use AutoConfiguration
#from RecExConfig.RecFlags  import rec  #obsolete
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/project/Atlantis/rttInputs/ESD_23Mar12.pool.root"]
#athenaCommonFlags.FilesInput=["ESD_upgrade.pool.root"]
jp.Rec.doJiveXML.set_Value_and_Lock(True)
athenaCommonFlags.EvtMax.set_Value_and_Lock(20)
jp.Rec.doWriteTAG=False
jp.Rec.doWriteAOD=False
jp.Rec.doCBNT=False
jp.Rec.doMonitoring=False
## The main topOptions, always needs along with AutoConfiguration:
include("RecExCommon/RecExCommon_topOptions.py")
##
## default is ExtendedTracks, not present in Cosmics
theTrackRetriever.PriorityTrackCollection = "Tracks"
#
### initial try for jOs to make Upgrade event xml-files smaller (target <15MB):
#    jpt 27Sep11
#
theEventData2XML.DataTypes.remove("JiveXML::SiSpacePointRetriever/SiSpacePointRetriever")
theEventData2XML.DataTypes.remove("JiveXML::SiClusterRetriever/SiClusterRetriever")
theEventData2XML.DataTypes.remove("JiveXML::PixelClusterRetriever/PixelClusterRetriever")
theEventData2XML.DataTypes.remove("JiveXML::TRTRetriever/TRTRetriever")
theEventData2XML.DataTypes.remove("JiveXML::TrigSiSpacePointRetriever/TrigSiSpacePointRetriever")
theCaloLArRetriever.LArlCellThreshold = 200.
theCaloHECRetriever.HEClCellThreshold = 200.

# geometry version written intoxml-file (for upgrade)
from AthenaCommon.GlobalFlags import globalflags
theEventData2XML.GeometryVersion = globalflags.DetDescrVersion()
theEventData2XML.OutputLevel = DEBUG
