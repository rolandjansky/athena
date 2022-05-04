# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def MuonCorrectionsToolCfg( name = 'MuonCorrectionsTool', **options):
    """Sets up the CP MuonCorrection tool and returns it."""
    acc = ComponentAccumulator()
    options['name'] = name
    acc.setPrivateTools(CompFactory.CP.MuonCalibrationAndSmearingTool( **options))

    return acc
