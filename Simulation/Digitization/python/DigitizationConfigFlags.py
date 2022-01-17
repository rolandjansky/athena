"""Construct ConfigFlags for Digitization

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaConfiguration.Enums import ProductionStep


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
    if constBunchSpacing == 25:
        return [1.0]

    # general case
    pattern = [0.0, 1.0]
    nBunches = (constBunchSpacing//25) - 2
    pattern += nBunches*[0.0]
    return pattern


def createDigitizationCfgFlags():
    """Return an AthConfigFlags object with required flags"""
    flags = AthConfigFlags()
    # Digitization Steering - needed for easy comparison with the
    # old-style configuration, but can potentially drop
    flags.addFlag("Digitization.DigiSteeringConf", "StandardPileUpToolsAlg")
    # Run Inner Detector noise simulation
    flags.addFlag("Digitization.DoInnerDetectorNoise", True)
    # Run pile-up digitization on one bunch crossing at a time?
    flags.addFlag("Digitization.DoXingByXingPileUp", False)
    # Run Calorimeter noise simulation
    flags.addFlag("Digitization.DoCaloNoise", True)
    # Compute and store DigiTruth information
    flags.addFlag("Digitization.DoDigiTruth", False)
    # Use high-gain Forward Calorimeters
    flags.addFlag("Digitization.HighGainFCal", False)
    # Use high-gain ElectroMagnetic EndCap Inner Wheel
    flags.addFlag("Digitization.HighGainEMECIW", True)
    # Do global pileup digitization
    flags.addFlag("Digitization.PileUp", False)
    # Temporary TGC flag
    flags.addFlag("Digitization.UseUpdatedTGCConditions", False)
    # Write out truth information?
    flags.addFlag("Digitization.TruthOutput", False)
    # Write out calorimeter digits
    flags.addFlag("Digitization.AddCaloDigi", False)
    # Write out thinned calorimeter digits
    flags.addFlag("Digitization.AddCaloDigiThinned", False)
    # Integer offset to random seed initialisation
    flags.addFlag("Digitization.RandomSeedOffset", 0)
    # Digitization extra input dependencies
    flags.addFlag("Digitization.ExtraInputs", [("xAOD::EventInfo", "EventInfo")])
    # Override the HIT file Run Number with one from a data run
    flags.addFlag("Digitization.DataRunNumber", -1)
    # Job number
    flags.addFlag("Digitization.JobNumber", 1)
    # Beam spot reweighting (-1 disables it)
    flags.addFlag("Digitization.InputBeamSigmaZ", -1)

    # Run radiation damage simulation for pixel planar sensors
    flags.addFlag("Digitization.DoPixelPlanarRadiationDamage", False)
    # Run the template version of the radiation damage
    flags.addFlag("Digitization.DoPixelPlanarRadiationDamageTemplate", False)
    # Run radiation damage simulation for pixel 3D sensors
    flags.addFlag("Digitization.DoPixel3DRadiationDamage", False)
    # Run the template version of the radiation damage
    flags.addFlag("Digitization.DoPixel3DRadiationDamageTemplate", False)

    # for PileUp digitization
    # Bunch structure configuration
    flags.addFlag("Digitization.PU.BunchStructureConfig", "")
    # Pile-up profile configuration
    flags.addFlag("Digitization.PU.ProfileConfig", "")
    # Custom pile-up profile configuration - fully custom or for mu range
    flags.addFlag("Digitization.PU.CustomProfile", "")
    # Force sequential event numbers
    flags.addFlag("Digitization.PU.ForceSequentialEventNumbers",
                  lambda prevFlags: prevFlags.Common.ProductionStep == ProductionStep.PileUpPresampling)
    # Beam Halo input collections
    flags.addFlag("Digitization.PU.BeamHaloInputCols", [])
    # LHC Bunch Structure (list of non-negative floats)
    flags.addFlag("Digitization.PU.BeamIntensityPattern",
                  lambda prevFlags: constBunchSpacingPattern(prevFlags.Beam.BunchSpacing))
    # Beam Gas input collections
    flags.addFlag("Digitization.PU.BeamGasInputCols", [])
    # LHC bunch spacing, in ns, to use in pileup digitization. Only multiples of 25 allowed.
    # Not necessarily equal to Beam.BunchSpacing
    flags.addFlag("Digitization.PU.BunchSpacing",
                  lambda prevFlags: prevFlags.Beam.BunchSpacing)
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
    # Offset into the input collections of high pt min-bias events
    flags.addFlag("Digitization.PU.HighPtMinBiasInputColOffset", 0)
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

    return flags


def digitizationRunArgsToFlags(runArgs, flags):
    """Fill digitization configuration flags from run arguments."""
    # from SimDigi
    if hasattr(runArgs, "DataRunNumber"):
        flags.Digitization.DataRunNumber = runArgs.DataRunNumber

    # from SimDigi
    if hasattr(runArgs, "jobNumber"):
        flags.Digitization.JobNumber = runArgs.jobNumber

    if hasattr(runArgs, "PileUpPresampling"):
        flags.Common.ProductionStep = ProductionStep.PileUpPresampling
    elif flags.Common.ProductionStep == ProductionStep.Default: # Do not override previous settings
        flags.Common.ProductionStep = ProductionStep.Digitization

    if hasattr(runArgs, "doAllNoise"):
        flags.Digitization.DoInnerDetectorNoise = runArgs.doAllNoise
        flags.Digitization.DoCaloNoise = runArgs.doAllNoise

    if hasattr(runArgs, "AddCaloDigi"):
        flags.Digitization.AddCaloDigi = runArgs.AddCaloDigi

    flags.Digitization.RandomSeedOffset = 0
    if hasattr(runArgs,"digiSeedOffset1"):
        flags.Digitization.RandomSeedOffset += int(runArgs.digiSeedOffset1)
    if hasattr(runArgs,"digiSeedOffset2"):
        flags.Digitization.RandomSeedOffset += int(runArgs.digiSeedOffset2)

    if hasattr(runArgs, "digiSteeringConf"):
        flags.Digitization.DigiSteeringConf = runArgs.digiSteeringConf + "PileUpToolsAlg"

    # TODO: Not covered yet as no flag equivalents exist yet
    # '--digiRndmSvc'
    # '--samplingFractionDbTag'


def pileupRunArgsToFlags(runArgs, flags):
    """Fill pile-up digitization configuration flags from run arguments."""
    if hasattr(runArgs, "numberOfLowPtMinBias"):
        flags.Digitization.PU.NumberOfLowPtMinBias = runArgs.numberOfLowPtMinBias

    if hasattr(runArgs, "numberOfHighPtMinBias"):
        flags.Digitization.PU.NumberOfHighPtMinBias = runArgs.numberOfHighPtMinBias

    if hasattr(runArgs, "numberOfBeamHalo"):
        flags.Digitization.PU.NumberOfBeamHalo = runArgs.numberOfBeamHalo

    if hasattr(runArgs, "numberOfBeamGas"):
        flags.Digitization.PU.NumberOfBeamGas = runArgs.numberOfBeamGas

    if hasattr(runArgs, "numberOfCavernBkg"):
        flags.Digitization.PU.NumberOfCavern = runArgs.numberOfCavernBkg

    if hasattr(runArgs, "bunchSpacing"):
        flags.Digitization.PU.BunchSpacing = runArgs.bunchSpacing

    if hasattr(runArgs, "pileupInitialBunch"):
        flags.Digitization.PU.InitialBunchCrossing = runArgs.pileupInitialBunch

    if hasattr(runArgs, "pileupFinalBunch"):
        flags.Digitization.PU.FinalBunchCrossing = runArgs.pileupFinalBunch

    # sanity check
    if flags.Digitization.PU.InitialBunchCrossing > flags.Digitization.PU.FinalBunchCrossing:
        raise ValueError("Initial bunch crossing should not be larger than the final one")

    if hasattr(runArgs, "inputLowPtMinbiasHitsFile"):
        from Digitization.PileUpUtils import generateBackgroundInputCollections
        flags.Digitization.PU.LowPtMinBiasInputCols = \
            generateBackgroundInputCollections(flags, runArgs.inputLowPtMinbiasHitsFile,
                                               flags.Digitization.PU.NumberOfLowPtMinBias, True)

    if hasattr(runArgs, "inputHighPtMinbiasHitsFile"):
        from Digitization.PileUpUtils import getInputCollectionOffset, generateBackgroundInputCollections
        if flags.Digitization.PU.HighPtMinBiasInputColOffset < 0:
            # Calculate a pseudo random offset into the collection from the jobNumber
            flags.Digitization.PU.HighPtMinBiasInputColOffset = getInputCollectionOffset(flags, runArgs.inputHighPtMinbiasHitsFile)

        flags.Digitization.PU.HighPtMinBiasInputCols = \
            generateBackgroundInputCollections(flags, runArgs.inputHighPtMinbiasHitsFile,
                                               flags.Digitization.PU.NumberOfHighPtMinBias, True)

    if hasattr(runArgs, "inputCavernHitsFile"):
        from Digitization.PileUpUtils import generateBackgroundInputCollections
        flags.Digitization.PU.CavernInputCols = \
            generateBackgroundInputCollections(flags, runArgs.inputCavernHitsFile,
                                               flags.Digitization.PU.NumberOfCavern, True)  # TODO: ignore?

    if hasattr(runArgs, "inputBeamHaloHitsFile"):
        from Digitization.PileUpUtils import generateBackgroundInputCollections
        flags.Digitization.PU.BeamHaloInputCols = \
            generateBackgroundInputCollections(flags, runArgs.inputBeamHaloHitsFile,
                                               flags.Digitization.PU.NumberOfBeamHalo, True)

    if hasattr(runArgs, "inputBeamGasHitsFile"):
        from Digitization.PileUpUtils import generateBackgroundInputCollections
        flags.Digitization.PU.BeamGasInputCols = \
            generateBackgroundInputCollections(flags, runArgs.inputBeamGasHitsFile,
                                               flags.Digitization.PU.NumberOfBeamGas, True)

    # TODO: Not covered yet as no flag equivalents exist yet
    # '--testPileUpConfig'


def setupDigitizationFlags(runArgs, flags):
    """Setup common digitization flags."""
    # autoconfigure pile-up if inputs are present
    if (hasattr(runArgs, "inputLowPtMinbiasHitsFile")
        or hasattr(runArgs, "inputHighPtMinbiasHitsFile")
        or hasattr(runArgs, "inputCavernHitsFile")
        or hasattr(runArgs, "inputBeamHaloHitsFile")
        or hasattr(runArgs, "inputBeamGasHitsFile")):
        flags.Digitization.PileUp = True

    if flags.Digitization.PileUp:
        flags.Input.OverrideRunNumber = True
        # keep this one True by default in CA-based config
        flags.Digitization.DoXingByXingPileUp = True
    else:
        flags.Input.OverrideRunNumber = flags.Digitization.DataRunNumber > 0
