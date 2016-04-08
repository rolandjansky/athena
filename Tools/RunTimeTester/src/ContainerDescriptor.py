# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Descriptor import Descriptor
class ContainerDescriptor(Descriptor):
      def __init__(self, paths, jobSerialNumber, logger):
            Descriptor.__init__(self, paths, jobSerialNumber, logger, {})
            self.descs = []
            self.jobGroup = 'ContainerGroup' # jobGroup is used by MinderFactory
            self.name = 'Container'
            self.identifiedName = self.name+str(self.jobSerialNumber)
            
      def addDescriptor(self, desc): self.descs.append(desc)
            
      def descriptors(self): return self.descs
 
      def deferredMinderUpdate(self): pass
