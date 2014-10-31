#!/usr/bin/env python

# @file AthenaCommon/share/athena-app.py

from __future__ import with_statement

__version__ = "$Revision: 1.2 $"
__author__  = "Sebastien Binet <binet@cern.ch>"
__doc__ = """a better? athena.py"""

#import user

import os
import sys

def main(options):
    import sys
    import os
    
    import AthenaCommon.ExitCodes as ExitCodes
    import AthenaCommon.AthApi as api
    app = api.AthApp(options)
        
    api.msg.info('running app.configure(...)')
    app.configure(joboptions=options.scripts,
                  commands=options.command,
                  extra_options=sys.argv[:])
    
    sc = 0
    if options.run_batch:
        api.msg.info("entering batch mode")
        try:
            sc = app.run().isSuccess()
            if options.dbg_stage == "fini":
                import AthenaCommon.Debugging as _dbg
                _dbg.hookDebugger()
        except:
            # print a stack trace to know something bad happened !
            import traceback, sys
            traceback.print_exc(file=sys.stderr)
            # update the exit-code
            if app.exitstate == ExitCodes.ALL_OK:
                app.exitstate = ExitCodes.UNKNOWN_EXCEPTION
            # re-raise, in case somebody wants to do something about it
            raise
        finally:
            if app.exitstate != ExitCodes.ALL_OK:
                # trigger the AthenaSummarySvc...
                # FIXME
                pass
            app.exit()
    else:
        api.msg.info("entering interactive mode")
        api.enter_interactive_loop(app=app)

    sc = app.exitstate
    return sc

def _process_cmdline_args():
    import AthenaCommon.AthOptionsParser as aop

    opts = aop.parse(chk_tcmalloc=False)

    #-----------------------------------------------------------------
    # side-effect: bootstrap ourselves and handle the environment
    # changing options (tcmalloc/stdcmalloc -> LD_PRELOAD)
    # => we may need to os.execvpe ourselves with the correct new env.
    _frozen = os.environ.get('_ATHENA_APP_FROZEN', '0')
    os.environ['_ATHENA_APP_FROZEN'] = _frozen
    # may not return !
    _bootstrap_env(opts)
    #-----------------------------------------------------------------
    
    from AthenaCommon.Logging import log as msg
    from AthenaCommon.Logging import logging
    msg.setLevel(getattr(logging, opts.msg_lvl))
    
    import AthenaCommon.ExitCodes as ExitCodes
    
    if len(opts.scripts)<=0 and opts.run_batch:
        msg.error("batch mode requires at least one joboptions")
        raise aop.AthOptionsError(reason=ExitCodes.INCLUDE_ERROR)

    return opts

def _bootstrap_env(options):
    """helper function to modify the current process environment according
    to `options` and os.execvpe.
    we set a special environment variable '_ATHENA_APP_FROZEN' to 1 to
    prevent infinite recursion.
    if we are already a 'frozen' process, perform some clean-up if any
    """
    _frozen = os.environ['_ATHENA_APP_FROZEN']
    if _frozen == '1':
        ### remove pre-load hack for proper execution of child-processes ------
        ldpreload = os.environ.get('LD_PRELOAD', '')
        if ldpreload:
            tcmlib = os.environ['TCMALLOCDIR'] + '/libtcmalloc.so'
            import string
            ldpreload = string.replace( ldpreload, tcmlib, '' )
            ldpreload = string.replace( ldpreload, '::', ':')
            ldpreload = ldpreload.strip(':')
            del tcmlib

            if not ldpreload:
                del os.environ['LD_PRELOAD']
            else:
                os.environ['LD_PRELOAD'] = ldpreload
        return

    # freeze...
    os.environ['_ATHENA_APP_FROZEN'] = '1'
    if options.use_tcmalloc:
        tcmalloc_dir = os.environ.get('TCMALLOCDIR', None)
        #dont display the warning if in AthAnalysisBase 
        if tcmalloc_dir is None and "ManaCore" not in os.environ.get("CMTEXTRATAGS",""):
            print "WARNING: $TCMALLOCDIR not defined, will use libc malloc"
        else:
            dso = os.path.join(tcmalloc_dir, 'libtcmalloc.so')
            if not os.path.exists(dso):
                raise OSError('library [%s] does not exist' % dso)
            ldpreload = os.environ.get('LD_PRELOAD', None)
            if ldpreload:
                os.environ['LD_PRELOAD'] = os.pathsep.join(
                    [dso] + ldpreload.split(os.pathsep)
                    )
            else:
                os.environ['LD_PRELOAD'] = dso
    else:
        # stdcmalloc
        pass

    # spawn a new process with the correct environment...
    import AthenaCommon.Utils.unixtools as uut
    exe = uut.FindFile('python',
                       os.environ['PATH'].split(os.pathsep),
                       os.X_OK)
    args= [exe] + sys.argv
    #print "---> args:",args
    os.execvpe(exe, args, os.environ)
            
if __name__ == "__main__":
    options = _process_cmdline_args()
    retcode=main(options)
    sys.exit(retcode)
    
