# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def checkTRT_DigitizationConfiguration():
    from AthenaCommon.Logging import logging
    logDigiConfigChecker = logging.getLogger( 'DigiConfigChecker' )

    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.simulateLVL1.TRT_on():
        from AthenaCommon.BeamFlags import jobproperties
        from Digitization.DigitizationFlags import digitizationFlags
        if (jobproperties.Beam.beamType != "cosmics") and ('RUNTRTLVL1' not in digitizationFlags.experimentalDigi()):
            DetFlags.simulateLVL1.TRT_setOff()
            logDigiConfigChecker.info("Switching DetFlags.simulateLVL1.TRT_setOff()")
            logDigiConfigChecker.info("Experts may enable LVL1 TrigT1TRT simulation by setting RUNTRTLVL1=True in a preExec or in their main job options.")

    if DetFlags.simulateLVL1.TRT_on() and DetFlags.pileup.any_on():
        logDigiConfigChecker.warning("LVL1::TrigT1TRT is not pile-up-friendly using it in this case may significantly degrade performance.")
    return


def checkDetFlagConfiguration():
    from AthenaCommon.Logging import logging,AthError
    logDigiConfigChecker = logging.getLogger( 'DigiConfigChecker' )

    from AthenaCommon.DetFlags import DetFlags
    if not DetFlags.geometry.any_on():
        AthError( "this digitization job needs some DetFlags.geometry On" )
    if not DetFlags.digitize.any_on():
        AthError( "this *digitization* job needs some DetFlags.digitize On" )
    if not DetFlags.writeRDOPool.any_on():
        logDigiConfigChecker.warning( "this digitization job will not write any RDO object" )
    return


def syncDetFlagsAndDigitizationJobProperties():
    from AthenaCommon.Logging import logging
    logDigiConfigChecker = logging.getLogger( 'DigiConfigChecker' )

    from AthenaCommon.DetFlags import DetFlags
    from Digitization.DigitizationFlags import digitizationFlags
    if not (digitizationFlags.doLowPtMinBias.get_Value() or digitizationFlags.doHighPtMinBias.get_Value() or
            digitizationFlags.doCavern.get_Value() or digitizationFlags.doBeamGas.get_Value() or
            digitizationFlags.doBeamHalo.get_Value()) :
        DetFlags.pileup.all_setOff()
        digitizationFlags.numberOfCollisions=0.0
    #--------------------------------------------------------------
    # Check if we're using separated or mixed minbias
    #--------------------------------------------------------------
    if DetFlags.pileup.any_on():
        digitizationFlags.numberOfCollisions=0.0
        if digitizationFlags.doLowPtMinBias.get_Value() or digitizationFlags.doHighPtMinBias.get_Value():
            logDigiConfigChecker.info("Setting numberOfCollisions=numberOfLowPtMinBias+numberOfHighPtMinBias.")
            if digitizationFlags.doLowPtMinBias.get_Value():
                digitizationFlags.numberOfCollisions += digitizationFlags.numberOfLowPtMinBias.get_Value()
            if digitizationFlags.doHighPtMinBias.get_Value():
                digitizationFlags.numberOfCollisions += digitizationFlags.numberOfHighPtMinBias.get_Value()
    return


def syncBeamAndDigitizationJobProperties():
    from AthenaCommon.Logging import logging
    logDigiConfigChecker = logging.getLogger( 'DigiConfigChecker' )

    from AthenaCommon.BeamFlags import jobproperties
    from Digitization.DigitizationFlags import digitizationFlags
    logDigiConfigChecker.info("Beam jobProperties values:")
    jobproperties.Beam.print_JobProperties()
    if jobproperties.Beam.numberOfCollisions.get_Value() != digitizationFlags.numberOfCollisions.get_Value() :
        logDigiConfigChecker.info("Setting jobproperties.Beam.numberOfCollisions equal to digitizationFlags.numberOfCollisions.")
        jobproperties.Beam.numberOfCollisions = digitizationFlags.numberOfCollisions.get_Value()
        jobproperties.Beam.override = True ## just incase - default is True

    if digitizationFlags.BeamIntensityPattern.statusOn :
        logDigiConfigChecker.info("Using Bunch Structure, so jobproperties.Beam.bunchSpacing does not need to match digitizationFlags.bunchSpacing.")
    elif (jobproperties.Beam.bunchSpacing.get_Value() != digitizationFlags.bunchSpacing.get_Value()) :
        logDigiConfigChecker.info("Not using Bunch Structure, so setting jobproperties.Beam.bunchSpacing equal to digitizationFlags.bunchSpacing.")
        jobproperties.Beam.bunchSpacing = digitizationFlags.bunchSpacing.get_Value()
        jobproperties.Beam.override = True ## just incase - default is True
    return

