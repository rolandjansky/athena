# Author: Ketevi A. Assamagan
# BNL, June 12, 2004

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ToolSvc

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

# this line is needed for reading AOD files made with pre-14.1.X releases
# If you leave it uncommented, you can still read post-14.1.X releases

include ("JetRec/ParticleJetCompatibility.py")



# The AOD input file
#ServiceMgr.EventSelector.InputCollections = [ "dcache:AOD.pool.root.1" ]
ServiceMgr.EventSelector.InputCollections = [ "AOD.pool.root" ]
#ServiceMgr.EventSelector.InputCollections = [ "/tmp/jgoncalo/AOD.065738._00001.pool.root.1"]

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
from UserAnalysis.UserAnalysisConf import AnalysisSkeleton
topSequence.CBNT_AthenaAware += AnalysisSkeleton() 
AnalysisSkeleton = AnalysisSkeleton()

############# The properties of the AnalysisSkeleton Algorithm
AnalysisSkeleton.AnalysisSelectionTool       = ToolSvc.UserAnalysisSelectionTool
AnalysisSkeleton.AnalysisPreparationTool     = ToolSvc.UserAnalysisPreparationTool
AnalysisSkeleton.AnalysisOverlapCheckingTool = ToolSvc.UserAnalysisOverlapCheckingTool
AnalysisSkeleton.AnalysisOverlapRemovalTool  = ToolSvc.UserAnalysisOverlapRemovalTool

IsAtlfast = False

AnalysisSkeleton.McParticleContainer = "SpclMC"
AnalysisSkeleton.ElectronContainer = "ElectronAODCollection"
AnalysisSkeleton.MissingETObject = "MET_RefFinal"
AnalysisSkeleton.DeltaRMatchCut = 0.2
AnalysisSkeleton.MaxDeltaR = 0.9999
AnalysisSkeleton.ElectronEtCut  = 10.0*GeV
AnalysisSkeleton.ElectronEtaCut = 2.5
AnalysisSkeleton.ElectronCone   = 0.9
AnalysisSkeleton.bjetWt_IP3DSV1Cut = 6
AnalysisSkeleton.bjet_etaCut = 2.5
AnalysisSkeleton.bjet_etCut = 15.0*GeV
AnalysisSkeleton.MissingETCut = 20.0*GeV
AnalysisSkeleton.OutputLevel = INFO
AnalysisSkeleton.IsAtlFastData = IsAtlfast
AnalysisSkeleton.SusyJetMinEt      = 50*GeV

# Change the selections if necesary
# Please read jO files in UserAnalysisUtils/ for other options
#
ToolSvc.UserAnalysisSelectionTool.IsAtlfastData = IsAtlfast
#AnalysisSkeleton.AnalysisSelectionTool.ElectronIsEMFlag="Loose"
ToolSvc.UserAnalysisSelectionTool.MuonPt=6.0*GeV
ToolSvc.UserAnalysisSelectionTool.JetPt=20.0*GeV
ToolSvc.UserAnalysisSelectionTool.ElectronEtaWindowCut=False
ToolSvc.UserAnalysisSelectionTool.ElectronEtaWindowCenter=1.445
ToolSvc.UserAnalysisSelectionTool.ElectronEtaWindow=0.075
ToolSvc.UserAnalysisSelectionTool.OutputLevel = INFO
ToolSvc.UserAnalysisSelectionTool.egDetailContainerName="egDetailAOD"

# configure the overlap checking tool
ToolSvc.UserAnalysisOverlapCheckingTool.OverlapDeltaR=0.2
ToolSvc.UserAnalysisOverlapCheckingTool.OverlapDeltaRWithJets=0.3

# Building the  containers of selected obejcts
ToolSvc.UserAnalysisPreparationTool.IsAtlfastData = IsAtlfast
ToolSvc.UserAnalysisPreparationTool.OutputLevel = INFO

#input cntainer keys to the pre-selection tool
ToolSvc.UserAnalysisPreparationTool.InputContainerKeys = [
    "ElectronAODCollection",
    "StacoMuonCollection",
    "TauRecContainer",
    "AntiKt4TowerJets",
    "PhotonAODCollection",
    "CaloCalTopoCluster",
    "TrackParticleCandidate"
   ]
# Output container keys after the pre-selections
ToolSvc.UserAnalysisPreparationTool.OutputContainerKeys=[ 
    "SelectedElectronCollection",
    "SelectedStacoMuonCollection",
    "SelectedTauRecContainer",
    "SelectedAntiKt4TowerJets",
    "SelectedPhotonAODCollection",
    "SelectedCaloCalTopoCluster",
    "SelectedTrackParticleCandidate"
   ]

