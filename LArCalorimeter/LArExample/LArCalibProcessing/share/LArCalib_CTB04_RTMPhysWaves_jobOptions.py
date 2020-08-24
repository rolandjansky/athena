###########################################################################
#
#  * read master waves from DB
#  * predict physics waves
#  * save waves to database/ntuples
#
###########################################################################

MasterWaveFolderTag  = "TB04-5"

doTest               = False
storeEmpty           = False

WriteNT              = True
NtupleFileName       = "LArRTMPhysWaves-NoTimeShift.root"

WritePOOLFile        = False
POOLFileDir          = "./"
#POOLFileDir          = "/shift/mdelmast/ctb2004/ofc/"
#POOLFileDir          = "/data/mdelmast/ctb2004/ofc/"
POOLFileName         = "LArRTMPhysWaves-NoTimeShift.pool.root"

WriteDB              = False
PhysWaveFolder       = "RTM"

TDriftVector         = [ 420 ,  475 ,  475 ,  475 ]

#
# Use RunNumber to select MasterWaves, and change the PhysWave IOV accordingly
#

# *** All periods! (no time shift applied)
IOVBeginRun          = 0
IOVEndRun            = 2147483647 #0x7FFFFFFF
RunNumber            = 2102103 # RecExTB default run
TimeShiftGuardRegion = 0

#
# FEB-dependent time shifts
#
theApp.Dlls += ["LArEventTest"]
theApp.TopAlg += ["FakeLArTimeOffset"]
FakeLArTimeOffset=Algorithm("FakeLArTimeOffset")
FakeLArTimeOffset.OutputLevel = INFO

FakeLArTimeOffset.FEBids = [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 
                             0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 
			     0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 
			     0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]

FakeLArTimeOffset.FEbTimeOffsets  = [ 0, 0, 0, 0, 0, 0, 0, 
                                      0, 0, 0, 0, 0, 0, 0, 
                                      0, 0, 0, 0, 0, 0, 0, 
				      0, 0, 0, 0, 0, 0, 0 ]

#include("TimeShiftFEB.py")

###########################################################################

# *** General IOV settings
IOVBeginEvent = 0
#IOVEndEvent   = 999999
IOVEndEvent   = 4294967295 #0xFFFFFFFF
#IOVEndEvent   = 4294967294 #0xFFFFFFFF -1

###########################################################################

#LArCondCnvDbServer = "atlobk01.cern.ch"
#LArCondCnvDbServer = "atlobk02.cern.ch"
LArCondCnvDbServer = "atlmysql01.cern.ch"

###########################################################################

include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )

#
# Fake the reconstruciton of one event from one run to trigger the OFC retrieval
#
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

#IOVDbSvc.OutputLevel = DEBUG

if ( not WriteDB ) :
	IOVDbSvc.userName   = "conditions"; 
	IOVDbSvc.userPwd    = "conditions";
else :
	IOVDbSvc.userName   = "condwriter"; 
	IOVDbSvc.userPwd    = "pwd4writer";
	
MasterWaveFolderName = ["/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveHigh" ,
                        "/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveMedium" ,
                        "/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveLow"]
		
if 'MasterWaveFolderTag' in dir():
	for i in range(len(MasterWaveFolderName)) :
		MasterWaveFolderName[i] += '<tag>'+ MasterWaveFolderTag + '</tag>'
	IOVDbSvc.Folders += MasterWaveFolderName ;

PhysWaveFolderName = "/lar/LArElecCalibTB04/LArPhysWaveContainer/"+PhysWaveFolder
IOVDbSvc.Folders  += [PhysWaveFolderName]
ShapeFolderName    = "/lar/LArElecCalibTB04/LArShapeComplete/"+PhysWaveFolder
IOVDbSvc.Folders  += [ShapeFolderName]
IOVDbSvc.Folders  += ["/lar/LArElecCalibTB04/LArWFParams/RTMLArWFParams"]

#
# IOV
#
ToolSvc.FillNovaIOVTool.BeginRun   = IOVBeginRun
ToolSvc.FillNovaIOVTool.EndRun     = IOVEndRun
ToolSvc.FillNovaIOVTool.BeginEvent = IOVBeginEvent
ToolSvc.FillNovaIOVTool.EndEvent   = IOVEndEvent

#
# predict physics waveforms from master waveforms
#
theApp.Dlls += ["LArRawUtils"]
theApp.Dlls += ["LArTools"]
theApp.Dlls += ["LArCalibUtils"]
theApp.Dlls += ["LArCalibTools"]

theApp.TopAlg += [ "LArRTMPhysWavePredictor" ]
LArRTMPhysWavePredictor = Algorithm( "LArRTMPhysWavePredictor" )
LArRTMPhysWavePredictor.KeyList             = [ "LArMasterWaveHigh" , "LArMasterWaveMedium" , "LArMasterWaveLow" ]
LArRTMPhysWavePredictor.WFParams2Container  = True  # necessary for LArWFParams2Ntuple...
LArRTMPhysWavePredictor.TestMode            = doTest # if True processes only one wave per container
LArRTMPhysWavePredictor.StoreEmpty          = storeEmpty

