
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

