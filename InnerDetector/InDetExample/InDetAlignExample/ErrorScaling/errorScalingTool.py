# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from glob import glob
#from decimal import Decimal
#import numpy
#import pylab

class errorScalingTool:
    def __init__(self, filename = 'results_iterC*.txt'):
        file = open(filename,'r')
        self.lines = file.readlines()
        for ii in self.lines:
            ii.strip()
        
    def getParamA(self):
        paramA = {}
        for ii,vv in enumerate(self.lines):
            if ii>5 and ii<14:
                paramA[vv.split()[0] + '_' + vv.split()[1]] = vv.split()[2]
        return paramA
        
    def getParamC(self):
        paramC = {}
        for ii,vv in enumerate(self.lines):
            if ii>5 and ii<14:
                paramC[vv.split()[0] + '_' + vv.split()[1]] = vv.split()[3]
        return paramC
        
    def getSigma0(self):
        getSigma0 = {}
        for ii,vv in enumerate(self.lines):
            if ii>17 and ii<26:
                getSigma0[vv.split()[0] + '_' + vv.split()[1]] = vv.split()[2]
        return getSigma0
        
    def getPull(self):
        getPull = {}
        for ii,vv in enumerate(self.lines):
            if ii>17 and ii<26:
                getPull[vv.split()[0] + '_' + vv.split()[1]] = vv.split()[4]
        return getPull
        
    def getPullError(self):
        getPullError = {}
        for ii,vv in enumerate(self.lines):
            if ii>17 and ii<26:
                getPullError[vv.split()[0] + '_' + vv.split()[1]] = vv.split()[5]
        return getPullError
        
    def getRMS(self):
        getRMS = {}
        for ii,vv in enumerate(self.lines):
            if ii>17 and ii<26:
                getRMS[vv.split()[0] + '_' + vv.split()[1]] = vv.split()[6]
        return getRMS
        
    def getRMSerror(self):
        getRMSerror = {}
        for ii,vv in enumerate(self.lines):
            if ii>17 and ii<26:
                getRMSerror[vv.split()[0] + '_' + vv.split()[1]] = vv.split()[7]
        return getRMSerror
        