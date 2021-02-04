# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from math import ceil

from AthenaCommon.Logging import logging


def pileUpCalc(nSignalEvts, refreshRate, nSubEvtPerBunch, nBunches):
    """Returns the toal number of needed events"""
    totalSubEvts = nBunches * nSubEvtPerBunch
    totalSubEvts += totalSubEvts * refreshRate * nSignalEvts
    return totalSubEvts


def generateBackgroundInputCollections(flags, initialList, nBkgEvtsPerCrossing, correctForEmptyBunchCrossings):
    """Preparing the list of required input PU files"""
    logger = logging.getLogger("PileUp")

    finalList = []

    nSignalEvts = 1000
    if flags.Exec.MaxEvents > 0:
        nSignalEvts = int(flags.Exec.MaxEvents)
        logger.info('Number of signal events (from Exec.MaxEvents) = %s.', nSignalEvts)
    else:
        nSignalEvts = 0
        from PyUtils.MetaReader import read_metadata
        for inFile in list(flags.Input.Files):
            try:
                metadata = read_metadata(inFile)
                metadata = metadata[inFile]  # promote all keys one level up
                nSignalEvts += int(metadata['nentries'])
                logger.debug('{} -> __Test__001__:\n{}'.format(__file__, nSignalEvts))
            except Exception as err:
                logger.warning("Unable to open file [%s]", inFile)
                logger.warning('caught:\n%s', err)
                import traceback
                traceback.print_exc()
        logger.info('Number of signal events (read from files) = %s.', nSignalEvts)

    nBkgEventsPerFile = 5000
    try:
        from PyUtils.MetaReader import read_metadata
        metadata = read_metadata(initialList[0])
        metadata = metadata[initialList[0]]  # promote all keys one level up
        nBkgEventsPerFile = int(metadata['nentries'])
        logger.debug('{} -> __Test__001__:\n{}'.format(__file__, nBkgEventsPerFile))
        logger.info('Number of background events per file (read from file) = %s.', nBkgEventsPerFile)
    except Exception:
        import traceback
        traceback.print_exc()
        logger.warning('Failed to count the number of background events in %s.'
                       'Assuming 5000 - if this is an overestimate the job may die.', initialList[0])

    nBunchesTotal = int(1 + flags.Digitization.PU.FinalBunchCrossing - flags.Digitization.PU.InitialBunchCrossing)
    nBunches = nBunchesTotal
    if correctForEmptyBunchCrossings:
        nBunches = int(ceil(float(nBunches) * float(flags.Digitization.PU.BunchSpacing)) / float(flags.Beam.BunchSpacing))
    logger.info('Simulating a maximum of %s colliding-bunch crossings (%s colliding+non-colliding total) per signal event', nBunches, nBunchesTotal)
    nBkgEventsForJob = pileUpCalc(float(nSignalEvts), 1.0, float(nBkgEvtsPerCrossing), nBunches)
    logger.info('Number of background events required: %s. Number of background events in input files: %s',
                nBkgEventsForJob, (nBkgEventsPerFile * len(initialList)))
    numberOfRepetitionsRequiredTmp = float(nBkgEventsForJob) / float(nBkgEventsPerFile * len(initialList))
    numberOfRepetitionsRequired = 1 + int(ceil(numberOfRepetitionsRequiredTmp))
    # FIXME many copies of this string seems rather inefficient
    for i in range(0, numberOfRepetitionsRequired):
        finalList += initialList
    logger.info('Expanding input list from %s to %s',
                len(initialList), len(finalList))
    return finalList


def loadPileUpProfile(fragment_string):
    """Load pile-up profile from file."""
    parts = fragment_string.split('.')
    if len(parts) < 2:
        raise ValueError('Pile-up profile configuration should be of the form Package.Module')

    from importlib import import_module
    loaded_module = import_module(fragment_string)
    function_def = getattr(loaded_module, 'setupProfile')
    return function_def()


