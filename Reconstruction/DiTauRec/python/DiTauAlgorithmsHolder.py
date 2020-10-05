# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

cached_instances = {}

sPrefix = 'DiTauRec_'


def getTestTool():
    _name = sPrefix + 'TestTool'

    if _name in cached_instances:
        return cached_instances[_name]

    from DiTauRec.DiTauRecConf import TestTool
    TestTool = TestTool(name=_name)

    cached_instances[_name] = TestTool
    return TestTool


def getSeedJetBuilder(jet_collection):
    _name = sPrefix + 'SeedJetBuilder'

    if _name in cached_instances:
        return cached_instances[_name]

    from DiTauRec.DiTauRecConf import SeedJetBuilder
    SeedJetBuilder = SeedJetBuilder(name=_name,
                                    JetCollection=jet_collection)

    cached_instances[_name] = SeedJetBuilder
    return SeedJetBuilder


def getSubjetBuilder():
    _name = sPrefix + 'SubjetBuilder'

    if _name in cached_instances:
        return cached_instances[_name]

    from DiTauRec.DiTauRecConf import SubjetBuilder
    SubjetBuilder = SubjetBuilder(name=_name,
                                  Rsubjet=0.2,
                                  Nsubjet=4,
                                  ptminsubjet=10000)

    cached_instances[_name] = SubjetBuilder
    return SubjetBuilder


# def getMuonSelectorTool():
#     _name = sPrefix + 'MuonSelectionTool'

#     if _name in cached_instances:
#         return cached_instances[_name]

#     from MuonSelectionTools.MuonSelectionToolsConf import CP__MuonSelectionTool
#     MuonSelectionTool = CP__MuonSelectionTool("MuonSelectionTool_forDiTaus",
#                                              MaxEta=2.7,   # default: 2.7
#                                              MuQuality=2)  # Tight,Medium,Loose,VeryLoose: 0,1,2,3

#     from AthenaCommon.AppMgr import ToolSvc
#     ToolSvc += MuonSelectionTool
#     # ToolSvc.MuonSelectionTool_forDiTaus.MaxEta = 2.7   # default: 2.7
#     # ToolSvc.MuonSelectionTool_forDiTaus.MuQuality = 2  # Tight,Medium,Loose,VeryLoose: 0,1,2,3

#     cached_instances[_name] = MuonSelectionTool
#     return MuonSelectionTool


def getElMuFinder():
    _name = sPrefix + 'ElMuFinder'

    if _name in cached_instances:
        return cached_instances[_name]

    from DiTauRec.DiTauRecConf import ElMuFinder
    ElMuFinder = ElMuFinder(name=_name,
                            ElectronContainer="Electrons",
                            MuonContainer="Muons",
                            ElectronMinPt=7000,
                            ElectronMaxEta=2.47,
                            MuonMinPt=7000,
                            MuonMaxEta=2.7, 
                            MuonQuality=3)   # Tight,Medium,Loose,VeryLoose: 0,1,2,3

    cached_instances[_name] = ElMuFinder
    return ElMuFinder


def getTVATool():
    _name = sPrefix + 'TVATool'

    if _name in cached_instances:
        return cached_instances[_name]

    from JetRecTools.JetRecToolsConf import TrackVertexAssociationTool
    TVATool = TrackVertexAssociationTool("TVATool_forDiTaus",
                                         TrackParticleContainer="InDetTrackParticles",
                                         TrackVertexAssociation="JetTrackVtxAssoc_forDiTaus",
                                         VertexContainer="PrimaryVertices",
                                         MaxTransverseDistance=2.5,  # in mm
                                         MaxLongitudinalDistance=2  # in mm
                                         )
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += TVATool

    from JetRec.JetRecConf import JetAlgorithm
    jetTrackAlg = JetAlgorithm(name=_name)
    jetTrackAlg.Tools = [TVATool]

    cached_instances[_name] = jetTrackAlg
    return jetTrackAlg


# requires getTVATool
def getVertexFinder():
    _name = sPrefix + 'VertexFinder'

    if _name in cached_instances:
        return cached_instances[_name]

    from DiTauRec.DiTauRecConf import VertexFinder
    VertexFinder = VertexFinder(name=_name,
                                PrimVtxContainerName="PrimaryVertices",
                                AssociatedTracks="GhostTrack",
                                TrackVertexAssociation="JetTrackVtxAssoc_forDiTaus")

    cached_instances[_name] = VertexFinder
    return VertexFinder


def getDiTauTrackFinder():
    _name = sPrefix + 'DiTauTrackFinder'

    if _name in cached_instances:
        return cached_instances[_name]

    from DiTauRec.DiTauRecConf import DiTauTrackFinder
    import tauRec.TauAlgorithmsHolder as TauAlgs
    DiTauTrackFinder = DiTauTrackFinder(name=_name,
                              MaxDrJet=1.0,
                              MaxDrSubjet=0.2,
                              TrackParticleContainer="InDetTrackParticles",
                              TrackSelectorTool=TauAlgs.getInDetTrackSelectorTool())
                              # ParticleCaloExtensionTool=TauAlgs.getParticleCaloExtensionTool())

    cached_instances[_name] = DiTauTrackFinder
    return DiTauTrackFinder


def getCellFinder(writeJetCells, writeSubjetCells):
    _name = sPrefix + 'CellFinder'

    if _name in cached_instances:
        return cached_instances[_name]

    # if JetCells are written, SubjetCells are written too
    writeSubjetCells = True if writeJetCells else writeSubjetCells

    from DiTauRec.DiTauRecConf import CellFinder
    CellFinder = CellFinder(name=_name,
                            writeJetCells=writeJetCells,
                            writeSubjetCells=writeJetCells,
                            ClusterContainer="CaloCalTopoClusters",
                            CellContainer="AllCalo",
                            Rsubjet=0.2)

    cached_instances[_name] = CellFinder
    return CellFinder


def getIDVarCalculator(useCells):
    _name = sPrefix + 'IDVarCalculator'

    if _name in cached_instances:
        return cached_instances[_name]

    from DiTauRec.DiTauRecConf import IDVarCalculator
    IDVarCalculator = IDVarCalculator(name=_name,
                                      useCells=useCells)

    cached_instances[_name] = IDVarCalculator
    return IDVarCalculator
