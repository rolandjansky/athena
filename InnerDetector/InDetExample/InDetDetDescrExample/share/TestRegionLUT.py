###############################################################
#
# Job options file
#
#==============================================================

from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.DetFlags import DetFlags

#
#==============================================================
# Load Detector Description for Pixel and SCT and TRT.
#==============================================================

#DetFlags.detdescr.all_setOn()
DetFlags.detdescr.ID_setOn()
#DetFlags.detdescr.pixel_setOn()
#DetFlags.detdescr.SCT_setOn()
#DetFlags.detdescr.TRT_setOn()

GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DetGeo.set_ctbh8()
#GlobalFlags.DetGeo.set_commis()
#DetFlags
GlobalFlags.DataSource.set_geant4()
#GlobalFlags.DataSource.set_data()

# Select the geometry version.
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion='ATLAS-CSC-02-00-00'

# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# Load the tables. This currently runs the test program as
# a temporary way to preload the tables, otherwise we would need the following lines.

from AthenaCommon.AlgSequence import AlgSequence
include("InDetRegionSelector/InDetRegionSelectorTable_jobOptions.py")

from InDetRegionSelector.InDetRegionSelectorConf import InDetDD__InDetRegionSelectorLUT
InDetRegionSelectorLUT = InDetDD__InDetRegionSelectorLUT(name                       = "InDetRegionSelectorLut",
                                                         DoTests                    = True)


topSequence = AlgSequence()
topSequence += InDetRegionSelectorLUT
print          InDetRegionSelectorLUT

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
topSequence.InDetRegionSelectorLut.OutputLevel = DEBUG

# To set global output level use
# athena -l DEBUG

#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10

#==============================================================
#
# End of job options file
#
###############################################################
