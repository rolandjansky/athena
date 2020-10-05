# -*- coding: utf-8 -*-
#========================================================================================
#
# Job options file for the ALFA data processing
#
# author: Petr Hamal
# email : petr.hamal@cern.ch
# date  : 30.06.2015
#
#========================================================================================

#----------------------------------------------------------------------------------------
# Initialize
#
# some variables to be used later in this script
# 
# EvtMax ... a maximum of events which is going to be processed, -1 means all events are used
# RunEpoch ... for ALFA_Ntuple, defines the LHC epoch. RunI = 1, RunII = 2
# CoolData ... true if if one wants to have some COOL data in the n-tuple, see ListCoolFolders
# MeasuredDataType ... switch between "tunnel" or "testbeam" motherboard to detector mapping
# DataType ... 1 for data, 0 for MC
# ListAlgoMD ... a list of MD algorithms used in the reconstruction, i.e. ["MDMultiple", "HalfReco", etc ...]
# ListAlgoOD ... a list of OD algorithms used in the reconstruction, at present only one OD algoritmh available
# MultiplicityOD
# DistanceCutOD
# MultiplicityMD
# OverlapCutMD
# UVCutMD
#               ... all previous variables available for MD/OD tracking/multple algorithms
# MetrologyType ... 3 for sw corrections frm DB used in ALFA_LocRecCorr), 2 ... user defined corrections via JO, 1 ... ideal metrology
# MDGeometryType ... 2 for a real MD fibre metrology, 1 for an ideal MD fibre metrology
# ODGeometryType ... 2 for a real OD fibre metrology, 1 for an ideal OD fibre metrology
# streamName ... name of the output stream for ALFA Ntuples
# CoolDirName ... name of the directory in the output file where COOL trees will be stored (default name is "COOL")


#----------------------------------------------------------------------------------------
FileSuffix        = "run267236"
ALFANtupleOutFile = 'ALFA_Ntuple_%s.root'     % FileSuffix # currently not used
EvtMax            = -1
RunEpoch          = 2
CoolData          = True
MeasuredDataType  = "tunnel"
DataType          = 1
ListAlgoMD        = ["MDMultiple"]
ListAlgoOD        = ["ODTracking"]
MultiplicityCutOD = 30
DistanceCutOD     = 0.5
MultiplicityCutMD = 5
OverlapCutMD      = 0.5
UVCutMD           = 3
MetrologyType     = 3
MDGeometryType    = 2
ODGeometryType    = 2
streamName        = "NtupleStream"

#-------------------------------------------------------------------------------------------------#
# COOL folders for ALFA
#
# a list of COOL folders which can be read by ALFA_Ntuple package, if some run does not contain
# some folder, simple comment it and it will be not used
#-------------------------------------------------------------------------------------------------#
ListCoolFolders = []
ListCoolFolders+= ["/RPO/DCS/BLM"]
ListCoolFolders+= ["/RPO/DCS/FECONFIGURATION"]
ListCoolFolders+= ["/RPO/DCS/HVCHANNEL"]
ListCoolFolders+= ["/RPO/DCS/LOCALMONITORING"]
ListCoolFolders+= ["/RPO/DCS/MOVEMENT"]
ListCoolFolders+= ["/RPO/DCS/RADMON"]
ListCoolFolders+= ["/RPO/DCS/TRIGGERRATES"]
ListCoolFolders+= ["/RPO/DCS/TRIGGERSETTINGS"]
ListCoolFolders+= ["/LHC/DCS/FILLSTATE"]
#ListCoolFolders+= ["/TDAQ/OLC/ALFA"]
ListCoolFolders+= ["/TDAQ/OLC/LHC/FILLPARAMS"]


#-------------------------------------------------------------------------------------------------#
from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AlgSequence import AlgSequence
topSeq=AlgSequence()

from AthenaCommon.AppMgr import theApp

import AthenaPoolCnvSvc.WriteAthenaPool

include("IOVDbSvc/IOVDbSvc_jobOptions.py")
IOVDbSvc = Service( "IOVDbSvc" )
from IOVDbSvc.CondDB import conddb
conddb.PreLoadData=True
IOVDbSvc.OutputLevel = ERROR

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')

