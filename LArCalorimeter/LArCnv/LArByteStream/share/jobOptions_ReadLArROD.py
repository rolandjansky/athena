#**************************************************************
#
# jopOptions file in LArByteStream package
#       Read in ByteStream events, and Access LArRawChannel
# 
#==============================================================
# eformat input
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

# EventStorage Input 
ByteStreamInputSvc =  svcMgr.ByteStreamInputSvc
EventSelector.Input = ["daq.csc12.0000000.Single.Stream.LB0000.Athena._0001.data"]
include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DataSource.set_geant4()
GlobalFlags.InputFormat.set_bytestream()
GlobalFlags.DetGeo.set_atlas()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion="ATLAS-CSC-01-02-00"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )
include ("CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "TileConditions/TileConditions_jobOptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )


# Read LArRawChannel 
include( "LArByteStream/ReadLArBS_jobOptions.py" )

# ToolSvc.LArRodDecoder.Print=true; 
# ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )
# ByteStreamCnvSvc.InitCnvs += [  "LArRawChannelContainer",  
#				"LArRawChannelCollection" ] 
# theApp.ExtSvc += [ "ByteStreamCnvSvc"] 
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel      = INFO
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 200
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("ChronoAuditor")()
theApp.AuditAlgorithms=True
theApp.AuditServices=True
theApp.AuditTools=True
