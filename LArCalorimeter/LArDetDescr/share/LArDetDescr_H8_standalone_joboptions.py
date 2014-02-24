print "building the LAr readout geometry ALONE ( no other subdetector )"

# make sure that Global flags are there : if not, assume MC
if not "GlobalFlags" in dir():
    from AthenaCommon.GlobalFlags import GlobalFlags
    GlobalFlags.DetGeo.set_ctbh8()
    GlobalFlags.DataSource.set_geant4()

# setup GeoModel : --- veryvery temporarily done in LArDetDescr_H8_joboptions.py
# include( "AtlasGeoModel/SetGeometryVersion.py" )
# include( "AtlasGeoModel/GeoModelInit.py" )

# make sure that other subdet are not built
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.LAr_setOn()
DetFlags.detdescr.Tile_setOn()

include("LArDetDescr/LArDetDescr_H8_joboptions.py")
