# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ContainerDescriptor import ContainerDescriptor

class ParallelDescriptor(ContainerDescriptor):
    def __init__(self, paths, logger, jobSerialNumber, argDict={}):
        ContainerDescriptor.__init__(self, paths, logger, jobSerialNumber)
        self.name = 'Parallel'
        self.identifiedName = self.name+str(self.jobSerialNumber)
