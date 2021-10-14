###########################################################################
#
# Example jobOptions to run TCM fit (LArTCmPhysWavePredictor)
#
###########################################################################
#
# Main steering options
#
###########################################################################

PhysWaveFolder = ["/lar/LArElecCalibTB04/LArPhysWaveContainer/LArPhysWave"]
PhysWaveTag    = ""

CaliWaveFolder = ["/lar/LArElecCalibTB04/LArCaliWaves/LArCaliWaveHigh"]
CaliWaveTag    = ""

LArCondCnvDbServer = "atlmysql01.cern.ch"

#
# Select run number to trigger IOV
#

RunDir    = "/castor/cern.ch/atlas/testbeam/combined/2004"
RunPrefix = "daq_SFI-51_calo"
RunNumber = 1004019

#
# IOV (if DB writing of predicted LArPhysWave and LArShape is enabled)
#
WriteNovaDB   = False
IOVBeginRun   = 0
IOVEndRun     = 2147483647 # 0x7FFFFFFF
IOVBeginEvent = 0
IOVEndEvent   = 4294967295 #0xFFFFFFFF

#
# ROOT file(s) with predicted physics wave and parameters
# Note: LArTCMPhysWavePredictor also outputs  custom ROOT file called "LArTCMPred.root"
#
WriteNtuple    = True 
NtupleFileName = "LArTCMOutput.root" #  (uses LArCalibTools)

###########################################################################
#
# Fake the reconstruction of one event from one run to trigger IOV
#
include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )

theApp.Dlls += [ "LArByteStream"]
ByteStreamInputSvc = Service( "ByteStreamInputSvc" )

ByteStreamInputSvc.InputDirectory += [RunDir]
ByteStreamInputSvc.FilePrefix     += [RunPrefix]
ByteStreamInputSvc.RunNumber       = [RunNumber]

theApp.EvtMax = 1

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/FREE"]

#
# Detector description (LAr only)
#
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DoIdChecks
DetDescrCnvSvc.LArIDFileName = "IdDictParser/IdDictLArCalorimeter_H8_2004.xml"

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.em_setOn() 

ToolSvc = Service( "ToolSvc" )
                                       
#
# Condition Database (read cumulated Physics waves and Calibration Waves)
#
include( "LArCondCnv/LArCondCnv_Config_jobOptions.py" )
include( "LArCondCnv/LArCondCnv_IOVDbSvc_jobOptions.py" )

IOVDbSvc = Service( "IOVDbSvc" )

IOVDbSvc.serverName = LArCondCnvDbServer
IOVDbSvc.DBname     = "conditions_ctb_2004"
if ( not WriteNovaDB ) :
        IOVDbSvc.userName   = "conditions" 
        IOVDbSvc.userPwd    = "conditions"
if ( WriteNovaDB ) :
        IOVDbSvc.userName   = "you_should_know_it" 
        IOVDbSvc.userPwd    = "othewise_do_not_attempt_to_write"

if 'PhysWaveTag' in dir():
	for i in range(len(PhysWaveFolder)) :
		PhysWaveFolder[i] += '<tag>'+ PhysWaveTag + '</tag>'

if 'CaliWaveTag' in dir():
	for i in range(len(CaliWaveFolder)) :
		CaliWaveFolder[i] += '<tag>'+ CaliWaveTag + '</tag>'

IOVDbSvc.Folders += PhysWaveFolder
IOVDbSvc.Folders += CaliWaveFolder

#
# IOV
#
ToolSvc.FillNovaIOVTool.BeginRun   = IOVBeginRun
ToolSvc.FillNovaIOVTool.EndRun     = IOVEndRun
ToolSvc.FillNovaIOVTool.BeginEvent = IOVBeginEvent
ToolSvc.FillNovaIOVTool.EndEvent   = IOVEndEvent

###########################################################################
#
# LArTCMPhysWavePredictor settings
#
###########################################################################

