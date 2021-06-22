"""Main ISF tools configuration with ComponentAccumulator

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Services.ISF_ServicesCoreConfigNew import GeoIDSvcCfg
from ISF_Services.ISF_ServicesConfigNew import (
    InputConverterCfg, TruthServiceCfg,
    LongLivedInputConverterCfg, AFIIParticleBrokerSvcCfg
)
from ISF_Tools.ISF_ToolsConfigNew import (
    ParticleKillerToolCfg, EnergyParticleOrderingToolCfg,
    ParticleOrderingToolCfg, MemoryMonitorToolCfg
)
from ISF_SimulationSelectors.ISF_SimulationSelectorsConfigNew import (
    DefaultAFIIGeant4SelectorCfg,
    DefaultLegacyAFIIFastCaloSimSelectorCfg,
    DefaultParticleKillerSelectorCfg,
    EtaGreater5ParticleKillerSimSelectorCfg,
    FullGeant4SelectorCfg,
    MuonAFIIGeant4SelectorCfg,
    PassBackGeant4SelectorCfg,
    DefaultFastCaloSimV2SelectorCfg,
    PionG4FastCaloGeant4Selector,
    ProtonG4FastCaloGeant4Selector,
    NeutronG4FastCaloGeant4Selector,
    ChargedKaonG4FastCaloGeant4Selector,
    KLongG4FastCaloGeant4Selector,
    DefaultFatrasSelectorCfg,
    MuonFatrasSelectorCfg,
    DefaultFastCaloSimSelectorCfg
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
    FastCaloSimV2ToolCfg,
)
from ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfigNew import (
    AFIIEntryLayerToolMTCfg
)
from ISF_FatrasServices.ISF_FatrasConfig import fatrasTransportToolCfg

# MT
def Kernel_GenericSimulatorMTCfg(flags, name="ISF_Kernel_GenericSimulatorMT", **kwargs):
    acc = ComponentAccumulator()

    if "ParticleKillerTool" not in kwargs:
        tool = acc.popToolsAndMerge(ParticleKillerToolCfg(flags))
        acc.addPublicTool(tool)
        pubTool = acc.getPublicTool(tool.name)
        kwargs.setdefault("ParticleKillerTool", pubTool) # public toolHandle

    if "GeoIDSvc" not in kwargs:
        acc.merge(GeoIDSvcCfg(flags))
        kwargs.setdefault("GeoIDSvc", acc.getService("ISF_GeoIDSvc"))

    if "InputConverter" not in kwargs:
        acc.merge(InputConverterCfg(flags))
        kwargs.setdefault("InputConverter", acc.getService("ISF_InputConverter"))

    if "TruthRecordService" not in kwargs:
        truthacc = TruthServiceCfg(flags)
        kwargs.setdefault("TruthRecordService", truthacc.getPrimary())
        acc.merge(truthacc)

    kwargs.setdefault("Cardinality", flags.Concurrency.NumThreads)
    kwargs.setdefault("OutputLevel", 1)
    kwargs.setdefault("InputEvgenCollection", "BeamTruthEvent")
    kwargs.setdefault("OutputTruthCollection", "TruthEvent")

    #Write MetaData container
    from G4AtlasApps.G4Atlas_MetadataNew import writeSimulationParametersMetadata
    acc.merge(writeSimulationParametersMetadata(flags))
    acc.addEventAlgo(CompFactory.ISF.SimKernelMT(name, **kwargs))
    return acc


def Kernel_GenericSimulatorNoG4MTCfg(flags, name="ISF_Kernel_GenericSimulatorNoG4MT", **kwargs):
    return Kernel_GenericSimulatorMTCfg(flags, name, **kwargs)


def Kernel_GenericG4OnlyMTCfg(flags, name="ISF_Kernel_GenericG4OnlyMT", **kwargs):
    acc = ComponentAccumulator()

    defaultG4SelectorRegions = set(["BeamPipeSimulationSelectors", "IDSimulationSelectors", "CaloSimulationSelectors", "MSSimulationSelectors"])
    if defaultG4SelectorRegions - kwargs.keys(): # i.e. if any of these have not been defined yet
        tool = acc.popToolsAndMerge(FullGeant4SelectorCfg(flags))
        acc.addPublicTool(tool)
        pubTool = acc.getPublicTool(tool.name)
        # SimulationSelectors are still public ToolHandleArrays currently
        kwargs.setdefault("BeamPipeSimulationSelectors", [pubTool])
        kwargs.setdefault("IDSimulationSelectors", [pubTool])
        kwargs.setdefault("CaloSimulationSelectors", [pubTool])
        kwargs.setdefault("MSSimulationSelectors", [pubTool])

    if "CavernSimulationSelectors" not in kwargs:
        tool = acc.popToolsAndMerge(DefaultParticleKillerSelectorCfg(flags))
        acc.addPublicTool(tool)
        pubTool = acc.getPublicTool(tool.name)
        kwargs.setdefault("CavernSimulationSelectors", [pubTool])

    acc.merge(Kernel_GenericSimulatorMTCfg(flags, name, **kwargs))
    return acc


def Kernel_FullG4MTCfg(flags, name="ISF_Kernel_FullG4MT", **kwargs):
    acc = ComponentAccumulator()

    if "SimulationTools" not in kwargs:
        kwargs.setdefault("SimulationTools", [
            acc.popToolsAndMerge(ParticleKillerToolCfg(flags)),
            acc.popToolsAndMerge(FullGeant4ToolCfg(flags))
        ]) #private ToolHandleArray

    acc.merge(Kernel_GenericG4OnlyMTCfg(flags, name, **kwargs))
    return acc


def Kernel_FullG4MT_LongLivedCfg(flags, name="ISF_Kernel_FullG4MT_LongLived", **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("SimulationTools", [
        acc.popToolsAndMerge(ParticleKillerToolCfg(flags)),
        acc.merge(LongLivedGeant4ToolCfg(flags))
    ])

    acc.merge(LongLivedInputConverterCfg(flags))
    kwargs.setdefault("InputConverter", acc.getService("ISF_LongLivedInputConverter"))

    acc.merge(Kernel_GenericG4OnlyMTCfg(flags, name, **kwargs))
    return acc


def Kernel_PassBackG4MTCfg(flags, name="ISF_Kernel_PassBackG4MT", **kwargs):
    acc = ComponentAccumulator()

    defaultG4SelectorRegions = set(["BeamPipeSimulationSelectors", "IDSimulationSelectors", "CaloSimulationSelectors", "MSSimulationSelectors"])
    if defaultG4SelectorRegions - kwargs.keys(): # i.e. if any of these have not been defined yet
        tool = acc.popToolsAndMerge(PassBackGeant4SelectorCfg(flags))
        acc.addPublicTool(tool)
        pubTool = acc.getPublicTool(tool.name)
        kwargs.setdefault("BeamPipeSimulationSelectors", [pubTool])
        kwargs.setdefault("IDSimulationSelectors", [pubTool])
        kwargs.setdefault("CaloSimulationSelectors", [pubTool])
        kwargs.setdefault("MSSimulationSelectors", [pubTool])

    if "CavernSimulationSelectors" not in kwargs:
        tool = acc.popToolsAndMerge(DefaultParticleKillerSelectorCfg(flags))
        acc.addPublicTool(tool)
        pubTool = acc.getPublicTool(tool.name)
        kwargs.setdefault("CavernSimulationSelectors", [pubTool])

    if "SimulationTools" not in kwargs:
        kwargs.setdefault("SimulationTools", [
            acc.popToolsAndMerge(ParticleKillerToolCfg(flags)),
            acc.popToolsAndMerge(PassBackGeant4ToolCfg(flags))
        ])

    if "ParticleOrderingTool" not in kwargs:
        kwargs.setdefault("ParticleOrderingTool", acc.popToolsAndMerge(EnergyParticleOrderingToolCfg(flags)))

    acc.merge(Kernel_GenericSimulatorMTCfg(flags, name, **kwargs))
    return acc


def Kernel_ATLFASTIIMTCfg(flags, name="ISF_Kernel_ATLFASTIIMT", **kwargs):
    acc = ComponentAccumulator()

    tool = acc.popToolsAndMerge(DefaultAFIIGeant4SelectorCfg(flags))
    acc.addPublicTool(tool)
    pubTool = acc.getPublicTool(tool.name)
    kwargs.setdefault("BeamPipeSimulationSelectors", [pubTool])
    kwargs.setdefault("IDSimulationSelectors", [pubTool])
    kwargs.setdefault("MSSimulationSelectors", [pubTool])

    caloSimSelectors = []
    tool = acc.popToolsAndMerge(MuonAFIIGeant4SelectorCfg(flags))
    acc.addPublicTool(tool)
    caloSimSelectors += [acc.getPublicTool(tool.name)]
    tool = acc.popToolsAndMerge(EtaGreater5ParticleKillerSimSelectorCfg(flags))
    acc.addPublicTool(tool)
    caloSimSelectors += [acc.getPublicTool(tool.name)]
    tool = acc.popToolsAndMerge(DefaultLegacyAFIIFastCaloSimSelectorCfg(flags))
    acc.addPublicTool(tool)
    caloSimSelectors += [acc.getPublicTool(tool.name)]
    kwargs.setdefault("CaloSimulationSelectors", caloSimSelectors)

    tool = acc.popToolsAndMerge(DefaultParticleKillerSelectorCfg(flags))
    acc.addPublicTool(tool)
    pubTool = acc.getPublicTool(tool.name)
    kwargs.setdefault("CavernSimulationSelectors", [pubTool])

    kwargs.setdefault("SimulationTools", [
        acc.popToolsAndMerge(ParticleKillerToolCfg(flags)),
        acc.popToolsAndMerge(LegacyAFIIFastCaloToolCfg(flags)),
        acc.popToolsAndMerge(AFIIGeant4ToolCfg(flags))
    ])

    kwargs.setdefault("ParticleOrderingTool", acc.popToolsAndMerge(EnergyParticleOrderingToolCfg(flags)))

    # not migrated simFlags.SimulationFlavour = "ATLFASTII"
    acc.merge(Kernel_GenericSimulatorMTCfg(flags, name, **kwargs))
    return acc


def Kernel_ATLFASTIIFMTCfg(flags, name="ISF_Kernel_ATLFASTIIFMT", **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("SimulationTools", [
        acc.popToolsAndMerge(ParticleKillerToolCfg(flags)),
        acc.popToolsAndMerge(FastCaloToolBaseCfg(flags)),
        acc.popToolsAndMerge(fatrasTransportToolCfg(flags))
    ])

    # not migrated 'simFlags.SimulationFlavour = "ATLFASTIIF"'
    acc.merge(Kernel_GenericSimulatorNoG4MTCfg(flags, name, **kwargs))
    return acc


def Kernel_G4FastCaloMT(flags, name="ISF_Kernel_G4FastCaloMT", **kwargs):
    acc = ComponentAccumulator()

    # BeamPipe, ID, MS Simulation Selectors
    tool = acc.popToolsAndMerge(DefaultAFIIGeant4SelectorCfg(flags))
    acc.addPublicTool(tool)
    pubTool = acc.getPublicTool(tool.name)
    kwargs.setdefault("BeamPipeSimulationSelectors", [pubTool])
    kwargs.setdefault("IDSimulationSelectors"      , [pubTool])
    kwargs.setdefault("MSSimulationSelectors"      , [pubTool])

    # CaloSimulationSelectors
    acc.addPublicTool(acc.popToolsAndMerge(MuonAFIIGeant4SelectorCfg(flags)))
    acc.addPublicTool(acc.popToolsAndMerge(EtaGreater5ParticleKillerSimSelectorCfg(flags)))
    acc.addPublicTool(acc.popToolsAndMerge(PionG4FastCaloGeant4Selector(flags)))
    acc.addPublicTool(acc.popToolsAndMerge(ProtonG4FastCaloGeant4Selector(flags)))
    acc.addPublicTool(acc.popToolsAndMerge(NeutronG4FastCaloGeant4Selector(flags)))
    acc.addPublicTool(acc.popToolsAndMerge(ChargedKaonG4FastCaloGeant4Selector(flags)))
    acc.addPublicTool(acc.popToolsAndMerge(KLongG4FastCaloGeant4Selector(flags)))
    acc.addPublicTool(acc.popToolsAndMerge(DefaultFastCaloSimV2SelectorCfg(flags)))
    kwargs.setdefault("CaloSimulationSelectors"    , [ acc.getPublicTool("ISF_MuonAFIIGeant4Selector"),
                                                       acc.getPublicTool("ISF_EtaGreater5ParticleKillerSimSelector"),
                                                       acc.getPublicTool("ISF_PionG4FastCaloGeant4Selector"),
                                                       acc.getPublicTool("ISF_ProtonG4FastCaloGeant4Selector"),
                                                       acc.getPublicTool("ISF_NeutronG4FastCaloGeant4Selector"),
                                                       acc.getPublicTool("ISF_ChargedKaonG4FastCaloGeant4Selector"),
                                                       acc.getPublicTool("ISF_KLongG4FastCaloGeant4Selector"),
                                                       acc.getPublicTool("ISF_DefaultFastCaloSimV2Selector") ])
    # CavernSimulationSelectors
    acc.addPublicTool(acc.popToolsAndMerge(DefaultParticleKillerSelectorCfg(flags)))
    kwargs.setdefault("CavernSimulationSelectors"  , [ acc.getPublicTool("ISF_DefaultParticleKillerSelector") ])

    kwargs.setdefault("SimulationTools"            , [ acc.popToolsAndMerge(ParticleKillerToolCfg(flags)),
                                                       acc.popToolsAndMerge(FastCaloSimV2ToolCfg(flags)),
                                                       acc.popToolsAndMerge(AFIIGeant4ToolCfg(flags)) ])
    kwargs.setdefault("ParticleOrderingTool"       ,   acc.popToolsAndMerge(ParticleOrderingToolCfg(flags)))

    tool = acc.popToolsAndMerge(AFIIEntryLayerToolMTCfg(flags))
    acc.addPublicTool(tool)
    kwargs.setdefault("EntryLayerTool"             ,   acc.getPublicTool(tool.name)) # public ToolHandle
    acc.merge(Kernel_GenericSimulatorMTCfg(flags, name, **kwargs))
    return acc


def Kernel_GenericSimulatorCfg(flags, name="ISF_Kernel_GenericSimulator", **kwargs):
    acc = ComponentAccumulator()

    if "TruthRecordService" not in kwargs:
        truthacc = TruthServiceCfg(flags)
        kwargs.setdefault("TruthRecordService", truthacc.getPrimary())
        acc.merge(truthacc)

    if "MemoryMonitoringTool" not in kwargs:
        tool = acc.popToolsAndMerge(MemoryMonitorToolCfg(flags))
        acc.addPublicTool(tool)
        pubTool = acc.getPublicTool(tool.name)
        kwargs.setdefault("MemoryMonitoringTool", pubTool)

    if "ParticleBroker" not in kwargs:
        acc.merge(AFIIParticleBrokerSvcCfg(flags))
        kwargs.setdefault("ParticleBroker", acc.getService("ISF_AFIIParticleBrokerSvc"))

    if "InputConverter" not in kwargs:
        acc.merge(InputConverterCfg(flags))
        kwargs.setdefault("InputConverter", acc.getService("ISF_InputConverter"))

    kwargs.setdefault("InputHardScatterCollection", "BeamTruthEvent")
    kwargs.setdefault("OutputHardScatterTruthCollection", "TruthEvent")
    kwargs.setdefault("DoCPUMonitoring", flags.Sim.ISF.DoTimeMonitoring)
    kwargs.setdefault("DoMemoryMonitoring", flags.Sim.ISF.DoMemoryMonitoring)

    acc.addEventAlgo(CompFactory.ISF.SimKernel(name, **kwargs))
    return acc

def Kernel_ATLFASTIIF_G4MSCfg(flags, name="ISF_Kernel_ATLFASTIIF_G4MS", **kwargs):
    acc = ComponentAccumulator()

    acc.addPublicTool(acc.popToolsAndMerge(DefaultParticleKillerSelectorCfg(flags)))
    acc.addPublicTool(acc.popToolsAndMerge(DefaultFatrasSelectorCfg(flags)))
    acc.addPublicTool(acc.popToolsAndMerge(MuonFatrasSelectorCfg(flags)))
    acc.addPublicTool(acc.popToolsAndMerge(EtaGreater5ParticleKillerSimSelectorCfg(flags)))
    acc.addPublicTool(acc.popToolsAndMerge(DefaultFastCaloSimSelectorCfg(flags)))
    acc.addPublicTool(acc.popToolsAndMerge(DefaultAFIIGeant4SelectorCfg(flags)))

    kwargs.setdefault("BeamPipeSimulationSelectors", [ acc.getPublicTool("ISF_DefaultParticleKillerSelector") ])
    kwargs.setdefault("IDSimulationSelectors"      , [ acc.getPublicTool("ISF_DefaultFatrasSelector") ])
    kwargs.setdefault("CaloSimulationSelectors"    , [ acc.getPublicTool("ISF_MuonFatrasSelector"),
                                                       acc.getPublicTool("ISF_EtaGreater5ParticleKillerSimSelector"),
                                                       acc.getPublicTool("ISF_DefaultFastCaloSimSelector") ])
    kwargs.setdefault("MSSimulationSelectors"      , [ acc.getPublicTool("ISF_DefaultAFIIGeant4Selector") ])
    kwargs.setdefault("CavernSimulationSelectors"  , [ acc.getPublicTool("ISF_DefaultParticleKillerSelector") ])
    # not migrated simFlags.SimulationFlavour = "ATLFASTIIF_MS"
    # simFlags.SimulationFlavour = "ATLFASTIIF_G4MS"

    acc.merge(Kernel_GenericSimulatorCfg(flags, name, **kwargs))
    return acc

def ISF_KernelCfg(flags):
    acc = ComponentAccumulator()
    if flags.Sim.ISF.Simulator in ('FullG4MT'):
        acc.merge(Kernel_FullG4MTCfg(flags))
    elif flags.Sim.ISF.Simulator in ('FullG4MT_LongLived'):
        acc.merge(Kernel_FullG4MT_LongLivedCfg(flags))
    elif flags.Sim.ISF.Simulator in ('PassBackG4MT'):
        acc.merge(Kernel_PassBackG4MTCfg(flags))
    elif flags.Sim.ISF.Simulator in ('G4FastCaloMT'):
        acc.merge(Kernel_G4FastCaloMT(flags))
    elif flags.Sim.ISF.Simulator in ('ATLFASTIIFMT'):
        acc.merge(Kernel_ATLFASTIIFMTCfg(flags))
    elif flags.Sim.ISF.Simulator in ('ATLFASTIIF_G4MS'):
        acc.merge(Kernel_ATLFASTIIF_G4MSCfg(flags))
    else:
        print('Unsupported Simulator %s, bailing out', flags.Sim.ISF.Simulator)
        import sys
        sys.exit(1)
    return acc
