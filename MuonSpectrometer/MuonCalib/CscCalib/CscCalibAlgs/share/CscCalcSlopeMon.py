runNumber = 2147483647

if('inputPat' not in dir()):
  inputPat = NovRunDir
  
import glob
myInputFiles = glob.glob(inputPat)

#Lots of pulses
#defInputFile = "/afs/cern.ch/user/l/lampen/public/bytestreamTest/daq.cal.0079571.No.Streaming.LB0000.CSC-EB._0001.data"
#myGeometryTag = 'ATLAS-GEO-01-00-00'
#myIsCosmics = False
#Old Cosmics
#defInputFile = "rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001209_file01.data"
#myGeometryTag = "MuonSpectrometer-CSC-CS-00"
#myIsCosmics = True
#Cosmics
#defInputFile = "rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001853_file01.data"
#myGeometryTag = "MuonSpectrometer-CSC-CS-00"
#myIsCosmics = True
############################

#######################################################################
#Input/Ouptut job settings. 
#Command line options can give an input file as well as an output prefix
#######################################################################
if('outputPre' not in dir()):
  outputPre = "pulser"

if('outputTextFile' not in dir()):
  outputTextFile = outputPre + ".cal"

if('outputFile' not in dir()):
  outputFile = outputPre + '.root'

#include('CscCalibAlgs/CscCalibCommonOptions.py')

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc

#---------------------------------------------------------------
# Detector Initialization #---------------------------------------------------------------
#Set to read from real data COOL data

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_data()
from IOVDbSvc.CondDB import conddb

DetDescrVersion = "ATLAS-GEONF-08-00-00"
include ("RecExCond/RecExCommon_flags.py")
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
include ("RecExCond/AllDet_detDescr.py")

#Setup conditions global tag
conddb.setGlobalTag('COMCOND-ES1P-002-00')

#Want to view most recent values
Service("IOVDbSvc").forceRunNumber = runNumber;
Service("IOVDbSvc").forceLumiblockNumber = 1

#---------------------------------------------------------------
# Event Loop  
#---------------------------------------------------------------
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

#---------------------------------------------------------------
# CSC ROD Decoder
#---------------------------------------------------------------
from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscROD_Decoder
CscRodDecoder = Muon__CscROD_Decoder(   name = "CscROD_Decoder",
    IsCosmics = False, #False for collision
    #IsCosmics = True, #False for collision
    IsOldCosmics = False)
CscRodDecoder.OutputLevel = WARNING
ToolSvc += CscRodDecoder

#---------------------------------------------------------------
# CSC BS -> RDO
#---------------------------------------------------------------
# --- ROD Decoder
from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CSC_RawDataProviderTool
MuonCscRawDataProviderTool = Muon__CSC_RawDataProviderTool(name    = "MuonCscRawDataProviderTool",
    Decoder = CscRodDecoder)
ToolSvc += MuonCscRawDataProviderTool
printfunc (MuonCscRawDataProviderTool)

# --- RawData Provider
from MuonByteStream.MuonByteStreamConf import Muon__CscRawDataProvider
topSequence += Muon__CscRawDataProvider(name         = "MuonCscRawDataProvider",
    ProviderTool = ToolSvc.MuonCscRawDataProviderTool)
printfunc (topSequence.MuonCscRawDataProvider)

# --- BS Converter 
theApp.Dlls   += [ "ByteStreamCnvSvc", "ByteStreamCnvSvcBase"]

# --- Services
theApp.ExtSvc += ["ByteStreamEventStorageInputSvc/ByteStreamInputSvc"]
theApp.ExtSvc += [ "ROBDataProviderSvc/ROBDataProviderSvc" ]
theApp.ExtSvc += [ "EventSelectorByteStream/EventSelector"]
theApp.EvtSel = "EventSelector"

#---------------------------------------------------------------
# Muon Converters
#---------------------------------------------------------------
include ( "MuonEventCnvTools/MuonEventCnvTools_jobOptions.py" )
include ( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )
#include ("MuonRdoToPrepData/MuonRdoToMuonPrepData_jobOptions.py")
 

#Set input files
ServiceMgr.EventSelector.Input = myInputFiles

#---------------------------------------------------------------
# CSC Calibration from DB
#---------------------------------------------------------------
include ("CscCalibTools/CscCalibTool_jobOptions.py")

theApp.EvtMax = -1 

#load main algorithms

from CscCalibAlgs.CscCalibAlgsConf import MuonCalib__CscCalcSlope
CscCalcSlope = MuonCalib__CscCalcSlope( "CscCalcSlope" )
topSequence += CscCalcSlope
#Specify output calibration file
CscCalcSlope.OutputFile =outputTextFile

#Define calculations to do. PedestalRun is contradictory to the other options. 
CscCalcSlope.DoCrossTalkFix = True
CscCalcSlope.DoBipolarFit = True
CscCalcSlope.GetPedFromFile = False
#CscCalcSlope.OutputLevel = DEBUG

#if(ToolSvc.CscRdoContByteStreamTool.IsCosmicData) :
#    CscCalcSlope.ExpectedChamberLayer = 1

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "MuonMonManager" )

# ******************************
# Muon Monitoring configuration
# ******************************
monMan = topSequence.MuonMonManager
#monMan = topSequence.PrimaryManager 
monMan.FileKey             = "CscCalibMon" #"muoncosmics" ## FileKey must match that given to THistSvc

## Set global monitoring parameters: see the AthenaMonManager class
# in the Control/AthenaMonitoring package monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "user"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1

from MuonCalibMonitoring.MuonCalibMonitoringConf import CscCalibMonToolSlope
CscCalibMonTool = CscCalibMonToolSlope (
        name = "CscCalibMonToolSlope"
        )
#ToolSvc += CscCalibMonTool
##CscCalibMonTool.OutputLevel = DEBUG
monMan.AthenaMonTools += [CscCalibMonTool]
#Conditions DB service
#from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag("COMCOND-003-00") 
from MuonCondSvc.CscCondDB import cscCondDB
cscCondDB.addPedFolders() #Set to read pedestal and noise folders from the database
cscCondDB.addPSlopeFolder() #Set to read slope folder from database
#if(#ToolSvc.CscRdoContByteStreamTool.IsCosmicData) :
#    cscCondDB.overrideBaseTag('Csc-cosmic-DEFAULTCOND')


svcMgr.MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.defaultLimit=1000000;
              
#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------

#--------------------------------------------------------------------------- 
printfunc ("List all DLL")
printfunc (theApp.Dlls)
printfunc ("List all ExtSvc")
printfunc (theApp.ExtSvc)
printfunc ("List of all top algorithms")
printfunc (theApp.TopAlg)
#--------------------------------------------------------------------------- 


from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["CscCalibMon DATAFILE='" + outputFile + "' OPT='RECREATE'"]


