###################################################################
# Job options to convert Identifier Mapping for H6 testbeam to POOL
#==================================================================

#use McEventSelector
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

theApp.Dlls += [ "GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ]

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.Dlls += [ "LArTools" ]


include("LArIdCnv/LArIdCnv_joboptions.py")
include("LArCondCnv/LArCondCnv_IdMapH6_jobOptions.py")
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )


MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )
EventSelector.RunNumber=1
#EventSelector.EventsPerRun=10;
EventSelector.EventsPerRun=2
EventSelector.FirstEvent=1

include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

# Exercise writing the objects to NOVA (no IOV).
theApp.OutStream     =["StreamDet"]
theApp.OutStreamType ="AthenaOutputStream"
StreamDet = Algorithm("StreamDet")

theApp.EvtMax=1
StreamDet.StoreName="DetectorStore"
StreamDet.EvtConversionSvc="AthenaPoolCnvSvc"

StreamDet.ItemList += ["LArOnOffIdMap#*"]
StreamDet.ItemList += ["LArFebRodMap#*"]
StreamDet.OutputFile = "LArIdMap_H6.pool.root"

# StreamDet.OutputFile =  "oracle://devdb/hma"

ToolSvc=Service("ToolSvc")
ToolSvc.LArRoI_Map.Print=False

