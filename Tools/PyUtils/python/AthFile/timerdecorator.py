# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# @file timerdecorator.py
# @purpose decorate functions which will have a limited alloted time to finish execution
# @date February 2011
# ripped off from:
#   http://code.activestate.com/recipes/483752/

import sys
import os
import threading

if 'linux' in sys.platform.lower():
    def _run_from_valgrind():
        """
        helper function to detect if one runs under valgrind or not
        """
        for l in open('/proc/self/maps'):
            if '/valgrind' in l:
                return True
        return False

else: # mac-os
    def _run_from_valgrind():
        """
        helper function to detect if one runs under valgrind or not
        """
        return 'VALGRIND_STARTUP_PWD' in os.environ
    
class TimeoutError(Exception):
    pass

def timelimit(timeout):
    def internal(function):
        def internal2(*args, **kw):
            class Calculator(threading.Thread):
                def __init__(self):
                    threading.Thread.__init__(self)
                    self.result = None
                    self.error = None
                
                def run(self):
                    try:
                        self.result = function(*args, **kw)
                    except BaseException:
                        self.error = sys.exc_info()[0]
            
            c = Calculator()
            c.start()
            if _run_from_valgrind():
                # don't set any timeout under valgrind...
                c.join()
            else:
                c.join(timeout)
            if c.is_alive():
                raise TimeoutError
            if c.error:
                raise c.error
            return c.result
        return internal2
    return internal
