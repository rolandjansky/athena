# testing jobOption for LAr commissioning monitoring on phase1 bytestream files
# make it possible to override following variables RS 06.01.06
#
if not 'RunNumber' in dir():
    RunNumber = 1268
if not 'LArDigitKey' in dir():
    LArMonFlags.LArDigitKey = "HIGH"
if not 'EvtNo' in dir():
    EvtNo=1000

if not 'InputDir' in dir():
#    InputDir = "/castor/cern.ch/atlas/LargFec/Installation"
    InputDir = "/castor/cern.ch/atlas/LargFec/ExpertWeek01"
#    InputDir = "/afs/cern.ch/user/i/isabelle/scratch0"

OutputDir="."

if not 'RefRunNumber' in dir():
    # runnumber of reference file
    RefRunNumber = 18727

# FilePrefix = "daq_ROS-1_"+LArDigitKey+"_InstPed"
# FilePrefix = "daq_ROS-1_EMF"
FilePrefix = "daq_ROS-1_EXPERT-WEEK"

#if not 'LArDigitKey' in dir():
#    LArDigitKey=LArDigitKey

if not 'LArRawChannelKey' in dir():
    LArRawChannelKey="LArRawChannels"

#----------------------------------------------------------

if not 'LArCondCnvDbServer' in dir():
    LArCondCnvDbServer = 'atlmysql01.cern.ch'

include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
include( "LArCondCnv/LArCondCnv_IdMapAtlas_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )
include ("CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")

#----------------------------------------------------------
# Compose input file name
# InputFileName='%(dir)s/%(prefix)s_00%(No)d_file01.data' % {"dir" : InputDir, "prefix" : FilePrefix,"No" : RunNumber}
# Compose output file name
# AidaHistOutputFileName='%(dir)s/AidaHistos_%(No)d.root' % {"dir" : OutputDir, "No" : RunNumber}
RootHistOutputFileName='%(dir)s/RootHistos_%(No)d.root' % {"dir" : OutputDir, "No" : RunNumber}
RootHistReferenceFileName='%(dir)s/RootHistos_%(No)d.root' % {"dir" : OutputDir, "No" : RefRunNumber}
# ---------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
DetFlags.Print()

# include ("AtlasGeoModel/SetGeometryVersion.py")
# include ("AtlasGeoModel/GeoModelInit.py")

# ---------------------------------------------------------

theApp.Dlls += [ "LArByteStream"]

ByteStreamInputSvc = Service( "ByteStreamInputSvc" )

ByteStreamInputSvc.InputDirectory = [ InputDir ]
ByteStreamInputSvc.FilePrefix     = [ FilePrefix ]
ByteStreamInputSvc.RunNumber      = [ RunNumber ]
# EventSelector.Input += [ InputFileName ]

ByteStreamEventStorageInputSvc = Service( "ByteStreamEventStorageInputSvc" )
ByteStreamEventStorageInputSvc.ReaderType = "Castor"

# read LArDigit
ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
# ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/LOW"]
# ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/MEDIUM"]
# ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/HIGH"]
# # ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"]
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/"+LArMonFlags.LArDigitKey()]
ByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DoIdChecks

ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRoI_Map.Print=FALSE

#------------------------------------------------

theApp.Dlls += [ "LArRawUtils","LArROD","LArCalibUtils", "LArTools","LArEventTest","LArCalibTools"]

# ---------------------------------------------------------

theApp.TopAlg += [ "LArRawChannelSimpleBuilder/LArRawChannelBuilder" ]
LArRawChannelBuilder = Algorithm("LArRawChannelBuilder")
LArRawChannelBuilder.DataLocation    = LArMonFlags.LArDigitKey()
LArRawChannelBuilder.LArRawChannelContainerName = LArRawChannelKey
LArRawChannelBuilder.maxSamp = 2
# LArRawChannelBuilder.RecoMode    = "CUBIC"
# LArRawChannelBuilder.CubicAdcCut = 50.
# LArRawChannelBuilder.OutputLevel = DEBUG

#----------------------------------------------------------
# THistService for native root in Athena
# new THistSvc, note, from Gaudi 0.16.1.7, it's in GaudiSvc !!!
# theApp.Dlls += [ "THistSvc" ]
theApp.Dlls += [ "GaudiSvc" ]
THistSvc = Algorithm( "THistSvc" )
# Note that the THistSvc doesn't do a "recreate", so ...
if os.path.exists(RootHistOutputFileName):
    os.remove(RootHistOutputFileName)
THistSvc.Output = ["MonitorOutPut DATAFILE='"+RootHistOutputFileName+"' OPT='New'"]
# THistSvc.Input =  ["ReferenceFile DATAFILE='"+RootHistReferenceFileName+"' OPT='Read'"]
#----------------------------------------------------------

theApp.Dlls += [ "AthenaMonitoring"]
CheckEveryNoEvents=100

theApp.Dlls += [ "AthenaMonitoring"]
theApp.Dlls += [ "LArMonTools"]
theApp.TopAlg += ["AthenaMon/LArMon1"]
LArMon1 = Algorithm( "LArMon1" )
LArMon1.CheckEveryNoEvents=CheckEveryNoEvents

# include all monitoring tools
include ("LArMonTools/LAr2DNoiseMonTool_jobOptions.py" )
include ("LArMonTools/LArDigitNoiseMonTool_jobOptions.py" )
include ("LArMonTools/LArDigMonTool_jobOptions.py" )
include ("LArMonTools/LArRawChannelMonTool_jobOptions.py" )
include ("LArMonTools/LArRawChannelNoiseMonTool_jobOptions.py" )
include ("LArMonTools/LArScaNoiseMonTool_jobOptions.py" )
include ("LArMonTools/LArEventInfoMonTool_jobOptions.py" )
include ("LArMonTools/LArFebNoiseMonTool_jobOptions.py")



#---------------------------
# Chrono svc
#---------------------------
theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ]

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
# Output level & events
MessageSvc.OutputLevel      = INFO
theApp.EvtMax = EvtNo
AthenaEventLoopMgr = Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.FailureMode=2
AthenaEventLoopMgr.OutputLevel = WARNING

# Job options to dump all objects available
# StoreGateSvc = Service( "StoreGateSvc" )
# StoreGateSvc.Dump = True
 
#EventData2XML.OutputLevel =1
MessageSvc.defaultLimit=1000000;
theApp.AuditAlgorithms=True
#theApp.AuditServices=True
#theApp.AuditTools=True # FIXME crash on finalisation 
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

ToolSvc.LArRawDataContByteStreamTool.SubDetectorId = 66

DetDescrCnvSvc.MuonIDFileName = "IdDictMuonSpectrometer_R.01.xml"
