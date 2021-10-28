# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from G4AtlasApps.SimFlags import simFlags
simFlags.PhysicsList = "FTFP_BERT_ATL"
simFlags.TruthStrategy = "MC15aPlus"

simFlags.RunNumber = 330000

simFlags.TRTRangeCut = 30.0
simFlags.TightMuonStepping = True

from AthenaCommon.Resilience import protectedInclude
protectedInclude("SimulationJobOptions/preInclude.BeamPipeKill.py")
protectedInclude("SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py")
# enable G4 optimisations
protectedInclude("SimulationJobOptions/preInclude.G4Optimizations.py")
