# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from G4AtlasApps.SimFlags import simFlags
simFlags.PhysicsList = "FTFP_BERT_ATL"
simFlags.TruthStrategy = "MC15aPlus"

simFlags.RunNumber = 410000

simFlags.TRTRangeCut = 30.0
simFlags.TightMuonStepping = True

from AthenaCommon.Resilience import protectedInclude
protectedInclude("RunDependentSimData/configLumi_simProfile_run410000_mc21a_MultiBeamspot.py")

from ISF_Config.ISF_jobProperties import ISF_Flags
protectedInclude("SimulationJobOptions/preInclude.BeamPipeKill.py")
if "ATLFAST" in ISF_Flags.Simulator() or "G4FastCalo" in ISF_Flags.Simulator():
    from IOVDbSvc.CondDB import conddb
    conddb.addOverride("/TILE/OFL02/CALIB/SFR","TileOfl02CalibSfr-SIM-07")
if "FullG4" in ISF_Flags.Simulator():
    protectedInclude("SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py")

# enable G4 optimisations
protectedInclude("SimulationJobOptions/preInclude.G4Optimizations.py")
