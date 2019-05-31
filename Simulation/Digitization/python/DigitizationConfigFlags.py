"""Construct ConfigFlags for Digitization

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaCommon.Logging import log
from PyUtils import AthFile

def getSpecialConfiguration(flags):
    """Return a dict of Special configuration as parsed from flags.Input.Files"""
    if len(flags.Input.Files) > 1:
        log.info("Multiple input files. Using the first for Digitization special configuration.")
    log.info("Obtaining Digitization special configuration from %s." % flags.Input.Files[0])
    File = AthFile.fopen(flags.Input.Files[0])
    # extract the special config list
    tag_info = File.infos.get("tag_info", {})
    SpecialCfg = tag_info.get("specialConfiguration", "").split(";")
    # fill containers
    preIncludes = []
    out = {}
    for KeyEqValue in SpecialCfg:
        # Ignore empty or "NONE" substrings, e.g. from consecutive or trailing semicolons
        if not KeyEqValue or KeyEqValue.upper() == "NONE":
            continue
        # If not in key=value format, treat as v, with k="preInclude"
        if "=" not in KeyEqValue:
            KeyEqValue = "preInclude=" + KeyEqValue
        # Handle k=v directives
        key, value = KeyEqValue.split("=")
        if key == "preInclude":
            preIncludes += value.split(",")
        else:
            out[key] = value
    # FIXME includes not migrated
    # from AthenaCommon.Include import include
    # for inc in preIncludes:
    #     include(inc)
    return out

def createDigitizationCfgFlags():
    """Return an AthConfigFlags object with required flags"""
    flags = AthConfigFlags()
    # Run Inner Detector noise simulation
    flags.addFlag("Digitization.DoInnerDetectorNoise", True)
    # Run pile-up digitization on one bunch crossing at a time?
    flags.addFlag("Digitization.DoXingByXingPileUp", False)
    # Run pile-up premixing
    flags.addFlag("Digitization.PileUpPremixing", False)
    # Special configuration read from flags.Input.Files
    flags.addFlag("Digitization.SpecialConfiguration", getSpecialConfiguration)
    # Run Calorimeter noise simulation
    flags.addFlag("Digitization.DoCaloNoise", True)
    # Run pile-up premixing
    flags.addFlag("Digitization.DoDigiTruth", True)
    # Use high-gain Forward Calorimeters
    flags.addFlag("Digitization.HighGainFCal", False)
    # Use high-gain ElectroMagnetic EndCap Inner Wheel
    flags.addFlag("Digitization.HighGainEMECIW", True)
    # Do global pileup digitization
    flags.addFlag("Digitization.Pileup", True)
    return flags

