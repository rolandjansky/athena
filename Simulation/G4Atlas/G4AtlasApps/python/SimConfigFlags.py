# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaConfiguration.AutoConfigFlags import GetFileMD
from G4AtlasApps.SimEnums import BeamPipeSimMode, CalibrationRun, CavernBackground, \
    LArParameterization, SimulationFlavour, TruthStrategy, VertexSource

#todo? add in the explanatory text from previous implementation

def createSimConfigFlags():
    scf=AthConfigFlags()

    scf.addFlag("Sim.ParticleID", False)
    scf.addFlag("Sim.CalibrationRun", CalibrationRun.DeadLAr, enum=CalibrationRun)

    scf.addFlag("Sim.CavernBackground", CavernBackground.Off, enum=CavernBackground)
    scf.addFlag("Sim.ReadTR", False)
    scf.addFlag("Sim.WorldRRange", False)  # 12500. / int or float
    scf.addFlag("Sim.WorldZRange", False)  # 22031. / int or float

    # the G4 offset. It was never changed, so no need to peek in file
    scf.addFlag("Sim.SimBarcodeOffset", 200000)

    # Forward region
    scf.addFlag("Sim.TwissFileBeam1", False)
    scf.addFlag("Sim.TwissFileBeam2", False)
    scf.addFlag("Sim.TwissEnergy", 8000000.)
    scf.addFlag("Sim.TwissFileBeta", 550.)
    scf.addFlag("Sim.TwissFileNomReal", False)  # "nominal", "real" / default to one of these?!
    scf.addFlag("Sim.TwissFileVersion", "v01")

    # G4AtlasAlg
    scf.addFlag("Sim.ReleaseGeoModel", True)
    scf.addFlag("Sim.RecordFlux", False)
    scf.addFlag("Sim.TruthStrategy", lambda prevFlags : TruthStrategy.Validation if prevFlags.Sim.ISF.ValidationMode else TruthStrategy.MC12,
                enum=TruthStrategy)
    scf.addFlag("Sim.G4Commands", ["/run/verbose 2"])
    scf.addFlag("Sim.FlagAbortedEvents", False)
    scf.addFlag("Sim.KillAbortedEvents", True)
    scf.addFlag("Sim.IncludeParentsInG4Event", False)

    # Do full simulation + digitisation + reconstruction chain
    scf.addFlag("Sim.DoFullChain", False)

    def _check_G4_version(prevFlags):
        version = ""
        if prevFlags.Input.Files:
            from AthenaConfiguration.AutoConfigFlags import GetFileMD
            version = GetFileMD(prevFlags.Input.Files).get("G4Version", "")
        if not version:
            from os import environ
            version = str(environ.get("G4VERS", ""))
        if not version:
            raise ValueError("Unknown G4 version")
        return version

    scf.addFlag("Sim.G4Version", _check_G4_version)

    scf.addFlag("Sim.PhysicsList", "FTFP_BERT_ATL")
    scf.addFlag("Sim.NeutronTimeCut", 150.) # Sets the value for the neutron out of time cut in G4
    scf.addFlag("Sim.NeutronEnergyCut", -1.) # Sets the value for the neutron energy cut in G4
    scf.addFlag("Sim.ApplyEMCuts", False) # Turns on the G4 option to apply cuts for EM physics
    scf.addFlag("Sim.MuonFieldOnlyInCalo", False) # Only muons see the B-field in the calo

    # G4AtlasToolsConfig
    scf.addFlag("Sim.RecordStepInfo", False)
    scf.addFlag("Sim.StoppedParticleFile", "")
    scf.addFlag("Sim.BeamPipeSimMode", BeamPipeSimMode.Normal, enum=BeamPipeSimMode)
    scf.addFlag("Sim.LArParameterization", LArParameterization.NoFrozenShowers, enum=LArParameterization)
    # TRT Range cut used in simulation in mm. Should be 0.05 or 30.
    scf.addFlag("Sim.TRTRangeCut",
                lambda prevFlags: float(GetFileMD(prevFlags.Input.Files).get('TRTRangeCut', 30.0)))

    # BeameffectsAlg
    scf.addFlag("Sim.VertexSource", VertexSource.CondDB, enum=VertexSource)

    # G4UserActions
    scf.addFlag("Sim.NRRThreshold", False)
    scf.addFlag("Sim.NRRWeight", False)
    scf.addFlag("Sim.PRRThreshold", False)
    scf.addFlag("Sim.PRRWeight", False)
    scf.addFlag("Sim.OptionalUserActionList", [])

    # G4FieldConfig
    scf.addFlag("Sim.G4Stepper", "AtlasRK4")
    scf.addFlag("Sim.G4EquationOfMotion", "")
    scf.addFlag("Sim.UsingGeant4", True)

    # Cosmics
    #  volume(s) used to do cosmics filtering
    #  G4 volume names from {"Muon", "Calo", "InnerDetector", "TRT_Barrel", "TRT_EC", "SCT_Barrel", "Pixel"}
    scf.addFlag("Sim.CosmicFilterVolumeNames", ["InnerDetector"])
    scf.addFlag("Sim.CosmicFilterID", False) # PDG ID to be filtered ("13")
    scf.addFlag("Sim.CosmicFilterPTmin", False) # min pT filtered in cosmics processing (MeV) ("5000")
    scf.addFlag("Sim.CosmicFilterPTmax", False) # max pT filtered in cosmics processing (MeV) ("6000")
    scf.addFlag("Sim.CosmicPtSlice", "Off") # 'slice1', 'slice2', 'slice3', 'slice4', 'NONE'

    # ISF
    scf.addFlag("Sim.ISFRun", False)
    scf.addFlag("Sim.ISF.Simulator", SimulationFlavour.Unknown, enum=SimulationFlavour)
    scf.addFlag("Sim.ISF.DoTimeMonitoring", True) # bool: run time monitoring
    scf.addFlag("Sim.ISF.DoMemoryMonitoring", True) # bool: run time monitoring
    scf.addFlag("Sim.ISF.ValidationMode", False) # bool: run ISF internal validation checks
    scf.addFlag("Sim.ISF.ReSimulation", False) # Using ReSimulation workflow
    scf.addFlag("Sim.ISF.UseTrackingGeometryCond", False) # Using Condition for tracking Geometry

    def _decideHITSMerging(prevFlags):
        simulator = prevFlags.Sim.ISF.Simulator
        # Further specialization possible in future
        if simulator in [SimulationFlavour.FullG4MT, SimulationFlavour.FullG4MT_QS, SimulationFlavour.PassBackG4MT]:
            doID = False
            doITk = False
            doCALO = False
            doMUON = False
        elif simulator in [SimulationFlavour.ATLFASTIIF_G4MS, SimulationFlavour.ATLFASTIIFMT, SimulationFlavour.ATLFAST3F_G4MS]:
            doID = True
            doITk = True
            doCALO = True
            doMUON = True
        elif simulator in [SimulationFlavour.ATLFASTIIMT, SimulationFlavour.ATLFAST3MT, SimulationFlavour.ATLFAST3MT_QS]:
            doID = False
            doITk = False
            doCALO = True
            doMUON = False
        elif simulator in [SimulationFlavour.Unknown]:
            doID = True
            doITk = True
            doCALO = True
            doMUON = True
        else:
            raise ValueError("Invalid simulator")
        return {"ID": doID, "CALO": doCALO, "MUON": doMUON, "ITk": doITk}

    scf.addFlag("Sim.ISF.HITSMergingRequired", _decideHITSMerging)

    scf.addFlag("Sim.FastCalo.ParamsInputFilename", "FastCaloSim/MC16/TFCSparam_run2_reprocessing.root") # filename of the input parametrizations file
    scf.addFlag("Sim.FastCalo.CaloCellsName", "AllCalo") # StoreGate collection name for FastCaloSim hits

    scf.addFlag("Sim.FastShower.InputCollection", "TruthEvent") # StoreGate collection name of modified TruthEvent for legayc FastCaloSim use

    # FastChain
    # Setting the BCID for Out-of-Time PU events, list of int
    scf.addFlag("Sim.FastChain.BCID", [1])
    # weights for Out-of-Time PU events
    scf.addFlag("Sim.FastChain.PUWeights_lar_em", [1.0]) # LAr EM
    scf.addFlag("Sim.FastChain.PUWeights_lar_hec", [1.0]) # LAr HEC
    scf.addFlag("Sim.FastChain.PUWeights_lar_bapre", [1.0]) # LAr Barrel presampler
    scf.addFlag("Sim.FastChain.PUWeights_tile", [1.0]) # Tile

    # Fatras
    scf.addFlag("Sim.Fatras.RandomStreamName", "FatrasRnd")
    scf.addFlag("Sim.Fatras.G4RandomStreamName", "FatrasG4")
    scf.addFlag("Sim.Fatras.TrkExRandomStreamName", "TrkExRnd")
    # Fatras fine tuning
    scf.addFlag("Sim.Fatras.MomCutOffSec", 50.) # common momentum cut-off for secondaries
    scf.addFlag("Sim.Fatras.HadronIntProb", 1.) # hadronic interaction scale factor
    scf.addFlag("Sim.Fatras.GaussianMixtureModel", True) # use Gaussian mixture model for Multiple Scattering
    scf.addFlag("Sim.Fatras.BetheHeitlerScale", 1.) # scale to Bethe-Heitler contribution

    scf.addFlag("Sim.BeamPipeCut", 100.0)
    scf.addFlag("Sim.TightMuonStepping", False)

    return scf


def simulationRunArgsToFlags(runArgs, flags):
    """Fill simulation configuration flags from run arguments."""
    if hasattr(runArgs, "DataRunNumber"):
        flags.Input.RunNumber = [runArgs.DataRunNumber]
        flags.Input.OverrideRunNumber = True
        flags.Input.LumiBlockNumber = [1] # dummy value

    if hasattr(runArgs, "physicsList"):
        flags.Sim.PhysicsList = runArgs.physicsList

    if hasattr(runArgs, "truthStrategy"):
        flags.Sim.TruthStrategy = TruthStrategy(runArgs.truthStrategy)

    # Not used as deprecated
    # '--enableLooperKiller'
    # '--perfmon'
    # '--randomSeed'
    # '--useISF'
