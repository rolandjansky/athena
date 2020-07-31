#**************************************************************
#
#       Read in ByteStream events, and Access CSC RDO
# 
#==============================================================

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AppMgr import ToolSvc

# Event Loop  
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

# CscByteStream + CscROD
include( "MuonByteStream/ReadCscRDO_jobOptions.py" )

#ServiceMgr.ByteStreamInputSvc.DumpEvent = True

# Simulated data
#ServiceMgr.eventSelector.Input=["daq.csc13.0000000.Single.Stream.LB0000.Athena._0001.data"]

# Real data - from cosmic test for example
# Old cosmic data before the ROB id = ROD id fix
#ServiceMgr.EventSelector.Input=[ 
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001090_file01.data",
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001105_file01.data",
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001109_file01.data",
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001113_file01.data",
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001122_file01.data",
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001125_file01.data",
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001127_file01.data",
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001128_file01.data",
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001129_file01.data",
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001130_file01.data",
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001133_file01.data",
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001134_file01.data",
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001136_file01.data",
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001139_file01.data",
#"rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001140_file01.data"
#"/afs/cern.ch/user/k/ketevi/tmp/daq_CSC-EB-RCD__0001139_file01.data"
#]

# Real data - from cosmic test for example
# New cosmic data after the ROB id = ROD id fix
ServiceMgr.EventSelector.Input=[
"/afs/cern.ch/user/k/ketevi/w0/data/daq_CSC-EB-RCD__0001190_file01.data"
]

# set this to true for cosmic data
from MuonByteStream.MuonByteStreamConf import CscRdoContByteStreamTool
ToolSvc += CscRdoContByteStreamTool()
ToolSvc.CscRdoContByteStreamTool.IsCosmicData = True

# set to True for the old cosmic data before the ROB id = ROD id fix
ToolSvc.CscRdoContByteStreamTool.IsOldCosmicData = False

# Detector Initialization 
DetDescrVersion = "ATLAS-CSC-01-02-00"
include ("RecExCond/RecExCommon_flags.py")
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
include ("RecExCond/AllDet_detDescr.py")
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc.MuonVersionOverride = "MuonSpectrometer-CSC-CS-00"

include ("CscCalibTools/CscCalibTool_jobOptions.py")
ToolSvc.CscCalibTool.ReadFromDatabase = True

#------------------- setup for 13.0.X
if ToolSvc.CscRdoContByteStreamTool.IsCosmicData:
   #********************************************************
   IOVDbSvc.OverrideTags += ["<prefix>/CSC/PED</prefix> <tag>CscPed-cosmic-L17-S10-S02-S04-00</tag>"]
   IOVDbSvc.OverrideTags += ["<prefix>/CSC/NOISE</prefix> <tag>CscNoise-cosmic-L17-S10-S02-S04-00</tag>"]
   #*******************************************************************8
#-----------------------------------------

###################### setup for 13.X.0
#from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
#from IOVDbSvc.CondDB import conddb
#if ToolSvc.CscRdoContByteStreamTool.IsCosmicData:
   #********************************************************
#   conddb.addOverride('/CSC/PED', 'CscPed-cosmic-L17-S10-S02-S04-00')
#   conddb.addOverride('/CSC/NOISE', 'CscNoise-cosmic-L17-S10-S02-S04-00')
   #*******************************************************************8
#########################################

#top algorithms to be run
include("MuonRdoToPrepData/CscRdoToCscPrepData_jobOptions.py")

from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData
CscRdoToCscPrepData.OutputLevel = DEBUG
from RegionSelector.RegSelToolConfig import makeRegSelTool_CSC
CscRdoToCscPrepData.RegSel_CSC = makeRegSelTool_CSC()

# Byte stream conversion service
theApp.ExtSvc += [ "ByteStreamCnvSvc" ]

# Create a Root Tuple of the raw data - RDO
CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")
from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import ReadCscRDO
topSequence.CBNT_AthenaAware += ReadCscRDO() 
ReadCscRDO = ReadCscRDO()
ReadCscRDO.OutputLevel = DEBUG

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel = INFO
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 500

##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='CscRDO_read.root' OPT='RECREATE'"]
from AnalysisTools.AnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.ExtraRefNames = [ "" ]
AANTupleStream.OutputName = 'CscRDO_read.root'
AANTupleStream.ExistDataHeader = False
AANTupleStream.OutputLevel = WARNING


