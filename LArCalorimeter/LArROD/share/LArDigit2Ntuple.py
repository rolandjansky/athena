if not 'CafJobInputs' in dir():
    CafJobInputs=[["/afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data"]]
    print "No input file given, use ",CafJobInputs[0]


if not 'CafJobOutputs' in dir():
    CafJobOutputs=["LArTiming.root"]

from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

include("LArConditionsCommon/LArMinimalSetup.py")

svcMgr.IOVDbSvc.GlobalTag="COMCOND-ES1C-001-00"

#Specify the input file(s)
svcMgr.EventSelector.Input=CafJobInputs[0]

# Specify the object you want to read from ByteStream
theByteStreamAddressProviderSvc = svcMgr.ByteStreamAddressProviderSvc
theByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"]
theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]


#Setting up the CBNTAA
from CBNT_Athena.CBNT_AthenaAwareCfg import CBNT_AthenaAware
theCBNT_AthenaAware= CBNT_AthenaAware()
CBNT_AthenaAware = topSequence.CBNT_AthenaAware
include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )

from LArROD.LArRODConf import CBNTAA_LArDigits
theCBNTAA_LArDigits = CBNTAA_LArDigits("CBNTAA_LArDigits")
theCBNTAA_LArDigits.MaxChannels = 200000
theCBNTAA_LArDigits.NEvents=-1 # Dump all
#theCBNTAA_LArDigits.DumpIterResults=True
theCBNTAA_LArDigits.SaveId=True
theCBNTAA_LArDigits.SaveSCAAddress = False
theCBNTAA_LArDigits.DumpCut=1300
theCBNTAA_LArDigits.ContainerKey = "FREE"
#theCBNTAA_LArDigits.DumpDisconnected=False
#theCBNTAA_LArDigits.SaveSCAAddress=False
theCBNTAA_LArDigits.OutputLevel=VERBOSE
CBNT_AthenaAware +=theCBNTAA_LArDigits
     

#Setting up the CBNTAA Output Stream
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
topSequence +=AANTupleStream()
AANTupleStream = topSequence.AANTupleStream
AANTupleStream.ExtraRefNames = [ "" ]
AANTupleStream.OutputName = CafJobOutputs[0]
AANTupleStream.ExistDataHeader = False

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output += ["AANT DATAFILE='"+CafJobOutputs[0]+"' OPT='RECREATE'"]


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
 
#svcMgr.ChronoStatSvc.ChronoDestinationCout = True

svcMgr.ChronoStatSvc.PrintUserTime     = True
svcMgr.ChronoStatSvc.PrintSystemTime   = True
svcMgr.ChronoStatSvc.PrintEllapsedTime = True
#svcMgr.ChronoStatSvc.NumberOfSkippedEventsForMemStat = 1

#svcMgr.ChronoStatSvc.AsciiStatsOutputFile = "chronoStats.ascii"

svcMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True

theApp.AuditAlgorithms = True

#Hack to load the right dict to write vector< vector < X > > to TTRee
import AthenaPython.PyAthena as PyAthena
PyAthena.load_library('AtlasSTLAddReflexDict')
