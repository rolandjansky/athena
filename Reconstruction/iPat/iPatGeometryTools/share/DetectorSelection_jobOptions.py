###############################################################
#
# DetectorSelection job options file
#
#==============================================================

# transmit detectors to be used for InDet reconstruction
from AthenaCommon.DetFlags import DetFlags
ToolSvc = Service( "ToolSvc" )
ToolSvc.DetectorSelection.processPixels = DetFlags.haveRIO.pixel_on()
ToolSvc.DetectorSelection.processSCT    = DetFlags.haveRIO.SCT_on()
ToolSvc.DetectorSelection.processTRT    = DetFlags.haveRIO.TRT_on()

#==============================================================
#
# End of DetectorSelection file
#
###############################################################
