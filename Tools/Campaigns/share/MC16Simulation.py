# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from G4AtlasApps.SimFlags import simFlags
simFlags.PhysicsList = "FTFP_BERT_ATL"
simFlags.TruthStrategy = "MC15aPlus"

simFlags.RunNumber = 284500

simFlags.TRTRangeCut = 30.0
simFlags.TightMuonStepping = True

from ISF_Config.ISF_jobProperties import ISF_Flags
from AthenaCommon.Resilience import protectedInclude
protectedInclude("SimulationJobOptions/preInclude.BeamPipeKill.py")
if "FullG4" in ISF_Flags.Simulator():
    protectedInclude("SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py")
