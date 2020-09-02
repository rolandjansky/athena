# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import re

from AthenaCommon.Logging import logging
from PyUtils.MetaReader import read_metadata

logger = logging.getLogger("OverlayMetadataConfig")

# Module level cache of file-metadata:
_fileMetadata = dict()

def _getFileMD(filenames):
    if type(filenames) == list:
        filename = filenames[0]
    else:
        filename = filenames

    if filename not in _fileMetadata:
        logger.info("Obtaining full metadata of %s", filename)

        thisFileMD = read_metadata(filename, None, 'full')
        _fileMetadata.update(thisFileMD)

    return _fileMetadata[filename]


def signalMetadataCheck(flags, simDict, tagInfoDict):
    """Check the metadata for signal HITS file"""
    logger.info("Checking Overlay configuration against Signal Simulation metadata...")

    simKeys = simDict.keys()
    tagInfoKeys = tagInfoDict.keys()

    # Check the PhysicsList set agrees with that used in the simulation
    if "PhysicsList" in simKeys:
        if re.match(simDict["PhysicsList"], flags.Sim.PhysicsList):
            logger.debug("Overlay configuration matches Signal Simulation metadata. [Sim.PhysicsList = %s]", flags.Sim.PhysicsList)
        else:
            flags.Sim.PhysicsList = simDict["PhysicsList"]
            logger.warning("Overlay Sim.PhysicsList does not match the PhysicsList used in the Signal Simulation step! Assume the value from the Signal Simulation step is correct!")
            logger.warning("Set Sim.PhysicsList = %s", flags.Sim.PhysicsList)
    else:
        logger.error("'PhysicsList' key not found in Signal Simulation metadata!")
        raise AssertionError("Signal Simulation metadata key not found")

    # Check the DetDescrVersion set agrees with that used in the simulation
    if "SimLayout" in simKeys:
        if re.match(simDict["SimLayout"], flags.GeoModel.AtlasVersion):
            logger.debug("Overlay configuration matches Signal Simulation metadata. [Geomodel.AtlasVersion = %s]",
                         flags.GeoModel.AtlasVersion)
        else:
            flags.GeoModel.AtlasVersion = simDict["SimLayout"]
            logger.warning("Overlay Geomodel.AtlasVersion does not match the value used in the Signal Simulation step! Assume the value from the Signal Simulation step is correct!")
            logger.warning("Set Geomodel.AtlasVersion = %s", flags.GeoModel.AtlasVersion)
    else:
        logger.error("'SimLayout' key not found in Signal Simulation metadata!")
        raise AssertionError("Signal Simulation metadata key not found")

    # Check the Conditions Tag set against that used in the simulation
    if "IOVDbGlobalTag" in tagInfoKeys:
        if not re.match(tagInfoDict["IOVDbGlobalTag"], flags.IOVDb.GlobalTag):
            logger.debug("Overlay configuration: [IOVDb.GlobalTag = %s], Signal Simulation metadata: [IOVDb.GlobalTag = %s]",
                         flags.IOVDb.GlobalTag, tagInfoDict['IOVDbGlobalTag'])
    else:
        logger.error("'IOVDbGlobalTag' key not found in Signal Simulation metadata!")
        raise AssertionError("Signal Simulation metadata key not found")

    # Set the TRTRangeCut digitizationFlag based on what was used during the simulation.
    if "TRTRangeCut" in simKeys:
        if not re.match(simDict["TRTRangeCut"], str(flags.Digitization.TRTRangeCut)):
            flags.Digitization.TRTRangeCut = simDict["TRTRangeCut"]
            logger.warning("Overlay Digitization.TRTRangeCut does not match the value used in the Signal Simulation step! Assume the value from the Signal Simulation step is correct!")
            logger.warning("Set Digitization.TRTRangeCut = %s", flags.Digitization.TRTRangeCut)
    else:
        logger.warning("'TRTRangeCut' key not found in Signal Simulation metadata!")

    # Check which sub-detectors were simulated
    # TODO: needed?

    logger.info("Completed checks of Overlay configuration against Signal Simulation metadata.")


