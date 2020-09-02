###########################################################################

readNovaDB          = True
PhysWaveFolderName  = ["/lar/LArElecCalibTB04/LArPhysWaveContainer/RTM"]
PhysWaveFolderTag   = "TB04-NoTimeShift-1"

POOLFileDir          = "./"
#POOLFileDir          = "/data/mdelmast/ctb2004/ofc/"
#POOLFileDir          = "/shift/mdelmast/ctb2004/ofc/"

readPOOLfile        = False
PhysWavePOOLFileIN  = "LArRTMPhysWaves-NoTimeShift.pool.root"

writePOOLfile       = False
#PhysWavePOOLFileOUT = "LArRTMPhysWaves-Shifted.pool.root"

writeNovaDB         = False
PhysWaveFolderNew   = "RTM"
#PhysWaveFolderNew   = "RTM_test"

writeNT             = False
RootFile            = "LArPhysWaveShifter.root"

###########################################################################

Period = "3,4,5,6a"
#Period = "6b-1"
#Period = "6b-2"
#Period = "6b-3"
#Period = "6b-4"
#Period = "7"
#Period = "8-1"
#Period = "8-3"

#include( "thisjob.py" )

###########################################################################

TimeShiftByHelper  = False
TimeShiftByFEB     = True

theApp.Dlls   += ["LArEventTest"]
theApp.TopAlg += ["FakeLArTimeOffset"]
FakeLArTimeOffset=Algorithm("FakeLArTimeOffset")
FakeLArTimeOffset.OutputLevel = INFO

FakeLArTimeOffset.FEBids = [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 
                             0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 
			     0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 
			     0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 
			     0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 
			     0x390d8000, 0x390e0000, 0x390e8000 ]

#
# All periods...
#
#IOVBeginRun          = 0
#IOVEndRun            = 2147483647 # 0x7FFFFFFF
#RunNumber            = 2102103    # RecExTB default run
#TimeShiftGuardRegion = 0
FakeLArTimeOffset.FEbTimeOffsets = [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ]

#
# minimum tstart per FEB computed on calibration (master) waveforms
#
#FakeLArTimeOffset.FEbTimeOffsets  = [  2, 30, 27, 26, 23, 19, 18, 18, 10,  3, 
#                                      17, 11,  4,  8,  5, 34, 32, 32, 31, 25, 
#				      24, 24, 17,  9, 21, 21, 10,  6 ]

#
# minimum tstart per FEB computed on predicted physiucs waveforms
#
#FakeLArTimeOffset.FEbTimeOffsets  = [  2, 33, 30, 30, 26, 22, 20, 21, 10,  3, 
#				       20, 15,  8,  5,  5, 37, 35, 34, 34, 28, 
#				       27, 28, 17,  9, 25, 25, 15, 10 ]


if ( Period == "3,4,5,6a" ) :
	IOVBeginRun          = 1000709
	IOVEndRun            = 1004234
	RunNumber            = 1000709
	TimeShiftGuardRegion = 10
	FakeLArTimeOffset.FEbTimeOffsets  = [ 22, 30, 27, 26, 23, 19, 18, 18, 10,  3, 
                                              17, 11,  4,  8, 25, 34, 32, 32, 31, 25, 
				              24, 24, 17,  9, 21, 21, 10,  6 ]
	PhysWavePOOLFileOUT = "LArRTMPhysWaves-Shifted-Periods:3,4,5,6a.pool.root"

if ( Period == "6b-1" ) :
	IOVBeginRun          = 2100024
	IOVEndRun            = 2100110
	RunNumber            = 2100024
	TimeShiftGuardRegion = 15
	FakeLArTimeOffset.FEbTimeOffsets  = [ 22, 30-10, 27, 26, 23, 19, 18, 18, 10,  3, 
                                              17, 11,  4,  8, 25, 34-15, 32, 32-10, 31, 25, 
				              24, 24, 17,  9, 21-15, 21-5, 10,  6 ]
	PhysWavePOOLFileOUT = "LArRTMPhysWaves-Shifted-Periods:6b-1.pool.root"

