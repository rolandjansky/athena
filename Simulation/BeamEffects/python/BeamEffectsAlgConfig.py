#!/usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define methods to configure beam effects with the ComponentAccumulator"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
# Compiled beam effects methods
# for documentation of method X, see Simulation__X._propertyDocDct
from BeamEffects.BeamEffectsConf import Simulation__GenEventValidityChecker
from BeamEffects.BeamEffectsConf import Simulation__GenEventRotator
from BeamEffects.BeamEffectsConf import Simulation__GenEventBeamEffectBooster
from BeamEffects.BeamEffectsConf import Simulation__GenEventVertexPositioner
from BeamEffects.BeamEffectsConf import Simulation__VertexBeamCondPositioner
from BeamEffects.BeamEffectsConf import Simulation__VertexPositionFromFile
from BeamEffects.BeamEffectsConf import Simulation__CrabKissingVertexPositioner
from BeamEffects.BeamEffectsConf import Simulation__LongBeamspotVertexPositioner
# For the Algorithm
from BeamEffects.BeamEffectsConf import Simulation__BeamEffectsAlg


# possible components from BeamEffectsConf
# todo names required to copy function name? what are names used for?
# todo add default construction options to make these potentiall useful
# todo verify and add suggestions made in todo
#--------------------------------------------------------------------------------------------------
## GenEventManipulators
def makeValidityChecker(name="GenEventValidityChecker", **kwargs):
    """Return a validity checker tool"""
    return Simulation__GenEventValidityChecker(name, **kwargs)

def makeGenEventRotator(name="GenEventRotator", **kwargs):
    """Return a vertex positioner tool"""
    return Simulation__GenEventRotator(name, **kwargs)

def makeGenEventBeamEffectBooster(name="GenEventBeamEffectBooster", **kwargs):
    """Return a lorentz booster tool"""
    # todo needs random seed, more?
    return Simulation__GenEventVertexPositioner(name, **kwargs)

def makeGenEventVertexPositioner(ConfigFlags,name="GenEventVertexPositioner", **kwargs):
    """Return a vertex positioner tool"""
    # todo needs input file(s?)
    #from BeamEffectsConfigFlagsTest import ConfigFlags #or have this as an argument?

    result=ComponentAccumulator()

    readVtxPosFromFile = ConfigFlags.Vertex.Source == "VertexOverrideFile.txt" or ConfigFlags.Vertex.Source == "VertexOverrideEventFile.txt"
    if readVtxPosFromFile:
        kwargs.setdefault("VertexShifters"          , [ Simulation__VertexPositionFromFile("VertexPositionFromFile") ])
    elif ConfigFlags.Vertex.Source == "CondDB" :
        acc, tool = makeVertexBeamCondPositioner(ConfigFlags) 
        result.merge(acc)
        kwargs.setdefault("VertexShifters"          , [ Simulation__VertexBeamCondPositioner('VertexBeamCondPositioner') ])
    elif ConfigFlags.Vertex.Source == "LongBeamspotVertexPositioner":
        kwargs.setdefault("VertexShifters"          , [ Simulation__LongBeamspotVertexPositioner('LongBeamspotVertexPositioner') ])

    
    return result, Simulation__GenEventVertexPositioner(name, **kwargs)

#--------------------------------------------------------------------------------------------------
## LorentzVectorGenerators
def makeVertexBeamCondPositioner(ConfigFlags,name="VertexBeamCondPositioner", **kwargs):
    """Return a conditional (? todo) vertex positioner tool"""
    # todo needs RandomSvc
    #from BeamEffectsConfigFlagsTest import ConfigFlags
    from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
    from BeamSpotConditions.BeamSpotConditionsConf import BeamSpotCondAlg
    from RngComps.RandomServices import RNG, AthEngines

    result = ComponentAccumulator()
    
    Engine = ConfigFlags.Random.Engine
    kwargs.setdefault('RandomSvc', AthEngines[Engine])
    
    
    #from AthenaCommon.Constants import VERBOSE
    #kwargs.setdefault('OutputLevel', VERBOSE) #if we wish to add verbose output to the tool
    
    result.merge(addFoldersSplitOnline(ConfigFlags,"INDET","/Indet/Onl/Beampos","/Indet/Beampos", className='AthenaAttributeList'))
    result.addCondAlgo(BeamSpotCondAlg( "BeamSpotCondAlg"))
    return result, Simulation__VertexBeamCondPositioner(name, **kwargs)

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

#----------------------------------------------------------------------------------------------------
def BeamEffectsAlgCfg(ConfigFlags, **kwargs):
    """Return an accumulator and algorithm for beam effects
    
    Arguments:
    ConfigFlags ---
    """
    acc = ComponentAccumulator()
    alg = Simulation__BeamEffectsAlg(name="BeamEffectsAlg", **kwargs)

    # Set default properties
    alg.ISFRun = False 
    alg.InputMcEventCollection = "GEN_EVENT"
    alg.OutputMcEventCollection = "BeamTruthEvent"

    accVertexPositioner, toolVertexPositioner = makeGenEventVertexPositioner(ConfigFlags)

     # Set (todo) the appropriate manipulator tools
    manipulators = []
    manipulators.append(makeValidityChecker())
    manipulators.append(makeGenEventRotator())
    manipulators.append(toolVertexPositioner) 
    # manipulators.append(makeGenEventBeamEffectBooster()) # todo segmentation violation
    # manipulators.append(makeVertexPositionFromFile()) # todo
    # manipulators.append(makeCrabKissingVertexPositioner()) # todo Callback registration failed
    # manipulators.append(makeLongBeamspotVertexPositioner()) # todo Callback registration failed
    alg.GenEventManipulators += manipulators

    #merge the accumulators
    acc.merge(accVertexPositioner)

    return acc, alg

if __name__ == "__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg


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
    ConfigFlags.Output.HITFileName = "myHITS.pool.root" #todo change the flag name in AllConfigFlags

    #set the source of vertex positioning
    #ConfigFlags.Vertex.Source = "VertexOverrideFile.txt"# Vertex.OverrideFile/Vertex.OverrideEventFile
    ConfigFlags.Vertex.Source = "CondDB" # Vertex.FromCondD
    #ConfigFlags.Vertex.Source = "LongBeamspotVertexPositioner"

    #included to stop segmentation error - TODO see why it's failing
    ConfigFlags.Input.isMC = True
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-14" #conditions tag for conddb (which one to use - old one for simulation)
    ConfigFlags.Input.RunNumber = 284500 # run test job with and without run number and 222510

    # Finalize 
    ConfigFlags.lock()

    ## Initialize a new component accumulator
    cfg = MainServicesSerialCfg() #use this syntax for storegate
    # Add configuration to read EVNT pool file
    cfg.merge(PoolReadCfg(ConfigFlags))

    # Make use of our defiend function
    acc, alg = BeamEffectsAlgCfg(ConfigFlags)
    cfg.merge(acc)

    # Add the algorithm into our accumulator
    cfg.addEventAlgo(alg, sequenceName="AthAlgSeq")

    # Add configuration to write HITS pool file
    cfg.merge( OutputStreamCfg(ConfigFlags,
     "HITS", 
     ItemList=["McEventCollection#" + alg.OutputMcEventCollection])) #which collection in storegate gets written to output file !!!!!!!!


    cfg.getService("StoreGateSvc").Dump=True
    cfg.printConfig(withDetails=True)
    ConfigFlags.dump()

    # Run it in athena
    cfg.run(maxEvents = 20)

    # Store in a pickle file
    with open("BeamEffectsAlg.pkl", "w") as f:
        cfg.store(f)
    
