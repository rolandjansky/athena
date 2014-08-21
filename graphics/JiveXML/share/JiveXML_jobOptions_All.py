jp.Rec.doWriteESD=False
jp.Rec.doWriteTAG=False
jp.Rec.doWriteAOD=False
#
# Run all retriever in one go, to have
# complete xml-file for easier tests.
#

include ("RecExCommon/myTopOptions.py")
#
include( "JiveXML/JiveXML_jobOptionBase.py" )
include ("JiveXML/DataTypes_All.py")
## to call absolutely all retrievers incl Geometry:
theEventData2XML.WriteGeometry = True

