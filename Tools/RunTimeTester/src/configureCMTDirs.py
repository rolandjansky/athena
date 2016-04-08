# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ShellCommand import ShellCommand

def configureCMTDirs(localRTTRun, cmtConfigurer, logger):
    # Not necessary if this is a local run
    if localRTTRun:
        logger.info('Local run, not configuring CMT dir')
        return
    
    cmtConfigurer.makeHomeCMTDir()
    
    nlines = -1 # do not limit the number of lines
    cmds = [cmtConfigurer.configure()]
    logger.info('cmds to configure cmt dir:\n%s' %str(cmds))
    shellCom = ShellCommand(cmds, logger, nlines)
    reply = shellCom.getReply()
    m = 'Reply on configuring CMT: %s' % str(reply)
    logger.info(m)
