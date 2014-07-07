#**************************************************************
#
# jopOptions file for writing ByteStream. It can do it in 2 ways:
# - by producing RDO from the digitization,
# - by reading RDO from POOL file
#
# This job options will produce the byte stream file RawEvent.re
# and in addition, an NTuple will be output for validation purposes
#==================================================================
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AppMgr import ToolSvc

# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool

# the Athena Pool converters
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )

include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

# Pool input - this could be the simulated Hit files
# in which case the digitization must be run of the fly to produce the RDO
# - or this could the persistified RDO read from POOL files, in which case,
# the digitization should not be run
#ServiceMgr.EventSelector.InputCollections = [ "/afs/cern.ch/atlas/project/muon/data/csc12/misal1_csc11.005145.PythiaZmumu.digit.RDO.v12003101_tid003501._00001.pool.root.1" ]
#ServiceMgr.EventSelector.InputCollections = [ "/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root" ]
ServiceMgr.EventSelector.InputCollections = [ "/afs/cern.ch/user/k/ketevi/w0/data/misal1_csc11.005850.WH120bb_pythia.simul.HITS.v12003103_tid003856._00001.pool.root.5" ]

# Detector Initialization Initialization
DetDescrVersion = "ATLAS-CSC-01-02-00"
include ("RecExCommon/RecExCommon_flags.py")
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
include ("RecExCond/AllDet_detDescr.py")

include ("CscCalibTools/CscCalibTool_jobOptions.py")
ToolSvc.CscCalibTool.ReadFromDatabase = False

#top algorithms to be run
include("CSC_Digitization/CSC_Digitization_jobOptions.py")
from CSC_Digitization.CSC_DigitizationConf import CscDigitBuilder
CscDigitBuilder.OutputLevel = DEBUG

# Conversion of CSC RDO from digitization 
include( "MuonByteStreamCnvTest/CscDigitToCscRDO_jobOptions.py" )
from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import CscDigitToCscRDO
CscDigitToCscRDO.OutputLevel = DEBUG

# Create a Root Tuple of the raw data - RDO
CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")
from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import ReadCscRDO
topSequence.CBNT_AthenaAware += ReadCscRDO() 
ReadCscRDO = ReadCscRDO()
ReadCscRDO.OutputLevel = DEBUG

# write to ByteStream 
include( "ByteStreamCnvSvc/WriteByteStream_EventStorage_jobOptions.py" )

# Conversion of CSC RDO to Byte Stream
include( "MuonByteStream/WriteCscRDO_jobOptions.py" )

MessageSvc.OutputLevel = INFO
theApp.EvtMax = 500

# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='CscRDO_write.root' OPT='RECREATE'"]
from AnalysisTools.AnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.ExtraRefNames = [ "" ]
AANTupleStream.OutputName = 'CscRDO_write.root'
AANTupleStream.ExistDataHeader = False
AANTupleStream.OutputLevel = WARNING

