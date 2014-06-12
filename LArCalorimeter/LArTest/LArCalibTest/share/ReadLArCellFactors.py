#from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_atlas() 
#GlobalFlags.Luminosity.set_zero()
#GlobalFlags.DataSource.set_geant4()
#GlobalFlags.InputFormat.set_pool()


include( "RecExCond/RecExCommon_flags.py" )

# Set local flags - only need LAr DetDescr
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.detdescr.Muon_setOff()

include( "AthenaCommon/Atlas.UnixStandardJob.py" )

include( "DetDescrCnvSvc/DetStore_joboptions.py" )

include ("RecExCond/AllDet_detDescr.py")

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
#DetDescrCnvSvc.DetectorManagers+= ["CaloTTDescrManager"]
#DetDescrCnvSvc.DetectorManagers += [ "CaloMgr" ]

include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )
theApp.Dlls+=["LArCalibTest"]
theApp.TopAlg+=["LArFakeCellFactor"]

LArFakeCellFactor=Algorithm("LArFakeCellFactor");
LArFakeCellFactor.CorrectionKey="ZeeCalib"
LArFakeCellFactor.InputFileName="zone2zoneCorrFactors.out"
#LArFakeCellFactor.OutputLevel=DEBUG


# -------- Service tuning -----------
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DoIdChecks

ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRoI_Map.Print=FALSE

# Number of events to be processed (default is 10)
theApp.EvtMax = 0

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
MessageSvc.defaultLimit=1000000
# Don't print event number each event
#AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
#AthenaEventLoopMgr.OutputLevel = INFO

# For dumping SG
## StoreGateSvc = Service( "StoreGateSvc" )
## StoreGateSvc.Dump = TRUE
## StoreGateSvc.OutputLevel = INFO

## DetectorStore = Service( "DetectorStore" )
## DetectorStore.Dump = TRUE
## DetectorStore.OutputLevel = INFO












