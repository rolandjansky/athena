# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, time
import logging

def findNightlyRelease(stampObject, logger):
    """If release is set to 'nightly', find out what is the current release
    and update the confDict dictionary accordingly."""

    (done, release) = stampObject.findNightlyRelease()
        
    waitTime = 5 # mins
    waitmsg  = 'NICOS nightly flag not currently available. '
    waitmsg += 'Will check again in %s minutes.' % str(waitTime)
        
    if done:
        msg  = 'Found the nightly release immediately: %s' % release
        logger.info(msg)
        return release

    logger.info(waitmsg)
    [logging.FileHandler.flush(handler) for handler in logger.handlers]

    while not done:
        waitTime = 15
        # time.sleep(60*waitTime)
        time.sleep(waitTime)
        (done, release) = stampObject.findNightlyRelease()
                
        if not done:
            if stampObject.giveUpWaitingForNightly():
                msg = 'NICOS stamp file will not come, giving up. Over and out.'
                logger.info(msg)
                return None
            logger.info(waitmsg)
            [logging.FileHandler.flush(handler) for handler in logger.handlers]
        else:
            msg  = 'Found the nightly release: %s' % release
            logger.info(msg)

    return release
