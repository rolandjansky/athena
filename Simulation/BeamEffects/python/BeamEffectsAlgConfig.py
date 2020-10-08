#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""Define methods to configure beam effects with the ComponentAccumulator"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
# Compiled beam effects methods
# for documentation of method X, see Simulation__X._propertyDocDct
Simulation__GenEventValidityChecker=CompFactory.Simulation.GenEventValidityChecker
Simulation__GenEventRotator=CompFactory.Simulation.GenEventRotator
Simulation__GenEventVertexPositioner=CompFactory.Simulation.GenEventVertexPositioner
Simulation__VertexBeamCondPositioner=CompFactory.Simulation.VertexBeamCondPositioner
Simulation__VertexPositionFromFile=CompFactory.Simulation.VertexPositionFromFile
Simulation__CrabKissingVertexPositioner=CompFactory.Simulation.CrabKissingVertexPositioner
Simulation__LongBeamspotVertexPositioner=CompFactory.Simulation.LongBeamspotVertexPositioner
Simulation__GenEventBeamEffectBooster=CompFactory.Simulation.GenEventBeamEffectBooster
# For the Algorithm
Simulation__BeamEffectsAlg=CompFactory.Simulation.BeamEffectsAlg


# possible components from BeamEffectsConf
# todo names required to copy function name? what are names used for?
# todo add default construction options to make these potentiall useful
# todo verify and add suggestions made in todo


## GenEventManipulators
def makeValidityChecker(name="GenEventValidityChecker", **kwargs):
    """Return a validity checker tool"""
    return Simulation__GenEventValidityChecker(name, **kwargs)


def makeGenEventRotator(name="GenEventRotator", **kwargs):
    """Return a event rotator tool"""
    return Simulation__GenEventRotator(name, **kwargs)


def makeGenEventBeamEffectBooster(name="GenEventBeamEffectBooster", **kwargs):
    """Return a lorentz booster tool"""
    # todo needs random seed, more?
    return Simulation__GenEventBeamEffectBooster(name, **kwargs)


def makeGenEventVertexPositioner(ConfigFlags,name="GenEventVertexPositioner", **kwargs):
    """Return a vertex positioner tool"""
    # todo needs input file(s?)

    acc = ComponentAccumulator()

    readVtxPosFromFile = ConfigFlags.Sim.Vertex.Source == "VertexOverrideFile.txt" or ConfigFlags.Sim.Vertex.Source == "VertexOverrideEventFile.txt"
    if readVtxPosFromFile:
        kwargs.setdefault("VertexShifters", [makeVertexPositionFromFile()])
    elif ConfigFlags.Sim.Vertex.Source == "CondDB" :
        tool = acc.popToolsAndMerge(makeVertexBeamCondPositioner(ConfigFlags))
        kwargs.setdefault("VertexShifters", [tool])
    elif ConfigFlags.Sim.Vertex.Source == "LongBeamspotVertexPositioner":
        kwargs.setdefault("VertexShifters", [makeLongBeamspotVertexPositioner()])

    acc.setPrivateTools(Simulation__GenEventVertexPositioner(name, **kwargs))
    return acc


## LorentzVectorGenerators
def makeVertexBeamCondPositioner(ConfigFlags,name="VertexBeamCondPositioner", **kwargs):
    """Return a conditional (? todo) vertex positioner tool"""
    from RngComps.RandomServices import RNG

    acc = ComponentAccumulator()
    
    acc.merge(RNG(engine=ConfigFlags.Random.Engine, name="AthRNGSvc"))
    kwargs.setdefault('RandomSvc', acc.getService("AthRNGSvc"))

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    acc.merge(BeamSpotCondAlgCfg(ConfigFlags))

    acc.setPrivateTools(Simulation__VertexBeamCondPositioner(name, **kwargs))
    return acc


def makeVertexPositionFromFile(name="VertexPositionFromFile", **kwargs):
    """Return a vertex positioner tool"""
    # todo input file? look at cxx for details
    return Simulation__VertexPositionFromFile(name, **kwargs)


def makeCrabKissingVertexPositioner(name="CrabKissingVertexPositioner", **kwargs):
    """Return a Crab-Kissing vertex positioner tool"""
    # todo needs BunchLength, RandomSvc, BunchShape
    return Simulation__CrabKissingVertexPositioner(name, **kwargs)


def makeLongBeamspotVertexPositioner(name="LongBeamspotVertexPositioner", **kwargs):
    """Return a long beamspot vertex positioner tool"""
    # todo needs LParameter and RandomSvc
    return Simulation__LongBeamspotVertexPositioner(name, **kwargs)


def BeamEffectsAlgBasicCfg(ConfigFlags, **kwargs):
    """Return an accumulator and algorithm for beam effects, wihout output"""
    acc = ComponentAccumulator()
    alg = Simulation__BeamEffectsAlg(name="BeamEffectsAlg", **kwargs)

    # Set default properties
    alg.ISFRun = ConfigFlags.Sim.ISFRun
    alg.InputMcEventCollection = "GEN_EVENT"
    alg.OutputMcEventCollection = "BeamTruthEvent"

    toolVertexPositioner = acc.popToolsAndMerge(makeGenEventVertexPositioner(ConfigFlags))

     # Set (todo) the appropriate manipulator tools
    manipulators = []
    manipulators.append(makeValidityChecker())
    manipulators.append(toolVertexPositioner) 
    # manipulators.append(makeGenEventBeamEffectBooster()) # todo segmentation violation
    # manipulators.append(makeVertexPositionFromFile()) # todo
    # manipulators.append(makeCrabKissingVertexPositioner()) # todo Callback registration failed
    # manipulators.append(makeLongBeamspotVertexPositioner()) # todo Callback registration failed
    alg.GenEventManipulators += manipulators

    acc.addEventAlgo(alg, sequenceName="AthAlgSeq", primary=True)
    return acc


def BeamEffectsAlgCfg(ConfigFlags, **kwargs):
    """Return an accumulator and algorithm for beam effects, with output"""
    acc = BeamEffectsAlgBasicCfg(ConfigFlags, **kwargs)
    # Set to write HITS pool file
    alg = acc.getPrimary()
    ItemList = ["McEventCollection#" + alg.OutputMcEventCollection]
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(ConfigFlags, "HITS", ItemList=ItemList, disableEventTag=True))
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
    inputDir = os.environ.get ('ATLAS_REFERENCE_DATA',
                                '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art')
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
    cfg.popToolsAndMerge(BeamEffectsAlgCfg(ConfigFlags))

    cfg.getService("StoreGateSvc").Dump=True
    cfg.printConfig(withDetails=True)
    ConfigFlags.dump()

    # Run it in athena
    cfg.run(maxEvents = 20)

    # Store in a pickle file
    with open("BeamEffectsAlg.pkl", "wb") as f:
        cfg.store(f)
    
