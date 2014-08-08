"""File: PackDistCommon.py

Common classes and utility functions of the PackDist package.
"""

__author__  = 'Grigori Rybkine <Grigori.Rybkine@cern.ch>'
__version__ = '0.2.1'
__date__ = 'Wed Oct 03 2012'

__all__ = ['Error', 'InputError', 'CommandError', 'exitstatus']

import sys
import os

class Error(Exception):
    """Base class for exceptions in this module."""
    def __str__(self):
        return ': '.join([str(arg) for arg in self.args])
    def write(self, file = sys.stderr):
        print >> file, '%s: %s' % (self.__class__.__name__, self)

class InputError(Error):
    """Exception raised for errors in the input.
    
    Attributes:
    expression() -- input expression in which the error occurred
    message() -- explanation of the error
    """

    def __init__(self, expression, message):
        Error.__init__(self, expression, message)
    def expression(self):
        return self.args[0]
    def message(self):
        return self.args[1]

class CommandError(Error):
    """Exception raised for errors executing shell commands.
    
    Attributes:
    args[0] -- shell command executing which the error occurred
    args[1] -- stderr and stdout of the command
    args[2] -- exit status of the command
    """

    def __init__(self, cmd, output, sc = None):
        Error.__init__(self, cmd, output, sc)

def exitstatus (status):
    """Return child exit status, if child terminated normally, None otherwise.

    Parameter status: child process status information as returned by os.wait(),
    or os.waitpid(),
    os.system(), close() method of file object returned by os.popen(),
    commands.getstatusoutput()
    """
    
    if os.WIFEXITED(status):
        return os.WEXITSTATUS(status)
    else:
        return None
