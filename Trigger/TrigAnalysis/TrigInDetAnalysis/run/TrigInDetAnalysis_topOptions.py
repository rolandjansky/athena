# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AppMgr import ServiceMgr
# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
#EventSelector.BackNavigation = True

ServiceMgr.EventSelector.InputCollections = [ "/project/gruppo1/atlas1/atlas/data/2009/data09_cos.00121330.physics_IDCosmic.recon.ESD.r733_tid073214/ESD.073214._000002.pool.root.2" ]

# The ESD/AOD input file
#include("esd-run90711.py")
#include("esd-run90792.py")
#include("esd-run90943.py")
#include("esd-run91335.py")
#include("esd-run91338.py")
#include("esd-run91391.py")


# Athena-Aware NTuple making Tools
CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")

# list of the algorithms to be executed at run time
from TrigInDetAnalysis.TrigInDetAnalysisConf import TrigInDetAnalysisSkeleton
topSequence.CBNT_AthenaAware += TrigInDetAnalysisSkeleton()
TrigInDetAnalysisSkeleton = TrigInDetAnalysisSkeleton()
TrigInDetAnalysisSkeleton.OutputLevel = DEBUG

##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='IDTrackingCBNT.aan.root' OPT='RECREATE'" ]
from AnalysisTools.AnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
AANTupleStream.OutputName = 'IDTrackingCBNT.aan.root'
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = WARNING

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = DEBUG

theApp.EvtMax = -1

from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
TriggerConfigGetter( "ReadPool" )

#to dump ESD/AOD content (do only if theApp.EvtMax = 1)
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = True 

