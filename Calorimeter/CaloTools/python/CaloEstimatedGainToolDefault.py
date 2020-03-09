# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# File: CaloTools/python/CaloEstimatedGainToolDefault.py
# Created: Aug 2019, sss
# Purpose: Configure CaloEstimatedGainTool.
#


def CaloEstimatedGainToolDefault():
    from LArCabling.LArCablingAccess import LArFebRodMapping, LArCalibIdMapping
    LArFebRodMapping()
    LArCalibIdMapping()

    from TileConditions.TileInfoConfigurator import TileInfoConfigurator
    TileInfoConfigurator()

    from TileConditions.TileCondToolConf import \
        getTileCondToolEmscale, \
        getTileCondToolNoiseSample
    from TileConditions.TileConditionsConf import TileCondIdTransforms

    from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
    adc2mev = LArADC2MeVCondAlgDefault()

    from CaloTools.CaloToolsConf import CaloEstimatedGainTool
    tool = CaloEstimatedGainTool ('CaloEstimatedGainTool',
                                  ADC2MeVKey = adc2mev.LArADC2MeVKey,
                                  TileCondIdTransforms = TileCondIdTransforms(),
                                  TileCondToolEmscale = getTileCondToolEmscale(),
                                  TileCondToolNoiseSample = getTileCondToolNoiseSample())
    return tool

