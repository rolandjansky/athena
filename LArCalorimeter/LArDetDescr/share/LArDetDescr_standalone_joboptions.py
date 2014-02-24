print "building the LAr readout geometry ALONE ( no other subdetector )"

# make sure that Global flags are there : if not, assume MC
if not "GlobalFlags" in dir():
    from AthenaCommon.GlobalFlags import GlobalFlags
    GlobalFlags.DetGeo.set_atlas()
    GlobalFlags.DataSource.set_geant4()
       
# make sure that other subdet are not built
if not "DetFlags" in dir():
    from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.LAr_setOn()
DetFlags.detdescr.Tile_setOn()

# setup GeoModel
include( "AtlasGeoModel/SetGeometryVersion.py" )
include( "AtlasGeoModel/GeoModelInit.py" )

include("LArDetDescr/LArDetDescr_joboptions.py")

