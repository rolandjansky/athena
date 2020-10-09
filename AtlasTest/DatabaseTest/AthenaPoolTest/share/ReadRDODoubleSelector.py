###############################################################
#
# Job options file
#
## @file ReadRDODoubleSelectror.py
##
## @brief For Athena POOL test: read two RDOs concurrently and write out a combined one
##
## @author Miha Muskinja <miha.muskinja@cern.ch>
#
#==============================================================

## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

# Set that we are running MC+MC overlay in MT mode
from OverlayCommonAlgs.OverlayFlags import overlayFlags
overlayFlags.isDataOverlay.set_Value_and_Lock(False)
overlayFlags.isOverlayMT.set_Value_and_Lock(True)

#--------------------------------------------------------------
# Load POOL support for DoubleEventSelector
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPoolDouble

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
DetFlags.detdescr.ID_setOn()
DetFlags.detdescr.Calo_setOn()
DetFlags.detdescr.Tile_setOff()
DetFlags.detdescr.Muon_setOff()
      
# set up all detector description description 
include ("RecExCond/AllDet_detDescr.py")

# the correct tag should be specified
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("OFLCOND-SDR-BS7T-04-00")

#--------------------------------------------------------------
# Input options
#--------------------------------------------------------------

# Add in DoubleEventSelector
svcMgr.DoubleEventSelector.InputCollections = [ "LArRDO.root" ]
svcMgr.SecondaryEventSelector.InputCollections = [ "InDetRDO.root" ]

svcMgr.DoubleEventSelector.OutputLevel = DEBUG
svcMgr.SecondaryEventSelector.OutputLevel = DEBUG

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application: AthenaPoolTest InDetRawData options
#--------------------------------------------------------------
from AthenaPoolTest.AthenaPoolTestConf import RDOReaderDoubleSelector
topSequence += RDOReaderDoubleSelector( "RDOReaderDoubleSelector" )
RDOReaderDoubleSelector.OutputLevel = DEBUG

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1", noTag=True )

# Run OutputStream as an algorithm
topSequence += AthenaPoolOutputStream( "Stream1", noTag=True )
# Must make sure that no OutStream's have been declared
theApp.OutStream     = []; 
# Stream's output file
Stream1.WritingTool = "AthenaOutputStreamTool"
Stream1.OutputFile  = "CombinedRDO.root"
# List of DO's to write out
Stream1.ItemList   += ["EventInfo#*"]
Stream1.ItemList   += ["CaloCellContainer#*"]
Stream1.ItemList   += ["PixelRDO_Container#*"]
Stream1.ItemList   += ["SCT_RDO_Container#*"]
Stream1.ItemList   += ["TRT_RDO_Container#*"]
Stream1.ItemList   += ["PixelRDOElemLinkVec#*"]

printfunc (Stream1.ItemList)


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = WARNING
svcMgr.MessageSvc.debugLimit  = 100000

from AthenaServices import AthenaServicesConf
AthenaEventLoopMgr = AthenaServicesConf.AthenaEventLoopMgr()
AthenaEventLoopMgr.OutputLevel = INFO
AthenaEventLoopMgr.UseSecondaryEventNumber = True
svcMgr += AthenaEventLoopMgr

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
