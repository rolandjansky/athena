
from RecExConfig.RecFlags  import rec
rec.doTruth.unlock()
rec.doTruth.set_Value_and_Lock(True)

from AthenaCommon.GlobalFlags import globalflags
globalflags.isOverlay.unlock()
globalflags.isOverlay.set_Value_and_Lock(True)

#To be more like MC
#from MuonRecExample.MuonRecFlags import muonRecFlags
#muonRecFlags.doSegmentT0Fit.set_Value_and_Lock(False)

#Since we already made digits during overlay
from LArROD.LArRODFlags import larRODFlags
larRODFlags.readDigits = False
from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.LArCoolChannelSelection.set_Value_and_Lock("")
#larCondFlags.OFCShapeFolder.set_Value_and_Lock("")

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArHVCorr.set_Value_and_Lock(False)
muonRecFlags.writeSDOs=True
rec.RunNumber=0
rec.Commissioning.set_Value_and_Lock(True)
jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0)
from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArCellEmMisCalib=False
include.block("TrigT2CaloCommon/LArCellBuilding.py")