def simulationMetadataCheck(sigdict, pudict):
    """Check the simulation metadata for pre-mixed pileup RDO file"""
    sigkeys = sigdict.keys()
    pukeys = pudict.keys()

    # Loop over metadata keys which must have matching values
    warningKeys = ['G4Version']
    sigKeysToCheck = ['PhysicsList', 'SimLayout', 'MagneticField', 'hitFileMagicNumber'] + warningKeys
    for o in sigKeysToCheck:
        try:
            assert o in pukeys
        except AssertionError:
            logger.error("%s key missing from Pre-mixed pile-up Simulation metadata!", o)
            raise AssertionError("Pre-mixed pile-up Simulation metadata key not found")
        try:
            assert o in sigkeys
        except AssertionError:
            logger.error("%s key missing from Signal Simulation metadata!", o)
            raise AssertionError("Signal Simulation metadata key not found")
        try:
            if not isinstance(pudict[o], type(sigdict[o])):
                assert re.match(str(pudict[o]), str(sigdict[o]))
            else:
                if isinstance(pudict[o], str):
                    assert re.match(pudict[o], sigdict[o])
                elif isinstance(pudict[o], int):
                    assert (pudict[o] == sigdict[o])
                else:
                    assert re.match(str(pudict[o]), str(sigdict[o]))
        except AssertionError:
            if o in warningKeys:
                logger.warning("Simulation metadata mismatch! Pre-mixed pile-up: [%s = %s] Signal: [%s = %s]", o, pudict[o], o, sigdict[o])
            else:
                logger.error("Simulation metadata mismatch! Pre-mixed pile-up: [%s = %s] Signal: [%s = %s]", o, pudict[o], o, sigdict[o])
                raise AssertionError("Simulation metadata mismatch")


def tagInfoMetadataCheck(sigdict, pudict):
    """Check the tag info metadata for pre-mixed pileup RDO File"""
    sigkeys = sigdict.keys()
    pukeys = pudict.keys()

    logger.debug("Signal /TagInfo ", sigdict)
    logger.debug("Pileup /TagInfo ", pudict)

    sigOnlyDict = dict()
    sigOnlyKeySet = set(sigkeys).difference(set(pukeys))
    logger.debug("The following keys only appear in Signal /TagInfo metadata:")
    logger.debug(sigOnlyKeySet)
    for key in sigOnlyKeySet:
        sigOnlyDict[key] = sigdict[key]
        logger.debug("  key: ", key, "value: ", sigdict[key])
        pass
    # TODO: extra
    keysToCompareSet = set(sigkeys).intersection(set(pukeys))
    logger.debug("The following keys appear in Signal and Pre-mixed pile-up /TagInfo metadata:")
    logger.debug(keysToCompareSet)
    
    # Loop over metadata keys which must have matching values
    warningKeys = ['IOVDbGlobalTag']
    sigKeysToCheck = warningKeys
    for o in sigKeysToCheck:
        try:
            assert o in pukeys
        except AssertionError:
            logger.error("%s key missing from Pre-mixed pile-up Simulation metadata!", o)
            raise AssertionError("Pre-mixed pile-up Simulation metadata key not found")
        try:
            assert o in sigkeys
        except AssertionError:
            logger.error("%s key missing from Signal Simulation metadata!", o)
            raise AssertionError("Signal Simulation metadata key not found")
        try:
            if not isinstance(pudict[o], type(sigdict[o])):
                assert re.match(str(pudict[o]), str(sigdict[o]))
            else:
                if isinstance(pudict[o], str):
                    assert re.match(pudict[o], sigdict[o])
                elif isinstance(pudict[o], int):
                    assert (pudict[o] == sigdict[o])
                else:
                    assert re.match(str(pudict[o]), str(sigdict[o]))
        except AssertionError:
            if o in warningKeys:
                logger.warning("Simulation metadata mismatch! Pre-mixed pile-up: [%s = %s] Signal: [%s = %s]", o, pudict[o], o, sigdict[o])
            else:
                logger.error("Simulation metadata mismatch! Pre-mixed pile-up: [%s = %s] Signal: [%s = %s]", o, pudict[o], o, sigdict[o])
                raise AssertionError("Simulation metadata mismatch")


def overlayMetadataCheck(flags):
    """Check overlay metadata"""
    if flags.Overlay.DataOverlay:
        files = flags.Input.Files
        filesPileup = flags.Input.SecondaryFiles
    else:
        files = flags.Input.SecondaryFiles
        filesPileup = flags.Input.Files

    signalMetadata = _getFileMD(files)
    signalSimulationMetadata = signalMetadata["/Simulation/Parameters"]
    signalTagInfoMetadata = signalMetadata["/TagInfo"]

    # signal check
    signalMetadataCheck(flags, signalSimulationMetadata, signalTagInfoMetadata)

    # pile-up check
    if not flags.Overlay.DataOverlay:
        pileupMetaDataCheck = _getFileMD(filesPileup)
        pileupSimulationMetadata = pileupMetaDataCheck["/Simulation/Parameters"]
        pileupTagInfoMetadata = pileupMetaDataCheck["/TagInfo"]
    
        logger.info("Checking Pre-mixed pile-up metadata against Signal Simulation metadata...")
        simulationMetadataCheck(signalSimulationMetadata, pileupSimulationMetadata)
        tagInfoMetadataCheck(signalTagInfoMetadata, pileupTagInfoMetadata)
        logger.info("Completed all checks against Pre-mixed pile-up Simulation metadata.")


def overlayMetadataWrite(flags):
    """Write overlay metadata"""
    from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg
    acc = IOVDbSvcCfg(flags)
    return acc
