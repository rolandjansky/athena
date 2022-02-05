jp.Rec.doWriteESD=False
jp.Rec.doWriteTAG=False

include ("RecExCond/RecExCommon_flags.py")

#### Use standard input testfile from RecExCommon, not this.
#### This was used in other RTT jOs:
#PoolRDOInput = ["/afs/cern.ch/atlas/offline/ReleaseData/v2/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"]

include ("RecExCommon/myTopOptions.py")

include( "JiveXML/JiveXML_jobOptionBase.py" )
include ("RecJiveXML/RecJiveXML_DataTypes.py")
### For debugging:
#theJetRecJetRetriever.OutputLevel = DEBUG
