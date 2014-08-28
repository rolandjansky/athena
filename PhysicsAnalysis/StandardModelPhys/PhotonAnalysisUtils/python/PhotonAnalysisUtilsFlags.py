# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class FileName(JobProperty):
    statusOn = False
    allowedTypes = ['str']
    StoredValue = "PAUReco.root"

class ProductionRelease(JobProperty):
    statusOn = True
    allowedTypes = ['str','float']
    StoredValue = 15.5

class UseTrigger(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class UseRealData(JobProperty):
    statusOn = False
    allowedTypes = ['bool']
    StoredValue = True

class UseGRLSelection(JobProperty):
    statusOn = True
    allowedType = ['bool']
    StoredValue = False

class GoodRunsListVec(JobProperty):
    statusOn = False
    allowedType = ['list']
    StoredValue = []

class DumpElectronTrigger(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class DumpMETTrigger(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class ElectronToPhoton(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class DumpNtuple(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class DumpHistograms(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class DumpTimeDifference(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class DumpCollectionTree(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    
class DumpTracks(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class FilteringTracks(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class DumpAdditionalEtCones(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class DumpAdditionalPartonEtCones(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class DumpCells(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class DumpFilteredCells(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class DumpClusters(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class DumpUEDClusters(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
                
class DoReRunEgamma(JobProperty):
    statusOn = False
    allowedTypes = ['bool']
    StoredValue = False

class DoPhotonPostProcessingReRunPID(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class RunEgammaFilter(JobProperty):
    statusOn = False
    allowedTypes = ['bool']
    StoredValue = False

class UsePAUconversionFlagTool(JobProperty):
    statusOn = False
    allowedTypes = ['bool']
    StoredValue = False

# If false, a higher-level trigger may be passed even if some of the
# lower-level triggers in the chain failed 
class EnforceTriggerLogicalFlow(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class JetCollectionName(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "AntiKt4TopoEMJets"

class PhotonContainer(JobProperty):
    statusOn = False
    allowedTypes = ['str']
    StoredValue = "PhotonAODCollection"

class PhotonDetailContainerMerged(JobProperty):
    statusOn = False
    allowedTypes = ['str']
    StoredValue = "egDetailAOD"

class PhotonsDetailsContainerName(JobProperty):
    statusOn = False
    allowedTypes = ['str']
    StoredValue = "egDetailAOD"

class PhotonContainerMerged(JobProperty):
    statusOn = False
    allowedTypes = ['str']
    StoredValue = "PhotonAODCollection"

class PhotonVxCandContainerMerged(JobProperty):
    statusOn = False
    allowedTypes = ['str']
    StoredValue = "ConversionCandidate"

class PhotonClusterContainerMerged(JobProperty):
    statusOn = False
    allowedTypes = ['str']
    StoredValue = "egClusterCollection"

class DoReRunJetFinding(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class JetAreaEtaBins(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [0.00, 1.50, 3.0, 4.0, 5.0]

class JetAreaJetsToRemove(JobProperty):
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 0

class AlgSequence(JobProperty):
    statusOn = False
    allowedTypes = ['str']
    StoredValue = ""
    
class UsePAUGetter(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class DumpStoreGate(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class DumpPAURecoTree(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class DumpHggUserDataTree(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class PrintJobOptions(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class DoPtMiss(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class UseOldPtCorrections(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class UseVoronoiArea(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class FindLowPtTruthJets(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class JetAreaMomentName(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "VoronoiArea"

class PhotonAnalysisUtilsFlags(JobPropertyContainer):
    pass

jobproperties.add_Container(PhotonAnalysisUtilsFlags)
list_jobproperties = [
    FileName,
    ProductionRelease,
    UseTrigger,
    UseRealData,
    UseGRLSelection,
    GoodRunsListVec,
    DumpElectronTrigger,
    DumpMETTrigger,
    ElectronToPhoton,
    DumpNtuple,
    DumpHistograms,
    DumpTimeDifference,
    DumpCollectionTree,
    DumpTracks,
    FilteringTracks,
    DumpAdditionalEtCones,
    DumpAdditionalPartonEtCones,
    DumpCells,
    DumpFilteredCells,
    DumpClusters,
    DumpUEDClusters,
    DoReRunEgamma,
    DoPhotonPostProcessingReRunPID,
    RunEgammaFilter,
    DoReRunJetFinding,
    JetAreaEtaBins,
    JetAreaJetsToRemove,
    JetCollectionName,
    JetAreaMomentName,
    UsePAUconversionFlagTool,
    PhotonContainer,
    PhotonDetailContainerMerged,
    PhotonsDetailsContainerName,
    PhotonVxCandContainerMerged,
    PhotonContainerMerged,
    PhotonClusterContainerMerged,
    EnforceTriggerLogicalFlow,
    AlgSequence,
    UsePAUGetter,
    DumpStoreGate,
    DumpPAURecoTree,
    DumpHggUserDataTree,
    PrintJobOptions,
    DoPtMiss,
    UseOldPtCorrections,
    UseVoronoiArea,
    FindLowPtTruthJets
    ]

for i in list_jobproperties:
    jobproperties.PhotonAnalysisUtilsFlags.add_JobProperty(i)

del list_jobproperties

PAUflags=jobproperties.PhotonAnalysisUtilsFlags
