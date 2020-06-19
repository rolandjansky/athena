###########################################################################
#
#  jobOptions to produce LAr OFCs:
#    * read calibration and/or physics waveforms from DB
#    * read autocorrelation matrixes from DB
#    * compute OFCs accordingly
#    * write OFCs to DB (and/or ascii file, Root ntuple) 
#
###########################################################################

doCalibOFC = False
doPhysOFC  = True

readNovaDB         = True

CaliWaveFolderName = ["/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveHigh","/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveMedium","/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveLow"]
CaliWaveFolderTag  = "TB04-Default"

PhysWaveFolderName = [ "/lar/LArElecCalibTB04/LArPhysWaveContainer/RTM" ]
PhysWaveFolderTag  = "" # HEAD

POOLFileDir          = "./"
#POOLFileDir          = "/data/mdelmast/ctb2004/ofc/"
#POOLFileDir          = "/shift/mdelmast/ctb2004/ofc/"

readPOOLFile       = False
CaliWavePOOLFile   = ""
PhysWavePOOLFile   = "LArRTMPhysWaves-NoTimeShift.pool.root"

writePOOLFile      = False
OFCCaliPOOLFile    = ""
OFCPhysPOOLFile    = "LArRTMOFC-NoTimeShift.pool.root"

writeNovaDB        = False
CaliOFCDBFolder    = "LArOFCCalib/MWF"
PhysOFCDBFolder    = "LArOFCPhys/RTM"

IOVBeginRun        = 0
IOVEndRun          = 2147483647 # 0x7FFFFFFF
RunNumber          = 2102103    # RecExTB default run

IncludeACFolder    = True
UseDefaultAutoCorr = True
AutoCorrFolderTag  = "TB04-Default"

WriteNT            = False
DumpOFC            = False
DumpCaliOFCfile    = "caliOFC.dat"
DumpPhysOFCfile    = "physOFC.dat"

###########################################################################

#Period = "Empty"

Period = "3" 
#Period = "4" 
#Period = "5" 
#Period = "6a"
#Period = "6b-1"
#Period = "6b-2"
#Period = "6b-3"
#Period = "6b-4"
#Period = "7"
#Period = "8-1" 
#Period = "8-3" 

#include("thisjob.py")

###########################################################################

if ( Period == "3" ) :
	IOVBeginRun = 1000709
	IOVEndRun   = 1000797
	RunNumber   = 1000709
	PhysWavePOOLFile = "LArRTMPhysWaves-Shifted-Periods:3,4,5,6a.pool.root"
	OFCPhysPOOLFile  = "LArRTMOFC-Periods:3.pool.root"

if ( Period == "4" ) :
	IOVBeginRun = 1000813
	IOVEndRun   = 1000918
	RunNumber   = 1000813
	PhysWavePOOLFile = "LArRTMPhysWaves-Shifted-Periods:3,4,5,6a.pool.root"
	OFCPhysPOOLFile  = "LArRTMOFC-Periods:4.pool.root"

if ( Period == "5" ) :
	IOVBeginRun = 1000919
	IOVEndRun   = 1001056
	RunNumber   = 1000919
	PhysWavePOOLFile = "LArRTMPhysWaves-Shifted-Periods:3,4,5,6a.pool.root"
	OFCPhysPOOLFile  = "LArRTMOFC-Periods:5.pool.root"

if ( Period == "6a" ) :
	IOVBeginRun = 1002007
	IOVEndRun   = 1004234
	RunNumber   = 1002007
	PhysWavePOOLFile = "LArRTMPhysWaves-Shifted-Periods:3,4,5,6a.pool.root"
	OFCPhysPOOLFile  = "LArRTMOFC-Periods:6a.pool.root"

if ( Period == "6b-1" ) :
	IOVBeginRun = 2100024	
	IOVEndRun   = 2100110
	RunNumber   = 2100024
	PhysWavePOOLFile = "LArRTMPhysWaves-Shifted-Periods:6b-1.pool.root"
	OFCPhysPOOLFile  = "LArRTMOFC-Periods:6b-1.pool.root"

if ( Period == "6b-2" ) :
	IOVBeginRun = 2100132	
	IOVEndRun   = 2100253
	RunNumber   = 2100132
	PhysWavePOOLFile = "LArRTMPhysWaves-Shifted-Periods:6b-2.pool.root"
	OFCPhysPOOLFile  = "LArRTMOFC-Periods:6b-2.pool.root"

