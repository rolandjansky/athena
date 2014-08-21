# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, os.path, string, sys
from time import time
from GaudiPython import PyAlgorithm
from AthenaCommon import Constants, Logging, SystemOfUnits
from AthenaCommon.AppMgr import theApp
from AthenaCommon.Include import include
import PyCintex

# TODO: Rename to AppProfiler, to avoid class/variable confusion
class _app_profiler(object):
    """
    Monitor the memory and CPU time used.

    This is used at different points in the initialization phase of the
    application. It runs if G4AtlasEngine.log.level <= 10.
    """

    def __init__(self, G4Eng):
        self._log = G4Eng.log
        self._vlevel = self._log.level
        self._firstTime = time()
        self._outputfileName = 'G4InitStats.out'
        self._isfirstCall = False

    def __call__(self, action):
        if self._vlevel <= 10 or self._log.level <= 10:
            pid = os.getpid()
            statm = string.split(open('/proc/%d/statm' % pid, 'r').readlines()[0])
            stat = string.split(open('/proc/%d/stat' % pid, 'r').readlines()[0])
            now = time()
            self._log.debug('G4AtlasEngine::app_profiler')
            print "                                                  "
            print "      G4AtlasEngine::app_profiler at ", action
            print "      Memory status: Virtual %d kB, RSS %d kB" % (int(stat[22])/1048.576,int(statm[1])*4)
            print "      Time since beginning: %f s" % (now - self._firstTime)
            print "                                                  "
            filemode = "a"
            if not self._isfirstCall:
                filemode = "w"
                self._isfirstCall = True
            outputfile = open(self._outputfileName, filemode)
            outputfile.write("%s %d %d %f\n" % (action, int(stat[22])/1048.576, int(statm[1])*4, now-self._firstTime))
