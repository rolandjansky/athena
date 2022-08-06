"""
ComponentAccumulator configuration for CalibrationDefaultProcessingTool

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def CalibrationDefaultProcessingToolCfg(flags, name="G4UA::CaloG4::CalibrationDefaultProcessing", **kwargs):
    acc = ComponentAccumulator()
    # this SD name is for ATLAS, H8
    kwargs.setdefault("SDName", "Default::Dead::Uninstrumented::Calibration::Region")
    acc.setPrivateTools(CompFactory.G4UA.CaloG4.CalibrationDefaultProcessingTool(name, **kwargs))
    return acc
