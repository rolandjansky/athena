# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file : ChapPy.py
# @author: Sebastien Binet <binet@cern.ch> 
# @purpose: Scripting interface to athena.py
#
# @date: July 2006
#

from __future__ import with_statement, print_function

"""
A set of python objects to ease the scripting of Athena from
a python script. It might even become more easy when Configurables
come in production.
"""

__author__  = "$Author: binet $"
__version__ = "$Revision: 1.3 $"

import sys
import os
import subprocess
import time
import six
from past.builtins import basestring

def dump( buf, stdout = sys.stdout ):
    """
    Little helper which prints out the content of a file descriptor
    """
    fname = None
    if isinstance(buf, str):
        fname = buf
    if six.PY3:
        import io
        file = io.IOBase # noqa: F811
    if isinstance(buf, file):
        fname = buf.name
    with open(fname, 'r') as fd:
        map(stdout.write, (l for l in fd.readlines()))
        pass
    return

class JobOptions( object ):

    def __init__( self, fileName = None ):
        object.__init__( self )
        self.fileName = fileName
        return
    
    def name(self):
        """
        return the name of the JobOptions.
         - a regular JobOptions file (from the release)
         - a user JobOptions file (laying in some private area)
         - a 'compiled' JobOptions file (created from a set of one-liners)
        """
        return self.fileName

    def __repr__( self ):
        return self.name()
    
    pass # JobOptions

from tempfile import NamedTemporaryFile
class JobOptionsCmd( JobOptions ):

    def __init__( self, cmds = [] ):

        # massaging of input variables
        if isinstance(cmds, str):
            cmds = [ cmds ]
            pass
        if not isinstance(cmds, list):
            cmds = [ cmds ]
            pass

        JobOptions.__init__( self, fileName = None )
        self.cmds    = cmds
        self.tmpFile = NamedTemporaryFile( suffix = ".py", mode = 'w+' )
        return

    def name( self ):

        # wipeout any previous content of the file
        self.tmpFile.file.truncate(0)

        # go at the beginning of the file to write our commands
        self.tmpFile.file.seek(0)
        
        for l in self.cmds:
            self.tmpFile.writelines( l + os.linesep )
            pass

        # commit our changes
        self.tmpFile.file.flush()

        return self.tmpFile.name
    
    pass # JobOptionsCmd

