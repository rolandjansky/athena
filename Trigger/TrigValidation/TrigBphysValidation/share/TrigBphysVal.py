# Author: Ketevi A. Assamagan
# BNL, June 12, 2004

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr
# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
#EventSelector.BackNavigation = True


# Particle Properties
from PartPropSvc.PartPropSvcConf import PartPropSvc

# the POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

# The AOD input file
#ServiceMgr.EventSelector.InputCollections = [ "dcache:AOD.pool.root.1" ]
ServiceMgr.EventSelector.InputCollections = ["AOD.pool.root"]

# Get the selection, overlap checking and overlap removal tools  
include ( "UserAnalysisUtils/UserAnalysisSelectionTool_jobOptions.py" )
include ( "UserAnalysisUtils/UserAnalysisPreparationTool_jobOptions.py" )
include ( "UserAnalysisUtils/UserAnalysisOverlapCheckingTool_jobOptions.py" )
include ( "UserAnalysisUtils/UserAnalysisOverlapRemovalTool_jobOptions.py" )

# Athena-Aware NTuple making Tools
CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")

# list of the algorithms to be executed at run time
from TrigBphysValidation.TrigBphysValidationConf import DsVal
topSequence.CBNT_AthenaAware += DsVal() 
DsVal = DsVal()
DsVal.OutputLevel = INFO
# track algo 1=SiTrack, 2=IdScan
DsVal.TrackAlgo=2  
# BdDsPi
#DsVal.GenBPid=511  
#DsVal.GenDsPid=431
# BsDsA1
DsVal.GenBPid=531  
DsVal.GenDsPid=-431

#### from Monika
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()

## set up trigger decision tool
from TrigDecision.TrigDecisionConfig import TrigDecisionTool
tdt = TrigDecisionTool()
ToolSvc += tdt

# might be needed for TriggerConfigGetter...
from RecExConfig.RecFlags  import rec
rec.readRDO=False
rec.readAOD=True
rec.doWriteAOD=False
rec.doWriteESD=False

# To read files with trigger config stored as in-file meta-data,
# i.e. 13.0.40 and above: ds
# To read AOD produced with 13.0.30 you need to change ds to aod: 
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList = ['ds']

# set up trigger config service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter()



##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='DsVal.aan.root' OPT='RECREATE'" ]
from AnalysisTools.AnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
AANTupleStream.OutputName = 'DsVal.aan.root'
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = WARNING

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = ERROR

# Number of Events to process
theApp.EvtMax = -1
#theApp.EvtMax = 10

###################### For interactive analysis
#include ("PyAnalysisCore/InitPyAnalysisCore.py")

from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

#### test MC dump ###

#theApp.Dlls  += ["TruthExamples" ]
#theApp.TopAlg = ["PrintMC"]
##-- PrintMC
#PrintMC = Algorithm( "PrintMC" )
##event SVC key
#PrintMC.McEventKey = "GEN_AOD"
## do you want output at all? TRUE/FALSE
#PrintMC.VerboseOutput = TRUE
## Event print style Vertex(traditional)/Barcode(barcode ordered)
#PrintMC.PrintStyle = "Barcode"
