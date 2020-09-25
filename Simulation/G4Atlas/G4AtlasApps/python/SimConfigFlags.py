# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

#todo? add in the explanatory text from previous implementation

def createSimConfigFlags():
    scf=AthConfigFlags()

    scf.addFlag("Sim.ParticleID",False)
    scf.addFlag("Sim.CalibrationRun", "DeadLAr") # "LAr", "Tile", "LAr+Tile", "DeadLAr", "Off"

    scf.addFlag("Sim.CavernBG",False) #"Write" , "Read" , "Signal" , "WriteWorld" , "SignalWorld"
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

    scf.addFlag("Sim.G4Version", "geant4.10.1.patch03.atlas02")
    scf.addFlag("Sim.PhysicsList", "FTFP_BERT_ATL")
    scf.addFlag("Sim.NeutronTimeCut", 150.) # Sets the value for the neutron out of time cut in G4
    scf.addFlag("Sim.NeutronEnergyCut", -1.) # Sets the value for the neutron energy cut in G4
    scf.addFlag("Sim.ApplyEMCuts", False) # Turns on the G4 option to apply cuts for EM physics

    #For G4AtlasToolsConfig
    scf.addFlag("Sim.RecordStepInfo",False)
    scf.addFlag("Sim.StoppedParticleFile", False)
    scf.addFlag("Sim.BeamPipeSimMode", "Normal")  ## ["Normal", "FastSim", "EGammaRangeCuts", "EGammaPRangeCuts"]
    scf.addFlag("Sim.LArParameterization", 2)  ## 0 = No frozen showers, 1 = Frozen Showers, 2 = DeadMaterial Frozen Showers

    #For BeameffectsAlg
    scf.addFlag("Sim.Vertex.Source", "CondDB" ) #"CondDB", "VertexOverrideEventFile.txt", "VertexOverride.txt","LongBeamspot"

    #for G4UserActions
    scf.addFlag("Sim.Layout","ATLAS-R2-2015-03-01-00")
    scf.addFlag("Sim.NRRThreshold", False)
    scf.addFlag("Sim.NRRWeight", False)
    scf.addFlag("Sim.PRRThreshold", False)
    scf.addFlag("Sim.PRRWeight", False)

    # For G4FieldConfigNew
    scf.addFlag("Sim.G4Stepper", "AtlasRK4")
    scf.addFlag("Sim.G4EquationOfMotion", "")
    scf.addFlag("Sim.UsingGeant4", True)

    # for cosmics
    #  volume(s) used to do cosmics filtering
    #  G4 volume names from {"Muon", "Calo", "InnerDetector", "TRT_Barrel", "TRT_EC", "SCT_Barrel", "Pixel"}
    scf.addFlag("Sim.CosmicFilterVolumeNames", ["InnerDetector"])
    scf.addFlag("Sim.CosmicFilterID", "13") # PDG ID to be filtered
    scf.addFlag("Sim.CosmicFilterPTmin", "5000") # min pT filtered in cosmics processing (MeV)
    scf.addFlag("Sim.CosmicFilterPTmax", "6000") # max pT filtered in cosmics processing (MeV)

    # For ISF
    scf.addFlag("Sim.ISFRun",False)
    scf.addFlag("Sim.ISF.Simulator", "ATLFASTII")
    scf.addFlag("Sim.ISF.DoTimeMonitoring", True) # bool: run time monitoring
    scf.addFlag("Sim.ISF.DoMemoryMonitoring", True) # bool: run time monitoring
    scf.addFlag("Sim.ISF.ValidationMode", False) # bool: run ISF internal validation checks
    
    def decideHITSMerging(prevFlags):
        simstr = prevFlags.Sim.ISF.Simulator
        if simstr.endswith("MT"):
            simstr = simstr[:-2]
        # Further specialization possible in future
        if simstr in ("FullG4", "PassBackG4"):
            doID = False
            doCALO = False
            doMUON = False
        elif simstr in ("ATLFASTII", "G4FastCalo"):
            doID = False
            doCALO = True
            doMUON = False
        else:
            doID = True
            doCALO = True
            doMUON = True
        return {"ID": doID, "CALO": doCALO, "MUON": doMUON}

    scf.addFlag("Sim.ISF.HITSMergingRequired", decideHITSMerging)

    scf.addFlag("Sim.FastCalo.ParamsInputFilename", "FastCaloSim/MC16/TFCSparam_v011.root") # filename of the input parametrizations file
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

    # Run dependent simulation
    # map from runNumber to timestamp; migrated from RunDMCFlags.py
    def getRunToTimestampDict():
        # this wrapper is intended to avoid an initial import
        from G4AtlasApps.RunToTimestampData import RunToTimestampDict
        return RunToTimestampDict
    scf.addFlag("Sim.RunToTimestampDict", lambda prevFlags: getRunToTimestampDict())

    return scf
