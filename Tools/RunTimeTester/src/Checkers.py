# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Checkers that run on  logfiles returned after processing.

simpleLogChecker - A Log checker that examines log files for the success message
                   printed by the  athena Applicationmanager.
		   Gets the logname from a JobInfo object.

PS
"""
from popen2      import Popen4 
import os,shutil, string, logging
from JobGroupKits import *
from os.path      import join
from Defs         import *
from Factory_SiteSpecifics   import SiteSpecificsFactory

# -----------------------------------------------------------------------------
logger = logging.getLogger('rtt')
# -----------------------------------------------------------------------------

success =  0
failure = -1

class Checker:
	def __init__(self):
		self.logger = logger

	def setLogger(self, newLogger): self.logger=newLogger

	def __call__(self, minder):
		return self.check(minder)
	
	def check(self):
		pass

#--------------------------------------------------------------------------------

class SimpleLogChecker(Checker):
	def __init__(self,searchStringList,vetoStringList=[], mustFindAllSearch=True):

		Checker.__init__(self)
		self.mustFindList      = searchStringList
		self.mustNotFindList   = vetoStringList
		self.mustFindAllSearch = mustFindAllSearch
		self.nSuccesses        = 0
		self.chainLength       = 0
		
	def equivalent(self, other):
		if len(self.__dict__.keys()) != len(other.__dict__.keys()):
			return False
		for k in self.__dict__.keys():
			if k not in other.__dict__.keys(): return False

		for e in self.mustFindList:
			if e not in other.mustFindList: return False
			
		for e in self.mustNotFindList:
			if e not in other.mustNotFindList: return False

		return True
			
	def check(self,jMinder):

		# The "raw" logname is jMinder.jDescriptor.log
		# It is copied to the results directory
		# with name jMinder.keepFiles[jMinder.jDescriptor.log]
		logName = join(jMinder.runPath, jMinder.log) #
		
		self.logger.debug("%s: looking for: %s" %(self.__class__.__name__, logName))
		
		if not os.path.exists(logName):
			m = "%s: No logfile found: %s" % (self.__class__.__name__, logName)
			self.logger.warning(m)
			return RTTdefs.error

		self.logger.debug("%s: found logfile: %s" % (self.__class__.__name__, logName))
                    
		logf=open(logName, "r")
		logContents=logf.read()
		logf.close()

		self.logger.debug("%s: read in logfile %s length %d" % (self.__class__.__name__, logName, len(logContents)))

                if len(logContents) ==0:
			return RTTdefs.error

		splices = self.spliceLogFile(logContents)

		self.logger.debug("%s: have sliced  logfile %s into %d " % (self.__class__.__name__, logName, len(splices)))

		self.chainLength = len(splices)

		tests = [(self.checkSearchString(s), self.checkVetoString(s)) for s in splices]
		self.logger.debug('results: %s', str(tests))
		self.nSuccesses = tests.count( (True, True) )
		self.logger.debug('length, successes: %d %d' % (self.chainLength, self.nSuccesses))

		if self.nSuccesses == self.chainLength:
			return RTTdefs.success
		else:
			return RTTdefs.error
		

	def spliceLogFile(self, text):
		# other log checkers elaborate this method
		return [text]
	
	def checkSearchString(self, text):
		"determine if all the substrings in list self.searchStrings"
		found = []
		for s in self.mustFindList:
			if s not in text:
				if self.mustFindAllSearch:
					self.logger.debug('%s: required string %s not found' % (self.__class__.__name__, s) )
					return False
			else:
				found.append(s)
		if not found:
			self.logger.debug('Did not match on any search strings!')
			return False
		return True

	
	def checkVetoString(self, text):

		for s in self.mustNotFindList:
			if s in text:
				self.logger.debug('%s: veto string %s found' % (self.__class__.__name__, s) )
				return False
		return True
	
#--------------------------------------------------------------------------------

class ChainedJobsLogChecker(SimpleLogChecker):

	def __init__(self, searchStringList, vetoStringList=[]):
		SimpleLogChecker.__init__(self, searchStringList, vetoStringList)
		
	def spliceLogFile(self, text):

		splitText = 'Starting job'

		njobs = text.count(splitText)
		
		if not njobs:
			return [text]
		
		splices = text.split(splitText)

		self.logger.debug('%s: spliced into %d return %d' % (self.__class__.__name__, len(splices),
								     len(splices[1:]) ) )
		return splices[1:]
#--------------------------------------------------------------------------------

class SimpleFileDiffer(Checker):
	def __init__(self):
		Checker.__init__(self)
		
	def check(self,jMinder):
		m = "SimpleFileDiffer is now obsolete. Please use the DCube framework instead."
		self.logger.error(m)
		return RTTdefs.error

#-------------------------------------------------------------------------------

class HistogramDiffer(Checker):
	def __init__(self,comparisonType):
		Checker.__init__(self)
	def check(self, jMinder):
		m = "HistogramDiffer is now obsolete. Please use the DCube framework instead."
		self.logger.error(m)
		return RTTdefs.error
	
