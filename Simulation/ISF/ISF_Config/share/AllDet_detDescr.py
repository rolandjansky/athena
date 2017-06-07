# include so that alldet_detdescr can be used directly from somewhere else
# (there is an include.block so that it is only loaded once)
include.block("ISF_Config/AllDet_detDescr.py")
# also block alternatives
include.block("RecExCond/AllDet_detDescr.py")
include.block("RecExCommon/AllDet_detDescr.py")

from AthenaCommon.Resilience import protectedInclude
from AthenaCommon.DetFlags import DetFlags
if DetFlags.detdescr.any_on():
    from AtlasGeoModel import SetGeometryVersion
    from AtlasGeoModel import GeoModelInit
    from AtlasGeoModel import SetupRecoGeometry

    # Beam Spot service job options - just defines conditions data access
    if DetFlags.detdescr.ID_on():
        protectedInclude("InDetBeamSpotService/BeamCondSvc.py" )

    import os
    if "AthSimulation_DIR" not in os.environ:
        if DetFlags.detdescr.LAr_on():
            protectedInclude( "LArDetDescr/LArDetDescr_joboptions.py" )

        if DetFlags.detdescr.Tile_on():
            protectedInclude( "TileConditions/TileConditions_jobOptions.py" )

        if DetFlags.detdescr.Muon_on():
             protectedInclude ("AmdcAth/AmdcAth_jobOptions.py")

    # MagneticField Service
    if DetFlags.detdescr.BField_on():
        import MagFieldServices.SetupField
