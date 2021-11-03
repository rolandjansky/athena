###########################################################################
#
#  * this jobOption is written to test functionality of calibration data classes
#    LArCaliPulseParamsComplete, LArDetCellParamsComplete, LArPhysCaliTdiffComplete,
#    LArTdriftComplete
#
#  * performed operations are:
#
#    - read master waves from DB
#    - extract wfparams and predict physics waves (LArPhysWavePredictor)
#    - save wfparams to DB folders LArCaliPulseParams, LArDetCellParams,
#      LArPhysCaliTdiff, LArTdrift
#    - save waves to ntuples (if requested)
#
###########################################################################

# include ("RecExCommon/RecExCommon_flags.py")
# DetFlags.ID_setOff()
# DetFlags.Muon_setOff()
# include ("RecExCommon/RecExCommon_topOptions.py")

include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DetectorManagers += ["CaloTTDescrManager"]
DetDescrCnvSvc.DetectorManagers += ["CaloMgr"]

LArCondCnvDbServer = 'atlmysql01.cern.ch'

theApp.Dlls += [ "LArByteStream"]

ByteStreamInputSvc = Service( "ByteStreamInputSvc" )
ByteStreamInputSvc.InputDirectory += ["/castor/cern.ch/atlas/testbeam/lar/2004"]
ByteStreamInputSvc.FilePrefix += ["daq_ROS-41_LargCalib"]
ByteStreamInputSvc.RunNumber = [3144]
theApp.EvtMax = 1

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )

ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/LOW"]
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/MEDIUM"]
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/HIGH"]
#ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"]

# include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )
include( "LArDetDescr/LArDetDescr_H8_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DoIdChecks

ToolSvc = Service( "ToolSvc" )

#obsolete!
# include("LArCondCnv_CalibTB04_jobOptions.py")
# include("my_LArCondCnv_TB04_jobOptions.py")
include( "LArCondCnv/LArCondCnv_Config_jobOptions.py" )
include( "LArCondCnv/LArCondCnv_IOVDbSvc_jobOptions.py" )

LArCondCnvSvc = Service( "LArCondCnvSvc" )

theApp.Dlls += [ "IOVDbSvc" ]
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "IOVDbSvc" ]

IOVDbSvc.serverName = LArCondCnvDbServer
IOVDbSvc.DBname     = "conditions_ctb_2004"; 
IOVDbSvc.userName   = "condwriter"; 
IOVDbSvc.userPwd    = "pwd4writer"; 

IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArCalibParams"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArPedestal"]

IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveHigh"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveMedium"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveLow"]

ToolSvc.FillNovaIOVTool.BeginRun   = 0
ToolSvc.FillNovaIOVTool.EndRun     = 9000000
ToolSvc.FillNovaIOVTool.BeginEvent = 0
ToolSvc.FillNovaIOVTool.EndEvent   = 999999

theApp.Dlls += ["LArRawUtils"]
theApp.Dlls += ["LArTools"]
theApp.Dlls += ["LArCalibUtils"]
theApp.Dlls += ["LArCalibTools"]

#
# extract parameters and predict physics waveforms from master waveforms
#
theApp.TopAlg += [ "LArPhysWavePredictor" ]
LArPhysWavePredictor=Algorithm( "LArPhysWavePredictor" )
LArPhysWavePredictor.OutputLevel = INFO
LArPhysWavePredictor.TestMode = TRUE # only one wave per container, to speed up test
LArPhysWavePredictor.KeyList=[ "LArMasterWaveHigh" , "LArMasterWaveMedium" , "LArMasterWaveLow" ]
# LArPhysWavePredictor.Tdrift       = [ 420 , 475 , 475 , 475 ]   # drift time for the four layers
# LArPhysWavePredictor.TimeShift = TRUE
ToolSvc.LArPhysWaveTool.NormalizeCali    = TRUE
LArPhysWavePredictor.WriteDB = FALSE

theApp.TopAlg += [ "LArWFParams2DB" ]
LArWFParams2DB=Algorithm( "LArWFParams2DB" )
LArWFParams2DB.OutputLevel = DEBUG
# LArWFParams2DB.ParamsFolderName = "test"
# LArWFParams2DB.WriteCaliPulseParams = False
# LArWFParams2DB.WriteDetCellParams = False
# LArWFParams2DB.WriteTdiff = False
# LArWFParams2DB.WriteTdrift = False

IOVDbSvc.OutputLevel = INFO

theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='LArWaves.root' OPT='NEW'" ]

AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=ERROR

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = WARNING
###########################################################################
