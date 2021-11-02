#======================================================================
# TOP Job options file for H6 TB2004 Calibration run decoding
#======================================================================

RunNumber = 80031 # EMEC calibration run
FilePrefix = "daq_calEmec_H6"

InputDirectory = "/castor/cern.ch/atlas/testbeam/cbt-ec2/calib"
OutputDir="."

#----------------------------------------------------------
doSim=False  # Used in RecExTB_CondDB.py
#----------------------------------------------------------
# Compose output file names
HistOutputFileName='%(dir)s/Histos_%(No)d.root' % {"dir" : OutputDir, "No" : RunNumber}
NtupleOutputFileName='%(dir)s/Ntuple_%(No)d.root' % {"dir" : OutputDir, "No" : RunNumber}
ESDOutputFileName='%(dir)s/ESD_%(No)d.root' % {"dir" : OutputDir, "No" : RunNumber}
# ------------------------------------

include( "AthenaCommon/Atlas.UnixStandardJob.py" )

ToolSvc = Service( "ToolSvc" )
ToolSvc.TBByteStreamCnvTool.Dump        = False
ToolSvc.TBByteStreamCnvTool.OutputLevel = INFO
    
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_ctbh6()
GlobalFlags.Luminosity.set_zero()

GlobalFlags.DataSource.set_data()
    
#####################################################
# Next 2 lines equivalent to some in LArTBFlags for H8
LArCondCnvDbServer = 'atlobk02.cern.ch'
LArEC04IOVDbTag = ""
######################################################

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.LAr_setOn()
DetFlags.makeRIO.LAr_setOn()
DetFlags.digitize.LAr_on()
DetFlags.readRDOBS.LAr_setOn()
DetFlags.Print()
include ("AtlasGeoModel/SetGeometryVersion.py")
include ("AtlasGeoModel/GeoModelInit.py")

#-----------------------
# ByteStream Input 
GlobalFlags.InputFormat.set_bytestream()
include( "TBCnv/TBReadH6BS_Calib_jobOptions.py" )

# Specify input file
ByteStreamInputSvc = Service( "ByteStreamInputSvc" )

ByteStreamInputSvc.InputDirectory = [ InputDirectory ]
ByteStreamInputSvc.FilePrefix     = [ FilePrefix ]
ByteStreamInputSvc.RunNumber      = [ RunNumber ]
    
# ------------------------------------------------------------
# Specify IOVDbSvc and access conditions database
include( "RecExTB/RecExTB_CondDB.py" )

#-------------------------------------------------------------
# Detector description, conditions database and BS converters

# # loading of the DetectorStore 
# include( "DetDescrCnvSvc/DetStore_joboptions.py" )

include ("LArDetDescr/LArDetDescr_H6_joboptions.py")
    
# LAr DB access   *** cut from LArCondCnv_EC04_jobOptions.py
# include("LArCondCnv/LArCondCnv_EC04_jobOptions.py")
include( "LArCondCnv/LArCondCnv_Config_jobOptions.py" )
    
IOVDbSvc = Service( "IOVDbSvc" )
    
LArEC04FolderList=["/lar/LArElecCalibEC04/LArPedestal",
                   "/lar/LArElecCalibEC04/LArAutoCorr",
                   "/lar/LArElecCalibEC04/LArRamp",
                   "/lar/LArElecCalibEC04/LArDAC2uA",
                   "/lar/LArElecCalibEC04/LAruA2MeV",
                   "/lar/LArElecCalibEC04/LArPhysWaveContainer/LArPhysWave",
                   "/lar/LArElecCalibEC04/LArShapeComplete/LArPhysWave",
                   "/lar/LArElecCalibEC04/LArOFCPhys/LArOFCPhysShape"]
 
if 'LArEC04IOVDbTag' in dir():
    for i in range(len(LArEC04FolderList)) :
        LArEC04FolderList[i] += '<tag>'+ LArEC04IOVDbTag + '</tag>'
            
IOVDbSvc.Folders += LArEC04FolderList;
            
print ' IOVDbSvc.Folders= ', IOVDbSvc.Folders ,  '\n'
 
# IOV server
IOVDbSvc.serverName = "atlobk02.cern.ch"
IOVDbSvc.DBname     = "conditions_ctb_2004"; 
IOVDbSvc.userName   = "conditions"; 
IOVDbSvc.userPwd    = "conditions";

#*** end cut from LArCondCnv_EC04_jobOptions.py
    
# -----------------------------
# Reconstruction
# non-ATLAS detectors

