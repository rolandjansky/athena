# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file : Valkyrie.py
# @author: Sebastien Binet <binet@cern.ch> 
# @purpose: Scripting interface to athena.py, from Valgrind
#
# @date: Januray 2007
#

"""
A set of python objects to ease the scripting of Athena from
a python script - and ran from Valgrind.
"""

__author__  = "$Author: binet $"
__version__ = "$Revision: 1.1.1.2 $"

import sys
import os
import commands
import subprocess

from AthenaCommon.ChapPy import *

from ValgrindFlags import jobproperties

class Valkyrie(object):

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

        class Valgrind:
            InstrumentAtStart = False
            ToggleCollect     = None
            ToolName          = "Nullgrind"
            TraceChildren     = True
            pass
        
    def __fetchValgrind(self):
        sc, out = commands.getstatusoutput( "which valgrind" )
        if sc != 0:
            raise RuntimeError, "Could not fetch valgrind executable: %s" % out
        else:
            return os.path.realpath(os.path.expandvars(out))

        pass

    def __getValgrindCmd(self):
        valFlags = jobproperties.ValgrindFlags
        #valFlags.print_JobProperties('tree&value')

        if valFlags.doMemCheck.get_Value():
            flags   = valFlags.MemCheck
            options = [
                "--tool=memcheck",
                "--show-reachable=%s" % flags.ShowReachable.get_Value(),
                "--leak-check=%s"     % flags.LeakCheck.get_Value(),
                "--leak-resolution=%s"% flags.LeakResolution.get_Value(),
                #"--gen-suppressions=all",
                ]

        if valFlags.doCallGrind.get_Value():
            flags   = valFlags.CallGrind
            options = [
                "--tool=callgrind",
                "--dump-instr=%s"      % flags.DumpInstruction.get_Value(),
                "--collect-jumps=%s"   % flags.CollectJumps.get_Value(),
                "--collect-systime=%s" % flags.CollectSystemTime.get_Value(),
                "--instr-atstart=%s"   % flags.InstrumentAtStart.get_Value(),
                "--collect-atstart=%s" % flags.CollectAtStart.get_Value(),
                ]
            if flags.ToggleCollect.statusOn:
                options += [ "--toggle-collect=%s" %
                             flags.ToggleCollect.get_Value() ]

        if valFlags.doMassif.get_Value():
            flags   = valFlags.Massif
            options = [
                "--tool=massif",
                "--heap=%s"   % flags.Heap.get_Value(),
                "--stacks=%s" % flags.Stacks.get_Value(),
                "--format=%s" % flags.Format.get_Value(),
                "--depth=%i"  % flags.Depth.get_Value()
                ]

        cmd = [
            self.valgrind,
            "--trace-children=%s" % valFlags.TraceChildren.get_Value(),
            "--num-callers=%i"    % valFlags.NumCallers.get_Value(),
            "--suppressions=%s"   % valFlags.SuppressionFile.get_Value(),
            ]

        return cmd + options
        
    def __init__( self,
                  jobOptions   = [ ],
                  cmdOptions   = None,
                  logFile      = open( "/dev/null", "w" ),
                  logLevel     = None,
                  showIncludes = False,
                  checkLeak    = False ):
        object.__init__( self )

        self.valgrind = self.__fetchValgrind()

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
            except IOError, err:
                print err
                print "Opening /dev/null for logging..."
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
        
    def run( self, monitor = sys.stdout ):

        sc, out = commands.getstatusoutput( "which athena.py" )
        if sc != 0:
            raise RuntimeError, "Could not fetch athena.py executable: %s"% out
        else:
            self.bin = os.path.realpath(os.path.expandvars(out))
            pass

        # prepare logFile
        try:
            self.logFile.truncate(0)
        except IOError,err:
            pass

        try:
            self.logFile.seek(0)
        except IOError,err:
            pass

        # build the (athena) command
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
                                  if jobO.name() != None ] )

        # add AthAppMgr commands
        if isinstance( self.EvtMax, int ):
            jo = JobOptionsCmd( "theApp.EvtMax = %i" % self.EvtMax )
            cmd += [ jo.name() ]
            pass

        # add ValgrindSvc to jobOptions
        if True:
            jo = JobOptionsCmd(\
"""\n
###############################
# Load Valgrind service
###############################
from Valkyrie.ValkyrieConf import ValgrindSvc
svcMgr += ValgrindSvc( OutputLevel = VERBOSE )
theApp.CreateSvc += [ svcMgr.ValgrindSvc.getFullJobOptName() ]
""" )
            cmd += [ jo.name() ]
            
        ## hooking the Valgrind part
        valgrindCmd = self.__getValgrindCmd()

        #print "cmd::",valgrindCmd +cmd
        p = subprocess.Popen( args = valgrindCmd + cmd,
                              stdout = self.logFile,
                              stderr = self.logFile )
        monitor.write(" :::running [")
        monitor.flush()
        while p.poll() == None:
            monitor.write(".")
            monitor.flush()
            time.sleep(5)
            pass
        monitor.write("]")
        monitor.flush()
        
        sc = p.returncode 
        if sc != 0:
            monitor.writelines( " ==> [ERR]" + os.linesep )
            print "Problem while running Athena !!"
            dump(self.logFile)
            pass
        else:
            monitor.writelines( " ==> [OK]" + os.linesep )
            pass

        self.logFile.flush()
        return sc
    
    pass # Valkyrie
