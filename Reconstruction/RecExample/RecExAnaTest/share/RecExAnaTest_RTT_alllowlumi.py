include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

# For 2 10**33 luminosity - KAA
from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.override=False
jobproperties.Beam.bunchSpacing=25
jobproperties.Beam.numberOfCollisions=4.6


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")

