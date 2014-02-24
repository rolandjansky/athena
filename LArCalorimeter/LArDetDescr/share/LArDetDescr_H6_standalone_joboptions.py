print "building the LAr readout geometry ALONE ( no other subdetector )"
    
# make sure that other subdet are not built
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.LAr_setOn()

include("LArDetDescr/LArDetDescr_H6_joboptions.py")

