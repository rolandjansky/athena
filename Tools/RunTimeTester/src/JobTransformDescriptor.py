#! /usr/local/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Class that stocks information about JobTransform jobs.
Responsible for setting up the run directory for a job. Eventually becomes a data member
of JobMinder.
"""
import os

from Paths               import Paths
from os.path             import join, exists
from Descriptor          import Descriptor
from formatCollection    import formatCollection
from RTTSException       import RTTCodingError
#from Tools2              import escapeTheCommand

# -------------------------------------------------------------------------
# import logging
# logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------
class JobTransformDescriptor(Descriptor):    
    
    def __init__(self, paths, logger, jobSerialNumber, argDict={}):

        self.datasets          = {}
        self.displayClass      = []
        self.displayProcess    = []
        self.displayComponent  = []
        
        # init the base class
        Descriptor.__init__(self, paths, logger, jobSerialNumber)

        # update with values from the pacakge config file
        # jobGroup, jobTransformCmd, jobTransformJobName
        self.name            = argDict.pop('jobTransformJobName')
        self.jobTransformCmd = argDict.pop('jobTransformCmd')
        self.__dict__.update(argDict)

        self.fixName()
        
        self.setPathExtensions()
        self.setRunPath(self.pathExtensions)
        # self.jobTransformCmd = escapeTheCommand(self.jobTransformCmd)
        
    # ----------------------------------------------------------------------

    def dataForXMLNode(self):
        names = ['displayComponent', 'displayClass', 'displayProcess']
        dict = Descriptor.dataForXMLNode(self)
        for n in names: dict[n] = self.__dict__[n]
        return dict

    # ----------------------------------------------------------------------
        
    def setPathExtensions(self):
        self.pathExtensions = [
            ('package',    self.paths.package),
            ('jobGroup',   self.jobGroup),
            ('jobName',    self.name),
            ('SN',         str(self.jobSerialNumber))
            ]        


    # ----------------------------------------------------------------------

    # seen by PyLint: no attribute pythonScriptName. Never called, so comment out
    #def script(self):
    #    script = '%s(%s)'
    #    script = self.pythonScriptName

    # ----------------------------------------------------------------------

    def deferredMinderUpdate(self):
       Descriptor.deferredDataSetUpdate(self) 
       self.updateJobTransformCommand()

    # ----------------------------------------------------------------------

    def updateJobTransformCommand(self):
        """
        replace the logical data set names in the jobtransform command with the
        corresponding physical data name.
        """

        m = '%s::updateTransformCmd() original job transform command \n\n%s\n' % (self.__class__.__name__, self.jobTransformCmd)
        self.logger.debug(m)
        
        for dsInstances in self.datasets.values():
            for dsInst in dsInstances:
                physicalName = dsInst.physical
                logicalName  = dsInst.logical
                if not logicalName: 
                    m = 'JobTransformDescriptor %s: logical dataset name not set for dataset %s' % (self.identifiedName, str(dsInst))
                    raise RTTCodingError(m)

                self.jobTransformCmd = self.jobTransformCmd.replace(logicalName,physicalName)

        m = '%s::updateTransformCmd() amended job transform command \n\n%s\n' % (self.__class__.__name__, self.jobTransformCmd)
        self.logger.debug(m)
       
    
    def __str__(self):
        s = '--------- JobTransformDescriptor ---------\n'
        s += Descriptor.__str__(self) 
        return s

    # ----------------------------------------------------------------------
    
    def dump(self):
        """Dumps out descriptor"""
        self.logger.info(self.__str__)

