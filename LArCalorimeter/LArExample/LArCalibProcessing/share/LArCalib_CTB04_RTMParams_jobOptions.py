###########################################################################
#
#  * read master waves from DB
#  * extract paramters according to RTM algorithms
#  * save parameters to Ntuple/NovaDB/local POOL file 
#
###########################################################################

MasterWaveFolderTag  = "TB04-5"

doTest          = False

WriteNT         = True
NtupleFileName  = "LArRTMParams.root"
WriteCaliWaveNT = False

WritePOOLFile   = True
POOLFileDir     = "./"
POOLFileName    = "LArRTMParams.pool.root"

WriteNovaDB     = False
NovaDBFolder    = "test"

###########################################################################
#
# Use RunNumber to select then MasterWaves IOV
# Change the Params IOV accordingly
#

DataDir      = "/castor/cern.ch/atlas/testbeam/combined/2004"
RunNumber    = 2102103 # RecExTB default run
FilePrefix   = "daq_SFI-51_combined"

# IOV extrema example (-inf, +inf)
IOVBeginRun  = 0
IOVEndRun    = 2147483647 # 0x7FFFFFFF

# General IOV settings
IOVBeginEvent = 0
IOVEndEvent   = 4294967295 # 0xFFFFFFFF

###########################################################################

LArCondCnvDbServer = "atlmysql01.cern.ch"
#LArCondCnvDbServer = "atlobk02.cern.ch"

###########################################################################

include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )

#
# Fake the reconstruction of one event from one run to trigger the IOV server
#
theApp.Dlls += [ "LArByteStream"]
ByteStreamInputSvc = Service( "ByteStreamInputSvc" )

ByteStreamInputSvc.InputDirectory += [DataDir]
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

if ( not WriteNovaDB ) :
	IOVDbSvc.userName   = "conditions"; 
	IOVDbSvc.userPwd    = "conditions";
else :
	IOVDbSvc.userName   = "you_should_know_it"; 
	IOVDbSvc.userPwd    = "otherwise_dont_try_to_write";
	
MasterWaveFolderName = ["/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveHigh" ,
                        "/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveMedium" ,
                        "/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveLow"]
		
if 'MasterWaveFolderTag' in dir():
	for i in range(len(MasterWaveFolderName)) :
		MasterWaveFolderName[i] += '<tag>'+ MasterWaveFolderTag + '</tag>'
	IOVDbSvc.Folders += MasterWaveFolderName ;

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

theApp.TopAlg += [ "LArRTMParamExtractor" ]
LArRTMParamExtractor = Algorithm( "LArRTMParamExtractor" )
LArRTMParamExtractor.KeyList     = [ "LArMasterWaveHigh" , "LArMasterWaveMedium" , "LArMasterWaveLow" ]
LArRTMParamExtractor.TestMode    = doTest # if True processes only one wave per container
LArRTMParamExtractor.OutputLevel = INFO

ToolSvc.LArWFParamTool.OutputLevel  = VERBOSE

#
# write calibration waves and extracted parameters to ntuple
# 
if ( WriteNT ) :
	theApp.TopAlg += [ "LArWFParams2Ntuple" ]
	LArWFParams2Ntuple = Algorithm( "LArWFParams2Ntuple" )
        LArWFParams2Ntuple.DumpCaliPulseParams = True
        LArWFParams2Ntuple.DumpDetCellParams   = True
	LArWFParams2Ntuple.DetStoreSuffix      = "_RTM"

	if ( WriteCaliWaveNT ) :
		theApp.TopAlg += [ "LArWaves2Ntuple<LArCaliWaveContainer>/LArCaliWaves2Ntuple"]
		LArCaliWaves2Ntuple = Algorithm( "LArCaliWaves2Ntuple" )
		LArCaliWaves2Ntuple.NtupleName   = "CALIWAVE"
		LArCaliWaves2Ntuple.DACSaturSkip = False
		LArCaliWaves2Ntuple.KeyList = [ "LArMasterWaveHigh" , "LArMasterWaveMedium" , "LArMasterWaveLow" ]

	theApp.Dlls += [ "RootHistCnv" ]
	theApp.HistogramPersistency = "ROOT"
	NTupleSvc = Service( "NTupleSvc" )
	NTupleSvc.Output = [ "FILE1 DATAFILE='"+NtupleFileName+"' OPT='NEW'" ]
#
# write extracted parameters to local POOL file
#
if ( WritePOOLFile ) :
	include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
	theApp.Dlls += [ "LArAthenaPoolPoolCnv" ]
	theApp.Dlls += [ "LArCondAthenaPoolPoolCnv" ]

	include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
	ToolSvc.ConditionsAlgStream.OutputFile = POOLFileDir+POOLFileName

	OutputConditionsAlg.ObjectList = [ "LArCaliPulseParamsComplete#LArCaliPulseParams_RTM" ,
	                                   "LArDetCellParamsComplete#LArDetCellParams_RTM" ]

	#OutputConditionsAlg.OutputLevel = DEBUG
	#ToolSvc.ConditionsAlgStream.OutputLevel = DEBUG

#
# write extracted parameters to Nova DB
#
if ( WriteNovaDB ) :
	theApp.TopAlg += [ "LArWFParams2DB" ]
	LArWFParams2DB = Algorithm( "LArWFParams2DB" )
	LArWFParams2DB.ParamsFolderName     = NovaDBFolder
	LArWFParams2DB.WriteCaliPulseParams = True
	LArWFParams2DB.WriteDetCellParams   = True
	LArWFParams2DB.DetStoreSuffix       = "_RTM"
	LArWFParams2DB.OutputLevel          = DEBUG
	
###########################################################################

#AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
#AthenaEventLoopMgr.OutputLevel = INFO

#MessageSvc = Service( "MessageSvc" )
#MessageSvc.OutputLevel = INFO

###########################################################################
