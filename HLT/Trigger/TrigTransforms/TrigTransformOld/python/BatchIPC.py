#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# vim: set fileencoding=utf-8 :
# Created by Hegoi Garitaonandia <hegoi.garitaonandia@cern.ch>

# in order to communicate between job and manager
import pickle
import sys

class Result: 
        def __init__(self):        
                self.result=                                \
                {                                        \
                'status'        : None,                        \
                'errorin'        : None,                        \
                'summary'        :                        \
                                        {                \
                                           'time': 0        \
                                        },                \
                'steps'                : []                        \
                }

        def isGood(self):
                return self.result['status']

        def getErrorType(self):
                return self.result['errorin']

        def serialize(self,filename):
                        thefile=file(filename,'w')        
                        pickle.dump(self.result,thefile)

        def load(self,filename):         
                thefile=file(filename,'r')        
                self.result=pickle.load(thefile)

        def setError(self,error,comment=''):
                self.result['status']        = False
                self.result['errorin']        = error
                self.result['comment']        = comment

        def setGood(self):
                self.result['status']=True

        def addStep(self,step):
                self.result['steps'].append(step)

        def getSteps(self): 
                return self.result['steps']

        def setJobId(self,jobid):
                self.result['jobid']=jobid

