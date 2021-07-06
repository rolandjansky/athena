# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


def FrozenShowersFCalOnly(ConfigFlags):
    """Turns on GFlash shower parametrization for FCAL"""
    ConfigFlags.Sim.LArParameterization = 3
    ConfigFlags.Sim.CalibrationRun = 'Off'


def BeamPipeKill(ConfigFlags):
    ConfigFlags.Sim.BeamPipeCut = 0.
    ConfigFlags.Sim.BeamPipeSimMode = 'FastSim'


def TightMuonStepping(ConfigFlags):
    ConfigFlags.Sim.TightMuonStepping = True
