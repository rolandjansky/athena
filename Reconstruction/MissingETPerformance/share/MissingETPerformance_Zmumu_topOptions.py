# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool

# Particle Properties
from PartPropSvc.PartPropSvcConf import PartPropSvc

# the POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

########################################################################################################
ServiceMgr.EventSelector.InputCollections = [
"/scratch/ayurkewi/Zmumu_files/mc08.106051.PythiaZmumu_1Lepton.recon.AOD.e347_s462_r635_tid047165_sub06186680/AOD.047165._20527.pool.root.1"
                                            ]

include ( "MissingETPerformance/MissingETData_jobOptions.py" )
include ( "MissingETPerformance/MissingETMuonData_jobOptions.py" )
include ( "MissingETPerformance/MissingETEventSelector_jobOptions.py" )
include ( "MissingETPerformance/BasicPlots_jobOptions.py" )
include ( "MissingETPerformance/ResolutionTool_jobOptions.py" )
include ( "MissingETPerformance/Linearity_jobOptions.py" )
include ( "MissingETPerformance/ZMuMu_jobOptions.py" )

# Athena-Aware NTuple making Tools
CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")

# Set up trigger configuration service and metadata service it relies on, for analysis job without RecExCommon
# set up trigger decision tool
from TrigDecision.TrigDecisionConfig import TrigDecisionTool
tdt = TrigDecisionTool()
ToolSvc += tdt
# flags needed for TriggerConfigGetter
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

# list of the algorithms to be executed at run time
from MissingETPerformance.MissingETPerformanceConf import MissingETPerformance
topSequence.CBNT_AthenaAware += MissingETPerformance() 
MissingETPerformance = MissingETPerformance()

############# The properties of the MissingETPerformance Algorithm
MissingETPerformance.OutputLevel = WARNING

############## trigger
#whether to require a trigger from the list to have fired
MissingETPerformance.FilterOnTrigger = False
#List of trigger names to accept
MissingETPerformance.TriggerNames = ["EF_e15_medium","EF_e5_medium","EF_J50"]

#turn on/off individual tools here
MissingETPerformance.doEventSelector                    = True
MissingETPerformance.doBasicPlotsTool                   = True
MissingETPerformance.doResolutionTool                   = True
MissingETPerformance.doLinearityTool                    = True
MissingETPerformance.doZMuMuTool                        = True
MissingETPerformance.doEtaRingsTool                     = False
MissingETPerformance.doTrigMissingETTool                = False
MissingETPerformance.doTrigVsOfflineMissingETTool       = False
MissingETPerformance.doFakeMissingETTool                = False
MissingETPerformance.doMuonTool                         = False
MissingETPerformance.doContainerComparatorTool          = False

#configure event selection
MissingETEventSelector.ElecPtCut              = 0.*GeV
MissingETEventSelector.MuonPtCut              = 20.*GeV
MissingETEventSelector.minRefFinalMET         = 0.*GeV
MissingETEventSelector.minInvMass             = 71.*GeV
MissingETEventSelector.maxInvMass             = 111.*GeV
MissingETEventSelector.SelectionType          = "hasZboson_mumu"

##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='METPerformance.root' OPT='RECREATE'" ]
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.OutputName = 'METPerformance.root'
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = WARNING

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = WARNING

# Number of Events to process
theApp.EvtMax = 1000000

AthenaEventLoopMgr = Service ( "AthenaEventLoopMgr")
try:
    AthenaEventLoopMgr.EventPrintoutInterval = 100
except Exception:
    print 'EventPrintoutInterval exception, defaults to 1'
