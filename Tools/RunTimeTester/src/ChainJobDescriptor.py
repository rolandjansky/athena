# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Descriptor import Descriptor
class ChainJobDescriptor(Descriptor):
    def __init__(self, paths, logger, jobSerialNumber, argDict={}):
        Descriptor.__init__(self, paths, logger, jobSerialNumber, argDict)
        self.descriptor = None
        self.outChainFiles = [] # filenames (posibly with subdirectories)
        self.name = 'ChainJob'
        self.jobGroup = 'ContainerGroup' # jobGroup is used by MinderFactory

        
    def setOutFiles(self, fileList):
        self.outChainFiles = fileList

    def addDescriptor(self, desc):
        self.descriptor = desc
        #self.jobGroup   = None
        #self.name = '%s_%s_' % (self.name, self.descriptor.identifiedName)
        #self.fixName()
        #self.log = ''
