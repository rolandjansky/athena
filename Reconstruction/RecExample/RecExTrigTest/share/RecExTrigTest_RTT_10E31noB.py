include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

rec.doTrigger=True
include("RecExCommon/RecExCommon_flags.py")
TriggerFlags.triggerMenuSetup = 'lumi1E31_no_Bphysics'

# main jobOption
include ("RecExCommon/rdotoesdprod.py")


include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")
