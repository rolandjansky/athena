include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

# For 2 10**33 luminosity - KAA
from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.override=False
jobproperties.Beam.bunchSpacing=25
jobproperties.Beam.numberOfCollisions=4.6

rec.doCBNT=False
rec.doHist=False

include ("RecExCommon/rdotoesdnotrigger.py")

include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")