class Athena( object ):

    class Options:
        class LogLevel:
            ALL     = "ALL"
            VERBOSE = "VERBOSE"
            DEBUG   = "DEBUG"
            INFO    = "INFO"
            WARNING = "WARNING"
            ERROR   = "ERROR"
            FATAL   = "FATAL"
            pass

    def __init__( self,
                  jobOptions   = [ ],
                  cmdOptions   = None,
                  logFile      = open( "/dev/null", "w" ),
                  logLevel     = None,
                  showIncludes = False,
                  checkLeak    = False ):
        object.__init__( self )

        ## check that we are not called/constructed from a normal Athena job
        if os.path.basename( sys.argv[0] ) == "athena.py":
            raise RuntimeError("This is not a normal Athena job !! "
                               "Run with 'chappy.py myjob.py' instead !")
        
        self.bin          = None
        self.cmdOptions   = cmdOptions
        self.logLevel     = logLevel
        self.showIncludes = showIncludes
        self.checkLeak    = checkLeak

        if isinstance(jobOptions, JobOptions):
            jobOptions = [ jobOptions ]
            pass
        if isinstance(jobOptions, str):
            # assume it is the name of the JobOptions file
            jobOptions = [ JobOptions(jobOptions) ]
            pass
        
        self.jobOptions = jobOptions

        if isinstance(logFile, str):
            try:
                logFile = open( logFile, "w" )
            except IOError as err:
                print (err)
                print ("Opening /dev/null for logging...")
                logFile = open( "/dev/null", "w" )
                pass
            pass
        self.logFile    = logFile

        # AthAppMgr forwarding
        self.EvtMax = None
        #self.InputCollections = None
        return

    def __repr__( self ):

        bin        = self.bin      or "<not yet fetched>"
        logLevel   = self.logLevel or "<default>"
        jobOptions = [ jo.name() for jo in self.jobOptions if jo.name() ]
        
        s = os.linesep.join( [
            "",
            "+-- Athena (scripting) Object ---",
            "| executable : %s" % bin,
            "| cmdOptions : %s" % self.cmdOptions,
            "| log level  : %s" % logLevel,
            "| show incls : %s" % self.showIncludes,
            "| check leaks: %s" % self.checkLeak,
            "| job options: %s" % jobOptions,
            "| log file   : %s" % self.logFile.name,
            "+--------------------------------",
            ] )
        return s
        
    def run( self, monitor = sys.stdout, env = None ):

        if env is None:
            import os
            env = dict(os.environ)
            pass

        out = subprocess.check_output( ['/bin/sh', '-c', "which athena.py" ] ).decode().strip()
        self.bin = os.path.realpath(os.path.expandvars(out.encode().strip()))

        # prepare logFile
        try:
            self.logFile.truncate(0)
        except IOError:
            pass

        try:
            self.logFile.seek(0)
        except IOError:
            pass

        # build the command
        cmd = [ self.bin, "--batch" ]
        if isinstance(self.cmdOptions, list):
            cmd.extend( self.cmdOptions )
            pass
        
        # check run options
        if self.logLevel:
            cmd.append( "--loglevel %s" % self.logLevel )
            pass

        if self.showIncludes:
            cmd.append( "--showincludes" )
            pass

        if self.checkLeak:
            cmd.append( "--leak-check-execute" )
            pass
        
        # build the jobOptions command line
        cmd.extend( [ jobO.name() for jobO in self.jobOptions
                                  if jobO.name() is not None ] )

        # add AthAppMgr commands
        if isinstance( self.EvtMax, int ):
            jo = JobOptionsCmd( "theApp.EvtMax = %i" % self.EvtMax )
            cmd += [ jo.name() ]
            pass

        p = subprocess.Popen( args = ["/bin/sh"] + cmd,
                              stdout = self.logFile,
                              stderr = self.logFile,
                              env = env )
        monitor.write(" :::running [")
        monitor.flush()
        while p.poll() is None:
            monitor.write(".")
            monitor.flush()
            time.sleep(5)
            pass
        monitor.write("]")
        monitor.flush()
        
        sc = p.returncode 
        if sc != 0:
            monitor.writelines( " ==> [ERR]" + os.linesep )
            print ("Problem while running Athena !!")
            dump(self.logFile)
            pass
        else:
            monitor.writelines( " ==> [OK]" + os.linesep )
            pass

        self.logFile.flush()
        return sc
    
    pass # Athena

### utils' functions ----------------------------------------------------------
def _make_jobo(job):
    import tempfile
    jobo = tempfile.NamedTemporaryFile(suffix='-jobo.py')
    import textwrap
    jobo.write(textwrap.dedent(job))
    jobo.flush()
    return jobo

### AthenaApp
class AthenaApp(object):
    '''another way at scripting `athena`
    '''

    def __init__(self, cmdlineargs=None):

        import tempfile
        self._jobo = tempfile.NamedTemporaryFile(suffix='-jobo.py', mode='w+')
        if cmdlineargs is None:
            cmdlineargs = []
        if isinstance(cmdlineargs, basestring):
            cmdlineargs = cmdlineargs.split()
        self._cmdlineargs = cmdlineargs[:]
        
        return

    def include(self, jobo_name):
        self._jobo.write('include("%s")\n'%jobo_name)
        self._jobo.flush()
        
    def __lshift__(self, o):
        if isinstance(o, str):
            import textwrap
            self._jobo.write(textwrap.dedent(o))
            self._jobo.flush()
            return
        raise TypeError('unexpected type %s'%type(o))
    
    def run(self, stdout=None, env=None):
        import os
        import subprocess as sub
        if env is None:
            env=dict(os.environ)
        athena_exe = sub.Popen(['which', 'athena.py'],
                               stdout=sub.PIPE,
                               env=env).communicate()[0].strip()
        
        athena_exe = os.path.expanduser(os.path.expandvars(athena_exe))
        athena_exe = os.path.realpath(athena_exe)
        athena_exe = os.path.abspath(athena_exe)

        #sh = sub.Popen('/bin/sh', stdin=sub.PIPE, stdout=stdout)
        #sh.stdin.write('%s %s'%(athena_exe,self._jobo.name))
        if stdout == os.devnull:
            stdout = open(os.devnull,'w')
        cmd = [athena_exe] + self._cmdlineargs + [self._jobo.name]
        return sub.call(cmd,
                        stdout=stdout,
                        stderr=sub.STDOUT,
                        env=env)
    
