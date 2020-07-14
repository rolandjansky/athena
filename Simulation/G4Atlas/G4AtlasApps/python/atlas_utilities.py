# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Provide different utilities like:

  MemorySnooper = process memory monitoring tool
  _frozen = a base class / metaclass for making objects immutable
"""

from __future__ import print_function

__author__  = 'A. Dell`Acqua, M. Gallas, A. Di Simone'

## ATLAS generic scintillator SD
class ScintillatorSD(object):
    """
    ScintillatorSD sensitive detector class.

    Common SD ('ScintillatorSD') for all the scintillators in a given setup. The
    hits will be collected in the same hit collection name but with different
    collection name.
    """
    def __init__(self, volume_list=''):
        """
        The list of volumes in which the ScintillatorSD will be applied must be
        provided here, example for the scintillator S1 the name is S1
        """
        self.List_Volumes=volume_list
        import AtlasG4Eng
        AtlasG4Eng.G4Eng.load_Lib('G4AncillarySD')
        AtlasG4Eng.G4Eng.load_Dict('G4AncillarySDDict')
        SD_HChandler = AtlasG4Eng.G4Eng.gbl.ScintillatorSD_PyHandler()
        #import PyG4Atlas
        ## SD = PyG4Atlas.SenDetector('G4AncillarySD', 'ScintillatorSD', 'ScintillatorSD') #FIXME SD to be migrated ATLASSIM-1752
        ## # Make a ScintillatorSD hit collection and each associated volume has a
        ## # collection name
        ## for v_n in self.List_Volumes:
        ##     SD.add_Volume(v_n+"::"+v_n)
        ##     AtlasG4Eng.G4Eng.log.debug('atlas_utilities:: ScintillatorSD:_init: added the volume '+v_n)
        ##     SD_HChandler.BuildDescription(v_n, v_n, v_n, True)
        ##     AtlasG4Eng.G4Eng.log.debug('atlas_utilities:: ScintillatorSD:_init: added hit collection name '+v_n)
        ## AtlasG4Eng.G4Eng.add_SenDetector(SD)


## Check memory
from AthenaPython import PyAthena
class MemorySnooper(PyAthena.Alg):
    """
    Python algorithm for tracking of memory usage

    It can be used from any jobOption by adding the lines:

      from G4AtlasApps.atlas_utilities import MemorySnooper
      theApp.TopAlg += MemorySnooper()
    """

    def __init__(self, name="MemorySnooper"):
        super(MemorySnooper, self).__init__(name=name)
        self.name = name
        self.file = open("memorytest.out", "a")
        self.memoV = 0.
        self.memoR = 0.
        self.eventNumber = 0
        self.now = 0

    def memcheck(self, app_phase):
        """
        Checks the memory at any time and print it to a file.
        """
        import os, string, time
        now = time.time()
        elapsed_time = now - self.now
        self.now = now

        pid = os.getpid()
        statm = open('/proc/%d/statm' % pid, 'r').readlines()[0].split()
        stat = open('/proc/%d/stat' % pid, 'r').readlines()[0].split()
        print ('*** The Application is in %s with %d kB of VMMEM and %d kB of RSS' % (app_phase,int(stat[22])/1048.576, int(statm[1])*4))
        self.file.write("%d %d %d %f\n" %  (self.eventNumber, int(stat[22])/1048.576, int(statm[1])*4, elapsed_time) )

    def memcheck_reduced(self,app_phase):
        """
        Checks the memory at any time and print it to a file and stdout
        only if is different from the previous value obtained.
        This method is for the use in the execute method (event by event).
        """
        import os, string, time
        now = time.time()
        elapsed_time = now - self.now
        self.now = now

        pid = os.getpid()
        statm = open('/proc/%d/statm' % pid, 'r').readlines()[0].split()
        stat = open('/proc/%d/stat' % pid, 'r').readlines()[0].split()
        if (int(stat[22])/1048.576 != self.memoV or int(statm[0])*4 != self.memoR):
            self.memoV = int(stat[22])/1048.576
            self.memoR = int(statm[0])*4
            print ('*** The Application is in %s with %d kB of VMMEM and %d kB of RSS' % (app_phase,int(stat[22])/1048.576, int(statm[1])*4))
            self.file.write("%d %d %d %f\n" %  (self.eventNumber, int(stat[22])/1048.576, int(statm[1])*4, elapsed_time ) )

    def initialize(self, reinit=False):
        import time
        self.eventNumber = -1
        self.memcheck('initialize')
        self.eventNumber = 0
        self.now = time.time()
        return True

    def start(self):
        self.eventNumber = -2
        self.memcheck('start')
        self.eventNumber = 0
        return True

    def execute(self):
        self.memcheck('execute')
        self.eventNumber += 1
        return True

    def finalize(self):
        self.eventNumber = -3
        self.memcheck('finalize')
        self.file.close()
        return True


## Tools for freezing configuration objects
def _freeze(set):
    """Raise an error when trying to set an undeclared name."""

    def set_attr(self,name,value):
        if hasattr(self,name): set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr

class _frozen(object):
    """Subclasses of _frozen are frozen.

     i.e. it is impossible to add new attributes to them and their
     instances."""
    __setattr__=_freeze(object.__setattr__)
    class __metaclass__(type):
        __setattr__=_freeze(type.__setattr__)
