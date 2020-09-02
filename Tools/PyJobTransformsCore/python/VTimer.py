# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import pickle
import os, time
from resource import getrusage, RUSAGE_SELF, RUSAGE_CHILDREN
    
def pymon():
    """Borrowed from PyMonUtils.py. Using RUSAGE_SELF and RUSAGE_CHILDREN."""
    cpu = -1
    vmem = -1
    rss = -1
    try:
        mem = open( '/proc/%d/statm' % os.getpid(), 'r' )
        cpu_self = getrusage( RUSAGE_SELF )
        cpu_children = getrusage( RUSAGE_CHILDREN )
        cpu = cpu_self.ru_utime + cpu_self.ru_stime + cpu_children.ru_utime + cpu_children.ru_stime # in seconds
        mem = mem.readlines()[ 0 ].split()
        pageSize = os.sysconf( 'SC_PAGE_SIZE' ) / ( 1024. * 1024. )
        vmem = int( mem[0] ) * pageSize
        rss  = int( mem[1] ) * pageSize
    finally:
        return cpu, vmem, rss
    


class VTimerContent( object ):
    def __init__( self ):
        self.reset()

    def toDict( self ):
        return { 'wallTime_Start' : self.wallTime_Start,
                 'wallTime_Stop' : self.wallTime_Stop,
                 'wallTime' : self.wallTime,
                 'cpuTime_Start' : self.cpuTime_Start,
                 'cpuTime_Stop' : self.cpuTime_Stop,
                 'cpuTime' : self.cpuTime,
                 'rss_Start' : self.rss_Start,
                 'rss_Stop' : self.rss_Stop,
                 'rss' : self.rss,
                 'vmem_Start' : self.vmem_Start,
                 'vmem_Stop' : self.vmem_Stop,
                 'vmem' : self.vmem }

    def reset( self ):
        self.started = None
        self.wallTime_Start = 0
        self.cpuTime_Start = 0
        self.wallTime_Stop = 0
        self.cpuTime_Stop = 0
        self.rss_Start = 0
        self.vmem_Start = 0
        self.rss_Stop = 0
        self.vmem_Stop = 0
        self.cpuTime = 0
        self.wallTime = 0
        self.rss = 0
        self.vmem = 0

