###########################################################################
#
#  Read calibration waves from POOL file, evaluate master waveforms, save 
#  them to database/ntuple/POOL file
#
###########################################################################

Gain = "HG"        # HG, MG, LG
Tag  = "XtalkCorr" # Standard, XtalkCorr

WriteRootFile = True
WritePoolFile = True

#Datadir       = "./"
Datadir       = "/shift/mdelmast/ctb2004/delay/postprod/"

PoolFileList  = [ Datadir+"LArCaliWaveMerger-"+Gain+"-"+Tag+".pool.root" ]
KeyInput      = "LArCaliWave-"+Gain
RootFileName  = Datadir+"LArMasterWaves-"+Gain+"-"+Tag+".root"
PoolFileName  = Datadir+"LArMasterWaves-"+Gain+"-"+Tag+".pool.root"
KeyOutput     = "LArMasterWave-"+Gain

###########################################################################

#LArCondCnvDbServer = "atlobk02.cern.ch"
LArCondCnvDbServer = "atlmysql01.cern.ch"

###########################################################################

#
# Detector description (LAr only, H8 CTB 2004)
#
include( "LArDetDescr/LArDetDescr_H8_joboptions.py" )
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DoIdChecks
DetDescrCnvSvc.LArIDFileName = "IdDictParser/IdDictLArCalorimeter_H8_2004.xml"

ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRoI_Map.Print = False

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.em_setOn() 

#
# Read POOL file
#
include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
theApp.Dlls += ["EventSelectorAthenaPool"] 
theApp.Dlls += ["LArAthenaPoolPoolCnv"]
theApp.Dlls += ["LArCondAthenaPoolPoolCnv"]

CondProxyProvider = Service( "CondProxyProvider" )
ProxyProviderSvc  = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
CondProxyProvider.InputCollections = PoolFileList

EventPersistencySvc = Service( "EventPersistencySvc" )
EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]

#
# build master waveforms from single-DAC calibration waves
#
theApp.Dlls += ["LArRawUtils"]
theApp.Dlls += ["LArTools"]
theApp.Dlls += ["LArCalibUtils"]

theApp.TopAlg += [ "LArMasterWaveBuilder" ]
LArMasterWaveBuilder = Algorithm("LArMasterWaveBuilder")

LArMasterWaveBuilder.KeyInput  = KeyInput
LArMasterWaveBuilder.KeyOutput = KeyOutput

LArMasterWaveBuilder.Normalize = False
LArMasterWaveBuilder.TimeShift = False

LArMasterWaveBuilder.DiffWaveNormPeak        = False # normally don't use this! for Sandrine's selection
LArMasterWaveBuilder.ListAllAnalysedChannels = False # complete output of all channels

LArMasterWaveBuilder.BuildMWwithOneDAC  = True  # useful for Low Gain

LArMasterWaveBuilder.LinearityTolerance = 0.035

LArMasterWaveBuilder.DACMinPS     = [  2000 , 20000 ,    100 ]
LArMasterWaveBuilder.DACMinStrips = [   100 ,  1000 ,    100 ]
LArMasterWaveBuilder.DACMinMiddle = [   100 ,  1000 ,    100 ]
LArMasterWaveBuilder.DACMinBack   = [   100 ,  1000 ,    100 ]
LArMasterWaveBuilder.DACMaxPS     = [ 15000 , 50000 ,  65000 ]
LArMasterWaveBuilder.DACMaxStrips = [   800 ,  8000 ,  65000 ]
LArMasterWaveBuilder.DACMaxMiddle = [   800 ,  8000 ,  65000 ]
LArMasterWaveBuilder.DACMaxBack   = [   800 ,  8000 ,  65000 ]

LArMasterWaveBuilder.WriteDB      = False
LArMasterWaveBuilder.OutputLevel  = INFO

#
# write master waveforms to ntuple/POOL
# 
if ( WriteRootFile ) :
	theApp.Dlls += ["LArCalibTools"]
	theApp.TopAlg += [ "LArWaves2Ntuple/LArCaliWaves2Ntuple"]
	LArCaliWaves2Ntuple = Algorithm( "LArCaliWaves2Ntuple" )
	LArCaliWaves2Ntuple.NtupleName   = "CALIWAVE"
	LArCaliWaves2Ntuple.DACSaturSkip = False
	#LArCaliWaves2Ntuple.KeyList     += [ KeyInput  ]
	LArCaliWaves2Ntuple.KeyList     += [ KeyOutput ]
	LArCaliWaves2Ntuple.OutputLevel  = INFO
	
	theApp.Dlls += [ "RootHistCnv" ]
	theApp.HistogramPersistency = "ROOT"
	NTupleSvc = Service( "NTupleSvc" )
	NTupleSvc.Output = [ "FILE1 DATAFILE='"+RootFileName+"' OPT='NEW'" ]
	
if ( WritePoolFile ) :
        include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
        theApp.Dlls   += [ "LArAthenaPoolPoolCnv" ]
        theApp.Dlls   += [ "LArCondAthenaPoolPoolCnv" ]

        include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
        ToolSvc.ConditionsAlgStream.OutputFile = PoolFileName
        
        OutputConditionsAlg.ObjectList = [ "LArCaliWaveContainer#"+KeyOutput]

###########################################################################

AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel = INFO

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = WARNING

###########################################################################

theApp.EvtMax = 1

###########################################################################
