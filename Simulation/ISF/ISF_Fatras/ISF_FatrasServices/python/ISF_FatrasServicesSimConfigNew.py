"""
ComponentAccumulator service configuration for ISF_Fatras simulator tool FatrasSimTool
Separated from main config to avoid circular dependences

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ISF_FatrasServices.ISF_FatrasServicesConfigNew import (
    FatrasSimulatorToolSTCfg,
    FatrasNewExtrapolationSimulatorToolSTCfg,
    FatrasPileupSimulatorToolSTCfg,
)
from ISF_SimulationSelectors.ISF_SimulationSelectorsConfigNew import (
    DefaultFatrasSelectorCfg, MuonFatrasSelectorCfg,
)

def FatrasSimulatorToolCfg(flags, name="ISF_FatrasSimulatorTool", **kwargs):
    acc = ComponentAccumulator()
    deftool = acc.popToolsAndMerge(DefaultFatrasSelectorCfg(flags))
    muontool = acc.popToolsAndMerge(MuonFatrasSelectorCfg(flags))
    kwargs.setdefault("IDSimulationSelectors", [deftool])
    kwargs.setdefault("CaloSimulationSelectors", [muontool])
    kwargs.setdefault("MSSimulationSelectors", [deftool])
    tool = acc.popToolsAndMerge(FatrasSimulatorToolSTCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def FatrasNewExtrapolationSimulatorToolCfg(flags, name="ISF_FatrasSNewExtrapolationimulatorTool", **kwargs):
    acc = ComponentAccumulator()
    deftool = acc.popToolsAndMerge(DefaultFatrasSelectorCfg(flags))
    muontool = acc.popToolsAndMerge(MuonFatrasSelectorCfg(flags))
    kwargs.setdefault("IDSimulationSelectors", [deftool])
    kwargs.setdefault("CaloSimulationSelectors", [muontool])
    kwargs.setdefault("MSSimulationSelectors", [deftool])
    tool = acc.popToolsAndMerge(FatrasNewExtrapolationSimulatorToolSTCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return tool

def FatrasPileupSimulatorToolCfg(flags, name="ISF_FatrasPileupSimulatorTool", **kwargs):
    acc = ComponentAccumulator()
    deftool = acc.popToolsAndMerge(DefaultFatrasSelectorCfg(flags))
    muontool = acc.popToolsAndMerge(MuonFatrasSelectorCfg(flags))
    kwargs.setdefault("IDSimulationSelectors", [deftool])
    kwargs.setdefault("CaloSimulationSelectors", [muontool])
    kwargs.setdefault("MSSimulationSelectors", [deftool])
    tool = acc.popToolsAndMerge(FatrasPileupSimulatorToolSTCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return tool
