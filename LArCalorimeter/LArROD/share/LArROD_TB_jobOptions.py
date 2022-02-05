#*******************************************************************
#
# jobOption to analyse testbeam data
#
#*******************************************************************
theApp.setup( NO_EVSEL )

# Detector description stuff
include( "DetDescrCnvSvc/DetStore_joboptions.py" )

include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )

include( "LArIdCnv/LArIdCnv_joboptions.py" )

#special LAr identifier dictionary for testbeam
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter_H8_2004.xml"
DetDescrCnvSvc.DoIdChecks
DetDescrCnvSvc.DetectorManagers += [ "CaloMgr" ]
theApp.Dlls += [ "CaloDetMgrDetDescrCnv" ]
# Use auditors
theApp.Dlls += [ "GaudiAud" ]
#---- EMTB-Files input --------------------
theApp.Dlls += ["LArTBCnv"];                                                 
theApp.ExtSvc += [ "LArTBCnvSvc" , "LArTBEventSelector/EventSelector"]
EventPersistencySvc.CnvServices     += [ "LArTBCnvSvc" ]
theApp.EvtSel = "EventSelector"
EventSelector.InputFiles = ["/afs/cern.ch/user/l/laplace/public/emtb/data/RUN218266.dat"]
#---- Condition data --------------------
#load relevant IOV libraries
theApp.Dlls += [ "IOVDbSvc" ]
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "IOVDbSvc" ]
# Use CERN offline DB, readonly access
IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.serverName="atlasdev1.cern.ch"
IOVDbSvc.DBname = "LArIOVDB"; 
IOVDbSvc.userName="readerLArIOV"; 
IOVDbSvc.userPwd =""; 
theApp.Dlls += [ "LArCondCnv" ]
theApp.ExtSvc += [ "LArCondCnvSvc" ]
EventPersistencySvc.CnvServices += [ "LArCondCnvSvc" ]
# Preload IOV folders
IOVDbSvc.Folders += ["/lar/LArElecCalibTB02/LArPedestal"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB02/LArAutoCorr"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB02/LArOFC"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB02/LArRamp"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB02/LArDAC2uA"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB02/LAruA2MeV"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB02/LArPhysWaveContainer/LArPhysWave"]
IOVDbSvc.Folders += ["/lar/LArElecCalibTB02/LArShapeComplete/LArPhysWave"]
IOVDbSvc.GlobalTag = ""
# Option for LArShapeComplete
LArCondCnvSvc = Service( "LArCondCnvSvc" )
LArCondCnvSvc.LArShapeMode=1; 
# ----- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#MessageSvc.OutputLevel =2;
MessageSvc.OutputLevel =3
#-----------------------------------
# LArDigit->LArRawChannel
#-----------------------------------
theApp.Dlls += ["LArRawUtils", "LArROD", "LArCalibUtils"]
theApp.TopAlg += [ "LArTBRawChannelBuilder" ]
LArTBRawChannelBuilder = Algorithm( "LArTBRawChannelBuilder" )
LArTBRawChannelBuilder.DataLocation = "FREE" 
LArTBRawChannelBuilder.LArRawChannelContainerName = "LArRawChannels" 
# 
LArTBRawChannelBuilder.GainMode = 1 ; 
# The following should be the same as HighGainValue, MiddleGainValue, LowGainValue 

from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
LArADC2MeVCondAlgDefault()
LArRawChannelBuilder = Algorithm( "LArRawChannelBuilder" )
LArRawChannelBuilder.HighGain_DigitFactor = 100. 
LArRawChannelBuilder.MiddleGain_DigitFactor = 10. 
LArRawChannelBuilder.LowGain_DigitFactor = 1. 
LArTBRawChannelBuilder.WithOF = TRUE 
LArTBRawChannelBuilder.FirstSample = 1
LArTBRawChannelBuilder.NSampleEff = 5
LArTBRawChannelBuilder.IsEMTB = TRUE
LArTBRawChannelBuilder.WorkMode = 1 
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='ntuple.root' OPT='NEW'" ]
#for root file, it is easier to start ntuple ID with a letter
# to be removed for hbook ntuple
#-----------------------------------
# LArRawChannel ->LArCell
#-----------------------------------
theApp.Dlls += ["LArRecUtils", "LArCellRec", "CaloRec"]
theApp.TopAlg += [ "LArCellMaker/CellMakerEM" ]
# CellMakerEM properties:
CellMakerEM = Algorithm( "CellMakerEM" )
CellMakerEM.LArRegion="LAr_EM"
CellMakerEM.CellContainer="LArEM"
CellMakerEM.CellBuilderID="LArCellBuilderFromLArRawChannel/cellbuilderEM"
# CellBuilder properties
cellbuilderEM = Algorithm( "cellbuilderEM" )
cellbuilderEM.RawChannelContainer="LArRawChannels"
cellbuilderEM.e_threshold =10000.0 
# CBNT stuff 
theApp.Dlls += [ "CBNT_Utils","CBNT_Athena" ]
theApp.TopAlg += [ "CBNT_Athena" ]
CBNT_Athena = Algorithm( "CBNT_Athena" )
CBNT_Athena.NtupleLocID="/FILE1/CBNT/t3333"
CBNT_Athena.Members = [ "CBNT_EventInfo" ]
CBNT_Athena.Members += [ "CBNT_CaloCell/CBNT_LArCellEM"]; 
CBNT_LArCellEM = Algorithm( "CBNT_LArCellEM" )
CBNT_LArCellEM.MaxNCells = 40000 
CBNT_LArCellEM.CellColl="LArEM"
CBNT_LArCellEM.NtupleLocID="/FILE1/CALO/168"; 
CBNT_LArCellEM.ThresholdGeVCells=10000.0
