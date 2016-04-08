# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaJobDescriptor import AthenaJobDescriptor
import os

class PseudoDescriptor(AthenaJobDescriptor):
    def __init__(self, paths, logger, jobSerialNumber, argDict={}):
        #dict = {'name':     'pseudoDescriptor',
        #        'jobGroup': 'Athena',
        #        'jobOptions': ['pseudoJobOptions1.py']
        #        }
        #argDict.update(dict)
        AthenaJobDescriptor.__init__(self, paths, logger, jobSerialNumber, argDict)
        #self.__dict__['runPath'] =  '/User/peter/RTTdevelopment/RTTChain/Work/pseudoRunPath'
        