if ( Period == "6b-3" ) :
	IOVBeginRun = 2100254	
	IOVEndRun   = 2100510
	RunNumber   = 2100254
	PhysWavePOOLFile = "LArRTMPhysWaves-Shifted-Periods:6b-3.pool.root"
	OFCPhysPOOLFile  = "LArRTMOFC-Periods:6b-3.pool.root"

if ( Period == "6b-4" ) :
	IOVBeginRun = 2100513	
	IOVEndRun   = 2100540
	RunNumber   = 2100513
	PhysWavePOOLFile = "LArRTMPhysWaves-Shifted-Periods:6b-4.pool.root"
	OFCPhysPOOLFile  = "LArRTMOFC-Periods:6b-4.pool.root"

if ( Period == "7" ) :
	IOVBeginRun = 2100558
	IOVEndRun   = 2100975
	RunNumber   = 2100558
	PhysWavePOOLFile = "LArRTMPhysWaves-Shifted-Periods:7.pool.root"
	OFCPhysPOOLFile  = "LArRTMOFC-Periods:7.pool.root"

if ( Period == "7-1" ) :
	IOVBeginRun = 2100558
	IOVEndRun   = 2100676 # ???
	RunNumber   = 2100558
	PhysWavePOOLFile = "LArRTMPhysWaves-Shifted-Periods:7-1.pool.root"
	OFCPhysPOOLFile  = "LArRTMOFC-Periods:7-1.pool.root"

if ( Period == "7-2" ) :
	IOVBeginRun = 2100683 # ???
	IOVEndRun   = 2100750
	RunNumber   = 2100683
	PhysWavePOOLFile = "LArRTMPhysWaves-Shifted-Periods:7-2.pool.root"
	OFCPhysPOOLFile  = "LArRTMOFC-Periods:7-2.pool.root"

if ( Period == "7-3" ) :
	IOVBeginRun = 2100751
	IOVEndRun   = 2100766
	RunNumber   = 2100751
	PhysWavePOOLFile = "LArRTMPhysWaves-Shifted-Periods:7-3.pool.root"
	OFCPhysPOOLFile  = "LArRTMOFC-Periods:7-3.pool.root"
	
if ( Period == "7-4" ) :
	IOVBeginRun = 2100767
	IOVEndRun   = 2100975 # ???
	RunNumber   = 2100767
	PhysWavePOOLFile = "LArRTMPhysWaves-Shifted-Periods:7-4.pool.root"
	OFCPhysPOOLFile  = "LArRTMOFC-Periods:7-4.pool.root"


if ( Period == "8-1" ) :
	IOVBeginRun = 2101022
	IOVEndRun   = 2101475
	RunNumber   = 2101022
	PhysWavePOOLFile  = "LArRTMPhysWaves-Shifted-Periods:8-1.pool.root"
	OFCPhysPOOLFile   = "LArRTMOFC-Periods:8-1.pool.root"

#if ( Period == "8-2" ) :
#	IOVBeginRun = 2101571
#	IOVEndRun   = 2101737
#	RunNumber   = 2101571
#	PhysWavePOOLFile  = "LArRTMPhysWaves-Shifted-Periods:8-2.pool.root"
#	OFCPhysPOOLFile   = "LArRTMOFC-Periods:8-2.pool.root"

if ( Period == "8-3" ) :
	IOVBeginRun = 2102095
	IOVEndRun   = 2102992
	RunNumber   = 2102095
	PhysWavePOOLFile  = "LArRTMPhysWaves-Shifted-Periods:8-3.pool.root"
	OFCPhysPOOLFile   = "LArRTMOFC-Periods:8-3.pool.root"

###########################################################################

IOVBeginEvent = 0
IOVEndEvent   = 4294967295 #0xFFFFFFFF
#IOVEndEvent   = 4294967294 #0xFFFFFFFF -1

###########################################################################

#LArCondCnvDbServer = "atlobk01.cern.ch"
#LArCondCnvDbServer = "atlobk02.cern.ch"
LArCondCnvDbServer = "atlmysql01.cern.ch"

###########################################################################

include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )

