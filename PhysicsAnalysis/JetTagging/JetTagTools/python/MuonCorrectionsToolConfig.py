# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# import the MuonCalibrationAndSmearingTool configurable
CP__MuonCalibrationAndSmearingTool=CompFactory.CP__MuonCalibrationAndSmearingTool

def MuonCorrectionsToolCfg( name = 'MuonCorrectionsTool', **options):
    """Sets up the CP MuonCorrection tool and returns it."""
    acc = ComponentAccumulator()
    options['name'] = name
    acc.setPrivateTools(CP__MuonCalibrationAndSmearingTool( **options))

    return acc
