# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""
Temporary module to wrap setting of ConfigFlags.Detector.SimulateXXX flags
"""


# detectors to enable by default in Run4 jobs
run4DefaultDetectors = [
    'Bpipe',
    'BCMPrime', 'ITkPixel', 'ITkStrip', 'HGTD',
    'LAr', 'Tile',
    'MDT', 'RPC', 'TGC', 'sTGC', 'MM'
]

# detectors to enable by default in Run3 jobs
run3DefaultDetectors = [
    'Bpipe',
    'BCM', 'DBM', 'Pixel', 'SCT', 'TRT',
    'LAr', 'Tile',
    'CSC', 'MDT', 'RPC', 'TGC', 'sTGC', 'MM'
]

# detectors to enable by default in Run2 jobs
run2DefaultDetectors = [
    'Bpipe',
    'BCM', 'DBM', 'Pixel', 'SCT', 'TRT',
    'LAr', 'Tile',
    'CSC', 'MDT', 'RPC', 'TGC'
]

# detectors to enable by default in Run1 jobs
run1DefaultDetectors = [
    'Bpipe',
    'BCM', 'Pixel', 'SCT', 'TRT',
    'LAr', 'Tile',
    'CSC', 'MDT', 'RPC', 'TGC'
]


def getDefaultDetectorsForRun(run):
    if run == "Run4":
        return run4DefaultDetectors
    if run == "Run3":
        return run3DefaultDetectors
    if run == "Run2":
        return run2DefaultDetectors

    return run1DefaultDetectors


def setupDetectorSimulateFlagsFromList(ConfigFlags, detectors):
    # Setup detector flags
    if detectors:
        from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
        setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)

    #set the detector flags: - all on currently
    #inner detectors
    ConfigFlags.Detector.SimulateBCM = ConfigFlags.Detector.GeometryBCM
    ConfigFlags.Detector.SimulateDBM = ConfigFlags.Detector.GeometryDBM
    ConfigFlags.Detector.SimulatePixel = ConfigFlags.Detector.GeometryPixel
    ConfigFlags.Detector.SimulateSCT = ConfigFlags.Detector.GeometrySCT
    ConfigFlags.Detector.SimulateTRT = ConfigFlags.Detector.GeometryTRT

    # Phase 2
    ConfigFlags.Detector.SimulateBCMPrime = ConfigFlags.Detector.GeometryBCMPrime
    ConfigFlags.Detector.SimulateITkPixel = ConfigFlags.Detector.GeometryITkPixel
    ConfigFlags.Detector.SimulateITkStrip = ConfigFlags.Detector.GeometryITkStrip
    ConfigFlags.Detector.SimulateHGTD = ConfigFlags.Detector.GeometryHGTD

    #muon
    ConfigFlags.Detector.SimulateMuon = ConfigFlags.Detector.GeometryMuon
    ConfigFlags.Detector.SimulateMDT = ConfigFlags.Detector.GeometryMDT
    ConfigFlags.Detector.SimulateRPC = ConfigFlags.Detector.GeometryRPC
    ConfigFlags.Detector.SimulateTGC = ConfigFlags.Detector.GeometryTGC
    ConfigFlags.Detector.SimulateCSC = ConfigFlags.Detector.GeometryCSC
    ConfigFlags.Detector.SimulatesTGC = ConfigFlags.Detector.GeometrysTGC
    ConfigFlags.Detector.SimulateMM = ConfigFlags.Detector.GeometryMM

    #Calo
    ConfigFlags.Detector.SimulateLAr = ConfigFlags.Detector.GeometryLAr
    ConfigFlags.Detector.SimulateTile = ConfigFlags.Detector.GeometryTile

    ConfigFlags.Detector.SimulateBpipe = ConfigFlags.Detector.GeometryBpipe
    ConfigFlags.Detector.SimulateCavern = ConfigFlags.Detector.GeometryCavern

    #forward region not migrated yet
    ConfigFlags.Detector.SimulateLucid = ConfigFlags.Detector.GeometryLucid
    ConfigFlags.Detector.SimulateZDC = ConfigFlags.Detector.GeometryZDC
    ConfigFlags.Detector.SimulateALFA = ConfigFlags.Detector.GeometryALFA
    ConfigFlags.Detector.SimulateAFP = ConfigFlags.Detector.GeometryAFP
    ConfigFlags.Detector.SimulateFwdRegion = ConfigFlags.Detector.GeometryFwdRegion
    ConfigFlags.Detector.SimulateForward =  ConfigFlags.Detector.GeometryForward
