#**************************************************************
#
# L2muCalibTest example
#
#==============================================================
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import *

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from TrigExL2muCalibTest.TrigExL2muCalibTestConf import L2muCalibTest
HelloWorld = L2muCalibTest("HelloWorld")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += HelloWorld
#
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
#
#--- Allow overwrite of muon calibration buffer name from OKS/
#--- online configuration
#
HelloWorld.AllowOksConfig = TRUE
#
#--- dump the property catalogue (when AllowOksConfig = TRUE) 
#
HelloWorld.DumpPropertyCatalogue = TRUE
#
#--- define the muon calibration buffer name and size
#
HelloWorld.MuonCalBufferName = "file://dummy-file-from-py-jobOptions"
HelloWorld.MuonCalBufferSize = 12345678
#
#==============================================================
#
# End of L2muCalibTest example
#
#**************************************************************
