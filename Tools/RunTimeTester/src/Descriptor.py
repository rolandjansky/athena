#! /usr/local/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Top class for the Descriptor heirarchy.
Contains stuff needed to make run directories.

"""
import os
import copy

from Paths               import Paths
from os.path             import join
from RTTpath             import RTTpath
from RTTSException       import RTTCodingError

# -------------------------------------------------------------------------
# import logging
# logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------
class Descriptor:    
    """Objects of this class hold information read from package UnifiedCOnfigurationFiles.
    They are used to instantiate JobMinders at which point the information is
    transferred to the minder"""
    
    # job serial number is now provided by the descriptor factory
    # jobSerialNumber=0  # class variable (static in C++)

    def __init__(self, paths, logger, jobSerialNumber, argDict = {}):
        # BaseDescriptor.__init__(self)
        
        self.logger          = logger
        self.paths           = paths
        self.jobSerialNumber = jobSerialNumber
        # self.jobSerialNumber=Descriptor.jobSerialNumber
        # Descriptor.jobSerialNumber=Descriptor.jobSerialNumber+1

        
        # Default values for name and identified name (which has the job serial
        # number stuck on the end of name). These should be overridden with more useful
        # values in subclasses.
        self.name              = "Job"
        # self.release           = paths.release
        self.jobDisplayName    = ''
        
        self.identifiedName    = self.name+str(self.jobSerialNumber)
        self.runPath           = '' # set later in this method
        self.resPath           = '' # set later in this method
        self.suggestedQueue    = ''
        self.jobGroup          = ''
        self.confederation     = None
        self.log               = ''
        self.elog              = ''
        self.suggestedBatchWallTime = ''

        self.rttPilotJob       = False # a flag to signal job is a pilot job
        self.rttATNJob         = False # job comes from ATN config file
        self.numberedOnly      = False # when to run the job
        self.limitedNightlies  = False # when to run the job

        self.jobDocString      = ''
        self.jobDocURL         = ''
        
        self.keepFilePatterns  = []   # fill from Jobs config file
        self.auxFilePatterns   = []   # fill from Jobs config file
        self.pScripts          = []   # fill from Jobs config file
        self.actions           = []   # filled by JobsXMLReader: list of UserTestDescriptor instances
        self.tests             = []   # filled by JobsXMLReader: list of UserTestDescriptor instances

        # name: need to come up with a name that be used to mkdir a directory
        # when added concatenated with a base path
        self.identifiedName   = self.name+str(self.jobSerialNumber)
        self.log              = self.identifiedName+"_log"
        self.hashString       = 'None'
        self.trendId          = None # CoolDozer id
        self.keepFiles        = {}
        self.missingDatasets  = []
        
        self.__dict__.update(argDict)
        
        # record of errors during coinstruction/configuration
        self.constructionError = False
        self.errorMessages     = []

        # a list of strings used to calculate run a nd results directory names
        self.fixName()

        self.setRunPath()
        # self.setHashString() # PS: the hash is in the dist used to update self.dict



    # --------------------------------------------------------------
    def hasData(self): return False
    # --------------------------------------------------------------
    
    def setRunPath(self, pathExtensions=[]):
        # Default setting of runpath, delegated to the PathsForDescriptors
        # object. The paths object as well as the present descriptor
        # is updated. Some descriptors act when
        # they have the runpath, and so will overide this function

        # --------------------------------------------------------------
    
        self.runPath    = RTTpath(self.paths.workDirs['packages'])
        self.resPath    = RTTpath(self.paths.resultsDirs['packages'])

        [self.runPath.postpend(item[1]) for item in pathExtensions]
        [self.resPath.postpend(item[1]) for item in pathExtensions]

        # this code has been written (too) quickly
        # we will store the final locations of resPath and runPath in Descriptor,
        # but not store the intermediate locations. These could have been put into the paths
        # object but:
        # - which one - not all clients of paths objects use a PathsForDescriptor object, so updating
        #   this might not be very useful
        # - to limit curly-queues: Descriptor is built from paths and then updates paths.. makes
        #   debugging , dumping etc difficult.
        
        self.runPath = str(self.runPath)
        self.resPath = str(self.resPath)

    # --------------------------------------------------------------

    def setName(self,name):
        self.name = name
        
    # --------------------------------------------------------------

    def setGroup(self, jobGroup):
        self.jobGroup = jobGroup

    # --------------------------------------------------------------

    def fixName(self):
        self.identifiedName   = self.name+str(self.jobSerialNumber)
        self.log              = self.identifiedName+"_log"
        # self.elog             = self.identifiedName+"_elog"

    # --------------------------------------------------------------

    def setupRunDir(self):
        """
        Allow descriptor specific actions for setting up the run
        directory. Specialisations implemented in derived classes
        """

        pass
    # --------------------------------------------------------------
    
    def __str__(self):
        """converts to a string"""
        s = '--------- Descriptor ---------\n'
        s += 'serial no.:            %s\n' % self.jobSerialNumber
        s += 'name:                  %s\n' % self.name
        s += 'identifiedName:        %s\n' % self.identifiedName
        s += 'log name:              %s\n' % self.log
        s += 'job group:             %s\n' % self.jobGroup
        s += 'runPath:               %s\n' % self.runPath
        s += 'resPath:               %s\n' % self.resPath
        s += 'suggested batch queue: %s\n' % self.suggestedQueue
        s += 'confederation:         %s\n' % self.confederation
        s += self.paths.__str__()
        
        return s

    # --------------------------------------------------------------
    
    def dump(self):
        """Dumps out jobname"""
        self.logger.info('|-------------------------------------------|')
        self.logger.info('|                                           |')
        self.logger.info('|            Descriptor  dump               |')
        self.logger.info('|                                           |')
        self.logger.info('|-------------------------------------------|')
        self.logger.info(self.__str__())

    def dataForXMLNode(self): return {'hashString':      self.hashString} # derived classes extend

    def deferredMinderUpdate(self): raise NotImplementedError()

    def deferredDataSetUpdate(self):
        self.datasets = {}

        # chainStoreArea must be set before this method is called.
        if not hasattr(self, 'chainStoreArea'):
            m = 'Descriptor %s is trying to use the chain store without its path being set' % self.identifiedName
            raise RTTCodingError(m)

        # container minders do not have chainStoreFiles
        if not hasattr(self, 'chainStoreFiles'): return

        # self.chainbstore files is a list of tuples like (dataSetInstance, dataDict of fallback files)
        #for item in self.__dict__.items(): print '%20s:    %s', item
            
        for datasetInstance, fallbackFileDict in self.chainStoreFiles:
            physicalDatasetName = os.path.join(self.chainStoreArea, datasetInstance.logical)
            self.logger.error('Descriptor.deferredDataSetUpdate => physicalDatasetName: %s' % physicalDatasetName)
            datasetInstance.physical = physicalDatasetName
            # if the physical file exists (ie is in chain store, use the datasetInstance,
            # otherwise make note of the fact it is missing
            datasetInstance.setStats() # update the mod time, size for web page displaying
            if datasetInstance.exists_():
                self.logger.error('Descriptor.deferredDataSetUpdate => File exists!')
                self.datasets.setdefault(datasetInstance.type, []).append(datasetInstance)
            else:
                self.logger.error('Descriptor.deferredDataSetUpdate => File NOT exists!')
                self.missingDatasets.append(datasetInstance)
                for dst in fallbackFileDict.keys():
                    self.datasets.setdefault(dst, []).extend(fallbackFileDict[dst])

    
        
        m = '%s::deferredDatasetUpdate: chainStoreFiles:\n %s\n\nfinale datasets:\n %s' % (self.__class__.__name__,
                                                                                           str(self.chainStoreFiles),
                                                                                           str(self.datasets))
        self.logger.debug(m)

    
    
