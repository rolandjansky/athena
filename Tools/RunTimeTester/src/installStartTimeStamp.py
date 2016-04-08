# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
def installStartTimeStamp(paths, startTime2, logger):
    os.system('touch %s' % os.path.join(paths.resultsDirs['packages'], 'rtt_run_start_%s' % startTime2))
    logger.debug('Created start time stamp file in %s' % paths.resultsDirs['packages'])