if ( Period == "6b-2" ) :
	IOVBeginRun          = 2100132
	IOVEndRun            = 2100253
	RunNumber            = 2100253
	TimeShiftGuardRegion = 10
	FakeLArTimeOffset.FEbTimeOffsets  = [ 22, 30-10, 27, 26, 23, 19, 18, 18, 10,  3, 
                                              17, 11,  4,  8, 25, 34-15, 32, 32, 31, 25, 
				              24, 24, 17,  9, 21-5, 21, 10,  6 ]
	PhysWavePOOLFileOUT = "LArRTMPhysWaves-Shifted-Periods:6b-2.pool.root"

if ( Period == "6b-3" ) :
	IOVBeginRun          = 2100254	
	IOVEndRun            = 2100510
	RunNumber            = 2100254
	TimeShiftGuardRegion = 15
	FakeLArTimeOffset.FEbTimeOffsets  = [ 22, 30-10, 27, 26, 23, 19, 18, 18, 10,  3, 
                                              17, 11,  4,  8, 25, 34, 32, 32, 31, 25, 
				              24, 24, 17,  9, 21-5, 21-10, 10,  6 ]
	PhysWavePOOLFileOUT = "LArRTMPhysWaves-Shifted-Periods:6b-3.pool.root"

if ( Period == "6b-4" ) :
	IOVBeginRun          = 2100513	
	IOVEndRun            = 2100540
	RunNumber            = 2100513
	TimeShiftGuardRegion = 15
	FakeLArTimeOffset.FEbTimeOffsets  = [ 22, 30-10, 27, 26, 23, 19, 18, 18, 10,  3, 
                                              17, 11,  4,  8, 25, 34, 32, 32, 31, 25, 
				              24, 24, 17,  9, 21-5, 21-10, 10,  6 ]
	PhysWavePOOLFileOUT = "LArRTMPhysWaves-Shifted-Periods:6b-4.pool.root"

if ( Period == "7" ) :
	IOVBeginRun          = 2100558
	IOVEndRun            = 2100975
	RunNumber            = 2100558	
	TimeShiftGuardRegion = 10
	FakeLArTimeOffset.FEbTimeOffsets  = [ 22-10, 30-10, 27, 26, 23, 19, 18, 18, 10,  3, 
                                              17, 11,  4-5,  8, 25-10, 34, 32, 32, 31, 25, 
				              24, 24, 17,  9, 21-5, 21-10, 10,  6 ]
	PhysWavePOOLFileOUT = "LArRTMPhysWaves-Shifted-Periods:7.pool.root"


if ( Period == "8-1" ) :
	IOVBeginRun          = 2101022
	IOVEndRun            = 2101475
	RunNumber            = 2101022
	TimeShiftGuardRegion = 20
	FakeLArTimeOffset.FEbTimeOffsets  = [ 22, 30, 27, 26, 23, 19, 18, 18, 10,  3, 
                                              17, 11,  4,  8, 25, 25, 25, 25, 25, 25, 
				              24, 24, 17,  9, 21, 21, 10,  6 ]
	PhysWavePOOLFileOUT = "LArRTMPhysWaves-Shifted-Periods:8-1.pool.root"

if ( Period == "8-3" ) :
	IOVBeginRun          = 2102095
	IOVEndRun            = 2102992
	RunNumber            = 2102095
	TimeShiftGuardRegion = 5
	FakeLArTimeOffset.FEbTimeOffsets  = [ 22, 30, 27, 26, 23, 19, 18, 18, 10,  3, 
                                              17, 11,  4,  8, 25, 34, 32, 32, 31, 25, 
				              24, 24, 17,  9, 21, 21, 10,  6 ]
	PhysWavePOOLFileOUT = "LArRTMPhysWaves-Shifted-Periods:8-3.pool.root"
	
