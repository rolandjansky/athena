"""Construct ConfigFlags for Digitization

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaConfiguration.AutoConfigFlags import GetFileMD
from AthenaCommon.Logging import log
from PyUtils import AthFile

def getSpecialConfiguration(flags):
    """Return a dict of Special configuration as parsed from flags.Input.Files"""
    if len(flags.Input.Files) > 1:
        log.info("Multiple input files. Using the first for Digitization special configuration.")
    log.info("Obtaining Digitization special configuration from %s", flags.Input.Files[0])
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
    # Compute and store DigiTruth information
    flags.addFlag("Digitization.DoDigiTruth", True)
    # Use high-gain Forward Calorimeters
    flags.addFlag("Digitization.HighGainFCal", False)
    # Use high-gain ElectroMagnetic EndCap Inner Wheel
    flags.addFlag("Digitization.HighGainEMECIW", True)
    # Do global pileup digitization
    flags.addFlag("Digitization.Pileup", True)
    # TRT Range cut used in simulation in mm. Should be 0.05 or 30.
    flags.addFlag("Digitization.TRTRangeCut", lambda prevFlags : float(GetFileMD(prevFlags.Input.Files).get('TRTRangeCut', 0.05)))
    # Write out truth information?
    flags.addFlag("Digitization.TruthOutput", False)
    # Write out calorimeter digits
    flags.addFlag("Digitization.AddCaloDigi", False)
    # Integer offset to random seed initialisation
    flags.addFlag("Digitization.RandomSeedOffset", 0)
    # Digitization extra input dependencies
    flags.addFlag("Digitization.ExtraInputs", [("xAOD::EventInfo", "EventInfo")])
    return flags


def digitizationRunArgsToFlags(runArgs, flags):
    """Fill digitization configuration flags from run arguments."""
    if hasattr(runArgs, "PileUpPremixing"):
        flags.Digitization.PileUpPremixing = runArgs.PileUpPremixing

    if hasattr(runArgs, "doAllNoise"):
        flags.Digitization.DoInnerDetectorNoise = runArgs.doAllNoise
        flags.Digitization.DoCaloNoise = runArgs.doAllNoise

    if hasattr(runArgs, "AddCaloDigi"):
        flags.Digitization.AddCaloDigi = runArgs.AddCaloDigi
    
    if hasattr(runArgs, "digiSeedOffset1"):
        flags.Digitization.RandomSeedOffset = runArgs.digiSeedOffset1

    # TODO: Not covered yet as no flag equivalents exist yet
    # '--digiRndmSvc'
    # '--digiSteeringConf'
    # '--samplingFractionDbTag'
