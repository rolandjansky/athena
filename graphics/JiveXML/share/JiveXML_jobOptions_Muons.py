jp.Rec.doTrigger=False
jp.Rec.doCBNT=False
jp.Rec.doWriteRDO=False
jp.Rec.doWriteESD=False
jp.Rec.doWriteAOD=False
jp.Rec.doWriteTAG=False
jp.Rec.doAOD=False

## fourth event in standard RecExCommon test
## contains no muons, run only first three
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax.set_Value_and_Lock(3)

include ("RecExRecoTest/RecExRecoTest_RTT_muon.py")

include( "JiveXML/JiveXML_jobOptionBase.py" )
include ("MuonJiveXML/MuonJiveXML_DataTypes.py")
