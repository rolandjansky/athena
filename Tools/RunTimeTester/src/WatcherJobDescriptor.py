# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Class descriptor for Watcher jobs. Watcher jobs 
iterate over some or all of the ATLAS offline release packages.
They are not Athena jobs.

Watcher descriptor has been found to need a lot of
athena job characteristics: it should be split off
into a derived class AthenaWatcherJobDescriptor....
"""

from Descriptor import Descriptor

class WatcherJobDescriptor(Descriptor):

    def __init__(self, paths, logger, jobSerialNumber, argDict={}):

        Descriptor.__init__(self, paths, jobSerialNumber, logger)
        if self.mode != 'Dummy': self.mode = 'Watcher'

        # a list of worker job minders
        self.confederation    = []
        self.package          = paths.package

        self.__dict__.update(argDict)

        self.name = self.jobGroup

        self.fixName()        
        self.setPathExtensions()
        self.setRunPath(self.pathExtensions)

    def setPathExtensions(self):

        # NOTE: we assume here that all watchers are watching athena jobs
        #       i.e ones with a package. Probably wrong long term, but we leave
        #       it as-is for now in order to go faster.
        self.pathExtensions = [
            ('package',    self.package),
            ('jobGroup',   self.jobGroup),
            ('jobName',    self.name),
            ('SN',         str(self.jobSerialNumber))
            ]
