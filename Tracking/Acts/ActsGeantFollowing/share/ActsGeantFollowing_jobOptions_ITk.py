#==============================================================
#
#
#		This job option runs the G4 simulation
#		of the ATLAS detector and the GeantFollower in ID (and MS)
#		It can be run using athena.py
#
#==============================================================


from __future__ import print_function

import sys
from argparse import ArgumentParser

from AthenaCommon.Configurable import Configurable
from AthenaCommon.Constants import INFO
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from ActsGeometry.ActsGeometryConfig import ActsExtrapolationToolCfg

# Set up logging and new style config
Configurable.configurableRun3Behavior = True

def defaultTestFlags(configFlags, args):


    ## Just enable ID for the moment.
    ConfigFlags.Input.isMC             = True
    ConfigFlags.GeoModel.useLocalGeometry = False
    detectors = [
      "ITkPixel",
      "ITkStrip",
      "Bpipe"
    ]
 
    from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
    setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)


    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-P2-ITK-24-00-00"
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-SIM-00-00-00"
    ConfigFlags.GeoModel.Align.Dynamic = False
    #ConfigFlags.Acts.TrackingGeometry.MaterialSource = "Input"
    # ConfigFlags.Acts.TrackingGeometry.MaterialSource = "material-maps.json"

    ConfigFlags.Detector.GeometryCalo = False
    ConfigFlags.Detector.GeometryMuon = False

    # # This should run serially for the moment.
    # ConfigFlags.Concurrency.NumThreads = 1
    ConfigFlags.Concurrency.NumConcurrentEvents = 1

    configFlags.Input.Files = [args.inputevntfile]
    
    configFlags.Output.HITSFileName = args.outputhitsfile

    from G4AtlasApps.SimEnums import BeamPipeSimMode, CalibrationRun, CavernBackground
    configFlags.Sim.CalibrationRun = CalibrationRun.Off
    configFlags.Sim.RecordStepInfo = False
    configFlags.Sim.CavernBackground = CavernBackground.Signal
    configFlags.Sim.ISFRun = False
    configFlags.Sim.BeamPipeSimMode = BeamPipeSimMode.FastSim
    configFlags.Sim.ReleaseGeoModel = False


    configFlags.Input.RunNumber = [284500]
    configFlags.Input.OverrideRunNumber = True
    configFlags.Input.LumiBlockNumber = [1]

def printAndRun(accessor, configFlags, args):
    """debugging and execution"""
    # Dump config
    if args.verboseAccumulators:
        accessor.printConfig(withDetails=True)
    if args.verboseStoreGate:
        accessor.getService("StoreGateSvc").Dump = True
    configFlags.dump()

    # Execute and finish
    sc = accessor.run(maxEvents=args.maxEvents)

    # Dump config summary
    accessor.printConfig(withDetails=False)

    # Success should be 0
    return not sc.isSuccess()


def ITkCfg(configFlags):
    acc = MainServicesCfg(configFlags)
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    acc.merge(PoolReadCfg(configFlags))
    acc.merge(PoolWriteCfg(configFlags))

    # add BeamEffectsAlg
    from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
    acc.merge(BeamEffectsAlgCfg(configFlags))

    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    itkPixel = ITkPixelReadoutGeometryCfg(ConfigFlags)
    acc.merge(itkPixel)

    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    itkStrip = ITkStripReadoutGeometryCfg(ConfigFlags)
    acc.merge(itkStrip)

    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    acc.merge(BeamPipeGeometryCfg(ConfigFlags))

    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    gmsAcc = GeoModelCfg(configFlags)
    acc.merge(gmsAcc)

    return acc

