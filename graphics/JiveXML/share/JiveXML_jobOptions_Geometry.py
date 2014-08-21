jp.Rec.doWriteESD=False
jp.Rec.doWriteTAG=False
jp.Rec.doWriteAOD=False
jp.Rec.doTrigger=False
jp.Rec.doMonitoring=False

##include ("RecExCommon/RecExCommon_flags.py")

include ("RecExCommon/myTopOptions.py")

include( "JiveXML/JiveXML_jobOptionBase.py" )
theEventData2XML.WriteGeometry = True
theEventData2XML.WriteToFile = False

## DEBUG output from both geometry writers
from GeometryJiveXML.GeometryJiveXMLConf import JiveXML__GeometryWriter
theGeometryWriter = JiveXML__GeometryWriter (name = "GeometryWriter")
theGeometryWriter.OutputLevel = DEBUG
ToolSvc += theGeometryWriter
from GeometryJiveXML.GeometryJiveXMLConf import JiveXML__MuonGeometryWriter
theMuonGeometryWriter = JiveXML__MuonGeometryWriter (name = "MuonGeometryWriter")
theMuonGeometryWriter.OutputLevel = DEBUG
ToolSvc += theMuonGeometryWriter
