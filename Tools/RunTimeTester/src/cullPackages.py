# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def cullPackages(cvsPackages, logger):

    packagesToRun = [p for p in cvsPackages if p.runMyJobs()]
    nAll = len(cvsPackages)
    nToRun = len(packagesToRun)
    m  = 'About to make TestRuns for '
    m += '%d packages\n' % (nToRun)
    m += '%d packages out of %d were ' % ((nAll-nToRun), nAll)
    m += 'dropped due to user selection\n'
    m += 'retained packages:\n'
    m += str([p.name for p in packagesToRun])

    logger.info(m)

    return packagesToRun
