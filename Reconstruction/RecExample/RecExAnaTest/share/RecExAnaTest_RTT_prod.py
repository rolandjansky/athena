include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

from RecExConfig.RecFlags import rec
rec.Production=True


# main jobOption
include ("RecExCommon/RecExCommon_flags.py")

# mof for default production
AODFlags.FastTrackParticle=False

include ("RecExCommon/RecExCommon_topOptions.py")

include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")