# add only folder in ListCollFolders
for folder in ListCoolFolders:
	if folder == "/RPO/DCS/BLM":             conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200</db> /RPO/DCS/BLM")
	if folder == "/RPO/DCS/FECONFIGURATION": conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200</db> /RPO/DCS/FECONFIGURATION")
	if folder == "/RPO/DCS/HVCHANNEL":       conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200</db> /RPO/DCS/HVCHANNEL")
	if folder == "/RPO/DCS/LOCALMONITORING": conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200</db> /RPO/DCS/LOCALMONITORING")
	if folder == "/RPO/DCS/MOVEMENT":        conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200</db> /RPO/DCS/MOVEMENT")
	if folder == "/RPO/DCS/RADMON":          conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200</db> /RPO/DCS/RADMON")
	if folder == "/RPO/DCS/TRIGGERRATES":    conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200</db> /RPO/DCS/TRIGGERRATES")
	if folder == "/RPO/DCS/TRIGGERSETTINGS": conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200</db> /RPO/DCS/TRIGGERSETTINGS")
	if folder == "/LHC/DCS/FILLSTATE":       conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200</db> /LHC/DCS/FILLSTATE")
	if folder == "/TDAQ/OLC/ALFA":           conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TDAQ;dbname=COMP200</db> /TDAQ/OLC/ALFA")
	if folder == "/TDAQ/OLC/LHC/FILLPARAMS": conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TDAQ;dbname=COMP200</db> /TDAQ/OLC/LHC/FILLPARAMS")

# defined DB with sw corrections used in ALFA_LocRecCorr
conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_FWD;dbname=COMP200</db> /FWD/ALFA/position_calibration <tag>FWDALFAposition_calibration-run373-v2</tag>")


#-------------------------------------------------------------------------------------------------#
# ByteStreamCnv
#
# FullFileName ... specify the input RAW file(s)
# if pathena is used, this file name is ignored and replaced by a dataset in --inDS
#-------------------------------------------------------------------------------------------------#
globalflags.InputFormat.set_Value_and_Lock('bytestream')
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
#ServiceMgr.EventSelector.Input = ["/afs/cern.ch/work/g/ggach/public/xAOD/myTest/RAW/data15_13TeV/data15_13TeV.00266904.calibration_ALFACalib.daq.RAW._lb0000._SFO-6._0002.data"]
ServiceMgr.EventSelector.Input = ["/afs/cern.ch/work/g/ggach/public/xAOD/myTest/RAW/data15_13TeV/data15_13TeV.00267358.physics_Main.merge.RAW._lb0005._SFO-2._0001.1"]
from ALFA_RawDataByteStreamCnv.ALFA_RawDataByteStreamCnvConf import ALFA_RawDataProvider


#-------------------------------------------------------------------------------------------------#
# Raw2Digit
#
# nothing can be changed here, everything is defined at the beggining in Initialize section
#-------------------------------------------------------------------------------------------------#
from ALFA_Raw2Digit.ALFA_Raw2DigitConf import ALFA_Raw2Digit
ALFA_Raw2Digit.MeasuredDataType = MeasuredDataType


#-------------------------------------------------------------------------------------------------#
# LocRec
#
# nothing can be changed here, everything is defined at the beggining in Initialize section
#-------------------------------------------------------------------------------------------------#
from ALFA_LocRec.ALFA_LocRecConf import ALFA_LocRec

ALFA_LocRec.DataType          = DataType
ALFA_LocRec.ListAlgoMD        = ListAlgoMD
ALFA_LocRec.ListAlgoOD        = ListAlgoOD
ALFA_LocRec.MultiplicityCutOD = MultiplicityCutOD
ALFA_LocRec.DistanceCutOD     = DistanceCutOD
ALFA_LocRec.MultiplicityCutMD = MultiplicityCutMD
ALFA_LocRec.OverlapCutMD      = OverlapCutMD
ALFA_LocRec.UVCutMD           = UVCutMD

ALFA_LocRec.MetrologyType     = MetrologyType

ALFA_LocRec.B7L1U_MDGeometryType = MDGeometryType
ALFA_LocRec.B7L1U_ODGeometryType = ODGeometryType
ALFA_LocRec.B7L1L_MDGeometryType = MDGeometryType
ALFA_LocRec.B7L1L_ODGeometryType = ODGeometryType
ALFA_LocRec.A7L1U_MDGeometryType = MDGeometryType
ALFA_LocRec.A7L1U_ODGeometryType = ODGeometryType
ALFA_LocRec.A7L1L_MDGeometryType = MDGeometryType
ALFA_LocRec.A7L1L_ODGeometryType = ODGeometryType
ALFA_LocRec.A7R1U_MDGeometryType = MDGeometryType
ALFA_LocRec.A7R1U_ODGeometryType = ODGeometryType
ALFA_LocRec.A7R1L_MDGeometryType = MDGeometryType
ALFA_LocRec.A7R1L_ODGeometryType = ODGeometryType
ALFA_LocRec.B7R1U_MDGeometryType = MDGeometryType
ALFA_LocRec.B7R1U_ODGeometryType = ODGeometryType
ALFA_LocRec.B7R1L_MDGeometryType = MDGeometryType
ALFA_LocRec.B7R1L_ODGeometryType = ODGeometryType


