# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ShellCommand import ShellCommand

from RTTSException import RTTCodingError
from RTTSException import RTTInputError

class FindJobOptionsSearchPathError(Exception):
    def __init__(self, msg): self.msg = msg
    def __str__(self): return str(self.msg)
    
def findJobOptionsSearchPath(cmds, paths, logger):

    logger.debug('Calculating JOBOPTIONSSEARCHPATH')

    # cmds.extend(['source setup.sh'])
    cmds.extend(['echo $JOBOPTSEARCHPATH', 'echo $PATH'])
    
    nlines = -1 # do not limit the number of lines
    shellCom = ShellCommand(cmds, logger, nlines)
    reply = shellCom.getReply()
    #m = 'Reply on setting up CMT to find JO searchpath: %s' % str(reply)
    #logger.debug(m)
    searchPath = ''
    # the env variable path will be used e.g. to locate pre athena scripts
    PATH       = '' 
    
    if len(reply)>1:
        searchPath = reply[-2]
        PATH = reply[-1]
        # These paths are critical to all Athena jobs. As these are most
        # of the RTT jobs, give up if the search path is not set.
    else:
        m = """Could not obtain env vars JOBOPTSEARCHPATH and PATH,
        cmds sent to seek env variables:
        %s
        Reply:  %s
        abandoning run """ % (str(cmds), str(reply))
        logger.critical(m)
        raise RTTInputError(m)
    
    # logger.debug('Found JOBOPTSEARCHPATH: %s' % searchPath)
    # logger.debug('Found PATH: %s' % PATH)

    def jobOpSearchPathSepChar():
        if paths.isNightly: return ':'
        if int(paths.release.split('.')[0]) < 13: return ','
        return ':'
    
    
    
    searchPaths = {
        'jobOptionsSearchPath': {'path':                    searchPath,
                                 'separater':  jobOpSearchPathSepChar()},
        'PATH':                 {'path':                           PATH,
                                 'separater':                       ':'}
        }
    
    return searchPaths