###########################################################################

# *** General IOV settings
IOVBeginEvent = 0
#IOVEndEvent   = 999999
IOVEndEvent   = 4294967295 #0xFFFFFFFF
#IOVEndEvent   = 4294967294 #0xFFFFFFFF - 1

###########################################################################

#LArCondCnvDbServer = "atlobk01.cern.ch"
#LArCondCnvDbServer = "atlobk02.cern.ch"
LArCondCnvDbServer = "atlmysql01.cern.ch"

###########################################################################

#
# Fake the reconstruction of one event from one run to trigger the OFC retrieval
#
include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
theApp.Dlls += [ "LArByteStream"]
ByteStreamInputSvc = Service( "ByteStreamInputSvc" )

include("RecExTB_SetFlags_jobOptions.py")

ByteStreamInputSvc.InputDirectory += ["/castor/cern.ch/atlas/testbeam/combined/2004"]
ByteStreamInputSvc.FilePrefix     += [FilePrefix]
ByteStreamInputSvc.RunNumber       = [RunNumber]

theApp.EvtMax = 1

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/LOW"]
ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/MEDIUM"]
ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/HIGH"]

#
# Detector description (LAr only)
#
include( "LArDetDescr/LArDetDescr_H8_joboptions.py" )
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DoIdChecks
DetDescrCnvSvc.LArIDFileName ="IdDictParser/IdDictLArCalorimeter_H8_2004.xml"

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.em_setOn() 

ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRoI_Map.Print = False

#
# Conditions DB
#
include( "LArCondCnv/LArCondCnv_Config_jobOptions.py" )
include( "LArCondCnv/LArCondCnv_IOVDbSvc_jobOptions.py" )

IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.serverName = LArCondCnvDbServer
IOVDbSvc.DBname     = "conditions_ctb_2004"

IOVDbSvc.OutputLevel = DEBUG

if ( not writeNovaDB ) :
	IOVDbSvc.userName   = "conditions"; 
	IOVDbSvc.userPwd    = "conditions";
else :
	IOVDbSvc.userName   = "condwriter"; 
	IOVDbSvc.userPwd    = "pwd4writer";
		
if ( readNovaDB	) :
	if 'PhysWaveFolderTag' in dir():
		for i in range(len(PhysWaveFolderName)) :
			PhysWaveFolderName[i] += '<tag>'+ PhysWaveFolderTag + '</tag>'
		IOVDbSvc.Folders += PhysWaveFolderName ;

#
# POOL
#
if ( readPOOLfile) :
	include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
	theApp.Dlls += ["EventSelectorAthenaPool"] 
	theApp.Dlls += [ "LArAthenaPoolPoolCnv" ]
	theApp.Dlls += [ "LArCondAthenaPoolPoolCnv" ]
	
	ProxyProviderSvc = Service( "ProxyProviderSvc" )
	ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
	
	CondProxyProvider = Service( "CondProxyProvider" )
	CondProxyProvider.InputCollection = [ POOLFileDir+PhysWavePOOLFileIN ]
	
	EventPersistencySvc = Service( "EventPersistencySvc" )
	EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]

#
# IOV
#
ToolSvc.FillNovaIOVTool.BeginRun   = IOVBeginRun
ToolSvc.FillNovaIOVTool.EndRun     = IOVEndRun
ToolSvc.FillNovaIOVTool.BeginEvent = IOVBeginEvent
ToolSvc.FillNovaIOVTool.EndEvent   = IOVEndEvent

#
# shift physics waveforms
#
theApp.Dlls += ["LArRawUtils"]
theApp.Dlls += ["LArTools"]
theApp.Dlls += ["LArCalibUtils"]
theApp.Dlls += ["LArCalibTools"]

theApp.TopAlg += [ "LArPhysWaveShifter" ]
LArPhysWaveShifter = Algorithm( "LArPhysWaveShifter" )
LArPhysWaveShifter.KeyList               =  [ "LArPhysWave" ]
#LArPhysWaveShifter.KeyList              +=  [ "LArMasterWaveHigh" , "LArMasterWaveMedium" , "LArMasterWaveLow" ]