#-------------------------------------------------------------------------------------------------#
# LocRecCorr
#
# nothing can be changed here, everything is defined at the beggining in Initialize section
#-------------------------------------------------------------------------------------------------#
from ALFA_LocRecCorr.ALFA_LocRecCorrConf import ALFA_LocRecCorr

ALFA_LocRecCorr.DataType        = DataType
ALFA_LocRecCorr.MetrologyType   = MetrologyType

ALFA_LocRecCorr.B7L1U_MDGeometryType = MDGeometryType
ALFA_LocRecCorr.B7L1U_ODGeometryType = ODGeometryType
ALFA_LocRecCorr.B7L1L_MDGeometryType = MDGeometryType
ALFA_LocRecCorr.B7L1L_ODGeometryType = ODGeometryType
ALFA_LocRecCorr.A7L1U_MDGeometryType = MDGeometryType
ALFA_LocRecCorr.A7L1U_ODGeometryType = ODGeometryType
ALFA_LocRecCorr.A7L1L_MDGeometryType = MDGeometryType
ALFA_LocRecCorr.A7L1L_ODGeometryType = ODGeometryType
ALFA_LocRecCorr.A7R1U_MDGeometryType = MDGeometryType
ALFA_LocRecCorr.A7R1U_ODGeometryType = ODGeometryType
ALFA_LocRecCorr.A7R1L_MDGeometryType = MDGeometryType
ALFA_LocRecCorr.A7R1L_ODGeometryType = ODGeometryType
ALFA_LocRecCorr.B7R1U_MDGeometryType = MDGeometryType
ALFA_LocRecCorr.B7R1U_ODGeometryType = ODGeometryType
ALFA_LocRecCorr.B7R1L_MDGeometryType = MDGeometryType
ALFA_LocRecCorr.B7R1L_ODGeometryType = ODGeometryType


#-------------------------------------------------------------------------------------------------#
# ALFA_Ntuple
#
# nothing can be changed here, everything is defined at the beggining in Initialize section
#-------------------------------------------------------------------------------------------------#
from ALFA_Ntuple.ALFA_NtupleConf import ALFA_Ntuple

ALFA_Ntuple.CoolData            = CoolData
ALFA_Ntuple.DataType            = DataType
ALFA_Ntuple.outputLocalRootFile = ALFANtupleOutFile
ALFA_Ntuple.listCoolFolders     = ListCoolFolders
ALFA_Ntuple.ListAlgoMD          = ListAlgoMD
ALFA_Ntuple.ListAlgoOD          = ListAlgoOD
ALFA_Ntuple.RunEpoch            = RunEpoch

#-------------------------------------------------------------------------------------------------#
# Event related parameters
#
# nothing can be changed here, everything is defined at the beggining in Initialize section

#-------------------------------------------------------------------------------------------------#
theApp.EvtMax = EvtMax


#-------------------------------------------------------------------------------------------------#
# a chain of ALFA algorithms
#
# do not change, only if you know what are you doing
#
# ALFA data chain is defined here, from RAW to n-tuple, via raw2digit and reconstructions
#-------------------------------------------------------------------------------------------------#
topSeq += ALFA_RawDataProvider("ALFA_RawDataProvider")
topSeq += ALFA_Raw2Digit("ALFA_Raw2Digit")
topSeq += ALFA_LocRec("ALFA_LocRec")
topSeq += ALFA_LocRecCorr("ALFA_LocRecCorr")
topSeq += ALFA_Ntuple("ALFA_Ntuple", RootStreamName = streamName)

#-------------------------------------------------------------------------------------------------#
# Stream/collection output - not needed if one wants ALFA_Ntuple output only (therefore commented)
#-------------------------------------------------------------------------------------------------#
#from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
#Stream1 = AthenaPoolOutputStream("Stream1")
#Stream1.OutputFile = "outPool.root"
#Stream1.ItemList+= ["ALFA_RawDataContainer#*"]
#Stream1.ItemList+= ["ALFA_HitCollection#*"]
#Stream1.ItemList+= ["ALFA_ODHitCollection#*"]
#Stream1.ItemList+= ["ALFA_DigitCollection#*"]
#Stream1.ItemList+= ["ALFA_ODDigitCollection#*"]
#Stream1.ItemList+= ["ALFA_LocRecEvCollection#*"]
#Stream1.ItemList+= ["ALFA_LocRecODEvCollection#*"]
#Stream1.ItemList+= ["ALFA_LocRecCorrEvCollection#*"]
#Stream1.ItemList+= ["ALFA_LocRecCorrODEvCollection#*"]


# ====================================================================
# Define output stream and create output files
# ====================================================================
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += [streamName + " DATAFILE='" + ALFANtupleOutFile + "' OPT='RECREATE'"]

#----------------------------------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 5
MessageSvc.defaultLimit = 9999999


#--- End *.py file  ---------------------------------------------------------------------
#conddb.dumpFolderTags('myfile.txt',True)
