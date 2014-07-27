
from TileRecUtils.TileRecFlags import jobproperties
jobproperties.TileRecFlags.readDigits.set_Value_and_Lock(False)

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.doLArAutoConfiguration.set_Value_and_Lock(False)

from LArROD.LArRODFlags import jobproperties
jobproperties.LArRODFlags.readDigits.set_Value_and_Lock(False)
jobproperties.LArRODFlags.readRawChannels.set_Value_and_Lock(True)

from CaloRec.CaloCellFlags import jobproperties;
jobproperties.CaloCellFlags.doDeadCellCorr.set_Value_and_Lock(False)

