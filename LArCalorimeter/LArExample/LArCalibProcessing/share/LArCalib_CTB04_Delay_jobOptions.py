###########################################################################
#
# Example jobOptions to reconstruction a CTB2004 delay run
#
# Last update: 23/11/2006 (Marco.Delmastro@cern.ch)
#
###########################################################################

RunNumber = 3144 # HIGH (small run, 1 pattern, Strips)

#RunNumber = 3317 # HIGH
#RunNumber = 3375 # MEDIUM
#RunNumber = 3533 # LOW

###########################################################################

#theApp.EvtMax = 100

###########################################################################

#Tag = "ExternalAveraging"
#RunAccumulator       = True
#UseAccumulatedDigits = True

Tag = "InternalAveraging"
RunAccumulator       = False
UseAccumulatedDigits = False

###########################################################################

StripsXtalkCorr = False

###########################################################################

SubtractPed     = True

WriteNtuple     = True
WritePOOLFile   = False

###########################################################################

WriteNovaDB     = False
PreviousRunToDB = 1 # 0 = overwrite ; 1 = append
FullFolderName  = "/lar/LArElecCalibTB04/LArCaliWaves/LArCaliWaveMedium"

###########################################################################

# if False read pedestal from new COOL database
ReadNovaDB = False

LArCondCnvDbServer = 'atlobk02.cern.ch'
#LArCondCnvDbServer = 'atlmysql01.cern.ch'

###########################################################################

# Each POOL file should have a container with unique key (to use LArCaliWaveMerger)
KeyOut = str(RunNumber) 
KeyOut = "LArCaliWave"

Outdir          = "./"

BaseName        = "LArDelay"
BaseName        = Outdir+BaseName+"_"+str(RunNumber)
if (StripsXtalkCorr) :
	BaseName += "_XtalkCorr"
else:
	BaseName += "_Standard"

if 'Tag' in dir():
	BaseName += "_"+Tag

RootFileName    = BaseName+".root"
POOLFileName    = BaseName+".pool.root"

###########################################################################

RunDir         = "/castor/cern.ch/atlas/testbeam/lar/2004"
RunPrefix      = "daq_ROS-41_LargCalib"

###########################################################################

IOVBeginRun    = 0
IOVEndRun      = 2147483647 # 0x7FFFFFFF
IOVBeginEvent  = 0
IOVEndEvent    = 4294967295 #0xFFFFFFFF

###########################################################################

include("LArCondCnv/LArCondCnv_IdMapAtlas_jobOptions.py" )
include("LArIdCnv/LArIdCnv_joboptions.py" )
include("CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
DetFlags.Print()

#
# Bytestream converter
#
include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )

theApp.Dlls += [ "LArByteStream"]

ByteStreamInputSvc = Service( "ByteStreamInputSvc" )
ByteStreamInputSvc.InputDirectory += [RunDir]
ByteStreamInputSvc.FilePrefix     += [RunPrefix]
ByteStreamInputSvc.RunNumber       = [RunNumber]

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += [ "LArCalibDigitContainer/HIGH",
                                            "LArCalibDigitContainer/MEDIUM",
                                            "LArCalibDigitContainer/LOW" ]

#
# Detector description (LAr only)
#

#include( "LArDetDescr/LArDetDescr_H8_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DoIdChecks
DetDescrCnvSvc.LArIDFileName = "IdDictParser/IdDictLArCalorimeter_H8_2004.xml"

ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRoI_Map.Print=False

#
# Conditions DB
#

if ( ReadNovaDB ) : 
	#
	# Read from Nova DB
	#
	include( "LArCondCnv/LArCondCnv_Config_jobOptions.py" )
	include( "LArCondCnv/LArCondCnv_IOVDbSvc_jobOptions.py" )

	LArCondCnvSvc = Service( "LArCondCnvSvc" )

	theApp.Dlls += [ "IOVDbSvc" ]
	ProxyProviderSvc = Service( "ProxyProviderSvc" )
	ProxyProviderSvc.ProviderNames += [ "IOVDbSvc" ]

	IOVDbSvc.serverName = LArCondCnvDbServer
	IOVDbSvc.DBname     = "conditions_ctb_2004";

	if ( not WriteNovaDB ) :
		IOVDbSvc.userName   = "conditions"; 
		IOVDbSvc.userPwd    = "conditions"; 
	else:
		IOVDbSvc.userName   = "condwriter"; 
		IOVDbSvc.userPwd    = "pwd4writer";

	IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArCalibParams"]
	IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArPedestal"]

	#
	# IOV (all CTB period)
	#
	ToolSvc = Service( "ToolSvc" )
	ToolSvc.FillNovaIOVTool.BeginRun   = IOVBeginRun
	ToolSvc.FillNovaIOVTool.EndRun     = IOVEndRun
	ToolSvc.FillNovaIOVTool.BeginEvent = IOVBeginEvent
	ToolSvc.FillNovaIOVTool.EndEvent   = IOVEndEvent

