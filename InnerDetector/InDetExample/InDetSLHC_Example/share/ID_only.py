#
#    Setup just the detectors we want
#    File to be execfile'd from all jobOptions
#
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
# DetFlags.pixel_setOff()
# DetFlags.SCT_setOff()
DetFlags.TRT_setOff()

DetFlags.AFP_setOff()
DetFlags.ALFA_setOff()
DetFlags.BCM_setOff()
DetFlags.ZDC_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

DetFlags.Truth_setOn()

DetFlags.BField_on()