# Use the output containers fromi the selection tool as input to the overalp removal tool
# the order matters 
# in this setting, first electrons will be chosen,
# then if any muon overlaps an electron, the latter is rejected, and so on

ToolSvc.UserAnalysisOverlapRemovalTool.InputContainerKeys=[  
    "SelectedElectronCollection",
    "SelectedStacoMuonCollection",
    "SelectedTauRecContainer",
    "SelectedAntiKt4TowerJets",
    "SelectedPhotonAODCollection",
    "SelectedCaloCalTopoCluster",
    "SelectedTrackParticleCandidate"
   ]

# The output container keys after the overlap-removal
# Note that a container of all leptons is provided on output
# as well as a container of all final state particles
ToolSvc.UserAnalysisOverlapRemovalTool.IsAtlfastData          = IsAtlfast
ToolSvc.UserAnalysisOverlapRemovalTool.OuputObjectKey         = "FinalStateObjectCollection"
ToolSvc.UserAnalysisOverlapRemovalTool.OutputLeptonKey        = "FinalStateLeptonCollection"
ToolSvc.UserAnalysisOverlapRemovalTool.OutputPhotonKey        = "FinalStatePhotonCollection"
ToolSvc.UserAnalysisOverlapRemovalTool.OutputElectronKey      = "FinalStateElectronCollection"
ToolSvc.UserAnalysisOverlapRemovalTool.OutputMuonKey          = "FinalStateMuonCollection"
ToolSvc.UserAnalysisOverlapRemovalTool.OutputTauJetKey        = "FinalStateTauJetCollection"
ToolSvc.UserAnalysisOverlapRemovalTool.OutputCalloClusterKey  = "FinalStateCaloClusterCollection"
ToolSvc.UserAnalysisOverlapRemovalTool.OutputTrackParticleKey = "FinalStateTrackParticleCollection"
ToolSvc.UserAnalysisOverlapRemovalTool.OutputJetKey           = "FinalStateJetCollection"
ToolSvc.UserAnalysisOverlapRemovalTool.OutputBJetKey          = "FinalStateBJetCollection"
ToolSvc.UserAnalysisOverlapRemovalTool.OutputLightJetKey      = "FinalStateLightJetCollection"

##########################################
##
## Set up the trigger part
##
##########################################

AnalysisSkeleton.DoTrigger = True
if AnalysisSkeleton.DoTrigger:
   # needed for TriggerConfigGetter (will change with 15.3.0)
   from RecExConfig.RecFlags  import rec
   rec.readRDO=False
   rec.readAOD=True
   rec.doWriteAOD=False
   rec.doWriteESD=False

   # To read files with trigger config stored as in-file meta-data,
   from TriggerJobOpts.TriggerFlags import TriggerFlags
   TriggerFlags.configurationSourceList = ['ds']

   # set up trigger config service
   from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
   cfg =  TriggerConfigGetter()

   ## chains and groups for which to print trigger statistics
   photons = ["L1_2EM13", "L2_2g10_mu6", "EF_2g10"]
   singletaus = ["EF_tau12_loose", "EF_tau16_loose", "EF_tau16i_loose", "EF_tau20_loose", "EF_tau20i_loose",
                 "EF_tau29_loose", "EF_tau29i_loose", "EF_tau38_loose", "EF_tau50_loose", "EF_tau84_loose"]
   twotaus = ["EF_2tau20i_loose", "EF_2tau29i_loose", "EF_2tau29i_medium"]
   combinedtaus = ["EF_tau12_loose_e10_loose", "L2_tau16i_loose_2j23", "EF_tau16i_loose_2j23", "EF_tau16i_loose_EFxe40"]
   AnalysisSkeleton.StatTriggerChains = photons + singletaus + twotaus + combinedtaus;

   #AnalysisSkeleton.InvestigateChain = 'L2_tau16i_loose_2j23'
   

## Done with trigger setup

##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='AnalysisSkeleton.aan.root' OPT='RECREATE'" ]
from AnalysisTools.AnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
AANTupleStream.OutputName = 'AnalysisSkeleton.aan.root'
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = WARNING

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 9999999

# Number of Events to process
theApp.EvtMax = -1
#theApp.EvtMax = 5

###################### For interactive analysis
#include ("PyAnalysisCore/InitPyAnalysisCore.py")

from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

#### test MC dump ###

#from TruthExamples.TruthExamplesConf import DumpMC
#topSequence += DumpMC()
#DumpMC.McEventKey = "GEN_AOD"
       
