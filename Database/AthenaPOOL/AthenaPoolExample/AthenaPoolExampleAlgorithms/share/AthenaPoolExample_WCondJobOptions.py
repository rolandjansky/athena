## @file AthenaPoolExample_WCondJobOptions.py
## @brief Example job options file to illustrate how to write conditions data to Pool.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: AthenaPoolExample_WCondJobOptions.py,v 1.19 2008-04-24 15:42:18 gemmeren Exp $
###############################################################
#
# This Job option:
# ----------------
# 1. Reads the data from the SimplePoolFile1.root file
#    had been written with the AthneaPoolExample_WriteJobOptions.py
# 2. Writes another SimplePoolFile4.root ROOT file using WriteCond algorithm
#    which contains conditions for the event sample (eg pedestals).
# ------------------------------------------------------------
# Expected output file (20 events):
# -rw-r--r--    1 gemmeren zp          16552 Dec  8 19:07 SimplePoolFile4.root
#
#==============================================================

## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 200000

#--------------------------------------------------------------
#---   Read portion  -----
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

svcMgr.EventSelector.InputCollections = [ "SimplePoolFile1.root" ]; # ** mandatory parameter ** // The input file name

#Explicitly specify the output file catalog
svcMgr.PoolSvc.WriteCatalog = "file:Catalog1.xml"

#--------------------------------------------------------------
#---   Secondary Write portion  ----- Don't change it !!!
#--------------------------------------------------------------
# Load the OutputStream
from OutputStreamAthenaPool.OutputStreamAthenaPool import AthenaPoolOutputConditionStream
Stream1 = AthenaPoolOutputConditionStream( "Stream1", "SimplePoolFile4.root" )
Stream1.ObjectList += [ "ExampleHitContainer#PedestalWriteData" ]

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__ReadData,AthPoolEx__WriteCond
topSequence += AthPoolEx__ReadData("ReadData")
from StoreGate.StoreGateConf import StoreGateSvc
topSequence += AthPoolEx__WriteCond( "WriteCond", DetStore  = StoreGateSvc( "DetectorStore" ),
                                     ConditionName = "PedestalWriteData",
                                     Weight = 0.0,
                                     Offset = 0.0 )

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3

svcMgr.EventSelector.OutputLevel = 2
svcMgr.AthenaPoolAddressProviderSvc.OutputLevel = 2
svcMgr.MetaDataSvc.OutputLevel = 2
svcMgr.PoolSvc.OutputLevel = 2
topSequence.ReadData.OutputLevel = 2
topSequence.WriteCond.OutputLevel = 2
Stream1.OutputLevel = 2

#
# End of job options file
#
###############################################################
