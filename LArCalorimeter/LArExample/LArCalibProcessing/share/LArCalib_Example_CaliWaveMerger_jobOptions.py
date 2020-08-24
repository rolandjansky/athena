###########################################################################
#
# Example jobOptions to merge several LArCaliWaveContainers into one
#
# Last update: 28/4/2006 (Marco.Delmastro@cern.ch)
#
###########################################################################

PoolFileList = [ "LArDelay_3144.pool.root" ,
                 "LArDelay_3462.pool.root" ]

ContainerKeyList = [ "3144" , "3462" ]

Outdir         = "./"

WriteNtuple    = True
RootFileName   = Outdir+"LArCaliWaveMerger.root"

WritePOOLFile  = True
POOLFileName   = Outdir+"LArCaliWaveMerger.pool.root"
KeyOut         = "LArCaliWave"

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
# Read POOL file(s)
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
# Merge LArCaliWaveContainers
#
theApp.Dlls += ["LArCalibUtils"]
theApp.TopAlg += ["LArCaliWaveMerger"]
LArCaliWaveMerger=Algorithm("LArCaliWaveMerger")
LArCaliWaveMerger.KeyList   = ContainerKeyList
LArCaliWaveMerger.KeyOutput = KeyOut
LArCaliWaveMerger.OutputLevel = DEBUG

if ( WriteNtuple ) :
	theApp.Dlls += ["LArCalibTools"]
	theApp.TopAlg += [ "LArWaves2Ntuple/LArCaliWaves2Ntuple"]
	LArCaliWaves2Ntuple = Algorithm( "LArCaliWaves2Ntuple" )
	LArCaliWaves2Ntuple.NtupleName  = "CALIWAVE"
	LArCaliWaves2Ntuple.KeyList     = [ KeyOut ]
	
	theApp.Dlls += [ "RootHistCnv" ]
	theApp.HistogramPersistency = "ROOT"
	NTupleSvc = Service( "NTupleSvc" )
	NTupleSvc.Output = [ "FILE1 DATAFILE='"+RootFileName+"' OPT='NEW'" ]

if ( WritePOOLFile ) :
        include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
        theApp.Dlls   += [ "LArAthenaPoolPoolCnv" ]
        theApp.Dlls   += [ "LArCondAthenaPoolPoolCnv" ]

        include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
        ToolSvc.ConditionsAlgStream.OutputFile = POOLFileName
        
        OutputConditionsAlg.ObjectList = [ "LArCaliWaveContainer#"+KeyOut]

###########################################################################

#AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
#AthenaEventLoopMgr.OutputLevel = WARNING

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = INFO
MessageSvc.defaultLimit = 1000;

#StoreGateSvc = Service("StoreGateSvc");
#StoreGateSvc.dump = False

###########################################################################

theApp.EvtMax = 1 # Leave unchanged!

###########################################################################
