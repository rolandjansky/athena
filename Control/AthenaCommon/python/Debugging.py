# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file AthenaCommon/python/Debugging.py
## @brief py-module to hold a few tools and utilities to help debugging
##        configurables and/or Athena application
## @author Sebastien Binet <binet@cern.ch>
## $Id: Debugging.py,v 1.1 2008-04-05 03:11:49 binet Exp $
###############################################################

__doc__ = """py-module to hold a few tools and utilities to help debugging
configurables and/or Athena application.
"""

class DbgStage:
    """Class to hold the stage at which the user asked to hook the debugger
    """
    value          = None
    allowed_values = ( "conf", "init", "exec", "fini" )
    pass # class DbgStage

def hookDebugger(debugger='gdb'):
    """debugging helper, hooks debugger to running interpreter process
    """
    import os
    pid = os.spawnvp(os.P_NOWAIT,
                     debugger, [debugger, '-q', 'python', str(os.getpid())])

    # give debugger some time to attach to the python process
    import time
    time.sleep( 1 )

    # verify the process' existence (will raise OSError if failed)
    os.waitpid( pid, os.WNOHANG )
    os.kill( pid, 0 )
    return

def hookStrace(out=None):
    """Same than for hookDebugger: spawn strace and attach to the running
    interpreter process
    """
    import os
    if out is None: out = 'athena.strace.log.%i' % os.getpid()
    if isinstance(out, file):
        out = out.name
    elif not isinstance(out,str):
        raise TypeError('argument 0 needs to be either a file or a filename')
    
    pid = os.spawnvp(os.P_NOWAIT,
                     'strace', ['strace',
                                '-f', # follow children
                                '-p', str(os.getpid()),
                                '-o', out])

    # give strace some time to attach to the python process
    import time
    time.sleep( 1 )

    # verify the process' existence (will raise OSError if failed)
    os.waitpid( pid, os.WNOHANG )
    os.kill( pid, 0 )
    return
