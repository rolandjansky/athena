# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file : iobench.py
# @author: Sebastien Binet <binet@cern.ch>
# @purpose: Measure the I/O performances of an application
#
# @date: July 2006
#

"""
A set of python objects to measure the I/O performances of an application.
"""

__author__  = "$Author: binet $"
__version__ = "$Revision: 1.4 $"

import sys
import resource
import time
import os

###-----------------------------------------------------
def getResources( usageMode = resource.RUSAGE_CHILDREN ):
    """
    A factory method to fetch the used resources at a given 't' time.
    Default mode is 'resource.RUSAGE_CHILDREN'
    """
    return Resource( utime = resource.getrusage( usageMode ).ru_utime,
                     stime = resource.getrusage( usageMode ).ru_stime,
                     rtime = time.time() )

class Resource(object):
    """
    A simple class to hold the resources used by a program:
     - user   time
     - system time
     - real   time
    """
    
    def __init__( self,
                  utime, stime, rtime ):
        object.__init__(self)

        self.utime = utime
        self.stime = stime
        self.rtime = rtime

        return

    def __repr__(self):
        s = os.linesep.join( [ "user : %s" % str( self.user() ),
                               "sys  : %s" % str( self.sys()  ),
                               "real : %s" % str( self.real() ) ] )
        return s

    def __add__(self, rhs):
        return Resource( self.utime + rhs.utime,
                         self.stime + rhs.stime,
                         self.rtime + rhs.rtime )

    def __iadd__(self, rhs):
        self.utime += rhs.utime
        self.stime += rhs.stime
        self.rtime += rhs.rtime
        return self

    def __sub__(self, rhs):
        return Resource( self.utime - rhs.utime,
                         self.stime - rhs.stime,
                         self.rtime - rhs.rtime )

    def __isub__(self, rhs):
        self.utime -= rhs.utime
        self.stime -= rhs.stime
        self.rtime -= rhs.rtime
        return self
    
    def user( self ):
        return self.utime

    def sys( self ):
        return self.stime

    def real( self ):
        return self.rtime

    pass # Resource

class Bench(object):

    def __init__( self,
                  nTimes = 1,
                  stdout = sys.stdout,
                  stderr = sys.stderr ):
        object.__init__(self)

        self.nTimes = nTimes
        self.data = []

    def run( self, fct, *args, **kwargs ):

        self.data = []

        for i in range(self.nTimes):
            iStart = getResources()
            out = fct( *args, **kwargs )
            self.data.append( getResources() - iStart )
            pass

        return

    def stats( self ):
        
        mean = Resource( 0., 0., 0. )
        for d in self.data:
            mean += d
            pass

        print ""
        print "## stats : (nbenchs = %i)" % self.nTimes
        print " time<User> = %8.3f s" % ( mean.user() /  float( self.nTimes ) )
        print " time<Sys > = %8.3f s" % ( mean.sys()  /  float( self.nTimes ) )
        print " time<Real> = %8.3f s" % ( mean.real() /  float( self.nTimes ) )
        
        return

    def save( self, fileName = "iobench.log" ):
        f = open( fileName, "w" )
        for data in self.data:
            f.writelines( "%s %s %s %s" % ( data.user(),
                                            data.sys(),
                                            data.real(),
                                            os.linesep ) )
            pass
        f.close()
        return
    
    pass # Bench

##
## ---- Athena specific part ----
##

import os
import ConfigParser
class ChronoStatsOutputParser( ConfigParser.ConfigParser ):
    """Subclass and specialize ConfigParser to make it case-sensitive
    """
    def optionxform( self, optionStr ):
        return optionStr

class ChronoStat(object):
    """
    A class mirroring the one from the (Gaudi) ChronoStatSvc which models a
    chrono measurement.
    It holds the following informations:
      - total time
      - min
      - mean
      - RMS
      - max
      - number of 'events'
    """
    def __init__( self, nbr, min, max, mean, rms, total ):
        object.__init__(self)
        self.total = total
        self.min   = min
        self.mean  = mean
        self.rms   = rms
        self.max   = max
        self.nbr   = nbr
        pass

    def __repr__(self):
        return os.linesep.join( [
            "min   = %s" % str( self.min   ),
            "max   = %s" % str( self.max   ),
            "mean  = %s" % str( self.mean  ),
            "RMS   = %s" % str( self.RMS   ),
            "total = %s" % str( self.total ),
            "#evts = %s" % str( self.nbr   ),
            ] )
    
