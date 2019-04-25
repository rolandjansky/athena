# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# import the MuonCalibrationAndSmearingTool configurable
from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__MuonCalibrationAndSmearingTool

def MuonCorrectionsToolCfg( name = 'MuonCorrectionsTool', **options):
    """Sets up the CP MuonCorrection tool and returns it."""
    acc = ComponentAccumulator()
    options['name'] = name
    acc.setPrivateTools(CP__MuonCalibrationAndSmearingTool( **options))

    return acc