#
# Fake the reconstruciton of one event from one run to trigger retrieval 
# of condition object
#
theApp.Dlls += [ "LArByteStream"]
ByteStreamInputSvc = Service( "ByteStreamInputSvc" )

include("RecExTB_SetFlags_jobOptions.py")

ByteStreamInputSvc.InputDirectory += ["/castor/cern.ch/atlas/testbeam/combined/2004"]
ByteStreamInputSvc.FilePrefix     += [FilePrefix]
ByteStreamInputSvc.RunNumber       = [RunNumber]

theApp.EvtMax = 1

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/FREE"]
ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/LOW"]
ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/MEDIUM"]
ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/HIGH"]

#
# Detector description (LAr only)
#
include( "LArDetDescr/LArDetDescr_H8_joboptions.py" )
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DoIdChecks
DetDescrCnvSvc.LArIDFileName = "IdDictParser/IdDictLArCalorimeter_H8_2004.xml"

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.em_setOn() 

ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRoI_Map.Print = False

#
# POOL
#
if ( readPOOLFile) :
	include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
	theApp.Dlls += ["EventSelectorAthenaPool"] 
	theApp.Dlls += [ "LArAthenaPoolPoolCnv" ]
	theApp.Dlls += [ "LArCondAthenaPoolPoolCnv" ]
	
	ProxyProviderSvc = Service( "ProxyProviderSvc" )
	ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
	
	CondProxyProvider = Service( "CondProxyProvider" )
	
	EventPersistencySvc = Service( "EventPersistencySvc" )
	EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]

#
# Condition DB
#
include( "LArCondCnv/LArCondCnv_Config_jobOptions.py" )
include( "LArCondCnv/LArCondCnv_IOVDbSvc_jobOptions.py" )

IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.serverName = LArCondCnvDbServer
IOVDbSvc.DBname = "conditions_ctb_2004";

IOVDbSvc.OutputLevel = DEBUG
#IOVDbMgr.OutputLevel = DEBUG

if ( not writeNovaDB ) :
	IOVDbSvc.userName   = "conditions" 
	IOVDbSvc.userPwd    = "conditions"

if ( writeNovaDB ) :
	IOVDbSvc.userName   = "condwriter" 
	IOVDbSvc.userPwd    = "pwd4writer"

AutoCorrFolderName = ["/lar/LArElecCalibTB04/LArAutoCorr"]
if 'AutoCorrFolderTag' in dir() :
	for i in range(len(AutoCorrFolderName)) :
		AutoCorrFolderName[i] += '<tag>'+ AutoCorrFolderTag + '</tag>'

if ( IncludeACFolder ) :
	IOVDbSvc.Folders += AutoCorrFolderName

if ( doCalibOFC ) :
	if ( readNovaDB ) :
        	if 'CaliWaveFolderTag' in dir():
			for i in range(len(CaliWaveFolderName)) :
				CaliWaveFolderName[i] += '<tag>'+ CaliWaveFolderTag + '</tag>'
		IOVDbSvc.Folders += CaliWaveFolderName
	if ( readPOOLFile ) :
		CondProxyProvider.InputCollections = [ POOLFileDir+CaliWavePOOLFile ]
	
if ( doPhysOFC ): 
	if ( readNovaDB ) :
		if 'PhysWaveFolderTag' in dir():
			for i in range(len(PhysWaveFolderName)) :
				PhysWaveFolderName[i] += '<tag>'+ PhysWaveFolderTag + '</tag>'
		IOVDbSvc.Folders += PhysWaveFolderName
	if ( readPOOLFile ) :
		CondProxyProvider.InputCollections = [ POOLFileDir+PhysWavePOOLFile ]

#
# IOV
#
ToolSvc.FillNovaIOVTool.BeginRun   = IOVBeginRun
ToolSvc.FillNovaIOVTool.EndRun     = IOVEndRun
ToolSvc.FillNovaIOVTool.BeginEvent = IOVBeginEvent
ToolSvc.FillNovaIOVTool.EndEvent   = IOVEndEvent

#
# OFC computation
#

theApp.Dlls += ["LArRawUtils"]
theApp.Dlls += ["LArTools"]
theApp.Dlls += ["LArCalibUtils"]

#
# evaluate OFCs for master waveforms ...
#