class ChronoStatReport(object):

    ReadKeyHdr  = "cObj_"
    WriteKeyHdr = "cRep_"
    KeySep      = "#"
    
    """
    This class stores the report of the ChronoStatSvc dump from Athena/Gaudi.
    """
    def __init__( self, fileName ):
        """
        fileName is supposed to contain the output of the ChronoStatSvc which
        has been configured with 'ChronoStatSvc.ChronoDestinationCout = True'
        """
        object.__init__( self )

        self.logFile = ChronoStatsOutputParser()
        self.logFile.read( fileName )

        self.stats = { }
        self.chronoStats = { }
        self.buildStats()
        pass

    def buildStats( self ):

        for hdr in self.logFile.sections():
            stat = [
                ChronoStat(
                    nbr  = int  (self.logFile.get(hdr, "cpu_%s_nbr"   % item)),
                    min  = float(self.logFile.get(hdr, "cpu_%s_min"   % item)),
                    max  = float(self.logFile.get(hdr, "cpu_%s_max"   % item)),
                    mean = float(self.logFile.get(hdr, "cpu_%s_mean"  % item)),
                    rms  = float(self.logFile.get(hdr, "cpu_%s_RMS"   % item)),
                    total= float(self.logFile.get(hdr, "cpu_%s_total" % item))
                    ) \
                for item in ['user', 'system', 'real']
                ]
                     
            self.chronoStats[hdr] = {
                'user'   : stat[0],
                'system' : stat[1],
                'real'   : stat[2],
                }
            del stat
            self.stats[hdr] = Resource(
                float(self.logFile.get(hdr, "cpu_user_mean")),
                float(self.logFile.get(hdr, "cpu_system_mean")),
                float(self.logFile.get(hdr, "cpu_real_mean"))
                )
        pass

    def getStats(self, key):
        return self.stats[key]

    def getPoolStats(self, storeGateKey, ioMode = "r" ):
        ioKey = storeGateKey
        # 'normalize' key
        if ioMode.lower() == "r":
            ioKey = storeGateKey.replace(ChronoStatReport.ReadKeyHdr,"")
            ioKey = ChronoStatReport.ReadKeyHdr + ioKey
        elif ioMode.lower() == "w":
            ioKey = storeGateKey.replace(ChronoStatReport.WriteKeyHdr,"")
            ioKey = ChronoStatReport.WriteKeyHdr + ioKey
        else:
            pass

        if not self.stats.has_key( ioKey ):
            print "Warning: no such key [%s] in stats !" % ioKey
            print "Available keys:",self.stats.keys()
            pass
        return self.stats[ ioKey ]

    def getPoolKeys(self):
        return [ k for k in self.stats.keys() \
                 if k.count(ChronoStatReport.WriteKeyHdr) > 0 ]
    
    pass # ChronoStatReport

            
###-----------------------------------------------------
## For compatibility with ATN tests
def workDir( fileName ):
    """Function to provide an automatic work dir, compatible with ATN tests
    """
    if os.environ.has_key('ATN_WORK_AREA'):
        workArea = os.environ['ATN_WORK_AREA']
    else:
        workArea = "/tmp"
        pass
    if not os.path.exists(workArea):
        os.makedirs(workArea)
    return os.path.join( workArea, fileName )

class ScOutput(object):

    def __init__(self, statusCode = 0, outputLog = ""):
        object.__init__(self)
        self.sc  = statusCode
        self.out = outputLog
        return

    pass # ScOutput

###-----------------------------------------------------
## Little helper to validate output of jobs
def doValidation( dbFiles, key ):
    """Helper function to validate output of jobs against 'registered' ASCII
    files.
    """
    import commands
    from TestTools.iobench import ScOutput
    print "## Validation of ASCII files [%s]:" % key
    print "## %15s : %s" % ( 'ref', dbFiles[key]['ref'] )
    print "## %15s : %s" % ( 'chk', dbFiles[key]['chk'] )
    sc,out = commands.getstatusoutput( "diff %s %s" %
                                       ( dbFiles[key]['ref'],
                                         dbFiles[key]['chk'] ) )
    if sc == 0 and len(out) == 0:
        print "==> Validation [OK]"
    else:
        print "==> Validation [ERROR]"
        pass
    return ScOutput(sc,out)

###-----------------------------------------------------
## Little helper to validate output of jobs
def doPostCheck( validationName, refFileName, chkFileName, chkFilter ):
    import commands
    print "## Validation of [%s]" % validationName
    print "## ref:    %s"   % refFileName
    print "## chk:    %s"   % chkFileName
    print "## filter: [%s]" % chkFilter
    sc, out = commands.getstatusoutput( "cat %s | %s | diff -u %s -" % \
                                        ( chkFileName, chkFilter,
                                          refFileName ) )
    if sc == 0 and len(out) == 0: print "==> Validation [OK]"
    else:                         print "==> Validation [ERROR]\n",\
                                        "*"*80,out,"*"*80
    return ScOutput(sc, out)

