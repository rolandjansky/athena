###############################################################
#
# Job options file
# This fragment creates GeoModel service and loads TileGeoModel
# It should not be used if more than one detector needs to be
# loaded. In this case use AtlasGeoModel/GeoModelInit instead
#
#==============================================================
if not 'TILEGEOMODEL_TILEGEOMODEL_HAS_BEEN_INCLUDED' in dir():
   TILEGEOMODEL_TILEGEOMODEL_HAS_BEEN_INCLUDED = 1

   from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
   DetDescrCnvSvc = DetDescrCnvSvc()
   DetDescrCnvSvc.IdDictFromRDB = True

   # Conditions DB setup and TagInfo
   from IOVDbSvc.CondDB import conddb
   import EventInfoMgt.EventInfoMgtInit

   # Load RDBAcessSvc (Access to parameter Database)
   #include ("RDBAccessSvc/RDBAccessSvcPdb_jobOptions.py")
    
   from AthenaCommon.AppMgr import ServiceMgr
   from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
   GeoModelSvc = GeoModelSvc()
   ServiceMgr += GeoModelSvc
   theApp.CreateSvc += [ "GeoModelSvc"]

   #load all identifiers
   #include( "TileIdCnv/TileIdCnv_jobOptions.py" )
   #include( "CaloIdCnv/CaloIdCnv_joboptions.py" )

   from TileGeoModel.TileGeoModelConf import TileDetectorTool
   GeoModelSvc.DetectorTools += [ TileDetectorTool() ]

   from AthenaCommon.GlobalFlags import jobproperties
   detDescrVersion = jobproperties.Global.DetDescrVersion()

   # Setup geometry version
   GeoModelSvc.AtlasVersion = detDescrVersion
   # for TileCal all versions are identical
   # so let's load just the DC3 default
   GeoModelSvc.TileVersionOverride = "TileCal-DC3-00"

   if (detDescrVersion.startswith("ctb") or
       detDescrVersion.startswith("CTB") or
       detDescrVersion.startswith("ATLAS-CTB") ) :
      GeoModelSvc.TileDetectorTool.TestBeam = TRUE 
      GeoModelSvc.AtlasVersion = "ATLAS-CTB-00"
      GeoModelSvc.TileVersionOverride = "TileTB-3B3EB-00"

###############################################################
#
# End of job options file
#
###############################################################
