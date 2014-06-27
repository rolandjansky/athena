# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class TruthParticleFilters(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = []

class JetCollections(JobProperty):
    statusOn = True
    allowedTypes = ['string','list']
    StoredValue = ['AntiKt4TopoEMJets','AntiKt6TopoEMJets','AntiKt4LCTopoJets','AntiKt6LCTopoJets']

class alltruthjets(JobProperty):
    statusOn = True
    allowedTypes = ['string','list']
    StoredValue = []

class FillR0p6Jets(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class DumpNearbyTracks(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class DumpAllTracks(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class ElectronDumpingLevel(JobProperty):
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 10

class DumpElectronToPhoton(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class DumpLotsOfTriggerInfo(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class DumpTauInformation(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class DumpTriggerObjectsInfo(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class DumpDetailedIsolationVariables(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class FillFullJVF(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class ReCalculateJetMoments(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class FillFullMET(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class RemoveTopoClusterCandidates(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class Fill_Etcone_ED_corrected(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [40]

class Fill_Etcone_corrected(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [40]

class Fill_TopoEtcone_corrected(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [20,30,40]

class Fill_ED_info(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class ConfigureCaloDepthTool(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class CaloDepth(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    StoredValue = 'showerdefault'

class EnableBTagging(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class BTaggingSuffix(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    StoredValue = ""

class RetagTruthBJets(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class AdditionalMETOutputLevel(JobProperty):
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 2

class FilterTracksForMET(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class SaveAllTruthParticles(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class CellDrCut(JobProperty):
    statusOn = True
    allowedTypes = ['float']
    StoredValue = 0.4

class SelectedCellsContainerName(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    StoredValue = "SelectedCells"

class JetTagD3PD_CollectionPostfix(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    StoredValue = ""

class FillMV3(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class FillJetFitterCharm(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class PhotonD3PDMakerFlags(JobPropertyContainer):
    pass

jobproperties.add_Container(PhotonD3PDMakerFlags)
list_jobproperties = [
    TruthParticleFilters,
    JetCollections,
    DumpNearbyTracks,
    DumpAllTracks,
    ElectronDumpingLevel,
    DumpElectronToPhoton,
    DumpLotsOfTriggerInfo,
    DumpTriggerObjectsInfo,
    DumpDetailedIsolationVariables,
    FillFullJVF,
    ReCalculateJetMoments,
    FillFullMET,
    DumpTauInformation,
    RemoveTopoClusterCandidates,
    Fill_Etcone_ED_corrected,
    Fill_Etcone_corrected,
    Fill_TopoEtcone_corrected,
    Fill_ED_info,
    ConfigureCaloDepthTool,
    CaloDepth,
    EnableBTagging,
    BTaggingSuffix,
    RetagTruthBJets,
    AdditionalMETOutputLevel,
    FilterTracksForMET,
    SaveAllTruthParticles,
    FillR0p6Jets,
    CellDrCut,
    SelectedCellsContainerName,
    JetTagD3PD_CollectionPostfix,
    alltruthjets,
    FillMV3,
    FillJetFitterCharm
    ]

for i in list_jobproperties:
    jobproperties.PhotonD3PDMakerFlags.add_JobProperty(i)

del list_jobproperties

PhotonD3PDMakerFlags=jobproperties.PhotonD3PDMakerFlags
