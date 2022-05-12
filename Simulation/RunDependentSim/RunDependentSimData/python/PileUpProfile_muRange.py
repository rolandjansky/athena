# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging


def setupProfile(flags, scaleTaskLength=1):
    logger = logging.getLogger("PileUp")
    customProfile = flags.Digitization.PU.CustomProfile
    # customProfile={'run':195847, 'startmu':15.0, 'endmu':25.0, 'stepmu':1.0, 'startlb':1, 'timestamp':1328039085}

    def _evts(x):
        return int(scaleTaskLength * x)

    try:
        logger.info('Setting-up custom pile-up range with user-selected run(s) %s', customProfile.get('run', -1))
    except AttributeError:
        logger.error("Set ConfigFlags.Digitization.PU.CustomProfile=\'{\"run\":1234,...}\' in preExec!")
        raise ValueError("Invalid ConfigFlags.Digitization.PU.CustomProfile provided. Run number is missing.")
    keys=['run', 'startmu', 'endmu', 'stepmu', 'startlb', 'timestamp']
    for key in customProfile.keys():
        if key not in keys:
            raise ValueError("Invalid ConfigFlags.Digitization.PU.CustomProfile provided, key: %s. Valid keys are: %s" % (key, keys)) 

    startMu = customProfile.get('startmu', 0.0)
    endMu = customProfile.get('endmu', 10.0)
    stepMu = customProfile.get('stepmu', 1.0)
    numberOfSteps = int(((endMu - startMu) / stepMu) + 1.0)
    if numberOfSteps < 2:
        raise RuntimeError("Step size too large!")

    stepsWithExtraEvent = 5000 % numberOfSteps
    eventsPerStep = (5000 - (5000 % numberOfSteps)) / numberOfSteps
    if _evts(eventsPerStep) < 2: raise RuntimeError("Step size too small!")

    runList = customProfile.get('run', 195847)
    if not isinstance(runList, list): runList = [runList]
    timestampList = customProfile.get('timestamp', 1328039085)
    if not isinstance(timestampList, list): timestampList = [timestampList]
    startlbList = customProfile.get('startlb', 1)
    if not isinstance(startlbList, list): startlbList = [startlbList]

    profile = []
    for currentRun, currentTimestamp, currentStartlb in zip(runList, timestampList, startlbList):
        for step in range(numberOfSteps):
            events = eventsPerStep + 1 if step < stepsWithExtraEvent else eventsPerStep
            mu = float(startMu + (step * stepMu))
            profile += [{'run': currentRun, 'lb': currentStartlb + step, 'starttstamp': currentTimestamp, 
                         'evts': _evts(events), 'mu': mu}]
    return profile
