# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RTTSException import RTTSystemResponseError

import commands

def setMaxMem():
    cmd = 'ulimit -Hv'
    sc, maxmem  = commands.getstatusoutput(cmd)
    if sc != 0:
        raise RTTSystemResponseError('bad status %d from %s %s' %  (sc, cmd, maxmem))
    
    cmd = 'ulimit -v %s' % maxmem
    sc, out = commands.getstatusoutput(cmd)
    if sc != 0: raise RTTSystemResponseError('bad status %d from %s %s' % (sc, cmd, out))

    return maxmem
if __name__ == '__main__':
   
    print setMaxMem()
