# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from ISF_HepMC_Tools import ParticleSimWhiteList_ExtraParticlesCfg, ParticleFinalStateFilterCfg, GenParticleInteractingFilterCfg, EtaPhiFilterCfg, ParticlePositionFilterDynamicCfg

def SimEventFilterCfg(flags, name="ISF_SimEventFilter", sequenceName='SimSequence', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault( "InputHardScatterCollection", "BeamTruthEvent")
    kwargs.setdefault( "GenParticleCommonFilters", [result.popToolsAndMerge(ParticlePositionFilterDynamicCfg(flags)),
                                                    result.popToolsAndMerge(EtaPhiFilterCfg(flags)),
                                                    result.popToolsAndMerge(GenParticleInteractingFilterCfg(flags))] )
    kwargs.setdefault( "GenParticleOldFilters", [result.popToolsAndMerge(ParticleFinalStateFilterCfg(flags))] )
    kwargs.setdefault( "GenParticleNewFilters", [result.popToolsAndMerge(ParticleSimWhiteList_ExtraParticlesCfg(flags))] )

    result.addEventAlgo(CompFactory.ISF.SimEventFilter(name, **kwargs), sequenceName) # TODO Need to ensure that sequence has been created
    return result

def InvertedSimEventFilterCfg(flags, name="ISF_InvertedSimEventFilter", sequenceName='CopyHitSequence', **kwargs):
    kwargs.setdefault("InvertFilter", True)
    return SimEventFilterCfg(flags, name, sequenceName, **kwargs)