if ( doCalibOFC ) :
	theApp.TopAlg += [ "LArOFCAlg<LArCaliWaveContainer>/LArCaliOFCAlg"]
	LArCaliOFCAlg = Algorithm( "LArCaliOFCAlg" )
	LArCaliOFCAlg.KeyList     = [ "LArMasterWaveHigh" , "LArMasterWaveMedium" , "LArMasterWaveLow" ]
	LArCaliOFCAlg.Nphase      = 50
	LArCaliOFCAlg.Ndelay      = 24
	LArCaliOFCAlg.TimeShift   = False 
	LArCaliOFCAlg.Verify      = True
	LArCaliOFCAlg.Normalize   = True
	LArCaliOFCAlg.OutputLevel = INFO
	if ( DumpOFC ) :
		LArCaliOFCAlg.DumpOFCfile = DumpCaliOFCfile
	if ( writeNovaDB ) :
		LArCaliOFCAlg.WriteDB   = True
		LArCaliOFCAlg.FolderOFC = CaliOFCDBFolder

#
# ... and for physics waveforms
#

if ( doPhysOFC ) :
	theApp.TopAlg += [ "LArOFCAlg<LArPhysWaveContainer>/LArPhysOFCAlg"]
	LArPhysOFCAlg = Algorithm( "LArPhysOFCAlg" )
	LArPhysOFCAlg.KeyList     = [ "LArMasterWaveHigh" , "LArMasterWaveMedium" , "LArMasterWaveLow" ]
	LArPhysOFCAlg.KeyList    += [ "LArPhysWave" ]
	LArPhysOFCAlg.KeyList    += [ "LArPhysWave_new" ]
	LArPhysOFCAlg.Nphase      = 50
	LArPhysOFCAlg.Ndelay      = 24
	LArPhysOFCAlg.TimeShift   = False
	LArPhysOFCAlg.Verify      = True
	LArPhysOFCAlg.Normalize   = False
	LArPhysOFCAlg.UseDefaultAutoCorr = UseDefaultAutoCorr
	LArPhysOFCAlg.OutputLevel = INFO
	if ( DumpOFC ) :
		LArPhysOFCAlg.DumpOFCfile = DumpPhysOFCfile
	if ( writeNovaDB ) :
		LArPhysOFCAlg.WriteDB         = True
		#LArPhysOFCAlg.PreviousRunToDB = 0
		LArPhysOFCAlg.FolderOFC       = PhysOFCDBFolder
		if ( Period == "Empty" ) :
			LArPhysOFCAlg.StoreEmpty = True # clean up DB folder
	
if ( writePOOLFile ) :
	include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
	theApp.Dlls   += [ "LArAthenaPoolPoolCnv" ]
	theApp.Dlls   += [ "LArCondAthenaPoolPoolCnv" ]	
	
	include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
	
	OutputConditionsAlg.ObjectList = [ "LArOFCComplete#LArOFC" ]
	
	if ( doCalibOFC ) :
		ToolSvc.ConditionsAlgStream.OutputFile = POOLFileDir+OFCCaliPOOLFile	
	if ( doPhysOFC ) :		
		ToolSvc.ConditionsAlgStream.OutputFile = POOLFileDir+OFCPhysPOOLFile
	
	OutputConditionsAlg.OutputLevel         = DEBUG
	ToolSvc.ConditionsAlgStream.OutputLevel = DEBUG

	
if ( WriteNT ) :
	theApp.Dlls += ["LArCalibTools"]
	theApp.TopAlg += [ "LArOFC2Ntuple"]
	LArOFC2Ntuple = Algorithm( "LArOFC2Ntuple" )	
	LArOFC2Ntuple.OutputLevel = INFO
	
	theApp.Dlls += [ "RootHistCnv" ]
	theApp.HistogramPersistency = "ROOT"
	NTupleSvc = Service( "NTupleSvc" )
	NTupleSvc.Output = [ "FILE1 DATAFILE='LArOFC.root' OPT='NEW'" ]
	
AthenaEventLoopMgr = Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel = VERBOSE

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO

###########################################################################

# write out a list of all Storegate collection with their keys and
# lock/unlock state. Very useful for debugging purpose

#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = TRUE
#DetectorStore = Service( "DetectorStore" )
#DetectorStore.Dump = TRUE
#ConditionStore = Service( "ConditionStore" )
#ConditionStore.Dump = TRUE
