include ("RecExRecoTest/RecExRecoTest_RTT_common.py")
rec.doTrigger=False
rec.AutoConfiguration = ['FieldAndGeo']

include ("RecExCommon/RecExCommon_flags.py")


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")

