#!/usr/bin/env python
"""

Run material mapping for tracking geometry.
Uses as MaterialStepsCollections as input.

"""
from AthenaCommon.Configurable import Configurable
from AthenaCommon.Logging import log
from argparse import ArgumentParser
from AthenaConfiguration.AllConfigFlags import ConfigFlags

Configurable.configurableRun3Behavior = True

# Argument parsing
parser = ArgumentParser("RunMaterialMappingITk.py")
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
parser.add_argument("--inputfile",
                    default="MaterialStepCollection.root",
                    help="The input material step file to use")
args = parser.parse_args()

# Some info about the job
print("----MaterialMapping for ITk geometry----")
print()
print("Using Geometry Tag: "+args.geometrytag)
if args.localgeo:
    print("...overridden by local Geometry Xml files")
print("Input File:"+args.inputfile)
if not args.detectors:
    print("Running complete detector")
else:
    print("Running with: {}".format(", ".join(args.detectors)))
print()

LocalDataBaseName = ConfigFlags.ITk.trackingGeometry.localDatabaseName
ConfigFlags.IOVDb.DBConnection='sqlite://;schema='+LocalDataBaseName+';dbname=OFLP200'

# necessity to create a new PoolFileCatalog
import os
if os.path.exists('./PoolFileCatalog.xml') :
  print('[!] PoolFileCatalog exists in the run directory (may use old PFN!)')
  print('[>] Deleting it now !')
  os.remove('./PoolFileCatalog.xml')

ConfigFlags.Input.isMC             = True

if args.localgeo:
  ConfigFlags.ITk.Geometry.AllLocal = True
  
from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
detectors = args.detectors if 'detectors' in args and args.detectors else ['ITkPixel', 'ITkStrip', 'HGTD']
detectors.append('Bpipe')  # always run with beam pipe
setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)

ConfigFlags.GeoModel.AtlasVersion = args.geometrytag
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-SIM-00-00-00"
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.TrackingGeometry.MaterialSource = "None"

ConfigFlags.Detector.GeometryCalo  = False
ConfigFlags.Detector.GeometryMuon  = False

# This should run serially for the moment.
ConfigFlags.Concurrency.NumThreads = 1
ConfigFlags.Concurrency.NumConcurrentEvents = 1

import glob
FileList = glob.glob(args.inputfile)
ConfigFlags.Input.Files = FileList

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

from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
cfg.merge(PoolReadCfg(ConfigFlags))

from TrkDetDescrAlgs.TrkDetDescrAlgsConfig import ITkMaterialMappingCfg
cfg.merge(ITkMaterialMappingCfg(ConfigFlags, 
                                name="ITkMaterialMapping"))
  
cfg.printConfig(withDetails = True, summariseProps = True)

events = args.maxEvents
if events<=0:
  events = 10000000000
cfg.run(maxEvents=events)
f=open("MaterialMappingITk.pkl","wb")
cfg.store(f)
f.close()

