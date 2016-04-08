# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import os.path
import sys

from Evaluate import Evaluate # from RTTSrcDir code base

class RTTSummaryFile:
    def __init__(self, path):
        if not os.path.exists(path):
            print '%s, inexistent' % path
            sys.exit(1)

        self.path = os.path.abspath(path)
        self.docEl_ = self.docEl(path)
        self.overviews = [Overview(o) for o in Evaluate('overview', self.docEl_)]
        self.packages  = [Package(p) for p in Evaluate('package', self.docEl_)]

    def docEl(self, path):
        from xml.dom.minidom import parse
        try:
            return parse(path).documentElement
        except:
            print 'Unable to parse %s' % path
            return None

    def getPackageByName(self, name):
        return [p for p in self.packages if p.name==name]


class Thing: # convenience parent class
    def __init__(self, elem):
        self.xml = elem
    def __getitem__(self, xpath):
        return Evaluate(xpath, self.xml)

class Package(Thing):
    def __init__(self, packageEl):
        Thing.__init__(self, packageEl)
        self.jobs = [Job(j) for j in Evaluate('minder', packageEl)]
        self.name = self['packageName/text()']

    def getJobsByFullName(self, name):
        return [j for j in self.jobs if j.fullname==fullname]
    def getJobsByName(self, name):
        return [j for j in self.jobs if j.name==name]
    def getJobsByStatus(self, status):
	return [j for j in self.jobs if j.status==status]
    def getAllSuccessJobs(self):
	return self.getJobsByStatus('success')
    def getAllFailedJobs(self):
	return self.getJobsByStatus('error')
    def hasOnlySuccessJobs(self):
	return len(self.getAllSuccessJobs()) == len(self.jobs)

class Job(Thing):
    def __init__(self, jobEl):
        Thing.__init__(self, jobEl)
        self.fullname = self['identifiedName/text()']
        self.name     = self['jobName/text()']
	self.status   = self['status/text()']
    def isSuccess(self):
        return self.status == 'success'
    def isFailed(self):
        return self.status == 'error'

class Overview(Thing):
    def __init__(self, ovEl):
        Thing.__init__(self, ovEl)
