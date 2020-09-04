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
    FullGeant4SelectorCfg,
    DefaultParticleKillerSelectorCfg,
    PassBackGeant4SelectorCfg,
)
from ISF_Geant4Tools.ISF_Geant4ToolsConfigNew import (
    FullGeant4ToolCfg, LongLivedGeant4ToolCfg,
    PassBackGeant4ToolCfg,
)
from ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigNew import (
    FastCaloToolBaseCfg,
)

# MT
def Kernel_GenericSimulatorMTCfg(flags, name="ISF_Kernel_GenericSimulatorMT", **kwargs):
    acc = ComponentAccumulator()

    tool = acc.popToolsAndMerge(ParticleKillerToolCfg(flags))
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

    tool = acc.popToolsAndMerge(FullGeant4SelectorCfg(flags))
    kwargs.setdefault("BeamPipeSimulationSelectors", [tool])
    kwargs.setdefault("IDSimulationSelectors", [tool])
    kwargs.setdefault("CaloSimulationSelectors", [tool])
    kwargs.setdefault("MSSimulationSelectors", [tool])

    tool = acc.popToolsAndMerge(DefaultParticleKillerSelectorCfg(flags))
    kwargs.setdefault("CavernSimulationSelectors", [tool])

    acc.merge(Kernel_GenericSimulatorMTCfg(flags, name, **kwargs))
    return acc


def Kernel_FullG4MTCfg(flags, name="ISF_Kernel_FullG4MT", **kwargs):
    acc = ComponentAccumulator()

    PKtool = acc.popToolsAndMerge(ParticleKillerToolCfg(flags))
    Fulltool = acc.popToolsAndMerge(FullGeant4ToolCfg(flags))
    kwargs.setdefault("SimulationTools", [PKtool, Fulltool])

    acc.merge(Kernel_GenericG4OnlyMTCfg(flags, name, **kwargs))
    return acc


def Kernel_FullG4MT_LongLivedCfg(flags, name="ISF_Kernel_FullG4MT_LongLived", **kwargs):
    acc = ComponentAccumulator()

    PKtool = acc.popToolsAndMerge(ParticleKillerToolCfg(flags))
    LLtool = acc.popToolsAndMerge(LongLivedGeant4ToolCfg(flags))
    kwargs.setdefault("SimulationTools", [PKtool, LLtool])

    acc.merge(LongLivedInputConverterCfg(flags))
    kwargs.setdefault("InputConverter",
                      acc.getService("ISF_LongLivedInputConverter"))

    acc.merge(Kernel_GenericG4OnlyMTCfg(flags, name, **kwargs))
    return acc


def Kernel_PassBackG4MTCfg(flags, name="ISF_Kernel_PassBackG4MT", **kwargs):
    acc = ComponentAccumulator()

    tool = acc.popToolsAndMerge(PassBackGeant4SelectorCfg(flags))
    kwargs.setdefault("BeamPipeSimulationSelectors", [tool])
    kwargs.setdefault("IDSimulationSelectors", [tool])
    kwargs.setdefault("CaloSimulationSelectors", [tool])
    kwargs.setdefault("MSSimulationSelectors", [tool])

    tool = acc.popToolsAndMerge(DefaultParticleKillerSelectorCfg(flags))
    kwargs.setdefault("CavernSimulationSelectors", [tool])

    PKtool = acc.popToolsAndMerge(ParticleKillerToolCfg(flags))
    PBtool = acc.popToolsAndMerge(PassBackGeant4ToolCfg(flags))
    kwargs.setdefault("SimulationTools", [PKtool, PBtool])

    tool = acc.popToolsAndMerge(EnergyParticleOrderingToolCfg(flags))
    kwargs.setdefault("ParticleOrderingTool", tool)

    acc.merge(Kernel_GenericSimulatorMTCfg(flags, name, **kwargs))
    return acc


def Kernel_ATLFASTIIFMTCfg(flags, name="ISF_Kernel_ATLFASTIIFMT", **kwargs):
    acc = ComponentAccumulator()

    PKtool = acc.popToolsAndMerge(ParticleKillerToolCfg(flags))
    FastCalotool = acc.popToolsAndMerge(FastCaloToolBaseCfg(flags))
    # TODO resolve this missing config
    raise NotImplementedError("No configuration for \"ISF_FatrasTool\"")
    Fatrastool = None
    # Fatrastool = acc.popToolsAndMerge(FatrasToolCfg(flags))
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
