#!/usr/bin/env python
"""

Run geantino processing for material step creation

"""

from argparse import ArgumentParser
from AthenaCommon.Configurable import Configurable
from AthenaCommon.Logging import log
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.ComponentFactory import CompFactory

# Set up logging and new style config
Configurable.configurableRun3Behavior = True

# Argument parsing
parser = ArgumentParser("RunGeantinoStepRecordingITk.py")
parser.add_argument("detectors", metavar="detectors", type=str, nargs="*",
                    help="Specify the list of detectors")
parser.add_argument("--simulate", default=True, action="store_true",
                    help="Run Simulation")
parser.add_argument("--localgeo", default=False, action="store_true",
                    help="Use local geometry Xml files")
parser.add_argument("-V", "--verboseAccumulators", default=False, 
                    action="store_true",
                    help="Print full details of the AlgSequence")
parser.add_argument("-S", "--verboseStoreGate", default=False, 
                    action="store_true",
                    help="Dump the StoreGate(s) each event iteration")
parser.add_argument("--maxEvents",default=10, type=int,
                    help="The number of events to run. 0 skips execution")
parser.add_argument("--skipEvents",default=0, type=int,
                    help="The number of events to skip")
parser.add_argument("--geometrytag",default="ATLAS-P2-ITK-24-00-00", type=str,
                    help="The geometry tag to use")
parser.add_argument("--inputevntfile",
                    default="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/EVNT/mc15_14TeV.singlegeantino_E10GeV_etaFlatnp0_6.5M.evgen.EVNT.pool.root",
                    help="The input EVNT file to use")
parser.add_argument("--outputhitsfile",default="myHITS.pool.root", type=str,
                    help="The output HITS filename")
args = parser.parse_args()


# Some info about the job
print("----GeantinoStepRecording for ITk geometry----")
print()
print("Using Geometry Tag: "+args.geometrytag)
if args.localgeo:
    print("...overridden by local Geometry Xml files")
print("Input EVNT File:"+args.inputevntfile)
if not args.detectors:
    print("Running complete detector")
else:
    print("Running with: {}".format(", ".join(args.detectors)))
print()

# Configure
if args.localgeo:
    ConfigFlags.GeoModel.useLocalGeometry = True

ConfigFlags.Input.Files = [args.inputevntfile]
ConfigFlags.Output.HITSFileName = args.outputhitsfile

ConfigFlags.GeoModel.AtlasVersion = args.geometrytag
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-SIM-00-00-00"
ConfigFlags.GeoModel.Align.Dynamic = False

ConfigFlags.Exec.SkipEvents = args.skipEvents

from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
detectors = args.detectors if 'detectors' in args and args.detectors else ['ITkPixel', 'ITkStrip', 'HGTD']
detectors.append('Bpipe')  # always run with beam pipe
setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)
  
log.debug('Lock config flags now.')
ConfigFlags.lock()

# Construct our accumulator to run
acc = MainServicesCfg(ConfigFlags)

### setup dumping of additional information
if args.verboseAccumulators:
  acc.printConfig(withDetails=True)
if args.verboseStoreGate:
  acc.getService("StoreGateSvc").Dump = True
  
log.debug('Dumping of ConfigFlags now.')
ConfigFlags.dump()

from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(PoolWriteCfg(ConfigFlags))

# add BeamEffectsAlg
from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
acc.merge(BeamEffectsAlgCfg(ConfigFlags))

beamcond = acc.getCondAlgo("BeamSpotCondAlg")

beamcond.useDB=False
beamcond.posX=0.0
beamcond.posY=0.0
beamcond.posZ=0.0
beamcond.sigmaX=0.0
beamcond.sigmaY=0.0
beamcond.sigmaZ=0.0
beamcond.tiltX=0.0
beamcond.tiltY=0.0

kwargs = {}

svcName = "G4UA::MaterialStepRecorderUserActionSvc"
from TrkG4UserActions.TrkG4UserActionsConfigNew import MaterialStepRecorderUserActionSvcCfg
acc.merge(MaterialStepRecorderUserActionSvcCfg(ConfigFlags,svcName,**kwargs))
kwargs.update(UserActionSvc=svcName)

if args.simulate:
  from G4AtlasAlg.G4AtlasAlgConfigNew import G4AtlasAlgCfg
  acc.merge(G4AtlasAlgCfg(ConfigFlags, "ITkG4AtlasAlg", **kwargs))
  
AthenaOutputStream=CompFactory.AthenaOutputStream
AthenaOutputStreamTool=CompFactory.AthenaOutputStreamTool
writingTool = AthenaOutputStreamTool( "MaterialStepCollectionStreamTool" )

outputStream = AthenaOutputStream(name = "MaterialStepCollectionStream",
                                  WritingTool = writingTool,
                                  ItemList=['EventInfo#*', 'Trk::MaterialStepCollection#*'],
                                  MetadataItemList = [ "EventStreamInfo#MaterialStepCollectionStream", "IOVMetaDataContainer#*" ],
                                  OutputFile = "MaterialStepCollection.root")

StoreGateSvc=CompFactory.StoreGateSvc
acc.addService(StoreGateSvc("MetaDataStore"))
outputStream.MetadataStore = acc.getService("MetaDataStore")

MakeEventStreamInfo=CompFactory.MakeEventStreamInfo
streamInfoTool = MakeEventStreamInfo( "MaterialStepCollectionStream_MakeEventStreamInfo" )
streamInfoTool.Key = "MaterialStepCollectionStream"
streamInfoTool.EventInfoKey = "EventInfo"
outputStream.HelperTools.append(streamInfoTool)
    
acc.addEventAlgo(outputStream)

acc.printConfig(withDetails = True, summariseProps = True)

acc.run(maxEvents=args.maxEvents)

f=open("GeantinoStepRecordingITk.pkl","wb")
acc.store(f)
f.close()
