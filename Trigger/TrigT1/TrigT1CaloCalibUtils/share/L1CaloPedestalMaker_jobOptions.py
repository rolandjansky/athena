#---------------------------------------------
# To compute pedestals
#
# To be included in RecExCommission_topOptions.py
#
#---------------------------------------------


L1CaloPedestalsMaker_Store2COOL = False
L1CaloPedestalsMaker_RecreateCOOLFolders = False


# -------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


# -------------------------------------------------------------
# L1CaloPedestalMaker
# -------------------------------------------------------------

from TrigT1CaloCalibUtils.TrigT1CaloCalibUtilsConf import L1CaloPedestalMaker
l1CaloPedestalMaker = L1CaloPedestalMaker("L1CaloPedestalMaker")
topSequence += l1CaloPedestalMaker

#l1CaloPedestalMaker.OutputLevel = DEBUG

# Input TriggerTowerContainer
l1CaloPedestalMaker.TriggerTowerLocation = "TriggerTowers"

# Number of events to compute window range
l1CaloPedestalMaker.NRefEvents = 50

# TDS key of the output pedestal container
l1CaloPedestalMaker.PedestalsKey = "L1CaloPedestals"

if L1CaloPedestalsMaker_Store2COOL:
	# to write out conditions objects
	# should remain false - not working at the moment
	l1CaloPedestalMaker.Store2POOL = False

	# to register conditions objects in the IOVDB
	l1CaloPedestalMaker.Store2COOL = True

# Set DB properties
L1CaloPedestalMaker.DBFolder = "/M3/L1CaloPedestals"
L1CaloPedestalMaker.DBTag = "COOL-TEST-001"

#--------------------------------------------------------------
# CaloTriggerTowerService
#--------------------------------------------------------------

#required for use of the CaloTriggerTowerService
include( "CaloConditions/CaloConditions_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )


###########################################################################
#
# POOL settings
#
###########################################################################


#--------------------------------------------------------------
# Options for IOVRegistrationSvc
include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )

#--------------------------------------------------------------
# Setting IOV range
#--------------------------------------------------------------

IOVBeginRun   = IOVRunNumberMin
IOVEndRun     = IOVRunNumberMax
IOVBeginLB = IOVLBNumberMin
IOVEndLB   = IOVLBNumberMax

import RegistrationServices.IOVRegistrationSvc
regSvc = svcMgr.IOVRegistrationSvc
#regSvc.OutputLevel = INFO

# Select the following to delete and recreate the folders. Default is
# NOT to recreate - the content of a folder is delete before being recreated
regSvc.RecreateFolders = L1CaloPedestalsMaker_RecreateCOOLFolders


#--------------------------------------------------------------
#  Use AthenaOutputStreamTool to write
#    Must "turn off" standard AthenaOutputStream
#--------------------------------------------------------------

if L1CaloPedestalsMaker_Store2COOL:

	theApp.OutStream = []

	from AthenaCommon.AppMgr import ToolSvc
	from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
	ToolSvc += AthenaPoolOutputStreamTool("COOLStream", OutputFile = "cool.pool.root")


#--------------------------------------------------------------
# L1CaloCondSvc settings
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ServiceMgr
from TrigT1CaloCondSvc.TrigT1CaloCondSvcConf import L1CaloCondSvc
# note we can add a fully configured service in one statement
ServiceMgr += L1CaloCondSvc()
L1CaloCondSvc.OutputLevel = INFO

# setup IOV range fro writing to COOL
L1CaloCondSvc.BeginRun   = IOVBeginRun
L1CaloCondSvc.EndRun     = IOVEndRun
L1CaloCondSvc.BeginLB = IOVBeginLB
L1CaloCondSvc.EndLB   = IOVEndLB


#--------------------------------------------------------------
# Set the connection string
#--------------------------------------------------------------

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
#IOVDbSvc.OutputLevel=VERBOSE


# Local sqlite file
IOVDbSvc.dbConnection = "impl=cool;techno=sqlite;schema=pedestals.db;DUMMY:COOLTEST"

# Dev DB area
#IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:xxxx:xxxx"

#if (SchemaPrivilege):
#    # To recreate folder or add DB, need to use schema account:
#    IOVDbSvc.dbConnection="impl=cool;techno=oracle;schema=ATLAS_COOL_CALO;ATLAS_COOLWRITE:OFLP200:ATLAS_COOL_CALO:"
#else:
#    # For db writing
#    IOVDbSvc.dbConnection="impl=cool;techno=oracle;schema=ATLAS_COOL_CALO;ATLAS_COOLWRITE:OFLP200:ATLAS_COOL_CALO_W:"


#MessageSvc = Service( "MessageSvc" )
#MessageSvc.OutputLevel             = DEBUG

