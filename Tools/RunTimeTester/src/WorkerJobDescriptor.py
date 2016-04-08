#! /usr/local/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Class that stocks information about Worker jobs.
Responsible for setting up the run directory for a job. Eventually becomes a data member
of JobMinder.
"""

from Paths               import Paths
from Descriptor          import Descriptor

# -------------------------------------------------------------------------
# import logging
# logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------

class WorkerJobDescriptor(Descriptor):    
    
    def __init__(self, paths, logger, jobSerialNumber, argDict={}):

        self.package          = '' # set by Jobs

        # stuff to do with data sets
        self.datasets      = {}

        # init the base class
        Descriptor.__init__(self, paths, logger, jobSerialNumber, argDict)


    # --------------------------------------------------------------

    def hasData(self):
        return len(self.datasets.keys()) > 0 

    # --------------------------------------------------------------
    
    def __str__(self):
        "Converts self to a string"

        s  = Descriptor.__str__(self)
        s += 'package:             %s\n' % str(self.package)
        s += 'Descriptor hasData:  %s\n' % str(self.hasData())
        for datasetType in self.datasets.keys():
            for dataset in self.datasets[datasetType]:
                s += 'Dataset type: ' + str(datasetType) + ', dataset physical name: ' + str(dataset.name) + '\n'
                
        return s

    # --------------------------------------------------------------
    
    def dump(self):
        """Dumps out descriptor"""

        self.logger.info('|-------------------------------------------|')
        self.logger.info('|Worker job descriptor  dump                |')
        self.logger.info('|-------------------------------------------|')
        self.logger.info(self.__str__())
        self.logger.info('')
