# include so that alldet_detdescr can be used directly from somewhere else
# (there is an include.block so that it is only loaded once) 
include.block("RecExCond/AllDet_detDescr.py")

include ("RecExCond/RecExCommon_DetFlags.py")

from AthenaCommon.Logging import logging
from AthenaCommon.Resilience import treatException,protectedInclude
from RecExConfig.RecFlags import rec

logAllDet_detDescr = logging.getLogger( 'AllDet_detDescr' )


if DetFlags.detdescr.any_on():
    # if an detdescr flag is on, then configure appropriately the
    # StoreGateSvc/DetectorStore and the DetDescrCnvSvc
    import DetDescrCnvSvc.DetStoreConfig

    # guess geometry version from globalflags
    # see AtlasGeomodel/SetGeometryVersion.py for the possibilities
    # e.g. DetDescrVersion="Rome-Initial"

    from AtlasGeoModel import SetGeometryVersion
    from AtlasGeoModel import GeoModelInit
    from AtlasGeoModel import SetupRecoGeometry

    if DetFlags.detdescr.Tile_on() and not rec.doAODMerging():
        
        protectedInclude( "TileConditions/TileConditions_jobOptions.py" )
        


    if DetFlags.detdescr.Calo_on() and not rec.doAODMerging():
        protectedInclude( "CaloIdCnv/CaloIdCnv_joboptions.py" )
        #FIXME tile and lar not independent
        protectedInclude( "TileIdCnv/TileIdCnv_jobOptions.py" )


    if DetFlags.detdescr.LAr_on() and not rec.doAODMerging():
        try:
            include( "LArDetDescr/LArDetDescr_joboptions.py" )
        except Exception:
            DetFlags.Calo_setOff()
            treatException("could not load LArDetDescr_jobOptions. Calo switched off")            



    from AthenaCommon.AppMgr import ServiceMgr 
    import DetDescrCnvSvc.DetStoreConfig
    ServiceMgr.DetDescrCnvSvc.DecodeIdDict = TRUE 



# Beam Spot service (only if ID requested)
    from AthenaCommon.GlobalFlags import globalflags
    # use even if commisisoning and not globalflags.DetGeo=='commis' :
    if DetFlags.detdescr.ID_on() and not rec.doAODMerging():
        protectedInclude("InDetBeamSpotService/BeamCondSvc.py" )        


    if DetFlags.detdescr.Muon_on() and not rec.doAODMerging():
        protectedInclude ("AmdcAth/AmdcAth_jobOptions.py")

        #if DetDescrVersion[0:3]=="DC2":
        from AthenaCommon.GlobalFlags import globalflags
        if globalflags.DetDescrVersion()[0:3]=="DC2":
            #FIXME should not be there
            MDTcablingSvc = Service ("MDTcablingSvc")
            MDTcablingSvc.RODfile = "P03_RODmap.data"
            RPCcablingSvc = Service ("RPCcablingSvc")
            RPCcablingSvc.ConfFileName = "P03conf.data"
            RPCcablingSvc.CorrFileName = "P03conf.corr"



    # MagneticField Service
    if DetFlags.detdescr.BField_on() and not rec.doAODMerging():
        #protectedInclude( "BFieldAth/BFieldAth_jobOptions.py" )
        import MagFieldServices.SetupField


