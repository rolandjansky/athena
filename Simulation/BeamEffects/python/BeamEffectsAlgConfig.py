#!/usr/bin/env python

# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""Define methods to configure beam effects with the ComponentAccumulator"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType, ProductionStep

# possible components from BeamEffectsConf
# todo names required to copy function name? what are names used for?
# todo add default construction options to make these potentiall useful
# todo verify and add suggestions made in todo


## GenEventManipulators
def ValidityCheckerCfg(flags, name="GenEventValidityChecker", **kwargs):
    """Return a validity checker tool"""
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Simulation.GenEventValidityChecker(name, **kwargs))
    return acc


def GenEventRotatorCfg(flags, name="GenEventRotator", **kwargs):
    """Return a event rotator tool"""
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Simulation.GenEventRotator(name, **kwargs))
    return acc


def GenEventBeamEffectBoosterCfg(flags, name="GenEventBeamEffectBooster", **kwargs):
    """Return a lorentz booster tool"""
    # todo needs random seed, more?
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Simulation.GenEventBeamEffectBooster(name, **kwargs))
    return acc


def GenEventVertexPositionerCfg(flags, name="GenEventVertexPositioner", **kwargs):
    """Return a vertex positioner tool"""
    # todo needs input file(s?)

    acc = ComponentAccumulator()

    readVtxPosFromFile = flags.Sim.Vertex.Source == "VertexOverrideFile.txt" or flags.Sim.Vertex.Source == "VertexOverrideEventFile.txt"
    if readVtxPosFromFile:
        kwargs.setdefault("VertexShifters", [acc.popToolsAndMerge(VertexPositionFromFileCfg(flags))])
    elif flags.Sim.Vertex.Source == "CondDB":
        kwargs.setdefault("VertexShifters", [acc.popToolsAndMerge(VertexBeamCondPositionerCfg(flags))])
    elif flags.Sim.Vertex.Source == "LongBeamspotVertexPositioner":
        kwargs.setdefault("VertexShifters", [acc.popToolsAndMerge(LongBeamspotVertexPositionerCfg(flags))])

    acc.setPrivateTools(CompFactory.Simulation.GenEventVertexPositioner(name, **kwargs))
    return acc


## LorentzVectorGenerators
def VertexBeamCondPositionerCfg(flags, name="VertexBeamCondPositioner", **kwargs):
    """Return a conditional (? todo) vertex positioner tool"""
    from RngComps.RandomServices import AthRNGSvcCfg

    acc = ComponentAccumulator()

    kwargs.setdefault("RandomSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    acc.merge(BeamSpotCondAlgCfg(flags))

    acc.setPrivateTools(CompFactory.Simulation.VertexBeamCondPositioner(name, **kwargs))
    return acc


def VertexPositionFromFileCfg(flags, name="VertexPositionFromFile", **kwargs):
    """Return a vertex positioner tool"""
    # todo input file? look at cxx for details
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Simulation.VertexPositionFromFile(name, **kwargs))
    return acc


def CrabKissingVertexPositionerCfg(flags, name="CrabKissingVertexPositioner", **kwargs):
    """Return a Crab-Kissing vertex positioner tool"""
    # todo needs BunchLength, RandomSvc, BunchShape
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Simulation.CrabKissingVertexPositioner(name, **kwargs))
    return acc


def LongBeamspotVertexPositionerCfg(flags, name="LongBeamspotVertexPositioner", **kwargs):
    """Return a long beamspot vertex positioner tool"""
    # todo needs LParameter and RandomSvc
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Simulation.LongBeamspotVertexPositioner(name, **kwargs))
    return acc


