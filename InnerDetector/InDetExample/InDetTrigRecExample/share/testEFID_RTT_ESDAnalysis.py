from AthenaCommon.AppMgr import ServiceMgr 
import AthenaPoolCnvSvc.ReadAthenaPool 
from PartPropSvc.PartPropSvcConf import PartPropSvc 

theApp.EvtMax = -1
theApp.SkipEvents = 0

ServiceMgr.EventSelector.InputCollections = ["data09_cosmag.00121513.ESD.pool.root"]

from AthenaCommon.AlgSequence import AlgSequence 
job=AlgSequence() 

from TrigInDetESDAlgs.TrigInDetESDAlgsConf import TrackAnalysis 
job += TrackAnalysis() 

TrackAnalysis.Phi_Tolerance = 0.01*rad
TrackAnalysis.Eta_Tolerance = 0.05

TrackAnalysis.numOfHitsCut = 5
TrackAnalysis.d0QualityCut = 500*mm
TrackAnalysis.z0QualityCut = 400*mm

########################################## 
# setup TTree registration Service 
# save ROOT histograms and Tuple 
from GaudiSvc.GaudiSvcConf import THistSvc 
ServiceMgr += THistSvc() 
ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='EFID_RTT_Histos.root' OPT='RECREATE'" ]

from AnalysisTools.AnalysisToolsConf import AANTupleStream 
job += AANTupleStream() 
AANTupleStream = AANTupleStream() 
AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ] 
AANTupleStream.OutputName = 'EFID_RTT_Histos.root' 
AANTupleStream.WriteInputDataHeader = True 
AANTupleStream.OutputLevel = WARNING 

job.TrackAnalysis.OutputLevel = DEBUG