###-----------------------------------------------------
from AthenaCommon import ChapPy
from tempfile import NamedTemporaryFile
class AthBench(object):

    def __init__( self,
                  athenaJob,
                  logFile,
                  nTimes = 1,
                  stdout = sys.stdout,
                  stderr = sys.stderr,
                  ioStatsLogFile = workDir("ioStats.out") ):
        object.__init__(self)

        self.athena = athenaJob
        self.logFileName = logFile
        self.nTimes = nTimes
        self.data = []
        self.chronoStats= []
        self.ioStatsFileName = ioStatsLogFile
        
    def run( self ):

        self.data = []
        self.chronoStats= []
        self.athena.jobOptions += [
            ChapPy.JobOptions( "TestTools/IoAuditor_fragment.py" ),
            ChapPy.JobOptionsCmd( "svcMgr.ChronoStatSvc.AsciiStatsOutputFile = \"%s\"" % self.ioStatsFileName )
            ]
        for i in range(self.nTimes):
            self.athena.logFile = open( "%s.%s" % (self.logFileName,i), "w" )
            iStart = getResources()
            out = self.athena.run()
            self.data.append( getResources() - iStart )
            self.chronoStats.append( ChronoStatReport(self.ioStatsFileName) )
            pass

        return

    def __printStat(self, res, title="Overall", isIO = False, keyType = ""):
        if isIO:
            u = "us/evt"
        else:
            u = "s"
            pass
        if   keyType == ChronoStatReport.WriteKeyHdr: keyType = "[WRITE]"
        elif keyType == ChronoStatReport.ReadKeyHdr:  keyType = "[READ]"
        else:
            pass
        print ""
        print "## stats : [%s] (nbenchs = %i) %s" % (title,
                                                     self.nTimes,
                                                     keyType)
        print " time<User> = %12.3f %s" % (res.user() / float(self.nTimes), u)
        print " time<Sys > = %12.3f %s" % (res.sys()  / float(self.nTimes), u)
        print " time<Real> = %12.3f %s" % (res.real() / float(self.nTimes), u)
        
    def ioStats(self, ioKeys = [], ioMode = "r"):

        if len(ioKeys) == 0:
            ioKeys = self.chronoStats[0].getPoolKeys()
            pass
        keyHdr = ""
        if ioMode.lower() == "r":
            keyHdr = ChronoStatReport.ReadKeyHdr
        elif ioMode.lower() == "w":
            keyHdr = ChronoStatReport.WriteKeyHdr
        for ioKey in ioKeys:
            mean = Resource( 0., 0., 0. )
            for d in self.chronoStats:
                mean += d.getPoolStats(ioKey, ioMode)
                pass
            self.__printStat(mean,
                             ioKey.replace(keyHdr,""),
                             isIO = True,
                             keyType = keyHdr)
            pass
        
        return
    
    def stats( self, doIoStats = False ):

        ## IO stats
        if doIoStats:
            self.ioStats()
        
        ## Overall stats
        mean = Resource( 0., 0., 0. )
        for d in self.data:
            mean += d
            pass
        self.__printStat(mean)
        return

    def save( self, fileName = "iobench.log" ):
        f = open( fileName, "w" )
        for data in self.data:
            f.writelines( "%s %s %s %s" % ( data.user(),
                                            data.sys(),
                                            data.real(),
                                            os.linesep ) )
            pass
        f.close()
        return
    
    pass # AthBench

class BenchSequence(object):
    """
    Class which holds results of benchs (exit codes) and declares victory
    when all the benchs returned successfully.
    """

    def __init__(self, name = "My Bench suite"):
        object.__init__(self)
        self.name   = name
        self.benchs = []

        return

    def __iadd__(self, rhs):
        if not isinstance( rhs, ScOutput ) and \
           not isinstance( rhs, list ):
            raise Exception, \
                  "attempt to add a '%s' to the BenchSuite !" % \
                  type(rhs).__name__
        
        if isinstance( rhs, ScOutput ):
            self.benchs.append( rhs )
            return self
        
        if isinstance( rhs, list ):
            for i in rhs:
                self += i
                pass
            return self

        return self

    def status(self):
        import math
        sc = 0
        for i in self.benchs:
            sc += math.fabs(i.sc)
            pass
        return sc == 0

    def printStatus(self):
        if self.status(): print "## [All tests SUCCESSFULLY completed]"
        else:             print "## [ERROR in at least one test !!]"
        return
    
    pass # BenchSequence
