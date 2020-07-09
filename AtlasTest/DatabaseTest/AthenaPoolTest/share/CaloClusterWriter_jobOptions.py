###############################################################
#
# Job options file
#
## @file CaloClusterWriter_jobOptions.py
##
## @brief For Athena POOL test: write out CaloClusterContainers
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
#
#==============================================================

# MC Event Selector
## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Set flags and load det descr
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.DetFlags     import DetFlags
from RecExConfig.RecFlags      import rec

# For general flags
rec.doAOD       = False
rec.doTrigger   = False
rec.doWriteTAG  = False
DetDescrVersion = "ATLAS-GEO-17-00-00"

# Set local flags - only need LAr DetDescr
DetFlags.detdescr.Calo_setOn()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.detdescr.Muon_setOff()
      
# set up all detector description description 
include ("RecExCond/AllDet_detDescr.py")

# the correct tag should be specified
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("OFLCOND-SDR-BS7T-04-00")

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20
#--------------------------------------------------------------
# Application: AthenaPoolTest InDetRawData options
#--------------------------------------------------------------

from AthenaPoolTest.AthenaPoolTestConf import LArCellContFakeWriter
topSequence += LArCellContFakeWriter( "LArCellContFakeWriter" )

from AthenaPoolTest.AthenaPoolTestConf import CaloClusterFakeWriter
topSequence += CaloClusterFakeWriter( "CaloClusterFakeWriter" )

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------

# Stream's output file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile =   "SimplePoolFile.root"
# List of DO's to write out
Stream1.ItemList+=["CaloCellContainer#*"]
Stream1.ItemList+=["CaloCellLinkContainer#*"]
Stream1.ItemList+=["CaloClusterContainer#*"]
Stream1.ItemList+=["CaloShowerContainer#*"]
Stream1.ItemList+=["EventInfo#*"]
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.OutputLevel     = WARNING
svcMgr.MessageSvc.debugLimit      = 100000
svcMgr.MessageSvc.errorLimit      = 100000
#svcMgr.ClassIDSvc.OutputLevel     = DEBUG
LArCellContFakeWriter.OutputLevel = DEBUG
CaloClusterFakeWriter.OutputLevel = DEBUG

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
