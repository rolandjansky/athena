###########################################################################
#
#  * read master waves from DB
#  * compute delta responses
#  * save delta response waves to ntuple
#
###########################################################################

include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DetectorManagers += ["CaloTTDescrManager"]
DetDescrCnvSvc.DetectorManagers += ["CaloMgr"]

#LArCondCnvDbServer = 'atlobk01.cern.ch'
LArCondCnvDbServer = 'atlobk02.cern.ch'

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

include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DoIdChecks

ToolSvc = Service( "ToolSvc" )

include("LArCondCnv_CalibTB04_jobOptions.py")
IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArPedestal"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveHigh"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveMedium"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArCaliWaves/LArMasterWaveLow"]
IOVDbSvc.serverName="atlobk01.cern.ch"
IOVDbSvc.DBname = "conditions_ctb_2004"
#IOVDbSvc.userName = "conditions"
#IOVDbSvc.userPwd  = "conditions"
IOVDbSvc.userName = "lar"
IOVDbSvc.userPwd  = "liquid_CondDB"

ToolSvc.FillNovaIOVTool.BeginRun   = 0
ToolSvc.FillNovaIOVTool.EndRun     = 9000000
ToolSvc.FillNovaIOVTool.BeginEvent = 0
ToolSvc.FillNovaIOVTool.EndEvent   = 999999

theApp.Dlls += ["LArRawUtils"]
theApp.Dlls += ["LArTools"]
theApp.Dlls += ["LArCalibUtils"]

#
# predict delta responses from master waveforms
#
theApp.TopAlg += [ "LArDeltaRespPredictor" ]
LArDeltaRespPredictor=Algorithm( "LArDeltaRespPredictor" )
LArDeltaRespPredictor.KeyList = [ "LArMasterWaveHigh" , "LArMasterWaveMedium" , "LArMasterWaveLow" ]
#LArDeltaRespPredictor.KeyList = [ "LArMasterWaveLow" ]
ToolSvc.LArDeltaRespTool.NormalizeCali    = TRUE
ToolSvc.LArDeltaRespTool.InjPointCorr     = TRUE
ToolSvc.LArDeltaRespTool.TimeOriginShift  = FALSE
ToolSvc.LArDeltaRespTool.SubtractBaseline = FALSE
LArDeltaRespPredictor.WriteDB  = FALSE

#
# write master delta responses to ntuple
# 
theApp.TopAlg += [ "LArWaves2Ntuple<LArCaliWaveContainer>/LArCaliWaves2Ntuple"]
LArCaliWaves2Ntuple = Algorithm( "LArCaliWaves2Ntuple" )
LArCaliWaves2Ntuple.NtupleName  = "DELTARESP"
LArCaliWaves2Ntuple.KeyList = [ "LArMasterWaveHigh_delta" , "LArMasterWaveMedium_delta" , "LArMasterWaveLow_delta" ]
#LArCaliWaves2Ntuple.KeyList = [ "LArMasterWaveLow_delta" ]

#
# output levels:
#
LArDeltaRespPredictor.OutputLevel    = INFO
ToolSvc.LArWFParamTool.OutputLevel   = INFO
ToolSvc.LArDeltaRespTool.OutputLevel = INFO	
LArCaliWaves2Ntuple.OutputLevel      = INFO
	
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='LArDeltaResp.root' OPT='NEW'" ]

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = WARNING
###########################################################################
