# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================================
# Provides configs for the tools used for building/thinning tracking related
# object containers and decorations in the DAODs
#==============================================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Track collection merger
def TrackParticleMergerCfg(ConfigFlags, name, **kwargs):
    """Configure the track particle merger tool"""
    acc = ComponentAccumulator()
    TrackParticleMerger = CompFactory.DerivationFramework.TrackParticleMerger
    acc.addPublicTool(TrackParticleMerger(name, **kwargs),
                      primary = True)
    return acc

# Used in vertex fit track decorator
def UsedInVertexFitTrackDecoratorCfg(ConfigFlags, name, **kwargs):
    """Configure the UsedInVertexFitTrackDecorator"""
    acc = ComponentAccumulator()
    UsedInVertexFitTrackDecorator = CompFactory.DerivationFramework.UsedInVertexFitTrackDecorator
    acc.addPublicTool(UsedInVertexFitTrackDecorator(name, **kwargs),
                      primary = True)
    return acc

# Hard scatter vertex decorator
def HardScatterVertexDecoratorCfg(ConfigFlags, name, **kwargs):
    """Configure the hard process vertex decorator"""
    acc = ComponentAccumulator()
    HardScatterVertexDecorator = CompFactory.DerivationFramework.HardScatterVertexDecorator
    acc.addPublicTool(HardScatterVertexDecorator(name, **kwargs),
                       primary = True)
    return acc

# Expression of Z0 at the primary vertex
def TrackParametersAtPVCfg(ConfigFlags, name, **kwargs):
    """Configure the TrackParametersAtPV tool"""
    acc = ComponentAccumulator()
    TrackParametersAtPV = CompFactory.DerivationFramework.TrackParametersAtPV
    acc.addPublicTool(TrackParametersAtPV(name, **kwargs),
                      primary = True)
    return acc

# Pseudotrack selector
def PseudoTrackSelectorCfg(ConfigFlags, name, **kwargs):
    """Configure the pseudotrack selector"""
    acc = ComponentAccumulator()
    PseudoTrackSelector = CompFactory.DerivationFramework.PseudoTrackSelector
    acc.addPublicTool(PseudoTrackSelector(name, **kwargs),
                      primary = True)
    return acc

# Tool for decorating tracks with the outcome of the track selector tool
def InDetTrackSelectionToolWrapperCfg(ConfigFlags, name, **kwargs):
    """Configure the InDetTrackSelectionToolWrapper"""
    acc = ComponentAccumulator()
    InDetTrackSelectionToolWrapper = CompFactory.DerivationFramework.InDetTrackSelectionToolWrapper
    acc.addPublicTool(InDetTrackSelectionToolWrapper(name, **kwargs),
                      primary = True)
    return acc

# Tool for thinning TrackParticle containers via string selection
def TrackParticleThinningCfg(ConfigFlags, name, **kwargs):
    """Configure the TrackParticleThining tool"""
    acc = ComponentAccumulator()
    TrackParticleThinning = CompFactory.DerivationFramework.TrackParticleThinning
    acc.addPublicTool(TrackParticleThinning(name,**kwargs),
                      primary = True)
    return acc

# Tool for thinning TrackParticles that aren't associated with muons
def MuonTrackParticleThinningCfg(ConfigFlags, name, **kwargs):
    """Configure the MuonTrackParticleThinning tool"""
    acc = ComponentAccumulator()
    MuonTrackParticleThinning = CompFactory.DerivationFramework.MuonTrackParticleThinning
    acc.addPublicTool(MuonTrackParticleThinning(name,**kwargs),
                      primary = True)
    return acc

# Tool for thinning TrackParticles that aren't associated with taus
def TauTrackParticleThinningCfg(ConfigFlags, name, **kwargs):
    """Configure the TauTrackParticleThinning tool"""
    acc = ComponentAccumulator()
    TauTrackParticleThinning = CompFactory.DerivationFramework.TauTrackParticleThinning
    acc.addPublicTool(TauTrackParticleThinning(name, **kwargs),
                      primary = True)
    return acc

# Tool for thinning TrackParticles that aren't associated high-pt di-taus
def DiTauTrackParticleThinningCfg(ConfigFlags, name, **kwargs):
    """Configure the DiTauTrackParticleThinning tool"""
    acc = ComponentAccumulator()
    DiTauTrackParticleThinning = CompFactory.DerivationFramework.DiTauTrackParticleThinning
    acc.addPublicTool(DiTauTrackParticleThinning(name, **kwargs),
                      primary = True)
    return acc

