include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

rec.doWriteESD=False
rec.doWriteAOD=False
rec.doWriteTAG=False

rec.doTrigger=False
# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")