#
# Possible time shifts:
#
LArPhysWaveShifter.TimeShiftByHelper     = TimeShiftByHelper
LArPhysWaveShifter.TimeShiftByIndex      = -1
LArPhysWaveShifter.TimeShiftByLayer      = False
LArPhysWaveShifter.TshiftLayer           = [ 13 , 23 , 4 , 8 ] # time shifts for PS and EMC samplings
LArPhysWaveShifter.TimeShiftByFEB        = TimeShiftByFEB
LArPhysWaveShifter.TimeShiftGuardRegion  = TimeShiftGuardRegion ;

#
# Minimum Tstart per FEB computation (use helper, produce TimeShiftFEB.py)
#
LArPhysWaveShifter.ComputeTimeShiftByFEB = False

LArPhysWaveShifter.OutputLevel           = VERBOSE

if ( writeNovaDB ) :
	LArPhysWaveShifter.WriteDB        = True
	LArPhysWaveShifter.PhysFolderName = PhysWaveFolderNew
	IOVDbSvc.OutputLevel              = INFO

if ( writePOOLfile ) :
	include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
	theApp.Dlls   += [ "LArAthenaPoolPoolCnv" ]
	theApp.Dlls   += [ "LArCondAthenaPoolPoolCnv" ]
	
	include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
	ToolSvc.ConditionsAlgStream.OutputFile = POOLFileDir+PhysWavePOOLFileOUT
	
	OutputConditionsAlg.ObjectList  = [ "LArPhysWaveContainer#LArPhysWave_new" ]
	#OutputConditionsAlg.ObjectList += [ "LArPhysWaveContainer#LArMasterWaveHigh_new" ]
	#OutputConditionsAlg.ObjectList += [ "LArPhysWaveContainer#LArMasterWaveMedium_new" ]
	#OutputConditionsAlg.ObjectList += [ "LArPhysWaveContainer#LArMasterWaveLOw_new" ]
			
	OutputConditionsAlg.OutputLevel = DEBUG
	
	ToolSvc.ConditionsAlgStream.OutputLevel = DEBUG

if ( writeNT ) :
	theApp.TopAlg += [ "LArWaves2Ntuple<LArPhysWaveContainer>/LArOldPhysWaves2Ntuple"]
	LArOldPhysWaves2Ntuple = Algorithm( "LArOldPhysWaves2Ntuple" )
	LArOldPhysWaves2Ntuple.NtupleName   = "PHYSWAVEOLD" 
	LArOldPhysWaves2Ntuple.KeyList      = [ "LArPhysWave" ]
	LArOldPhysWaves2Ntuple.KeyList     += [ "LArMasterWaveHigh" , "LArMasterWaveMedium" , "LArMasterWaveLow" ]

	theApp.TopAlg += [ "LArWaves2Ntuple<LArPhysWaveContainer>/LArNewPhysWaves2Ntuple"]
	LArNewPhysWaves2Ntuple = Algorithm( "LArNewPhysWaves2Ntuple" )
	LArNewPhysWaves2Ntuple.NtupleName   = "PHYSWAVENEW" 
	LArNewPhysWaves2Ntuple.KeyList      = [ "LArPhysWave_new" ]
	LArNewPhysWaves2Ntuple.KeyList     += [ "LArMasterWaveHigh" , "LArMasterWaveMedium" , "LArMasterWaveLow" ]

	theApp.Dlls += [ "RootHistCnv" ]
	theApp.HistogramPersistency = "ROOT"
	NTupleSvc = Service( "NTupleSvc" )
	NTupleSvc.Output = [ "FILE1 DATAFILE='"+RootFile+"' OPT='NEW'" ]


AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel = INFO

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = INFO
MessageSvc.defaultLimit = 10000;
MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"
###########################################################################
