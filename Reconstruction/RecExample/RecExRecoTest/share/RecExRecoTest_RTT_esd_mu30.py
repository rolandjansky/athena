include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

# For 2 10**33 luminosity - KAA
from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.override=False
jobproperties.Beam.bunchSpacing=50
jobproperties.Beam.numberOfCollisions=30.0

from CaloTools.CaloNoiseFlags import jobproperties
jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(1.45*30/8)

rec.doCBNT=False
rec.doHist=False

include ("RecExCommon/rdotoesdnotrigger.py")

include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")