#LArRTMPhysWavePredictor.Tdrift              = [  420 ,  475 ,  475 ,  475 ] # drift times for PS and EMC samplings
#LArRTMPhysWavePredictor.Tdrift              = [  400 ,  400 ,  400 ,  400 ] # drift times for PS and EMC samplings
LArRTMPhysWavePredictor.Tdrift              = TDriftVector

LArRTMPhysWavePredictor.UseDoubleTriangle   = False
LArRTMPhysWavePredictor.Tdrift2             = [ 1200 , 1200 , 1200 , 1200 ]
LArRTMPhysWavePredictor.WeightTriangle2     = [ 0.01 , 0.01 , 0.01 , 0.01 ] 

LArRTMPhysWavePredictor.TimeShiftByHelper     = False
LArRTMPhysWavePredictor.TimeShiftByIndex      = -1
LArRTMPhysWavePredictor.TimeShiftByLayer      = False
LArRTMPhysWavePredictor.Tshift                = [ 13 , 23 , 4 , 8 ] # time shifts for PS and EMC samplings
LArRTMPhysWavePredictor.TimeShiftByFEB        = False
LArRTMPhysWavePredictor.TimeShiftGuardRegion  = TimeShiftGuardRegion ;

LArRTMPhysWavePredictor.ComputeTimeShiftByFEB = True

ToolSvc.LArPhysWaveTool.NormalizeCali    = True
#ToolSvc.LArPhysWaveTool.InjPointCorr     = True # use defaults for barrel layers, otherwise don't apply!
ToolSvc.LArPhysWaveTool.TimeOriginShift  = False
ToolSvc.LArPhysWaveTool.SubtractBaseline = False

#
# write master and phys waves and parameters to database
# 
if ( WriteDB ) :
	LArRTMPhysWavePredictor.WriteDB            = True
	LArRTMPhysWavePredictor.PhysWaveFolderName = PhysWaveFolder

#
# write master and phys waves and parameters to ntuples
# 
if ( WriteNT ) :
	theApp.TopAlg += [ "LArWaves2Ntuple<LArCaliWaveContainer>/LArCaliWaves2Ntuple"]
	LArCaliWaves2Ntuple = Algorithm( "LArCaliWaves2Ntuple" )
	LArCaliWaves2Ntuple.NtupleName   = "CALIWAVE"
	LArCaliWaves2Ntuple.DACSaturSkip = False
	LArCaliWaves2Ntuple.KeyList = [ "LArMasterWaveHigh" , "LArMasterWaveMedium" , "LArMasterWaveLow" ]
#
# Note: the phys wave container has the same name as the used cali wave container:
#
	theApp.TopAlg += [ "LArWaves2Ntuple<LArPhysWaveContainer>/LArPhysWaves2Ntuple"]
	LArPhysWaves2Ntuple = Algorithm( "LArPhysWaves2Ntuple" )
	LArPhysWaves2Ntuple.NtupleName   = "PHYSWAVE" 
	LArPhysWaves2Ntuple.DACSaturSkip = False
	LArPhysWaves2Ntuple.KeyList = [ "LArMasterWaveHigh" , "LArMasterWaveMedium" , "LArMasterWaveLow" ]
#
# Note the params container has the same name as the used cali wave container:
#
	theApp.TopAlg += [ "LArWFParams2Ntuple" ]
	LArWFParams2Ntuple = Algorithm( "LArWFParams2Ntuple" )
	LArWFParams2Ntuple.KeyList = [ "LArMasterWaveHigh" , "LArMasterWaveMedium" , "LArMasterWaveLow" ]
        LArWFParams2Ntuple.DumpCaliPulseParams = True
        LArWFParams2Ntuple.DumpDetCellParams   = True
        LArWFParams2Ntuple.DumpPhysCaliTdiff   = True
        LArWFParams2Ntuple.DumpTdrift          = True
	LArWFParams2Ntuple.DetStoreSuffix      = "_RTM"

if ( WritePOOLFile ) :
	include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
	theApp.Dlls   += [ "LArAthenaPoolPoolCnv" ]
	theApp.Dlls   += [ "LArCondAthenaPoolPoolCnv" ]

	include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
	ToolSvc.ConditionsAlgStream.OutputFile = POOLFileDir+POOLFileName
	OutputConditionsAlg.ObjectList = [ "LArPhysWave" ]
	OutputConditionsAlg.ObjectList = [ "LArPhysWaveContainer#LArMasterWaveHigh" ,
	                                   "LArPhysWaveContainer#LArMasterWaveMedium" ,
					   "LArPhysWaveContainer#LArMasterWaveLow" ]
	
	OutputConditionsAlg.OutputLevel = DEBUG
	ToolSvc.ConditionsAlgStream.OutputLevel = DEBUG

#
# output levels
#
LArRTMPhysWavePredictor.OutputLevel = INFO
ToolSvc.LArWFParamTool.OutputLevel  = INFO
ToolSvc.LArPhysWaveTool.OutputLevel = INFO
if ( WriteNT ) :
	LArCaliWaves2Ntuple.OutputLevel = INFO
	LArPhysWaves2Ntuple.OutputLevel = INFO
	LArWFParams2Ntuple.OutputLevel  = INFO
if ( WriteDB ) :
	IOVDbSvc.OutputLevel = INFO

theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='"+NtupleFileName+"' OPT='NEW'" ]

AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel = INFO

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
###########################################################################
