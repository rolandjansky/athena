# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## @file AthenaCommon/python/Debugging.py
## @brief py-module to hold a few tools and utilities to help debugging
##        configurables and/or Athena application
## @author Sebastien Binet <binet@cern.ch>
## $Id: Debugging.py,v 1.1 2008-04-05 03:11:49 binet Exp $
###############################################################

__doc__ = """py-module to hold a few tools and utilities to help debugging
configurables and/or Athena application.
"""

import io

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
    if isinstance(out, io.IOBase):
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


def allowPtrace():
    """On kernels with Yama enabled, ptrace may not work by default on processes
which are not decendants of the tracing process.  Among other things, that
causes the way we attach the debugger to fail.  However, we can disable this
on a per-process basis.  Do that here.

See https://www.kernel.org/doc/Documentation/security/Yama.txt and prctl(2).
"""

    # First test to see if ptrace restrictions are enabled.
    import os
    # Return if this kernel does not support ptrace restrictions.
    if not os.path.exists ('/proc/sys/kernel/yama/ptrace_scope'): return

    # Return if ptrace restrictions are disabled.
    if open('/proc/sys/kernel/yama/ptrace_scope').readline().strip() == '0':
        return

    # Use prctl to try to enable ptrace.
    from ctypes import CDLL
    libc = CDLL("libc.so.6")
    # Args are PTRACE_SET_PTRACER (4HYama) and
    # PR_SET_PTRACER_ANY ((unsigned long)-1).
    libc.prctl (0x59616d61, 0xffffffffffffffff)

    return
