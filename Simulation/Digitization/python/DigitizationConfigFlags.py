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


def constBunchSpacingPattern(constBunchSpacing):
    """Return a valid value for Digitization.BeamIntensity, which 
    matches the specified constBunchSpacing
    """
    if type(constBunchSpacing) is not int:
        raise TypeError("constBunchSpacing must be int, "
                        "not %s" % type(constBunchSpacing).__name__)
    if constBunchSpacing % 25 != 0:
        raise ValueError("constBunchSpacing must be a multiple of 25, "
                            "not %s" % constBunchSpacing)
    
    # special case
    if constBunchSpacing == 25 :
        return [1.0]
    
    # general case
    pattern = [0.0, 1.0]
    nBunches = (constBunchSpacing//25) - 2
    pattern += nBunches*[0.0]
    return pattern


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
    
    # for PileUp digitization
    # Beam Halo input collections
    flags.addFlag("Digitization.PU.BeamHaloInputCols", [])
    # LHC Bunch Structure (list of non-negative floats)
    flags.addFlag("Digitization.PileUpBunchSpacing", 25)
    flags.addFlag("Digitization.PU.BeamIntensityPattern", lambda prevFlags: constBunchSpacingPattern(prevFlags.Digitization.PileUpBunchSpacing))
    # Beam Gas input collections
    flags.addFlag("Digitization.PU.BeamGasInputCols", [])
    # LHC bunch spacing, in ns, to use in pileup digitization. Only multiples of 25 allowed.
    # Not necessarily equal to Beam.BunchSpacing
    flags.addFlag("Digitization.PU.BunchSpacing", 0)
    # PileUp branch crossing parameters
    flags.addFlag("Digitization.PU.InitialBunchCrossing", -32)
    flags.addFlag("Digitization.PU.FinalBunchCrossing", 6)
    # Add the cavern background every bunch, independent of any bunch structure?
    flags.addFlag("Digitization.PU.CavernIgnoresBeamInt", False)
    # Cavern input collections
    flags.addFlag("Digitization.PU.CavernInputCols", [])
    # Central bunch crossing location in the BeamIntensityPattern
    flags.addFlag("Digitization.PU.FixedT0BunchCrossing", 0)
    # Superimpose mixed high pt minimum bias events (pile-up) on signal events?
    # If so, set this to a list of: High Pt Mixed ND, SD, DD minimum bias input collections
    flags.addFlag("Digitization.PU.HighPtMinBiasInputCols", [])
    # Superimpose mixed low pt minimum bias events (pile-up) on signal events?
    # If so, set this to a list of: Low Pt Mixed ND, SD, DD minimum bias input collections
    flags.addFlag("Digitization.PU.LowPtMinBiasInputCols", [])
    # Number of low pt min-bias events to superimpose per signal event per beam crossing
    flags.addFlag("Digitization.PU.NumberOfLowPtMinBias", 0.0)
    # Number of high pt min-bias events to superimpose per signal event per beam crossing
    flags.addFlag("Digitization.PU.NumberOfHighPtMinBias", 0.0)
    # Number of beam gas events to superimpose per signal event per beam crossing
    flags.addFlag("Digitization.PU.NumberOfBeamGas", 0.0)
    # Number of beam halo events to superimpose per signal event per beam crossing
    flags.addFlag("Digitization.PU.NumberOfBeamHalo", 0.0)
    # Number of mixed ND, SD, DD min-bias events to superimpose per signal event per beam crossing
    flags.addFlag("Digitization.PU.NumberOfCollisions", 0.0)
    # Number of cavern events to superimpose per signal event per beam crossing
    flags.addFlag("Digitization.PU.NumberOfCavern", 0.0)
    # Repeating pattern to determine which events to simulate when using Stepping Cache
    flags.addFlag("Digitization.PU.SignalPatternForSteppingCache", [])
    # Configure EvtIdModifierSvc with a list of dictionaries of the form:
    # {'run': 152166, 'lb': 202, 'starttstamp': 1269948352889940910, 'dt': 104.496, 'evts': 1, 'mu': 0.005, 'force_new': False}
    flags.addFlag("Digitization.PU.RunAndLumiOverrideList", [])
    
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