def BeamEffectsAlgCfg(flags, name="BeamEffectsAlg", **kwargs):
    """Return an accumulator and algorithm for beam effects, wihout output"""
    acc = ComponentAccumulator()

    kwargs.setdefault("ISFRun", flags.Sim.ISFRun)

    # Set default properties
    if flags.Sim.DoFullChain and flags.Digitization.PileUp:
        kwargs.setdefault("InputMcEventCollection", "OriginalEvent_SG+GEN_EVENT")
    else:
        kwargs.setdefault("InputMcEventCollection", "GEN_EVENT")
    kwargs.setdefault("OutputMcEventCollection", "BeamTruthEvent")

    # Set (todo) the appropriate manipulator tools
    manipulators = []
    manipulators.append(acc.popToolsAndMerge(ValidityCheckerCfg(flags)))
    if flags.Beam.Type is not BeamType.Cosmics and flags.Sim.CavernBG != "Read":
        manipulators.append(acc.popToolsAndMerge(GenEventVertexPositionerCfg(flags)))
    # manipulators.append(acc.popToolsAndMerge(GenEventBeamEffectBoosterCfg(flags))) # todo segmentation violation
    # manipulators.append(acc.popToolsAndMerge(VertexPositionFromFileCfg(flags))) # todo
    # manipulators.append(acc.popToolsAndMerge(CrabKissingVertexPositionerCfg(flags))) # todo Callback registration failed
    # manipulators.append(acc.popToolsAndMerge(LongBeamspotVertexPositionerCfg(flags))) # todo Callback registration failed
    kwargs.setdefault("GenEventManipulators", manipulators)

    acc.addEventAlgo(CompFactory.Simulation.BeamEffectsAlg(name, **kwargs), primary=True)
    return acc


def BeamEffectsAlgOutputCfg(flags, **kwargs):
    """Return an accumulator and algorithm for beam effects, with output"""
    acc = BeamEffectsAlgCfg(flags, **kwargs)
    # Set to write HITS pool file
    alg = acc.getPrimary()
    ItemList = ["McEventCollection#" + alg.OutputMcEventCollection]
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags, "HITS", ItemList=ItemList, disableEventTag=True))
    return acc


def BeamSpotFixerAlgCfg(flags, name="BeamSpotFixerAlg", **kwargs):
    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    acc = BeamSpotCondAlgCfg(flags)

    kwargs.setdefault("InputKey", "Input_EventInfo")
    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("OutputKey", flags.Overlay.BkgPrefix + "EventInfo")
    else:
        kwargs.setdefault("OutputKey", "EventInfo")

    acc.addEventAlgo(CompFactory.Simulation.BeamSpotFixerAlg(name, **kwargs))
    return acc


def BeamSpotReweightingAlgCfg(flags, name="BeamSpotReweightingAlg", **kwargs):
    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    acc = BeamSpotCondAlgCfg(flags)

    kwargs.setdefault("Input_beam_sigma_z", flags.Digitization.InputBeamSigmaZ)

    acc.addEventAlgo(CompFactory.Simulation.BeamSpotReweightingAlg(name, **kwargs))
    return acc


if __name__ == "__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg


    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from SimulationFlags import ConfigFlagsSimulation

    ConfigFlags.join(ConfigFlagsSimulation)
    # Set up logging and config behaviour
    log.setLevel(DEBUG)
    Configurable.configurableRun3Behavior = 1

    import os
    inputDir = os.environ.get ("ATLAS_REFERENCE_DATA",
                               "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art")
     # Provide input
    ConfigFlags.Input.Files = [
         inputDir +
         "/SimCoreTests/e_E50_eta34_49.EVNT.pool.root"
         ]


    # Specify output
    ConfigFlags.Output.HITSFileName = "myHITS.pool.root"

    #set the source of vertex positioning
    #ConfigFlags.Sim.Vertex.Source = "VertexOverrideFile.txt"# Vertex.OverrideFile/Vertex.OverrideEventFile
    ConfigFlags.Sim.Vertex.Source = "CondDB" # Vertex.FromCondD
    #ConfigFlags.Sim.Vertex.Source = "LongBeamspotVertexPositioner"

    #included to stop segmentation error - TODO see why it's failing
    ConfigFlags.Input.isMC = True
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-14" #conditions tag for conddb (which one to use - old one for simulation)
    ConfigFlags.Input.RunNumber = [284500] # run test job with and without run number and 222510

    # Finalize
    ConfigFlags.lock()

    ## Initialize a new component accumulator
    cfg = MainServicesCfg(ConfigFlags) #use this syntax for storegate
    # Add configuration to read EVNT pool file
    cfg.merge(PoolReadCfg(ConfigFlags))

    # Make use of our defiend function
    cfg.merge(BeamEffectsAlgCfg(ConfigFlags))

    cfg.getService("StoreGateSvc").Dump=True
    cfg.printConfig(withDetails=True)
    ConfigFlags.dump()

    # Run it in athena
    cfg.run(maxEvents = 20)

    # Store in a pickle file
    with open("BeamEffectsAlg.pkl", "wb") as f:
        cfg.store(f)
