"""Main ISF tools configuration with ComponentAccumulator

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Services.ISF_ServicesCoreConfigNew import GeoIDSvcCfg
from ISF_Services.ISF_ServicesConfigNew import (
    InputConverterCfg, ParticleBrokerSvcCfg,
    TruthServiceCfg, LongLivedInputConverterCfg,
)
from ISF_Tools.ISF_ToolsConfigNew import (
    MemoryMonitorToolCfg, ParticleKillerToolCfg,
    EnergyParticleOrderingToolCfg,
)
from ISF_SimulationSelectors.ISF_SimulationSelectorsConfigNew import (
    DefaultAFIIGeant4SelectorCfg,
    DefaultLegacyAFIIFastCaloSimSelectorCfg,
    DefaultParticleKillerSelectorCfg,
    EtaGreater5ParticleKillerSimSelectorCfg,
    FullGeant4SelectorCfg,
    MuonAFIIGeant4SelectorCfg,
    PassBackGeant4SelectorCfg,
)
from ISF_Geant4Tools.ISF_Geant4ToolsConfigNew import (
    AFIIGeant4ToolCfg,
    FullGeant4ToolCfg,
    LongLivedGeant4ToolCfg,
    PassBackGeant4ToolCfg,
)
from ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigNew import (
    FastCaloToolBaseCfg,
    LegacyAFIIFastCaloToolCfg,
)
from ISF_FatrasServices.ISF_FatrasConfig import fatrasSimToolCfg

# MT
def Kernel_GenericSimulatorMTCfg(flags, name="ISF_Kernel_GenericSimulatorMT", **kwargs):
    acc = ComponentAccumulator()

    acc.merge(ParticleKillerToolCfg(flags))
    tool = acc.getPublicTool("ISF_ParticleKillerTool")
    kwargs.setdefault("ParticleKillerTool", tool)

    acc.merge(GeoIDSvcCfg(flags))
    kwargs.setdefault("GeoIDSvc", acc.getService("ISF_GeoIDSvc"))

    acc.merge(InputConverterCfg(flags))
    kwargs.setdefault("InputConverter", acc.getService("ISF_InputConverter"))

    truthacc = TruthServiceCfg(flags)
    kwargs.setdefault("TruthRecordService", truthacc.getPrimary())
    acc.merge(truthacc)

    kwargs.setdefault("Cardinality", flags.Concurrency.NumThreads)
    kwargs.setdefault("OutputLevel", 1)
    kwargs.setdefault("InputEvgenCollection", "BeamTruthEvent")
    kwargs.setdefault("OutputTruthCollection", "TruthEvent")

    acc.addEventAlgo(CompFactory.ISF.SimKernelMT(name, **kwargs))
    return acc


def Kernel_GenericSimulatorNoG4MTCfg(flags, name="ISF_Kernel_GenericSimulatorNoG4MT", **kwargs):
    return Kernel_GenericSimulatorMTCfg(flags, name, **kwargs)


def Kernel_GenericG4OnlyMTCfg(flags, name="ISF_Kernel_GenericG4OnlyMT", **kwargs):
    acc = ComponentAccumulator()

    acc.merge(FullGeant4SelectorCfg(flags))
    tool = acc.getPublicTool("ISF_FullGeant4Selector")
    kwargs.setdefault("BeamPipeSimulationSelectors", [tool])
    kwargs.setdefault("IDSimulationSelectors", [tool])
    kwargs.setdefault("CaloSimulationSelectors", [tool])
    kwargs.setdefault("MSSimulationSelectors", [tool])

    acc.merge(DefaultParticleKillerSelectorCfg(flags))
    tool = acc.getPublicTool("ISF_DefaultParticleKillerSelector")
    kwargs.setdefault("CavernSimulationSelectors", [tool])

    acc.merge(Kernel_GenericSimulatorMTCfg(flags, name, **kwargs))
    return acc


def Kernel_FullG4MTCfg(flags, name="ISF_Kernel_FullG4MT", **kwargs):
    acc = ComponentAccumulator()

    acc.merge(ParticleKillerToolCfg(flags))
    acc.merge(FullGeant4ToolCfg(flags))
    PKtool = acc.getPublicTool("ISF_ParticleKillerTool")
    Fulltool = acc.getPublicTool("ISF_FullGeant4Tool")
    kwargs.setdefault("SimulationTools", [PKtool, Fulltool])

    acc.merge(Kernel_GenericG4OnlyMTCfg(flags, name, **kwargs))
    return acc


def Kernel_FullG4MT_LongLivedCfg(flags, name="ISF_Kernel_FullG4MT_LongLived", **kwargs):
    acc = ComponentAccumulator()

    acc.merge(ParticleKillerToolCfg(flags))
    acc.merge(LongLivedGeant4ToolCfg(flags))
    PKtool = acc.getPublicTool("ISF_ParticleKillerTool")
    LLtool = acc.getPublicTool("ISF_LongLivedGeant4Tool")
    kwargs.setdefault("SimulationTools", [PKtool, LLtool])

    acc.merge(LongLivedInputConverterCfg(flags))
    kwargs.setdefault("InputConverter",
                      acc.getService("ISF_LongLivedInputConverter"))

    acc.merge(Kernel_GenericG4OnlyMTCfg(flags, name, **kwargs))
    return acc


def Kernel_PassBackG4MTCfg(flags, name="ISF_Kernel_PassBackG4MT", **kwargs):
    acc = ComponentAccumulator()

    acc.merge(PassBackGeant4SelectorCfg(flags))
    tool = acc.getPublicTool("ISF_PassBackGeant4Selector")
    kwargs.setdefault("BeamPipeSimulationSelectors", [tool])
    kwargs.setdefault("IDSimulationSelectors", [tool])
    kwargs.setdefault("CaloSimulationSelectors", [tool])
    kwargs.setdefault("MSSimulationSelectors", [tool])

    acc.merge(DefaultParticleKillerSelectorCfg(flags))
    tool = acc.getPublicTool("ISF_DefaultParticleKillerSelector")
    kwargs.setdefault("CavernSimulationSelectors", [tool])

    acc.merge(ParticleKillerToolCfg(flags))
    acc.merge(PassBackGeant4ToolCfg(flags))
    PKtool = acc.getPublicTool("ISF_ParticleKillerTool")
    PBtool = acc.getPublicTool("ISF_PassBackGeant4Tool")
    kwargs.setdefault("SimulationTools", [PKtool, PBtool])

    acc.merge(EnergyParticleOrderingToolCfg(flags))
    tool = acc.getPublicTool("ISF_EnergyParticleOrderingTool")
    kwargs.setdefault("ParticleOrderingTool", tool)

    acc.merge(Kernel_GenericSimulatorMTCfg(flags, name, **kwargs))
    return acc


def Kernel_ATLFASTIIMTCfg(flags, name="ISF_Kernel_ATLFASTIIMT", **kwargs):
    acc = ComponentAccumulator()

    acc.merge(DefaultAFIIGeant4SelectorCfg(flags))
    tool = acc.getPublicTool("ISF_DefaultAFIIGeant4Selector")
    kwargs.setdefault("BeamPipeSimulationSelectors", [tool])
    kwargs.setdefault("IDSimulationSelectors", [tool])
    kwargs.setdefault("MSSimulationSelectors", [tool])

    acc.merge(MuonAFIIGeant4SelectorCfg(flags))
    acc.merge(EtaGreater5ParticleKillerSimSelectorCfg(flags))
    acc.merge(DefaultLegacyAFIIFastCaloSimSelectorCfg(flags))
    Mutool = acc.getPublicTool("ISF_MuonAFIIGeant4Selector")
    EtaGtool = acc.getPublicTool("ISF_EtaGreater5ParticleKillerSimSelector")
    DefLegtool = acc.getPublicTool("ISF_DefaultLegacyAFIIFastCaloSimSelector")
    kwargs.setdefault("CaloSimulationSelectors", [Mutool, EtaGtool, DefLegtool])

    acc.merge(DefaultParticleKillerSelectorCfg(flags))
    tool = acc.getPublicTool("ISF_DefaultParticleKillerSelector")
    kwargs.setdefault("CavernSimulationSelectors", [tool])

    acc.merge(ParticleKillerToolCfg(flags))
    acc.merge(LegacyAFIIFastCaloToolCfg(flags))
    acc.merge(AFIIGeant4ToolCfg(flags))
    PKtool = acc.getPublicTool("ISF_ParticleKillerTool")
    Legtool = acc.getPublicTool("ISF_LegacyAFIIFastCaloTool")
    AFIItool = acc.getPublicTool("ISF_AFIIGeant4Tool")
    kwargs.setdefault("SimulationTools", [PKtool, Legtool, AFIItool])

    acc.merge(EnergyParticleOrderingToolCfg(flags))
    tool = acc.getPublicTool("ISF_EnergyParticleOrderingTool")
    kwargs.setdefault("ParticleOrderingTool", tool)

    # not migrated simFlags.SimulationFlavour = "ATLFASTII"
    acc.merge(Kernel_GenericSimulatorMTCfg(flags, name, **kwargs))
    return acc


def Kernel_ATLFASTIIFMTCfg(flags, name="ISF_Kernel_ATLFASTIIFMT", **kwargs):
    acc = ComponentAccumulator()

    acc.merge(ParticleKillerToolCfg(flags))
    acc.merge(FastCaloToolBaseCfg(flags))
    acc.merge(fatrasSimToolCfg(flags))
    PKtool = acc.getPublicTool("ISF_ParticleKillerTool")
    FastCalotool = acc.getPublicTool("ISF_FastCaloTool")
    Fatrastool = acc.getPublicTool("ISF_FatrasSimTool")
    kwargs.setdefault("SimulationTools", [PKtool, FastCalotool, Fatrastool])

    # not migrated 'simFlags.SimulationFlavour = "ATLFASTIIF"'
    acc.merge(Kernel_GenericSimulatorNoG4MTCfg(flags, name, **kwargs))
    return acc


# non-MT
# Note: mostly not migrated
def Kernel_GenericSimulatorCfg(flags, name="ISF_Kernel_GenericSimulator", **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputConverterCfg(flags))
    kwargs.setdefault("InputConverter", acc.getService("ISF_InputConverter"))

    acc.merge(ParticleBrokerSvcCfg(flags))
    kwargs.setdefault("ParticleBroker", acc.getService("ISF_ParticleBrokerSvc"))

    truthacc = TruthServiceCfg(flags)
    kwargs.setdefault("TruthRecordService", truthacc.getPrimary())
    acc.merge(truthacc)

    tool = acc.popToolsAndMerge(MemoryMonitorToolCfg(flags))
    kwargs.setdefault("MemoryMonitoringTool", tool)

    kwargs.setdefault("DoCPUMonitoring", flags.Sim.ISF.DoTimeMonitoring)
    kwargs.setdefault("DoMemoryMonitoring", flags.Sim.ISF.DoMemoryMonitoring)
    kwargs.setdefault("InputHardScatterCollection", "BeamTruthEvent")
    kwargs.setdefault("OutputHardScatterTruthCollection", "TruthEvent")

    acc.addEventAlgo(CompFactory.ISF.SimKernelCfg(name, **kwargs))
    return acc


def Kernel_GenericSimulatorNoG4Cfg(flags, name="ISF_Kernel_GenericSimulatorNoG4", **kwargs):
    return Kernel_GenericSimulatorCfg(flags, name, **kwargs)
