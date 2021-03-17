# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
###############################################################
#
# Job options file for CaloNoise2Ntuple
#
#==============================================================

# configuration for data

if 'RunNumber' not in dir():
   RunNumber = 258914
if 'LumiBlock' not in dir():
   LumiBlock = 1
if 'GlobalTag' not in dir():
   GlobalTag =  'CONDBR2-BLKPA-2015-09'
if 'Geometry' not in dir():
   Geometry = 'ATLAS-GEO-21-00-01'
if 'outputNtuple' not in dir():
   outputNtuple="cellcorr.root"
if 'InputDB' not in dir():
   InputDB="COOLOFL_LAR/CONDBR2"

from RecExConfig.RecFlags import rec
rec.RunNumber.set_Value_and_Lock(RunNumber)


from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
from AthenaCommon.Resilience import protectedInclude
protectedInclude( "PerfMonComps/PerfMonSvc_jobOptions.py" )

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.LAr_setOn()
DetFlags.Tile_setOn()
DetFlags.digitize.all_setOff()

from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('data')

from CaloTools.CaloNoiseFlags import jobproperties
jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(-1.)

import AthenaCommon.AtlasUnixGeneratorJob

# Get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

# Setup Db stuff
import AthenaPoolCnvSvc.AthenaPool

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion=Geometry

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include("TileConditions/TileConditions_jobOptions.py" )
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

conddb.addFolder("","<db>"+InputDB+"</db>/LAR/CellCorrOfl/EnergyCorr")
if "folderTag" in dir():
   conddb.addOverride("/LAR/CellCorrOfl/EnergyCorr",folderTag)

svcMgr.IOVDbSvc.GlobalTag = GlobalTag

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from CaloCondPhysAlgs.CaloCondPhysAlgsConf import CaloCellEnergyCorr2Ntuple
theCalo2Ntuple = CaloCellEnergyCorr2Ntuple("CaloCellEnergyCorr2Ntuple")
theCalo2Ntuple.InputKey="/LAR/CellCorrOfl/EnergyCorr"
topSequence += theCalo2Ntuple

#--------------------------------------------------------------
#--- Dummy event loop parameters
#--------------------------------------------------------------
svcMgr.EventSelector.RunNumber         = RunNumber
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = LumiBlock
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
svcMgr.EventSelector.OverrideRunNumber=True
theApp.EvtMax                          = 1


#  ------------------------------------------------------------------
# ---  Ntuple 
# ------------------------------------------------------------------
if not hasattr(ServiceMgr, 'THistSvc'):
   from GaudiSvc.GaudiSvcConf import THistSvc
   ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output  = ["file1 DATAFILE='"+outputNtuple+"' OPT='RECREATE'"]


#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
svcMgr.MessageSvc.debugLimit       = 100000
svcMgr.MessageSvc.infoLimit        = 100000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"
svcMgr.IOVDbSvc.OutputLevel        = INFO
