#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#**************************************************************
#
#       Csc Pedestal Calibration
# 
#==============================================================

runNumber = 2147483647

NovRunDir="/raid02/lampen/datasets/csc/pedRuns/nov/raw/*.data"

import glob

if('inputPat' not in dir()):
  inputPat = NovRunDir
  
myInputFiles = glob.glob(inputPat)
#ServiceMgr.EventSelector.InputCollections=ListFromCastor(CastorDir)


if('outputPre' not in dir()):
  outputPre = "pedestal"

if('outputFile' not in dir()):
  outputFile = outputPre + '.root'

if( 'inputOnlCalibFile' not in dir()):
  inputOnlCalibFile = ""

#myInputFiles = [
#    prelimDir + 'data_test.00124166.calibration_ped.daq.RAW._lb0000._CSC-EB._0002.data'
#]

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

#Set your conditions global tag
conddb.setGlobalTag('COMCOND-ES1P-002-00')

#August 09 cosmics
#conddb.setGlobalTag('COMCOND-MONC-002-00')

#Sept 08 cosmics
#conddb.setGlobalTag('COMCOND-REPC-002-02')

conddb.addOverride("/CSC","Csc-REF-001-00")

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
cscRawDataProvider = Muon__CscRawDataProvider(name    = "MuonCscRawDataProvider",
                                              ProviderTool = ToolSvc.MuonCscRawDataProviderTool)


from RegionSelector.RegSelToolConfig import makeRegSelTool_CSC
cscRawDataProvider.RegionSelectionTool = makeRegSelTool_CSC()

topSequence += cscRawDataProvider

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


#---------------------------------------------------------------
# BS input files
#---------------------------------------------------------------

printfunc ('File list is')
printfunc (myInputFiles)

ServiceMgr.EventSelector.Input = myInputFiles
#ServiceMgr.EventSelector.Input = myTestFiles

#---------------------------------------------------------------
# CSC Calibration from DB
#---------------------------------------------------------------
include ("CscCalibTools/CscCalibTool_jobOptions.py")

theApp.EvtMax = -1 

#load main algorithms
from CscCalibAlgs.CscCalibAlgsConf import MuonCalib__CscCalcPed
CscCalcPed = MuonCalib__CscCalcPed( "CscCalcPed" )
topSequence += CscCalcPed

#Specify output calibration file
CscCalcPed.OutputFile = outputPre + ".cal"
#CscCalcPed.CalOutputVersion = "00-00" #old hash based
CscCalcPed.CalOutputVersion = "03-00"  #New, sca address  based
CscCalcPed.OnlineCalibFile = inputOnlCalibFile
CscCalcPed.DoBitHists = False
CscCalcPed.DoSampHists = False
CscCalcPed.DoCorrelation = False;
#CscCalcPed.OutputLevel =VERBOSE
#Define calculations to do. PedestalRun is contradictory to the other options. 

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "MuonMonManager" )

# ******************************
# Muon Monitoring configuration
# ******************************
monMan = topSequence.MuonMonManager
#monMan = topSequence.PrimaryManager 
monMan.FileKey             = "CscCalibMon" #"muoncosmics" ## FileKey must match that given to THistSvc

## Set global monitoring parameters: see the AthenaMonManager class
# in the Control/AthenaMonitoring package
monMan.ManualDataTypeSetup = True 
monMan.DataType            = "monteCarlo"
monMan.Environment         = "user"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1

from MuonCalibMonitoring.MuonCalibMonitoringConf import CscCalibMonToolPed
CscCalibMonTool = CscCalibMonToolPed ("CscCalibMonToolPed" )
#ToolSvc += CscCalibMonTool
monMan.AthenaMonTools += [CscCalibMonTool]
CscCalibMonTool.GetDebugForAllChannels = False
CscCalibMonTool.StatusReportFileName = outputPre + "CalibReport.txt"
if('reportPrefix' in dir()):
  CscCalibMonTool.StatusReportPrefix = reportPrefix

#CscCalibMonTool.OutputLevel = VERBOSE

#Ensure what we need from cond database is loaded
from MuonCondSvc.CscCondDB import cscCondDB
cscCondDB.addPedFolders() #Set to read pedestal and noise folders from the database
cscCondDB.addRmsFolder()
cscCondDB.addF001Folder()

#--------------------------------------------------------------
# Output files.
#--------------------------------------------------------------

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["CscCalibMon DATAFILE='" + outputFile + "' OPT='RECREATE'"]

ServiceMgr.MessageSvc.infoLimit = 0
ServiceMgr.MessageSvc.debugLimit = 0
