# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @Package PyJobTransforms.trfDecorators
#  @brief Some useful decorators used by the transforms
#  @author atlas-comp-transforms-dev@cern.ch
#  @version $Id: trfDecorators.py 696484 2015-09-23 17:20:28Z graemes $

import functools
import os
import sys
import time

import PyJobTransforms.trfUtils as trfUtils
from PyJobTransforms.trfExitCodes import trfExit

from PyJobTransforms.trfLogger import logging
msg = logging.getLogger(__name__)


## @brief Redirect stdout/err to /dev/null
# Useful wrapper to get rid of ROOT verbosity...
# N.B. May be somewhat dangerous in its present form - all errors disappear
# even ones you might want to see :-)
def silent(func):
    def silent_running(*args, **kwargs):
        # Create some filehandles to save the stdout/err fds to    
        save_err  = os.open('/dev/null', os.O_WRONLY)
        save_out  = os.open('/dev/null', os.O_WRONLY)
        os.dup2(sys.stderr.fileno(), save_err)
        os.dup2(sys.stdout.fileno(), save_out)

        # Now open 'quiet' file handles and attach stdout/err
        quiet_err  = os.open('/dev/null', os.O_WRONLY)
        quiet_out  = os.open('/dev/null', os.O_WRONLY)
        os.dup2(quiet_err, sys.stderr.fileno())
        os.dup2(quiet_out, sys.stdout.fileno())
        
        # Execute function
        rc = func(*args, **kwargs)

        sys.stderr.flush()
        sys.stdout.flush()
        
        # Restore fds
        os.dup2(save_err, sys.stderr.fileno())
        os.dup2(save_out, sys.stdout.fileno())

        os.close (save_err)
        os.close (save_out)
        os.close (quiet_err)
        os.close (quiet_out)

        return rc
    # Make the wrapper look like the wrapped function
    functools.update_wrapper(silent_running, func)
    return silent_running


## @brief Decorator to wrap a transform in outer try: ... except: ...
def stdTrfExceptionHandler(func):
    def exception_wrapper(*args, **kwargs):
        # Setup imports which the wrapper needs
        import signal
        import traceback

        import PyJobTransforms.trfExceptions as trfExceptions

        try:
            return func(*args, **kwargs)
        
        except KeyboardInterrupt:
            msg.critical('Caught a keyboard interrupt - exiting at your request.')
            trfUtils.infanticide(message=True)
            sys.exit(128 + signal.SIGINT)
            
        # This subclass is treated as a 'normal' exit condition
        # but it should never happen in production as it's a transform definition error
        except trfExceptions.TransformSetupException as e:
            msg.critical('Transform setup failed: {0}'.format(e.errMsg))
            msg.critical('To help you debug here is the stack trace:')
            msg.critical(traceback.format_exc(None))
            msg.critical('(Early exit - no job report is produced)')
            trfUtils.infanticide(message=True)
            sys.exit(e.errCode)

        except trfExceptions.TransformException as e:
            msg.critical('Got a transform exception in the outer exception handler: {0!s}'.format(e))
            msg.critical('Stack trace is...')
            msg.critical(traceback.format_exc(None))
            msg.critical('Job reports are likely to be missing or incomplete - sorry')
            msg.critical('Please report this as a transforms bug!')
            trfUtils.infanticide(message=True)
            sys.exit(trfExit.nameToCode('TRF_UNEXPECTED_TRF_EXCEPTION'))
            
        except Exception as e:
            msg.critical('Got a general exception in the outer exception handler: {0!s}'.format(e))
            msg.critical('Stack trace is...')
            msg.critical(traceback.format_exc(None))
            msg.critical('Job reports are likely to be missing or incomplete - sorry')
            msg.critical('Please report this as a transforms bug!')
            trfUtils.infanticide(message=True)
            sys.exit(trfExit.nameToCode('TRF_UNEXPECTED_OTHER_EXCEPTION'))
            
    functools.update_wrapper(exception_wrapper, func)
    return exception_wrapper


## @brief Decorator to dump a stack trace when hit by SIGUSR
# Note that this decorator has to go inside the stdTrfExceptionHandler
# Or the general exception catcher catches the SigUser exception.
def sigUsrStackTrace(func):
    import signal
    import traceback
    
    class SigUsr1(Exception):
        pass
    
    def sigHandler(signum, frame):
        msg.info('Handling signal %d in sigHandler', signum)
        raise SigUsr1
    
    def signal_wrapper(*args, **kwargs):
        signal.signal(signal.SIGUSR1, sigHandler)
        
        try:
            return func(*args, **kwargs)
        
        except SigUsr1:
            msg.error('Transform received SIGUSR1. Exiting now with stack trace...')
            msg.error('(The important frame is usually the one before this trfDecorators module.)')
            msg.error(traceback.format_exc(None))
            trfUtils.infanticide(message=True)
            sys.exit(128 + signal.SIGUSR1)  
            
    functools.update_wrapper(signal_wrapper, func)
    return signal_wrapper



