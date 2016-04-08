include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

rec.doTrigger=True
rec.doWriteESD=False # uncomment if do not write ESD

include ("RecExCommon/rdotoesd.py")

include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")
