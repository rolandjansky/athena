#!/usr/bin/env python
"""

Run material validation to check material maps for tracking geometry.

"""

from AthenaCommon.Configurable import Configurable
from AthenaCommon.Logging import log
from argparse import ArgumentParser
from AthenaConfiguration.AllConfigFlags import ConfigFlags

Configurable.configurableRun3Behavior = True

# Argument parsing
parser = ArgumentParser("RunMaterialValidationITk.py")
parser.add_argument("detectors", metavar="detectors", type=str, nargs="*",
                    help="Specify the list of detectors")
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
parser.add_argument("--geometrytag",default="ATLAS-P2-ITK-24-00-00", type=str,
                    help="The geometry tag to use")
args = parser.parse_args()

# Some info about the job
print("----MaterialValidation for ITk geometry----")
print()
print("Using Geometry Tag: "+args.geometrytag)
if args.localgeo:
    print("...overridden by local Geometry Xml files")
print()

ConfigFlags.Input.isMC             = True

if args.localgeo:
  ConfigFlags.GeoModel.useLocalGeometry = True
  
from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
detectors = args.detectors if 'detectors' in args and args.detectors else ['ITkPixel', 'ITkStrip', 'HGTD']
detectors.append('Bpipe')  # always run with beam pipe
setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)

ConfigFlags.GeoModel.AtlasVersion = args.geometrytag
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-SIM-00-00-00"
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.TrackingGeometry.MaterialSource = "COOL"

ConfigFlags.Detector.GeometryCalo  = False
ConfigFlags.Detector.GeometryMuon  = False

# This should run serially for the moment.
ConfigFlags.Concurrency.NumThreads = 1
ConfigFlags.Concurrency.NumConcurrentEvents = 1

ConfigFlags.ITk.trackingGeometry.loadLocalDbForMaterialMaps=True
LocalDataBaseName = ConfigFlags.ITk.trackingGeometry.localDatabaseName
ConfigFlags.IOVDb.DBConnection='sqlite://;schema='+LocalDataBaseName+';dbname=OFLP200'

log.debug('Lock config flags now.')
ConfigFlags.lock()

from AthenaConfiguration.MainServicesConfig import MainServicesCfg
cfg=MainServicesCfg(ConfigFlags)

### setup dumping of additional information
if args.verboseAccumulators:
  cfg.printConfig(withDetails=True)
if args.verboseStoreGate:
  cfg.getService("StoreGateSvc").Dump = True
  
log.debug('Dumping of ConfigFlags now.')
ConfigFlags.dump()

from TrkDetDescrAlgs.TrkDetDescrAlgsConfig import MaterialValidationCfg
cfg.merge(MaterialValidationCfg(ConfigFlags))
  
cfg.printConfig(withDetails = True, summariseProps = True)

## as no input file, maxEvents should be a valid number
cfg.run(maxEvents=args.maxEvents)
f=open("MaterialValidationITk.pkl","wb")
cfg.store(f)
f.close()