#theApp.Dlls += ["LArRawUtils"]
#theApp.Dlls += ["LArTools"]
theApp.Dlls += ["LArCalibUtils"]

theApp.TopAlg += ["LArTCMPhysWavePredictor"]
LArTCMPhysWavePredictor = Algorithm( "LArTCMPhysWavePredictor" )

LArTCMPhysWavePredictor.TestMode          = True
LArTCMPhysWavePredictor.OutputLevel       = DEBUG

LArTCMPhysWavePredictor.DataFromFile      = False # for debug, to use self file from emtb
LArTCMPhysWavePredictor.KeyList           = ["LArCaliWaveHigh","LArCaliWaveMedium","LArCaliWaveLow"] ;
LArTCMPhysWavePredictor.MinuitOutputLevel = 1    # -1 no output, 1 std output
LArTCMPhysWavePredictor.RootRawDump       = True # dump all data to root file
LArTCMPhysWavePredictor.RootOutputFile    = "LArTCMPred.root"

LArTCMPhysWavePredictor.FilterCells = True # global switch of filter cells

# this option is not yet implemented, should be merged from branch LArCalibUtils-00-02-03-TCMfit-01
#LArTCMPhysWavePredictor.FilterCells_Region_Barrel_End = True; # switch for barrel end region

LArTCMPhysWavePredictor.FilterCells_Layer = 2

LArTCMPhysWavePredictor.FilterCells_Phi = True
LArTCMPhysWavePredictor.FilterCells_Phi_Min = 7
LArTCMPhysWavePredictor.FilterCells_Phi_Max = 9

LArTCMPhysWavePredictor.FilterCells_Eta = True 
LArTCMPhysWavePredictor.FilterCells_Eta_Min = 0
LArTCMPhysWavePredictor.FilterCells_Eta_Max = 31

LArTCMPhysWavePredictor.WriteDB    = WriteNovaDB
LArTCMPhysWavePredictor.FolderName = "LArElecCalibTB04" 
LArTCMPhysWavePredictor.FolderName_LArPhysWave      = "/lar/LArElecCalibTB04/LArPhysWaveContainer/TCM"   #  folder for writing predicted LArPhysWave
# this option is not yet implemented, should be merged from branch LArCalibUtils-00-02-03-TCMfit-01
#LArTCMPhysWavePredictor.FolderName_LArShapeComplete = "/lar/LArElecCalibTB04/LArShapeComplete/TCM"       # folder for writing LArWFParams
LArTCMPhysWavePredictor.FolderName_WFParams         = "/lar/LArElecCalibTB04/LArWFParams/TCMLArWFParams" # folder for writing LArWFParams

###########################################################################
#
# write predicted  phys waves and parameters to ntuples
# 
if ( WriteNtuple ) :
	theApp.Dlls += ["LArCalibTools"]
#
# Note: the predicted phys wave container has "LArPhysWavesTCM"
#
        theApp.TopAlg += [ "LArWaves2Ntuple/LArPhysWaves2Ntuple"]
        LArPhysWaves2Ntuple = Algorithm( "LArPhysWaves2Ntuple" )
        LArPhysWaves2Ntuple.NtupleName   = "PHYSWAVE" 
        LArPhysWaves2Ntuple.ReadCaliWave = False
        LArPhysWaves2Ntuple.KeyList = [ "LArPhysWavesTCM" ]
#
# Note the params container has key "LArWFParamsTCM"
#
        theApp.TopAlg += [ "LArWFParams2Ntuple" ]
        LArWFParams2Ntuple = Algorithm( "LArWFParams2Ntuple" )
        LArWFParams2Ntuple.KeyList = [ "LArWFParamsTCM" ]

	theApp.Dlls += [ "RootHistCnv" ]
	theApp.HistogramPersistency = "ROOT"
	NTupleSvc = Service( "NTupleSvc" )
	NTupleSvc.Output = [ "FILE1 DATAFILE='"+NtupleFileName+"' OPT='NEW'" ]

###########################################################################

AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel = INFO

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO

###########################################################################
