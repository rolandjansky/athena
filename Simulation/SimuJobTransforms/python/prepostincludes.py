"""pre- and post-includes for New style transform integrations

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

def preInclude_FrozenShowersFCalOnly(ConfigFlags):
    """based on preInclude.FrozenShowersFCalOnly.py
    #==============================================================
    # SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py
    # - Turns on GFlash shower parametrization for FCAL
    #==============================================================
    """
    ConfigFlags.Sim.LArParameterization=3
    ConfigFlags.Sim.CalibrationRun = 'Off' #   simFlags.CalibrationRun.set_Off()

def preInclude_BeamPipeKill(ConfigFlags):
    """based on share/BeamPipeKill.py"""
    ConfigFlags.Sim.BeamPipeCut = 0.
    ConfigFlags.Sim.BeamPipeSimMode = "FastSim"

# Note - is this flag used?
def preInclude_TightMuonStepping(ConfigFlags):
    """ previously EVNTtoHITS:simFlags.TightMuonStepping=True'"""
    ConfigFlags.Sim.TightMuonStepping = True
