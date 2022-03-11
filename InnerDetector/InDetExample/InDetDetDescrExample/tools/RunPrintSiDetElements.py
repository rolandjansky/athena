#!/usr/bin/env python
"""Run PrintSiDetectorElements

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
import sys
from argparse import ArgumentParser

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags

# Set up logging and new style config
Configurable.configurableRun3Behavior = True


# Argument parsing
parser = ArgumentParser("PrintSiDetectorElements.py")
parser.add_argument("detectors", metavar="detectors", type=str, nargs="*",
                    help="Specify the list of detectors")
parser.add_argument("--localgeo", default=False, action="store_true",
                    help="Use local geometry XML files")
parser.add_argument("--geometrytag",default="ATLAS-P2-ITK-24-00-00", type=str,
                    help="The geometry tag to use")
args = parser.parse_args()


# Some info about the job
print("----PrintSiDetectorElements----")
print()
if args.localgeo:
    print("Using local Geometry XML files")
if not args.detectors:
    print("Running complete detector")
else:
    print("Running with: {}".format(", ".join(args.detectors)))
print()

# Configure
ConfigFlags.Concurrency.NumThreads = 1
if ConfigFlags.Concurrency.NumThreads > 0:
    ConfigFlags.Scheduler.ShowDataDeps = True
    ConfigFlags.Scheduler.ShowDataFlow = True
    ConfigFlags.Scheduler.ShowControlFlow = True

ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.GeoModel.AtlasVersion = args.geometrytag
ConfigFlags.Input.isMC = True
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC15c-SDR-14-05"
ConfigFlags.Input.Files = []



if args.localgeo:
    ConfigFlags.ITk.Geometry.AllLocal = True
if args.detectors:
    from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
    setupDetectorsFromList(ConfigFlags, args.detectors, toggle_geometry=True)

ConfigFlags.lock()

# Construct our accumulator to run
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
acc = MainServicesCfg(ConfigFlags)
from AthenaConfiguration.ComponentFactory import CompFactory

# ITk Pixel
if ConfigFlags.Detector.EnableITkPixel:
    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    acc.merge(ITkPixelReadoutGeometryCfg(ConfigFlags))

    ReadPixelDetElements = CompFactory.ReadSiDetectorElements('ReadPixelDetElements')
    ReadPixelDetElements.ManagerName = "ITkPixel"
    ReadPixelDetElements.DetEleCollKey = "ITkPixelDetectorElementCollection"
    ReadPixelDetElements.UseConditionsTools = False
    acc.addEventAlgo(ReadPixelDetElements)

    PrintPixelDetElements = CompFactory.PrintSiElements('PrintPixelDetElements')
    PrintPixelDetElements.OutputLevel = 5
    PrintPixelDetElements.DetectorManagerNames = ["ITkPixel"]
    acc.addEventAlgo(PrintPixelDetElements)

# ITk Strip
if ConfigFlags.Detector.EnableITkStrip:
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    acc.merge(ITkStripReadoutGeometryCfg(ConfigFlags))

    ReadStripDetElements = CompFactory.ReadSiDetectorElements('ReadStripDetElements')
    ReadStripDetElements.ManagerName = "ITkStrip"
    ReadStripDetElements.DetEleCollKey = "ITkStripDetectorElementCollection"
    ReadStripDetElements.UseConditionsTools = False
    acc.addEventAlgo(ReadStripDetElements)

    PrintStripDetElements = CompFactory.PrintSiElements('PrintStripDetElements')
    PrintStripDetElements.OutputLevel = 5
    PrintStripDetElements.DetectorManagerNames = ["ITkStrip"]
    acc.addEventAlgo(PrintStripDetElements)

# Execute and finish
sc = acc.run(maxEvents=1)

# Success should be 0
sys.exit(not sc.isSuccess())