def ActsGeantFollowerCfg(configFlags, name="ActsGeantFollowerTool", **kwargs):
    
    result = ComponentAccumulator()

    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    result.merge(TrackingGeometrySvcCfg(configFlags))

    from ActsGeometry.ActsGeometryConfig import NominalAlignmentCondAlgCfg
    nomAli = NominalAlignmentCondAlgCfg(configFlags, OutputLevel=INFO)
    result.merge(nomAli)

    from ActsGeometry.ActsGeometryConfig import ActsTrackingGeometrySvcCfg
    tgSvc = ActsTrackingGeometrySvcCfg(configFlags, OutputLevel=INFO)
    result.merge(tgSvc)

    print('DEF WRITER : ')
    Actsextrapol = ActsExtrapolationToolCfg(ConfigFlags,
                                        InteractionMultiScatering = True,
                                        InteractionEloss = True,
                                        InteractionRecord=True,
                                        OutputLevel=INFO)                                 
    result.merge(Actsextrapol)

    from TrkConfig.AtlasExtrapolationEngineConfig import AtlasExtrapolationEngineCfg
    extrapAcc = AtlasExtrapolationEngineCfg(configFlags)
    AtlasExtrapolationEngine = extrapAcc.getPrimary()
    result.merge(extrapAcc)


    #Setup Helper
    followingHelper = CompFactory.ActsGeantFollowerHelper("ActsGeantFollowerHelper",
                                                          **kwargs,
                                                          ExtrapolationEngine=AtlasExtrapolationEngine,
                                                          ActsExtrapolator=Actsextrapol.getPrimary(),
                                                          ExtrapolateDirectly=False,
                                                          ExtrapolateIncrementally=True,
                                                          OutputLevel=INFO)
    result.addPublicTool(followingHelper)

    #Setting up the CA for the ActsGeantFollower
    from ActsGeantFollowing.ActsGeantFollowingConfig import ActsGeantFollowerToolCfg
    actionAcc = ComponentAccumulator()
    actions = []
    actions += [actionAcc.popToolsAndMerge(ActsGeantFollowerToolCfg(configFlags))]
    actionAcc.setPrivateTools(actions)
    ActsGeantFollowerAction = result.popToolsAndMerge(actionAcc)
    
    #Retrieving the default action list
    from G4AtlasServices.G4AtlasUserActionConfigNew import getDefaultActions
    defaultActions = result.popToolsAndMerge(getDefaultActions(configFlags))

    #Adding LengthIntegrator to defaults
    actionList = (defaultActions + ActsGeantFollowerAction)

    #Setting up UserActionsService
    kwargs.setdefault("UserActionTools",actionList)
    result.addService(CompFactory.G4UA.UserActionSvc(name, **kwargs))

    return result


# Argument parsing
parser = ArgumentParser("ActsGeantFollowing_jobOption_ITk.py")
parser.add_argument("--simulate", default=True, action="store_true",
                    help="Run Simulation")
parser.add_argument("-V", "--verboseAccumulators", default=False, 
                    action="store_true",
                    help="Print full details of the AlgSequence")
parser.add_argument("-S", "--verboseStoreGate", default=False, 
                    action="store_true",
                    help="Dump the StoreGate(s) each event iteration")
parser.add_argument("--maxEvents",default=-1, type=int,
                    help="The number of events to run. 0 skips execution")
parser.add_argument("--inputevntfile",
                    # default="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/e_E50_eta0-25.evgen.pool.root",
                    default="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetSLHC_Example/inputs/pgun_2M_10GeV_geantinos_Eta6_v2_EVNT.root",
                    # default="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetSLHC_Example/inputs/EVNT.09244569._000001.pool.root.1",
                    help="The input EVNT file to use")
parser.add_argument("--outputhitsfile",default="myHITS.pool.root", type=str,
                    help="The output HITS filename")
args = parser.parse_args()

# Configure
defaultTestFlags(ConfigFlags, args)
ConfigFlags.lock()

# Construct our accumulator to run
acc = ITkCfg(ConfigFlags)
kwargs = {}

svcName = "ActsGeantFollowerTool"
acc.merge(ActsGeantFollowerCfg(ConfigFlags,svcName,**kwargs))
kwargs.update(UserActionSvc=svcName)

from G4AtlasAlg.G4AtlasAlgConfigNew import G4AtlasAlgCfg
acc.merge(G4AtlasAlgCfg(ConfigFlags, "ITkG4AtlasAlg", **kwargs))

# dump pickle
with open("ITkTest.pkl", "wb") as f:
    acc.store(f)

# Print and run
sys.exit(printAndRun(acc, ConfigFlags, args))
