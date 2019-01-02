#!/usr/bin/env python

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

def makeGenEventVertexPositioner(name="GenEventVertexPositioner", **kwargs):
    """Return a vertex positioner tool"""
    # todo needs input file(s?)
    return Simulation__GenEventVertexPositioner(name, **kwargs)

#--------------------------------------------------------------------------------------------------
## LorentzVectorGenerators
def makeVertexBeamCondPositioner(name="VertexBeamCondPositioner", **kwargs):
    """Return a conditional (? todo) vertex positioner tool"""
    # todo needs RandomSvc
    return Simulation__VertexBeamCondPositioner(name, **kwargs)

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
def BeamEffectsAlgCfg(configFlags, **kwargs):
    """Return an accumulator and algorithm for beam effects
    
    Arguments:
    configFlags --- todo not used
    """
    acc = ComponentAccumulator()
    alg = Simulation__BeamEffectsAlg(name="BeamEffectsAlg", **kwargs)

    # Set default properties (todo - check if we can remove ISFRun and InputMCEventCollection?)
    alg.ISFRun = False
    alg.InputMcEventCollection = "GEN_EVENT"
    alg.OutputMcEventCollection = "BeamTruthEvent"

     # Set (todo) the appropriate manipulator tools
    manipulators = []
    manipulators.append(makeValidityChecker())
    manipulators.append(makeGenEventRotator())
    manipulators.append(makeGenEventVertexPositioner())
    # manipulators.append(makeGenEventBeamEffectBooster()) # todo segmentation violation
    # manipulators.append(makeVertexBeamCondPositioner()) # todo Callback registration failed
    # manipulators.append(makeVertexPositionFromFile()) # todo
    # manipulators.append(makeCrabKissingVertexPositioner()) # todo Callback registration failed
    # manipulators.append(makeLongBeamspotVertexPositioner()) # todo Callback registration failed
    alg.GenEventManipulators += manipulators
    return acc, alg

if __name__ == "__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

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
    ConfigFlags.Output.HITFileName = "myHITS.pool.root"
    # Finalize 
    ConfigFlags.lock()

    ## Initialize a new component accumulator
    cfg = MainServicesSerialCfg() #This syntax for storegate
    # Add configuration to read EVNT pool file
    cfg.merge(PoolReadCfg(ConfigFlags))

    # Make use of our defiend function
    acc, alg = BeamEffectsAlgCfg(ConfigFlags)
    cfg.merge(acc)

    # Add the algorithm into our accumulator
    cfg.addEventAlgo(alg, sequenceName="AthAlgSeq")

    # Add configuration to write HITS pool file
    outConfig = OutputStreamCfg(ConfigFlags, "HITS", ItemList=["McEventCollection#" + alg.OutputMcEventCollection])
    cfg.merge (outConfig)

    cfg.getService("StoreGateSvc").Dump=True
    cfg.printConfig(withDetails=True)
    ConfigFlags.dump()

    # Run it in athena
    cfg.run(maxEvents = 20)

    # Store in a pickle file
    with open("BeamEffectsAlg.pkl", "w") as f:
        cfg.store(f)
    
