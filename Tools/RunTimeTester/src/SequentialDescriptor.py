# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ContainerDescriptor import ContainerDescriptor

class SequentialDescriptor(ContainerDescriptor):
    def __init__(self, paths, logger, jobSerialNumber, argDict={}):
        ContainerDescriptor.__init__(self, paths, logger, jobSerialNumber)
        self.name = 'Sequential'
        self.identifiedName = self.name+str(self.jobSerialNumber)
