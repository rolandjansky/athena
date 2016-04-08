# MuonGeometry.py
#
# David Adams
# March 2007
# January 2008 - Modify so that a default geometry version is assigned for release
# 12 and later if DetDescrVersion is undefined or is set to "default".
#
# Job options to set up the muon geometry.

include("CscValidationUtil/MuAtlasRelease.py")

# Most up-to-date (working) version
if atrel1<13:
  gopt = 3
elif atrel1==13 and atrel2==0:
  gopt = 3
elif atrel1==14 and atrel2>=1:
  gopt = 5
else:
  gopt = 6

# Set geometry version for release 12.
if atrel1==12:
  DefaultDetDescrVersion = "ATLAS-CSC-01-02-00"
elif atrel1==13:
  DefaultDetDescrVersion = "ATLAS-CSC-02-00-00"
elif atrel1==14:
  DefaultDetDescrVersion = "ATLAS-CSC-02-01-00"
else:
  DefaultDetDescrVersion = "UNDEFINED"

# Use release default if geometry version is not specified.
if not "DetDescrVersion" in vars() or DetDescrVersion=="default" or DetDescrVersion=="":
  DetDescrVersion = DefaultDetDescrVersion
  print "MuonGeometry.py: Using default geometry: " + DetDescrVersion

print "MuonGeometry.py: Using geometry option " + str(gopt) + " with version " + DetDescrVersion

if not "MuonGeometryUseCalo" in vars():
  MuonGeometryUseCalo = False

if not "MuonGeometryUseCaloCond" in vars():
  MuonGeometryUseCaloCond = False

print "MuonGeometry.py: MuonGeometryUseCalo: " + str(MuonGeometryUseCalo)
print "MuonGeometry.py: MuonGeometryUseCaloCond: " + str(MuonGeometryUseCaloCond)

# January 2008
# Configurable change in release 13.2.0 (and earlier?).
if gopt == 6:
  print "MuonGeometry.py: Using RecExCond detector description."
  include("RecExCond/AllDet_detDescr.py")
elif gopt == 5:
  if MuonGeometryUseCaloCond:
    print "MuonGeometry.py: Using RecExCommon detector description."
    include("RecExCommon/AllDet_detDescr.py")
  else:
    from AthenaCommon.GlobalFlags import globalflags
    # Set geometry version.
    if len(DetDescrVersion) and DetDescrVersion != "UNDEFINED":
      globalflags.DetDescrVersion = DetDescrVersion
    from AtlasGeoModel import SetGeometryVersion
    print "MuonGeometry.py: Global flags:"
    print globalflags
    # Set subdetector flags.
    # Flags are off by default. Set them *before* initialization.
    from AthenaCommon.DetFlags import DetFlags
    #DetFlags.detdescr.all_setOn()
    DetFlags.detdescr.ID_setOn()
    DetFlags.detdescr.Muon_setOn()
    DetFlags.detdescr.LAr_setOff()
    DetFlags.detdescr.Tile_setOff()
    # Need calo geometry to read cells.
    if atrel1 >= 13:
      DetFlags.detdescr.LAr_setOn()
      DetFlags.detdescr.Calo_setOn()
      DetFlags.detdescr.Tile_setOn()
    # Initialize geometry.
    from AtlasGeoModel import GeoModelInit
    print "MuonGeometry.py: Initialization completed."

# Sep07: Changes from Woo Chun.
# Dec07: Enable ID as needed for release 13 AOD conversion.
elif gopt == 4:
  from DetDescrCnvSvc.DetDescrCnvSvcConf import AtlasGeoModel
  from AthenaCommon.GlobalFlags import GlobalFlags
  from AthenaCommon.JobProperties import jobproperties
  DDversion = jobproperties.Global.DetDescrVersion()
  if (DDversion == "ATLAS-CSC-00-00-00"):
    jobproperties.Global.DetDescrVersion = "ATLAS-CSC-01-02-00"
  GlobalFlags.DetGeo.set_atlas()
  GlobalFlags.DataSource.set_geant4()
  from AthenaCommon.DetFlags import DetFlags
  DetFlags.detdescr.Muon_setOn()
  if atrel1 >= 13:
    DetFlags.detdescr.ID_setOn()        
  from AtlasGeoModel import SetGeometryVersion
  from AtlasGeoModel import GeoModelInit
  from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
  GeoModelSvc = GeoModelSvc()

# March 30, 2007 prescription from Thijs.        
# Then, updated on Oct 28, 2007
# Dec07: Enable ID as needed for release 13 AOD conversion.
# May08: If conditions are requested, used RecExCommon to load
# all geometry and conditions.
elif gopt == 3:   
  if MuonGeometryUseCaloCond:
    print "MuonGeometry.py: Using RecExCommon detector description."
    include("RecExCommon/AllDet_detDescr.py")
  else:
    from AthenaCommon.GlobalFlags import GlobalFlags      
    GlobalFlags.DetGeo.set_atlas()        
    GlobalFlags.DataSource.set_geant4()   
    from AthenaCommon.DetFlags import DetFlags    
    DetFlags.detdescr.Muon_setOn()        
    if atrel1 >= 13:
      DetFlags.detdescr.ID_setOn()        
    # Need calo geometry to read cells.
    if atrel1 >= 13 and MuonGeometryUseCalo:
      DetFlags.detdescr.LAr_setOn()
      DetFlags.detdescr.Calo_setOn()
      DetFlags.detdescr.Tile_setOn()
      if  MuonGeometryUseCaloCond:
        include( "LArIdCnv/LArIdCnv_joboptions.py" )
        include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
        include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
        include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
        include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" ) 
        include( "TileConditions/TileConditions_jobOptions.py" )
        include( "TileIdCnv/TileIdCnv_jobOptions.py" )
    include("AtlasGeoModel/SetGeometryVersion.py")        
    include("AtlasGeoModel/GeoModelInit.py")

# March 30, 2007 prescription from Stefania.
elif gopt == 2:
  include ( "DetDescrCnvSvc/DetStore_joboptions.py" )
  DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
  DetDescrCnvSvc.IdDictFromRDB = True
  include ( "AtlasGeoModel/SetGeometryVersion.py" )
  include ("RDBAccessSvc/RDBAccessSvcPdb_jobOptions.py")
  theApp.Dlls += [ "GeoModelSvc" ]
  theApp.CreateSvc += [ "GeoModelSvc"]
  include( "AtlasGeoModel/MuonGeoModel.py" )

# Use RecExCommon
else:
  include("RecExCond/RecExCommon_flags.py")
  DetFlags.detdescr.ID_setOff()
  DetFlags.detdescr.LAr_setOff()
  DetFlags.detdescr.Calo_setOff()
  DetFlags.detdescr.Tile_setOff()
  DetFlags.detdescr.Muon_setOn()
  include("RecExCond/AllDet_detDescr.py")
  #GeoModelSvc.IgnoreTagDifference = True