class VTimer( object ):
    def __init__( self, name = 'VTimer', logger = None ):
        self.name = name
        self._resultsDict = None
        self._resultsStack = []
        self._resultsFile = None
        self.logger = logger

    def reset( self ):
        self._resultsDict = None
        self._resultsStack = []
        self._resultsFile = None
        self._print( 'All timers have been deleted.' )
        return

    def _makeNewResultsDict( self, name ):
        return { 'name' : name, 'vTimerContent' : VTimerContent(), 'trfList' : [] }

    def _print( self, x, severity = 'info', altTimerName = None ):
        if altTimerName is None:
            tName = self.name
        else:
            tName = altTimerName
        x = '%s %s' % ( tName, x )
        try:
            getattr( self.logger, severity )( x )
        except Exception:
            print (x)

    def toHMS( self, seconds = 0 ):
        if seconds < 0:
            return '%s secs' % seconds
        seconds = int( seconds )
        hours = seconds / 3600
        seconds -= 3600 * hours
        minutes = seconds / 60
        seconds -= 60 * minutes
        return '%02d hrs %02d mins %02d secs' % ( hours, minutes, seconds )

    def start( self, name = 'DEFAULT' ):
        try:
            currentResultsDict = self._resultsStack[ -1 ]
        except IndexError: # Initial use when _resultsDict is still None.
            self._resultsDict = self._makeNewResultsDict( name )
            self._resultsStack.append( self._resultsDict )
            currentResultsDict = self._resultsDict
        else: # normal case. New sub-trf started.
            newResultsDict = self._makeNewResultsDict( name )
            currentResultsDict[ 'trfList' ].append( newResultsDict )
            self._resultsStack.append( newResultsDict )
            currentResultsDict = newResultsDict
        tContent = currentResultsDict[ 'vTimerContent' ]
        tContent.started = True
        tContent.wallTime_Start = time.time()
        tContent.cpuTime_Start, tContent.vmem_Start, tContent.rss_Start = pymon()
        self._print( 'start [%s] wall-time: %s, vmem: %s Mb, rss: %s Mb' % ( name, time.strftime("%a %b %d %H:%M:%S %Y %Z", time.localtime( tContent.wallTime_Start ) ), tContent.vmem_Start, tContent.rss_Start ) )

    def _getDiffFromComponentTrfs( self, resultsDict ):
        trfList = resultsDict[ 'trfList' ]
        _rss, _vmem, _wallTime, _cpuTime = 0, 0, 0, 0
        for rDict in trfList:
            tContent = rDict[ 'vTimerContent' ]
            _rss += tContent.rss
            _vmem += tContent.vmem
            _wallTime += tContent.wallTime
            _cpuTime += tContent.cpuTime
        tContent = resultsDict[ 'vTimerContent' ]
        return tContent.wallTime - _wallTime, tContent.cpuTime - _cpuTime, tContent.vmem - _vmem, tContent.rss - _rss

    def stop( self, name = 'DEFAULT'):
        try:
            currentResultsDict = self._resultsStack.pop( -1 )
        except IndexError:
            self._print( 'not initialised.' )
            return
        tContent = currentResultsDict[ 'vTimerContent' ]
        # check if correct timer is to be stopped.
        if currentResultsDict[ 'name' ] != name:
            self._print( 'trying to stop wrong timer. Current timer is %s.' % currentResultsDict[ 'name' ] )
            self._resultsStack.append( currentResultsDict ) # re-insert back onto stack.
            return
        if tContent.started is None:
            self._print( 'not started [%s]' % name )
            # not re-inserting back onto _resultsStack as should not be on the stack in the first place.
            return
        elif tContent.started is False:
            self._print( 'already stopped [%s]' % name )
            # not re-inserting back onto _resultsStack as should not be on the stack in the first place.
            return
        tContent.started = False # stop timer
        tContent.cpuTime_Stop, tContent.vmem_Stop, tContent.rss_Stop = pymon()
        tContent.wallTime_Stop = time.time()
        tContent.cpuTime = tContent.cpuTime_Stop - tContent.cpuTime_Start
        tContent.wallTime = tContent.wallTime_Stop - tContent.wallTime_Start
        tContent.rss = tContent.rss_Stop - tContent.rss_Start
        tContent.vmem = tContent.vmem_Stop - tContent.vmem_Start
        # Insert an 'Other' entry in currentResultsDict['trfList']
        # This special entry will be the diff between the currentResultsDict and it's component trfs.
        # Any other time consuming executions not already listed as a component trf will be 'caught'
        # in this artificially created component trf entry.
        if currentResultsDict[ 'trfList' ]:
            otherResultsDict = self._makeNewResultsDict( 'Other' )
            other_tContent = otherResultsDict[ 'vTimerContent' ]
            other_tContent.started = False 
            other_tContent.wallTime, other_tContent.cpuTime, other_tContent.vmem, other_tContent.rss = self._getDiffFromComponentTrfs( currentResultsDict )
            currentResultsDict[ 'trfList' ].append( otherResultsDict )
        self._print( 'stop [%s] wall-time: %s, vmem: %s Mb, rss: %s Mb' % ( name, time.strftime("%a %b %d %H:%M:%S %Y %Z", time.localtime( tContent.wallTime_Stop ) ), tContent.vmem_Stop, tContent.rss_Stop ) )
        self.report( currentResultsDict )

    def report( self, resultsDict ):
        name = resultsDict[ 'name' ]
        tContent = resultsDict[ 'vTimerContent' ]
        if tContent.started is None: # timer not yet started
            self._print( 'not started [%s]' % name )
            return
        elif tContent.started: # VTimer still running.
            _cpuTime_Stop, _vmem_Stop, _rss_Stop = pymon()
            _wallTime_Stop = time.time()
            _rss = _rss_Stop - tContent.rss_Start
            _vmem = _vmem_Stop - tContent.vmem_Start
            _wallTime = _wallTime_Stop - tContent.wallTime_Start
            _cpuTime = _cpuTime_Stop - tContent.cpuTime_Start        
        else: # False. VTimer already stopped.
            _rss = tContent.rss
            _vmem = tContent.vmem
            _cpuTime = tContent.cpuTime
            _wallTime = tContent.wallTime
        self._print( 'report [%s] wall-time: %s, cpu-time: %s, vmem: %s Mb, rss: %s Mb' % ( name, self.toHMS( _wallTime ), self.toHMS( _cpuTime ), _vmem, _rss ) )

    def setResultsFile( self, fileName ):
        """Write results file for composite/top level transform only."""
        if fileName is None:
            self._resultsFile = None # reset results file
            return
        if self._resultsFile is None and not os.path.exists( fileName ):
            self._resultsFile = fileName

    def writeResultsToFile( self, fileName, force = None ):
        if self._resultsFile != fileName and not force: 
            return
        try:
            fileObj = open( fileName, 'w' )
        except Exception:
            self._print( ' : Could not open %s for writing.' % fileName )
            return
        pickle.dump( self._resultsDict, fileObj, pickle.HIGHEST_PROTOCOL )
#        tDict = {}
#        for t, tResult in self._resultsDict.iteritems():
#            if t == 'DEFAULT':
#                continue
#            tDict[ t ] = tContent.toDict()
#        pickle.dump( tDict, fileObj, pickle.HIGHEST_PROTOCOL )
        fileObj.close()

vTimer = VTimer( 'Transform Timer' )