def generatePileUpProfile(flags,
                          profile,
                          randomMuSampling=False,
                          sequentialEventNumbers=False,
                          doNotCorrectMaxEvents=False):
    """Generate pile-up profile"""
    logger = logging.getLogger("PileUp")
    logger.info('Doing RunLumiOverride configuration from file.')

    jobNumber = flags.Digitization.JobNumber
    maxEvents = flags.Exec.MaxEvents

    if maxEvents == -1:
        raise SystemExit("maxEvents = %d is not supported! Please set this to the number of events per file times the number of files per job." % (
            maxEvents,))
    if not doNotCorrectMaxEvents:
        # round up to nearest 100 events..
        corrMaxEvents = ceil(float(maxEvents)/100.0)*100.0
    else:
        logger.warning(
            "Using the actual number of HITS input events for this job -- not for production use!")
        corrMaxEvents = maxEvents

    # We may need to repeat this run for long production jobs.
    # NB: unlike vanilla variable-mu jobs, it's possible to waste
    #  up to trfMaxEvents-1 events per complete run in prodsys if
    #  the number of events specified by this run is not evenly
    #  divisible by trfMaxEvents.
    JobMaker = loadPileUpProfile(profile)
    runMaxEvents = sum(lb["evts"] for lb in JobMaker)
    logger.info("There are %d events in this run.", runMaxEvents)
    jobsPerRun = int(ceil(float(runMaxEvents)/corrMaxEvents))
    logger.info("Assuming there are usually %d events per job. (Based on %d events in this job.)",
                corrMaxEvents, maxEvents)
    logger.info("There must be %d jobs per run.", jobsPerRun)

    # Override event numbers with sequential ones if requested
    if sequentialEventNumbers:
        logger.info("All event numbers will be sequential.")

    # Random mu sampling
    if randomMuSampling:
        logger.info("Mu values will be sampled randomly from the set profile.")
        # Load needed tools
        from Digitization.RunDependentMCTaskIterator import getRandomlySampledRunLumiInfoFragment
        fragment = getRandomlySampledRunLumiInfoFragment(
            jobnumber=(jobNumber-1),
            task=JobMaker,
            maxEvents=maxEvents,
            sequentialEventNumbers=sequentialEventNumbers)
    else:
        # Load needed tools
        from Digitization.RunDependentMCTaskIterator import getRunLumiInfoFragment
        fragment = getRunLumiInfoFragment(
            jobnumber=(jobNumber-1),
            task=JobMaker, maxEvents=maxEvents,
            sequentialEventNumbers=sequentialEventNumbers)
    
    # Remove lumiblocks with no events
    for element in fragment:
        if element['evts'] == 0:
            logger.warning('Found lumiblock with no events!  This lumiblock will not be used:\n (' + element.__str__() + ')' )
    fragment = [x for x in fragment if x['evts'] != 0]

    from RunDependentSimComps.RunLumiConfigTools import condenseRunLumiInfoFragment
    logger.info("Writing RunDMC trigger configuration fragment to file.  listOfRunsEvents = %s",
                condenseRunLumiInfoFragment(fragment, "RunDMCTriggerRunsInfo.py"))

    flags.Input.RunAndLumiOverrideList = fragment


def scaleNumberOfCollisions(flags):
    """Scale the number of events per crossing to the largest value in job.
    
    Note: beam halo and beam gas will NOT be scaled!"""
    logger = logging.getLogger("PileUp")

    maxMu = max(element['mu'] for element in flags.Input.RunAndLumiOverrideList)
    if not (maxMu > 0 and flags.Digitization.PU.NumberOfCollisions):
        return

    scale = maxMu / flags.Digitization.PU.NumberOfCollisions
    nCollisions = flags.Digitization.PU.NumberOfCollisions
    if nCollisions:
        flags.Digitization.PU.NumberOfCollisions = maxMu
        logger.info("Changing Digitization.PU.NumberOfCollisions from %s to %s",
                    nCollisions, flags.Digitization.PU.NumberOfCollisions)

    if flags.Digitization.PU.NumberOfLowPtMinBias:
        old = flags.Digitization.PU.NumberOfLowPtMinBias
        flags.Digitization.PU.NumberOfLowPtMinBias *= scale
        logger.info("Changing Digitization.PU.NumberOfLowPtMinBias from %s to %s",
                    old, flags.Digitization.PU.NumberOfLowPtMinBias)

    if flags.Digitization.PU.NumberOfHighPtMinBias:
        old = flags.Digitization.PU.NumberOfHighPtMinBias
        flags.Digitization.PU.NumberOfHighPtMinBias *= scale
        logger.info("Changing Digitization.PU.NumberOfHighPtMinBias from %s to %s",
                    old, flags.Digitization.PU.NumberOfHighPtMinBias)

    if flags.Digitization.PU.NumberOfCavern:
        old = flags.Digitization.PU.NumberOfCavern
        flags.Digitization.PU.NumberOfCavern *= scale
        logger.info("Changing Digitization.PU.NumberOfCavern from %s to %s",
                    old, flags.Digitization.PU.NumberOfCavern)


def setupPileUpFlags(flags, bunchTrains, pileUpProfile):
    # Setup beam intensity pattern
    parts = bunchTrains.split('.')
    if len(parts) < 2:
        raise ValueError('Bunch trains configuration should be of the form Package.Module')

    from importlib import import_module
    loaded_module = import_module(bunchTrains)
    function_def = getattr(loaded_module, 'setupBunchTrains')
    function_def(flags)

    # Setup pile-up profile
    generatePileUpProfile(flags, pileUpProfile)

    flags.Digitization.PU.NumberOfCollisions = flags.Digitization.PU.NumberOfLowPtMinBias + flags.Digitization.PU.NumberOfHighPtMinBias
    scaleNumberOfCollisions(flags)
