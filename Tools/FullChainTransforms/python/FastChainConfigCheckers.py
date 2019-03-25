# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration


def syncDigitizationAndSimulationJobProperties():
    from AthenaCommon.Logging import logging
    logDigiConfigChecker = logging.getLogger( 'FastChainConfigChecker' )

    from G4AtlasApps.SimFlags import simFlags
    from Digitization.DigitizationFlags import digitizationFlags
    logDigiConfigChecker.info("Digitization jobProperties values:")
    digitizationFlags.print_JobProperties()

    if digitizationFlags.TRTRangeCut.get_Value() != simFlags.TRTRangeCut.get_Value() :
        logDigiConfigChecker.info("Setting digitizationFlags.TRTRangeCut equal to simFlags.TRTRangeCut.")
        digitizationFlags.TRTRangeCut = simFlags.TRTRangeCut.get_Value()

    import os
    G4Version = str(os.environ['G4VERS'])
    if digitizationFlags.SimG4VersionUsed.get_Value() != G4Version :
        logDigiConfigChecker.info("Setting digitizationFlags.SimG4VersionUsed equal to G4VERS from environment. ")
        digitizationFlags.SimG4VersionUsed = G4Version

    if digitizationFlags.physicsList.get_Value() != simFlags.PhysicsList.get_Value() :
        logDigiConfigChecker.info("Setting digitizationFlags.physicsList equal to simFlags.PhysicsList.")
        digitizationFlags.physicsList = simFlags.PhysicsList.get_Value()

    return