theApp.TopAlg += ["TBEventStreamer/EventInfoStreamer"]
EventInfoStreamer = Algorithm( "EventInfoStreamer" )
EventInfoStreamer.ToolNames += [ "TBEventInfoStreamerTool/EventTypeSelector" ]
# possible selection: "Physics", "Special", "f/e_Calibration",
# "Random", "BPC_Calibration", default: "Physics"
EventInfoStreamer.EventTypeSelector.AcceptedTypes=["f/e_Calibration"]

# theApp.TopAlg += [ "LArCalibDigitPreProcessor<TBLArCalibDigitContainer>/TBLArDigProcessorLOW" ]
# TBLArDigProcessorLOW = Algorithm("TBLArDigProcessorLOW")
# TBLArDigProcessorLOW.InputContainers = ["LOW"]
# TBLArDigProcessorLOW.OutputContainer = "LOW"
# TBLArDigProcessorLOW.OutputLevel     = INFO

theApp.TopAlg += [ "LArCalibDigitPreProcessor<TBLArCalibDigitContainer>/TBLArCalDigProcessorMEDIUM" ]
TBLArCalDigProcessorMEDIUM = Algorithm("TBLArCalDigProcessorMEDIUM")
TBLArCalDigProcessorMEDIUM.InputContainers = ["MEDIUM"]
TBLArCalDigProcessorMEDIUM.OutputContainer = "MEDIUM"
TBLArCalDigProcessorMEDIUM.OutputLevel     = INFO

# theApp.TopAlg += [ "LArCalibDigitPreProcessor<TBLArCalibDigitContainer>/TBLArDigProcessorHIGH" ]
# TBLArDigProcessorHIGH = Algorithm("TBLArDigProcessorHIGH")
# TBLArDigProcessorHIGH.InputContainers  = ["HIGH"]
# TBLArDigProcessorHIGH.OutputContainer  = "HIGH"
# TBLArDigProcessorHIGH.OutputLevel      = INFO

# -- basic monitoring to test the calibration mapping
theApp.TopAlg += ["AthenaMon/LArCalDigMon"]
LArCalDigMon = Algorithm("LArCalDigMon")

LArCalDigMon.AthenaMonTools += ["LArDigMonTool<LArCalibDigitContainer>/LArCalDigMonTool"]
ToolSvc.LArCalDigMonTool.histoPathBase = "/stat/LArCalibDigMon"
ToolSvc.LArCalDigMonTool.digitKey      = "MEDIUM"
ToolSvc.LArCalDigMonTool.pedMode       = "SAMPLE"
ToolSvc.LArCalDigMonTool.pedSamp       = 0
ToolSvc.LArCalDigMonTool.adcCut        = 50
ToolSvc.LArCalDigMonTool.OutputLevel   = INFO

# -- and try building cali waves

theApp.Dlls += ["LArCalibUtils"]
theApp.Dlls += ["LArCalibTools"]

theApp.TopAlg += ["LArCaliWaveBuilder"]
LArCaliWaveBuilder = Algorithm( "LArCaliWaveBuilder" )
LArCaliWaveBuilder.KeyList = ["MEDIUM"]
LArCaliWaveBuilder.SubtractPed = False
LArCaliWaveBuilder.OutputLevel=INFO

theApp.TopAlg += [ "LArWaves2Ntuple<LArCaliWaveContainer>/LArCaliWaves2Ntuple"]
LArCaliWaves2Ntuple = Algorithm( "LArCaliWaves2Ntuple" )
LArCaliWaves2Ntuple.NtupleName="CALIWAVE"

theApp.TopAlg += ["AthenaMon/LArWaveMon"]
LArWaveMon = Algorithm("LArWaveMon")
LArWaveMon.AthenaMonTools += ["LArWaveMonTool/wavemon"]
ToolSvc.wavemon.histoPathBase = "/stat/wave"
ToolSvc.wavemon.waveKey      = "MEDIUM"

# -- use root histos
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"

HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = HistOutputFileName

NTupleSvc = Service( "NTupleSvc" )
# NtupleSvc.Output=["FILE1 DATAFILE='Waves.root' TYP='ROOT' OPT='NEW'"]
NTupleSvc.Output = [ "FILE1 DATAFILE='"+NtupleOutputFileName+"' OPT='NEW'" ]

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
theApp.EvtMax = 1000000
# EventSelector.SkipEvents = 300;
AthenaEventLoopMgr = Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.FailureMode=2
AthenaEventLoopMgr.OutputLevel = WARNING

# # Dump all objects available
# StoreGateSvc = Service( "StoreGateSvc" )
# StoreGateSvc.Dump = True
 
MessageSvc.defaultLimit=1000000;
theApp.AuditAlgorithms=True
#theApp.AuditServices=True
#theApp.AuditTools=True # FIXME crash on finalisation 
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"
