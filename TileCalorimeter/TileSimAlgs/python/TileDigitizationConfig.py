"""Combined Tile Digitization functions

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from TileSimAlgs.TileHitToTTL1Config import TileHitToTTL1Cfg
from TileSimAlgs.TileDigitsMakerConfig import TileDigitsMakerCfg
from TileSimAlgs.TileHitVecToCntConfig import TileHitOutputCfg

# TODO implement all configuration. Status is progress for DigitizationConfigNew_test.py

def TileDigitizationCfg(flags):
    """Return ComponentAccumulator with standard Tile Digitization configuration"""
    acc = TileHitToTTL1Cfg(flags)
    acc.merge(TileDigitsMakerCfg(flags))
    acc.merge(TileHitOutputCfg(flags))
    return acc