def timelimited(timeout=None, retry=1, timefactor=1.5, sleeptime=10, defaultrc=None):

    import traceback
    import queue
    import multiprocessing as mp

    from sys import exc_info
    from PyJobTransforms.trfExceptions import TransformTimeoutException, TransformInternalException

    msg = logging.getLogger(__name__)

    def internal(func):
        
        ## @brief Run our wrapped function on the multiprocess queue
        #  @detail Run wrapper function and use the message queue to communicate status and result
        #  @return None. However, on the message queue add a tuple with two members:
        #   - @c key, which is True if function exited properly, False if an exception occurred
        #   - @c result, which is the output of the function or a tuple of exception information
        def funcWithQueue(queue, *args, **kwargs):
            try:
                result = func(*args, **kwargs)
                queue.put((True, result))
            except Exception:
                exc0=exc_info()[0]
                exc1=exc_info()[1]
                exc2=traceback.format_exc()
                msg.warning('In time limited function %s an exception occurred', func.__name__)
                msg.warning('Original traceback:')
                msg.warning(exc2)            
                queue.put((False,(exc0, exc1, exc2))) 
            
        def funcWithTimeout(*args, **kwargs):
            ltimeout=timeout
            lretry=retry
            ltimefactor=timefactor
            lsleeptime=sleeptime
            ldefaultrc=defaultrc

            if 'timeout' in kwargs:
                ltimeout=kwargs.pop('timeout')
            if 'retry' in kwargs:
                lretry=kwargs.pop('retry')
            if 'timefactor' in kwargs:    
                ltimefactor=kwargs.pop('timefactor')
            if 'sleeptime' in kwargs:    
                lsleeptime=kwargs.pop('sleeptime')
            if 'defaultrc' in kwargs:    
                ldefaultrc=kwargs.pop('defaultrc')

            if ltimeout is None:
                # Run function normally with no timeout wrapper
                msg.debug('Running {0}: {1} {2} without timeout'.format(func, args, kwargs))
                return func(*args, **kwargs)
                
            n=0
            while n<=lretry:
                msg.info('Try %i out of %i (time limit %s s) to call %s.', n+1, retry+1, ltimeout, func.__name__)
                starttime = time.time()
                q=mp.Queue(maxsize=1)
                nargs = (q,) + args
                proc=mp.Process(target=funcWithQueue, args=nargs, kwargs=kwargs)
                proc.start()
                try:
                    # Wait for function to run and return, but with a timeout
                    flag,result = q.get(block=True, timeout=ltimeout)
                    proc.join(60)
                    msg.info('Executed call within %d s.', time.time()-starttime)
                    if flag:
                        return result
                    else:
                        msg.warning('But an exception occurred in function %s.', func.__name__)
                        msg.warning('Returning default return code %s.', ldefaultrc)
                        return ldefaultrc
                except queue.Empty:
                    # Our function did not run in time - kill increase timeout
                    msg.warning('Timeout limit of %d s reached. Kill subprocess and its children.', ltimeout)
                    parent=proc.pid
                    pids=[parent]
                    pids.extend(trfUtils.listChildren(parent=parent, listOrphans = False))
                    trfUtils.infanticide(pids)
                    proc.join(60) # Ensure cleanup
                    if n!=lretry:
                        msg.info('Going to sleep for %d s.', lsleeptime)                    
                        time.sleep(lsleeptime)
                    n+=1
                    ltimeout*=ltimefactor
                    lsleeptime*=ltimefactor
                except IOError:
                    errMsg = "IOError while communicating with subprocess"
                    msg.error(errMsg)
                    raise TransformInternalException(trfExit.nameToCode("TRF_EXTERNAL"), errMsg)

            msg.warning('All %i tries failed!', n)
            raise TransformTimeoutException(trfExit.nameToCode('TRF_EXEC_TIMEOUT'), 'Timeout in function %s' % (func.__name__))
            
        return funcWithTimeout
    
        functools.update_wrapper(funcWithTimeout, func)
        
    return internal
