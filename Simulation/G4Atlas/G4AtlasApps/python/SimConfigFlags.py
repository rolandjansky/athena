# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaConfiguration.AutoConfigFlags import GetFileMD

#todo? add in the explanatory text from previous implementation

def createSimConfigFlags():
    scf=AthConfigFlags()

    scf.addFlag("Sim.ParticleID",False)
    scf.addFlag("Sim.CalibrationRun", "DeadLAr") # "LAr", "Tile", "LAr+Tile", "DeadLAr", "Off"

    scf.addFlag("Sim.CavernBG","Off") #"Write" , "Read" , "Signal" , "WriteWorld" , "SignalWorld"
    scf.addFlag("Sim.ReadTR",False)
    scf.addFlag("Sim.WorldRRange", False) #12500. #int or float
    scf.addFlag("Sim.WorldZRange", False) #22031. #int or float

    # the G4 offset. It was never changed, so no need to peek in file
    scf.addFlag("Sim.SimBarcodeOffset", 200000)

    #for forward region
    scf.addFlag("Sim.TwissFileBeam1",False)
    scf.addFlag("Sim.TwissFileBeam2",False)
    scf.addFlag("Sim.TwissEnergy",8000000.)
    scf.addFlag("Sim.TwissFileBeta",550.)
    scf.addFlag("Sim.TwissFileNomReal",False) #"nominal","real" #default to one of these?!
    scf.addFlag("Sim.TwissFileVersion","v01")

    #for G4AtlasAlg
    #in simflags
    scf.addFlag("Sim.ReleaseGeoModel",True)
    scf.addFlag("Sim.RecordFlux",False)
    scf.addFlag("Sim.TruthStrategy", "MC12")
    scf.addFlag("Sim.G4Commands",["/run/verbose 2"])
    #in atlasflags
    scf.addFlag("Sim.FlagAbortedEvents",False)
    scf.addFlag("Sim.KillAbortedEvents",True)
    scf.addFlag("Sim.IncludeParentsInG4Event",False)

    # Do full simulation + digitisation + reconstruction chain
    scf.addFlag("Sim.DoFullChain", False)

    def _check_G4_version(prevFlags):
        version = ''
        if len(prevFlags.Input.Files)>0:
            from AthenaConfiguration.AutoConfigFlags import GetFileMD
            version = GetFileMD(prevFlags.Input.Files).get('G4Version', '')
        if not version:
            from os import environ
            version = str(environ.get('G4VERS', ''))
        if not version:
            raise ValueError("Unknown G4 version")
        return version

    scf.addFlag("Sim.G4Version", lambda prevFlags : _check_G4_version(prevFlags))

    scf.addFlag("Sim.PhysicsList", "FTFP_BERT_ATL")
    scf.addFlag("Sim.NeutronTimeCut", 150.) # Sets the value for the neutron out of time cut in G4
    scf.addFlag("Sim.NeutronEnergyCut", -1.) # Sets the value for the neutron energy cut in G4
    scf.addFlag("Sim.ApplyEMCuts", False) # Turns on the G4 option to apply cuts for EM physics
    scf.addFlag("Sim.MuonFieldOnlyInCalo", False) # Only muons see the B-field in the calo

    #For G4AtlasToolsConfig
    scf.addFlag("Sim.RecordStepInfo",False)
    scf.addFlag("Sim.StoppedParticleFile", '')
    scf.addFlag("Sim.BeamPipeSimMode", "Normal")  ## ["Normal", "FastSim", "EGammaRangeCuts", "EGammaPRangeCuts"]
    scf.addFlag("Sim.LArParameterization", 0)  ## 0 = No frozen showers, 1 = Frozen Showers, 2 = DeadMaterial Frozen Showers
    # TRT Range cut used in simulation in mm. Should be 0.05 or 30.
    scf.addFlag("Sim.TRTRangeCut",
                  lambda prevFlags: float(GetFileMD(prevFlags.Input.Files).get('TRTRangeCut', 30.0)))

    #For BeameffectsAlg
    scf.addFlag("Sim.Vertex.Source", "CondDB" ) #"CondDB", "VertexOverrideEventFile.txt", "VertexOverride.txt","LongBeamspot"

    #for G4UserActions
    scf.addFlag("Sim.Layout","ATLAS-R2-2015-03-01-00")
    scf.addFlag("Sim.NRRThreshold", False)
    scf.addFlag("Sim.NRRWeight", False)
    scf.addFlag("Sim.PRRThreshold", False)
    scf.addFlag("Sim.PRRWeight", False)
    scf.addFlag("Sim.OptionalUserActionList", [])

    # For G4FieldConfigNew
    scf.addFlag("Sim.G4Stepper", "AtlasRK4")
    scf.addFlag("Sim.G4EquationOfMotion", "")
    scf.addFlag("Sim.UsingGeant4", True)

    # for cosmics
    #  volume(s) used to do cosmics filtering
    #  G4 volume names from {"Muon", "Calo", "InnerDetector", "TRT_Barrel", "TRT_EC", "SCT_Barrel", "Pixel"}
    scf.addFlag("Sim.CosmicFilterVolumeNames", ["InnerDetector"])
    scf.addFlag("Sim.CosmicFilterID", False) # PDG ID to be filtered ("13")
    scf.addFlag("Sim.CosmicFilterPTmin", False) # min pT filtered in cosmics processing (MeV) ("5000")
    scf.addFlag("Sim.CosmicFilterPTmax", False) # max pT filtered in cosmics processing (MeV) ("6000")
    scf.addFlag("Sim.CosmicPtSlice", 'Off') #slice1, 'slice2', 'slice3', 'slice4', 'NONE'

    # For ISF
    scf.addFlag("Sim.ISFRun",False)
    scf.addFlag("Sim.ISF.Simulator", "ATLFASTII")
    scf.addFlag("Sim.ISF.DoTimeMonitoring", True) # bool: run time monitoring
    scf.addFlag("Sim.ISF.DoMemoryMonitoring", True) # bool: run time monitoring
    scf.addFlag("Sim.ISF.ValidationMode", False) # bool: run ISF internal validation checks
    scf.addFlag("Sim.ISF.ReSimulation", False) # Using ReSimulation workflow
    scf.addFlag("Sim.ISF.UseTrackingGeometryCond", False) # Using Condition for tracking Geometry

    def decideHITSMerging(prevFlags):
        simstr = prevFlags.Sim.ISF.Simulator
        # Further specialization possible in future
        if simstr in ("FullG4MT", "FullG4MT_QS", "PassBackG4MT"):
            doID = False
            doITk = False
            doCALO = False
            doMUON = False
        elif simstr in ("ATLFASTIIF_G4MS", "ATLFASTIIFMT", "ATLFAST3F_G4MS"):
            doID = True
            doITk = True
            doCALO = True
            doMUON = True
        elif simstr in ("ATLFASTIIMT", "ATLFAST3MT", "ATLFAST3MT_QS"):
            doID = False
            doITk = False
            doCALO = True
            doMUON = False
        else:
            doID = True
            doITk = True
            doCALO = True
            doMUON = True
        return {"ID": doID, "CALO": doCALO, "MUON": doMUON, "ITk": doITk}

    scf.addFlag("Sim.ISF.HITSMergingRequired", decideHITSMerging)

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
        flags.Sim.TruthStrategy = runArgs.truthStrategy

    # Not used as deprecated
    # '--enableLooperKiller'
    # '--perfmon'
    # '--randomSeed'
    # '--useISF'