else:
	#
	# Read from COOL DB
	#
	doSim = False
	include("IOVDbSvc/IOVRecExTB.py")

	# Set up db connection 
	LArH8DBServer     = "ATLAS_COOLPROD"
	LArH8DataBase     = "TBP130"
	LArH8DBConnection = " <dbConnection>impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;"+LArH8DBServer+":"+LArH8DataBase+":ATLAS_COOL_READER:</dbConnection> "
	
	include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

	LArTB04FolderTag_Calib    = "TB04-Default"
	LArTB04FolderTag_Pedestal = "TB04-Default"
	
	LArH8FolderList = ["/LAR/LArElecCalibH8/LArCalibParams<tag>LArCalibParams-"+LArTB04FolderTag_Calib+"</tag>",
	                   "/LAR/LArElecCalibH8/LArPedestal<tag>LArPedestal-"+LArTB04FolderTag_Pedestal+"</tag>"]
	
	for i in range(len(LArH8FolderList)):
		LArH8FolderList[i] += LArH8DBConnection
	
	IOVDbSvc = Service( "IOVDbSvc" )
	IOVDbSvc.Folders += LArH8FolderList

#
# Delay run reconstruction
#
if not 'RunAccumulator' in dir():
	RunAccumulator = True
if not 'UseAccumulatedDigits' in dir():
	UseAccumulatedDigits = True
if not 'StripsXtalkCorr' in dir():
	StripsXtalkCorr = False

	

theApp.Dlls += ["LArRawUtils"]
theApp.Dlls += ["LArTools"]
theApp.Dlls += ["LArCalibUtils"]
theApp.Dlls += ["LArRecUtils"]
theApp.Dlls += ["LArROD"]

if ( RunAccumulator ) :
	theApp.topAlg += ["LArCalibDigitsAccumulator"]
	LArCalibDigitsAccumulator = Algorithm("LArCalibDigitsAccumulator")
	LArCalibDigitsAccumulator.KeyList   += [ "HIGH" , "MEDIUM" , "LOW" ]
	### DelayScale example: in CTB 2004 240 TTCrx steps in a sampling period
	LArCalibDigitsAccumulator.DelayScale = (25./240.)*ns
	
if ( StripsXtalkCorr ) :
	theApp.topAlg += ["LArStripsCrossTalkCorrector"]
	LArStripsCrossTalkCorrector = Algorithm("LArStripsCrossTalkCorrector")
	LArStripsCrossTalkCorrector.KeyList += [ "HIGH" , "MEDIUM" , "LOW" ]
	LArStripsCrossTalkCorrector.ADCsaturation = 4095
	LArStripsCrossTalkCorrector.UseAccumulatedDigits = UseAccumulatedDigits
	LArStripsCrossTalkCorrector.OutputLevel = VERBOSE

theApp.Dlls   += ["LArCalibUtils"]
theApp.TopAlg += ["LArCaliWaveBuilder"]
LArCaliWaveBuilder=Algorithm("LArCaliWaveBuilder")

LArCaliWaveBuilder.UseAccumulatedDigits = UseAccumulatedDigits
LArCaliWaveBuilder.KeyList         = [ "HIGH" , "MEDIUM" , "LOW" ]
LArCaliWaveBuilder.KeyOutput       = KeyOut

if ( StripsXtalkCorr ) :
	LArCaliWaveBuilder.ADCsaturation = 0
else :
	LArCaliWaveBuilder.ADCsaturation = 4095
LArCaliWaveBuilder.SubtractPed     = SubtractPed
LArCaliWaveBuilder.RecAllCells     = False

LArCaliWaveBuilder.WriteDB         = WriteNovaDB
LArCaliWaveBuilder.PreviousRunToDB = PreviousRunToDB
#LArCaliWaveBuilder.FolderName      = "LArElecCalibTB04"
LArCaliWaveBuilder.FullFolderName  = FullFolderName 

if ( WriteNtuple ) :
	theApp.Dlls += ["LArCalibTools"]
	theApp.TopAlg += [ "LArWaves2Ntuple/LArCaliWaves2Ntuple"]
	LArCaliWaves2Ntuple = Algorithm( "LArCaliWaves2Ntuple" )
	LArCaliWaves2Ntuple.NtupleName  = "CALIWAVE"
	LArCaliWaves2Ntuple.KeyList     = [ KeyOut ]
	
	theApp.Dlls += [ "RootHistCnv" ]
	theApp.HistogramPersistency = "ROOT"
	NTupleSvc = Service( "NTupleSvc" )
	NTupleSvc.Output = [ "FILE1 DATAFILE='"+RootFileName+"' OPT='NEW'" ]

if ( WritePOOLFile ) :
        include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
        theApp.Dlls   += [ "LArAthenaPoolPoolCnv" ]
        theApp.Dlls   += [ "LArCondAthenaPoolPoolCnv" ]

        include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
        ToolSvc.ConditionsAlgStream.OutputFile = POOLFileName
        
        OutputConditionsAlg.ObjectList = [ "LArCaliWaveContainer#"+KeyOut]

###########################################################################

#EventSelector.SkipEvents = 38400

#RDBAccessSvc.UseDBConnSvc = False 

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = INFO
MessageSvc.defaultLimit = 1000;

###########################################################################
