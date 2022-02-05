#################################################################
#
# LArPhysWaveBuilder example jobOptions 
#
# Last update: 17/1/2006 (Marco.Delmastro@cern.ch)
#
#################################################################

#
# Change these values according to your need...
#

RawDataDirectory  = "/castor/cern.ch/atlas/testbeam/combined/2004"

RawDataFilePrefix = "daq_SFO-51_combined"
RunNumber         = 2100750
RootProfileName   = "PhysWave2100750.root"

#RawDataFilePrefix = "daq_SFI-51_combined"
#RunNumber         = 2101322
#RootProfileName   = "PhysWave2101322.root"

#################################################################

#LArCondCnvDbServer = "atlobk01.cern.ch"
LArCondCnvDbServer = "atlobk02.cern.ch"
#LArCondCnvDbServer = "atlmysql01.cern.ch"

#################################################################

#
# Raw data file decoding
#
include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
theApp.Dlls += [ "LArByteStream"]
ByteStreamInputSvc = Service( "ByteStreamInputSvc" )
ByteStreamInputSvc.InputDirectory += [RawDataDirectory]
ByteStreamInputSvc.FilePrefix     += [RawDataFilePrefix]
ByteStreamInputSvc.RunNumber       = [RunNumber]

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"]
#
# these are most probably not needed since we are lloking to physics data...
#
#ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/LOW"]
#ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/MEDIUM"]
#ByteStreamAddressProviderSvc.TypeNames += ["LArCalibDigitContainer/HIGH"]

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
ToolSvc.LArRodDecoder.FirstSample = 2

#
# CondDB settings
#
include("LArTBRec/LArTBFlags_jobOptions.py" )
include("LArCondCnv/LArCondCnv_TB04_jobOptions.py")

theApp.Dlls += [ "IOVDbSvc" ]
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "IOVDbSvc" ]

IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.serverName  = LArCondCnvDbServer
IOVDbSvc.DBname      = "conditions_ctb_2004"
IOVDbSvc.userName    = "conditions"
IOVDbSvc.userPwd     = "conditions"

#
# IOV for DB writing
#
ToolSvc.FillNovaIOVTool.BeginRun   = 1000000
ToolSvc.FillNovaIOVTool.EndRun     = 9000000
ToolSvc.FillNovaIOVTool.BeginEvent = 0
ToolSvc.FillNovaIOVTool.EndEvent   = 9999999

#
# Beam Instruments decoding and TBPhase Reconstruction
#
include( "TBCnv/TBReadH8BS_jobOptions.py" )
theApp.Dlls   += [ "TBRec" ]
theApp.TopAlg += ["TBPhaseRec"]
TBPhaseRec     =  Algorithm("TBPhaseRec")
include( "TBRec/H8PhaseRec_jobOptions.py" )
TBPhaseRec.TimeBins    = 24 # 24 bin as in CTB 2004 delay profiles
TBPhaseRec.OutputLevel = INFO

#
# LAr packages
#
theApp.Dlls += ["LArRawUtils"]
theApp.Dlls += ["LArTools"] 
theApp.Dlls += ["LArEventTest"]
theApp.Dlls += ["LArCalibUtils"]     
theApp.Dlls += ["LArRecUtils"]

from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
LArADC2MeVCondAlgDefault()

#
# LArPhysWaveBuilder settings
#
theApp.TopAlg += ["LArPhysWaveBuilder/LArPhysWaveBuilder"]
LArPhysWaveBuilder = Algorithm("LArPhysWaveBuilder")
LArPhysWaveBuilder.GainCont = "FREE"
LArPhysWaveBuilder.Gain = 1
LArPhysWaveBuilder.OutputLevel = INFO

LArPhysWaveBuilder.NEntriesCut = 10

LArPhysWaveBuilder.PSCut     = 1000.*MeV
LArPhysWaveBuilder.StripsCut = 1000.*MeV
LArPhysWaveBuilder.MiddleCut = 7000.*MeV
LArPhysWaveBuilder.BackCut   = 1000.*MeV

LArPhysWaveBuilder.Normalize = True

LArPhysWaveBuilder.RootRawDump    = True
LArPhysWaveBuilder.RootOutputFile = RootProfileName

#
# DO NOT WRITE TO DB!
#
LArPhysWaveBuilder.WriteDB  = False
LArPhysWaveBuilder.UpdateDB = False
LArPhysWaveBuilder.FolderName_LArPhysWave = "/lar/LArElecCalibTB04/LArPhysWaveContainer/LArPhysWave"

ToolSvc.LArParabolaPeakrecoTool.correctBias = True # to use with parabola

#############################################################################

theApp.EvtMax = 100

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = WARNING
MessageSvc.infoLimit   = 100000

AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.FailureMode = 2
AthenaEventLoopMgr.OutputLevel = INFO

#############################################################################
