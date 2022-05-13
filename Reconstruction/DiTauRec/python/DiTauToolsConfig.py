# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SeedJetBuilderCfg(ConfigFlags, **kwargs):
    """Configure the seed jet builder"""

    _name = f"{kwargs['prefix']}SeedJetBuilder"
    acc = ComponentAccumulator()
        
    SeedJetBuilder = CompFactory.SeedJetBuilder(name=_name, JetCollection=kwargs["JetCollection"])
    acc.setPrivateTools(SeedJetBuilder)

    return(acc)


def SubjetBuilderCfg(ConfigFlags, **kwargs):
    """Configure the subjet builder"""

    _name = f"{kwargs['prefix']}SubjetBuilder"
    acc = ComponentAccumulator()

    SubjetBuilder = CompFactory.SubjetBuilder(name=_name,
                                              Rsubjet=0.2,
                                              Nsubjet=4,
                                              ptminsubjet=10000)
    acc.setPrivateTools(SubjetBuilder)
    return(acc)


def ElMuFinderCfg(ConfigFlags, **kwargs):
    """Configure the ElMuFiner"""
    _name = f"{kwargs['prefix']}ElMuFinder"

    acc = ComponentAccumulator()

    ElMuFinder = CompFactory.ElMuFinder(name=_name,
                                        ElectronContainer="Electrons",
                                        MuonContainer="Muons",
                                        ElectronMinPt=7000,
                                        ElectronMaxEta=2.47,
                                        MuonMinPt=7000,
                                        MuonMaxEta=2.7, 
                                        MuonQuality=3)   # Tight,Medium,Loose,VeryLoose: 0,1,2,3

    acc.setPrivateTools(ElMuFinder)
    return(acc)

def TVAToolCfg(ConfigFlags, **kwargs):
    """Configure the TVA tool"""

    _name = f"{kwargs['prefix']}TVATool"
    acc = ComponentAccumulator()

    TVATool = CompFactory.TrackVertexAssociationTool("TVATool_forDiTaus",
                                                     TrackParticleContainer="InDetTrackParticles",
                                                     TrackVertexAssociation="JetTrackVtxAssoc_forDiTaus",
                                                     VertexContainer="PrimaryVertices",
                                                     MaxTransverseDistance=2.5,  # in mm
                                                     MaxLongitudinalDistance=2)  # in mm
    # This presumably needs to be public since otherwise the VertexFinderAlg won't be able to use it...
    acc.addPublicTool(TVATool)
    jetTrackAlg = CompFactory.JetAlgorithm(name=_name, Tools = [TVATool])
    acc.addEventAlgo(jetTrackAlg)
    return(acc)

# requires getTVATool - see above
def VertexFinderCfg(ConfigFlags, **kwargs):
    """Configure the vertex finder"""

    _name = f"{kwargs['prefix']}VertexFinder"
    acc = ComponentAccumulator()

    VertexFinder = CompFactory.VertexFinder(name=_name,
                                            PrimVtxContainerName="PrimaryVertices",
                                            AssociatedTracks="GhostTrack",
                                            TrackVertexAssociation="JetTrackVtxAssoc_forDiTaus")
    
    acc.setPrivateTools(VertexFinder)
    return(acc)


def DiTauTrackFinderCfg(ConfigFlags, **kwargs):
    """Configure the di-tau track finder"""    
 
    _name = f"{kwargs['prefix']}DiTauTrackFinder"
    acc = ComponentAccumulator()
    
    from tauRec.TauToolHolder import InDetTrackSelectorToolCfg 
    InDetTrackSelectorTool = acc.popToolsAndMerge(InDetTrackSelectorToolCfg(ConfigFlags))
    acc.addPublicTool(InDetTrackSelectorTool)

    DiTauTrackFinder = CompFactory.DiTauTrackFinder(name=_name,
                                                    MaxDrJet=1.0,
                                                    MaxDrSubjet=0.2,
                                                    TrackParticleContainer="InDetTrackParticles",
                                                    TrackSelectorTool=InDetTrackSelectorTool)

    acc.setPrivateTools(DiTauTrackFinder)
    return(acc)

# TODO: Doesn't seem to to be in use. Commenting for now
#def getCellFinder(writeJetCells, writeSubjetCells):
#    _name = sPrefix + 'CellFinder'
#
#    if _name in cached_instances:
#        return cached_instances[_name]
#
#    # if JetCells are written, SubjetCells are written too
#    writeSubjetCells = True if writeJetCells else writeSubjetCells
#
#    from DiTauRec.DiTauRecConf import CellFinder
#    CellFinder = CellFinder(name=_name,
#                            writeJetCells=writeJetCells,
#                            writeSubjetCells=writeJetCells,
#                            ClusterContainer="CaloCalTopoClusters",
#                            CellContainer="AllCalo",
#                            Rsubjet=0.2)
#
#    cached_instances[_name] = CellFinder
#    return CellFinder


def IDVarCalculatorCfg(ConfigFlags, **kwargs):
    """Configure the IDVarCalculator"""

    _name = f"{kwargs['prefix']}IDVarCalculator"
    acc = ComponentAccumulator()

    IDVarCalculator = CompFactory.IDVarCalculator(name=_name,
                                                  useCells=kwargs['useCells'])
    acc.setPrivateTools(IDVarCalculator)
    return(acc)
