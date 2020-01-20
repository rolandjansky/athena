# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# File: FastCaloSim/python/AddNoiseCellBuilderTool.py
# Created: Aug 2019, sss
# Purpose: Configure AddNoiseCellBuilderTool.
#


def AddNoiseCellBuilderToolDefault (name = 'AddNoiseCellBuilderTool', **kwargs):
    from CaloTools.CaloEstimatedGainToolDefault import CaloEstimatedGainToolDefault
    estimatedGainTool = CaloEstimatedGainToolDefault()

    from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
    CaloNoiseCondAlg ('electronicNoise')
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("RandomSvc"                        , simFlags.RandomSvcMT())
    from FastCaloSim.FastCaloSimConf import AddNoiseCellBuilderTool
    return AddNoiseCellBuilderTool (name,
                                    NoiseKey = 'electronicNoise',
                                    CaloEstimatedGainTool = estimatedGainTool,
                                    **kwargs)
