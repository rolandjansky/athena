from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTIDE_Ambi.set_Value_and_Lock(False)
InDetFlags.doPixelPRDFormation.set_Value_and_Lock(False)
InDetFlags.doSCT_PRDFormation.set_Value_and_Lock(True)
InDetFlags.doTRT_PRDFormation.set_Value_and_Lock(True)

from AthenaCommon.DetFlags import DetFlags
DetFlags.makeRIO.pixel_setOff()
DetFlags.readRIOPool.pixel_setOn()
DetFlags.haveRIO.pixel_on()
