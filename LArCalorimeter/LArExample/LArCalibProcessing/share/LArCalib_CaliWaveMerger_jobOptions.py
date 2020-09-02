###########################################################################
#
# Example jobOptions to merge several LArCaliWaveContainers into one
#
# Last update: 30/11/2007 <Marco.Delmastro@cern.ch>
#
# Note: Works with release 13.0.x, need  migration to 13.x.0
#
###########################################################################

if not 'PoolFileList' in dir():
	printfunc ("Please provide a PoolFileList!")
	theApp.exit()
	
if not 'ContainerKeyList' in dir():
	printfunc ("Please provide a ContainerKeyList!")
	theApp.exit()

if not 'PoolFileDir' in dir():
	PoolFileDir = "." 

if not 'KeyOutput' in dir():
	KeyOutput = "LArCaliWave"

if not 'BaseName' in dir():
	BaseName  = "LArCaliWaveMerger"

if not 'WritePoolFile' in dir():
	WritePoolFile = True

if not 'WriteRootFile' in dir():
	WriteRootFile = False

if not 'GroupingType' in dir():
	GroupingType = "ExtendedSubDetector"

if not 'OutDir' in dir():
	OutDir = "."
PoolFileName = OutDir+"/"+BaseName+".pool.root"
RootFileName = OutDir+"/"+BaseName+".root"

###########################################################################
#
# Global settings
#
###########################################################################

include("LArCalibProcessing/LArCalib_MinimalSetup.py")

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

ToolSvc = Service("ToolSvc")
ToolSvc.LArRoI_Map.Print = False

###############################################################################
#
# Read POOL file(s)
#
###############################################################################

include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

CondProxyProvider = Service( "CondProxyProvider" )
ProxyProviderSvc  = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
CondProxyProvider.InputCollections += PoolFileList

EventPersistencySvc = Service( "EventPersistencySvc" )
EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]

###############################################################################
#
# Merge LArCaliWaveContainers
#
###############################################################################

from LArCalibUtils.LArCalibUtilsConf import LArCaliWaveMerger
LArCaliWaveMerger = Algorithm("LArCaliWaveMerger")
LArCaliWaveMerger.KeyList      = ContainerKeyList
LArCaliWaveMerger.KeyOutput    = KeyOutput
LArCaliWaveMerger.GroupingType = GroupingType
LArCaliWaveMerger.OutputLevel  = DEBUG
topSequence += LArCaliWaveMerger

if ( WriteRootFile ) :
	
	from LArCalibTools.LArCalibToolsConf import LArWave2Ntuple as LArCaliWaves2Ntuple
	LArCaliWaves2Ntuple = LArCaliWaves2Ntuple("LArCaliWaves2Ntuple")
	LArCaliWaves2Ntuple.NtupleName = "CALIWAVE"
	LArCaliWaves2Ntuple.KeyList    = [ KeyOutput ]
	topSequence += LArCaliWaves2Ntuple
	
	theApp.HistogramPersistency = "ROOT"
	NTupleSvc = Service( "NTupleSvc" )
	NTupleSvc.Output = [ "FILE1 DATAFILE='"+RootFileName+"' OPT='NEW'" ]

if ( WritePoolFile ) :
        include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

        include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
        ToolSvc.ConditionsAlgStream.OutputFile = PoolFileName
        OutputConditionsAlg.ObjectList = [ "LArCaliWaveContainer#"+KeyOutput]

###########################################################################

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = INFO
MessageSvc.defaultLimit = 1000;

if not 'VerboseOutput' in dir():
	VerboseOutput = False
	
if ( VerboseOutput):
	StoreGateSvc = Service( "StoreGateSvc" )
	StoreGateSvc.Dump = True
	DetectorStore = Service( "DetectorStore" )
	DetectorStore.Dump = True
	ConditionStore = Service( "ConditionStore" )
	ConditionStore.Dump = True

###########################################################################

theApp.EvtMax = 1 # Leave this unchanged!

###########################################################################
