from __future__ import print_function
from __future__ import division
from builtins import object
from future.utils import iteritems

from builtins import int

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#######################################################################################################
# Defines
# class MixStreamConfig,
#    which contains the configuration of the streaming job (sfo, run, lumi blks, lumi DB, partition DB)
# and class MixingPartitioner,
#    which contains the MixingSelectors and configures the Athena algorithms.
# Loopy dependency: The trf has a MixStreamConfig and a PartitionArg that has a MixingPartitioner,
#    which also has (that) MixStreamConfig.
#######################################################################################################

#__all__ = []

from PyJobTransformsCore.TransformConfig import *
from RecJobTransforms.MixingSelector import *
from math import sqrt
import re

class ListOfSelectors(UniqueList):
	"""List of class MixingSelector, for configuration of job."""
	def __init__(self,doc,default=None,allowedValues=None):
		UniqueList.__init__(self,doc,default,allowedValues)				
	def _checkType(self,variableName,value):
		"""Add check that all entries are MixingSelectors"""
		# check that <value> is a list or tuple
		value = UniqueList._checkType(self,variableName,value)
		# check that all entries are strings
		for v in value:
			valType = type(v).__name__
			if not isinstance(v, MixingSelector):
				raise JobConfigError("Entry %r in %s is not a MixingSelector (but an %s)" % \
						     (v,variableName,valType) )					
		return value
	
class FloatList(Descriptor):
	"""List of floats, which can have repeated entries.
	List of allowed value has unit length and is the minimum SIZE of the list."""
	def __init__(self,doc,default=None,allowedValues=None):
		if default is None: default = []
		Descriptor.__init__(self,doc,default,allowedValues)
	def _checkType(self,variableName,value):
		"""Check that <value> is a list or tuple, and make the tuple a list."""
		valType = type(value).__name__
		if not isinstance(value, list) and not isinstance(value, tuple):
			raise JobConfigError('%s should be a list or tuple. Got %s instead.' & (variableName, valType))
		if isinstance(value, tuple):
			value = list(value)
		for v in value:
			valType = type(v).__name__
			if not isinstance(v, float):
				raise JobConfigError("Entry %r in %s is not a float (but an %s)" %  (v,variableName,valType) )
		return value

	def _checkValue(self,variableName,value):
		# check that size is allowed
		if len(self.allowedValues()):
			allowed = self.allowedValues()[0]
			if len(value) < allowed: raise JobConfigError( 'The list %s has size %r, less than %s' % (variableName, len(value), allowed) )
		return value	

	def _checkAllowedValues(self,variableName,allowedValues):
		# just get the first value and ensure it is a positive integer
		newAllowed=[]
		if not allowedValues:
			return newAllowed
		if isinstance(allowedValues[0], int) and allowedValues[0] >= 0:
			newAllowed += [ allowedValues[0] ]
		return newAllowed
				 
class MixStreamConfig(TransformConfig):
	# not allow dynamic members
	__slots__ = ()
	NumberOfBatchJobs = Integer("Number of batch jobs in total, for dividing up input files",5)	
	NumberOfSFO = Integer("Number of SFO simulated in test, for setting event numbers",5)
	NumberOfLumiBlocksPerRun = Integer("Number of LB per run simulated in test, for setting run numbers",30)
	RunNumberBase = Integer("Base for calculating run number", 1)
	LuminosityPartition = FloatList("Division of the total task luminosity as a list of floats",[1.0,], [1])
	JobSelectors = ListOfSelectors("List of unique MixingSelectors for input config.  See Reconstruction.MixingSelector for details.",[])

	def WhichSFO(self, WhichPartition):
		if WhichPartition >= self.NumberOfBatchJobs: return -1
		return WhichPartition % self.NumberOfSFO
	def WhichLumiBlock(self, WhichPartition):
		if WhichPartition >=  self.NumberOfBatchJobs: return -1
		return (WhichPartition/self.NumberOfSFO) % self.NumberOfLumiBlocksPerRun	
	def WhichRunNumber(self, WhichPartition):
		if WhichPartition >=  self.NumberOfBatchJobs: return -1
		return self.RunNumberBase + (WhichPartition/self.NumberOfSFO)/self.NumberOfLumiBlocksPerRun
	def FinishedLuminosityFraction(self, WhichPartition):
		if WhichPartition >=  self.NumberOfBatchJobs: return -1
		lp = self.LuminosityPartition
		theLB = (WhichPartition/self.NumberOfSFO)
		totLB = (self.NumberOfBatchJobs/self.NumberOfSFO)
		return sum(lp[0:theLB])/sum(lp[0:totLB])
	def ThisLuminosityFraction(self, WhichPartition):
		if WhichPartition >=  self.NumberOfBatchJobs: return -1
		lp = self.LuminosityPartition
		theLB = (WhichPartition/self.NumberOfSFO)
		totLB = (self.NumberOfBatchJobs/self.NumberOfSFO)		
		return ((lp[theLB])/sum(lp[0:totLB]))
	def __init__(self,name='mixStreamConfig',metaData=[]):
		TransformConfig.__init__(self,name,metaData)
            
# make default configuration object -- once.
if not 'mixStreamConfig' in dir():
	mixStreamConfig = MixStreamConfig()

	
############################################
# MixingPartitioner		
############################################		
# Usage:
# m = MixingPartitioner()                      #Selects files
# m.SetPartition(thisJob)                      #for batch job number \<thisJob\>
# m.AddSample(5009,49.01,myListFromDq2)                #Add some QCD jet files from the list of PFN strings \<myListFromDq2\>
# m.ConfigureSelectors()                               #Do the math to get ONLY needed files for this job.  Short file lists generated for staging.
#    # later, in your athena job options...
# ...a,b = m.ConfigureServices()                       #Configures strings to "exec" to set jobOpts of your MixingEventSelector. See trf for details on usage.
# ...StreamAlg.SamplesToWeight = m.ListOfSamplesToWeight()
# ...StreamAlg.SampleWeights   = m.ListOfSampleWeights()
############################################
				
class MixingPartitioner(object):
	""" This collects input files to add to the stager, and generates python to configure EventMixer and your optional MCRunNumber-based event weighting service. """	
	def __init__(self):
		self.__ThisPartition = -1     
		self.__Selectors = {}           #dataset aliases (like 5009) : MixingSelector object
		self.__DatasetsToWeight = {}    #hash of dataset weights (indexed by MC run number)
		self.__ThisScaleFactor = -1
		self.__ScaleFactorSum = -1
		self.__NumPartitions = -1
			
	def SetPartition(self,thisPartition):
		"""Must be done  when partitionArg and mixStreamConfig gets a value."""
		if self.__ThisPartition < 0 :                                                              #first time
			for s in mixStreamConfig.JobSelectors:                                                     
				self.__Selectors[s.DSID()] = s
			self.__NumPartitions =  mixStreamConfig.NumberOfBatchJobs
			
		self.__ThisPartition = thisPartition		
		if self.__ThisPartition < 0 :
			print("*** MixingPartitioner: WARNING you requested an invalid partition number: setting to 0. *** ")
			self.__ThisPartition = 0;		
		else:
			if self.__ThisPartition >= (self.__NumPartitions - 1):
				print("*** MixingPartitioner: WARNING you requested an invalid (big) partition number: setting to", self.__NumPartitions - 1, ". *** ")
				self.__ThisPartition = self.__NumPartitions - 1;
			else:
				print("*** MixingPartitioner: INFO you requested partition number", self.__ThisPartition)
		self.__ThisScaleFactor = mixStreamConfig.ThisLuminosityFraction(self.__ThisPartition)      #the job config knows partitioning
		self.__ScaleFactorSum  = mixStreamConfig.FinishedLuminosityFraction(self.__ThisPartition)  #the job config knows partitioning
		

	def AddSample(self, fileList, ksample, nEvents, newAlias=-1, nEventsPerFile=50):
		if not (ksample in self.__Selectors):
			self.__Selectors[ksample] = MixingSelector(self.__NumPartitions,ksample,nEvents,nEventsPerFile)
			self.__Selectors[ksample].addNewCatalog(fileList)
		else:
			self.__Selectors[ksample].addNewCatalog(fileList)
			if newAlias > 0: self.__Selectors[ksample].addAlias(newAlias)						
			
	def ConfigureServices(self):
		EventMixerList = []
		serviceString = "#Input selectors "
		soFar = self.SoFar()
		perSFOnow = self.PerSFOnow()		
		for ksample in self.__Selectors:		
			EventMixerList += [ self.__Selectors[ksample].trigger(soFar,perSFOnow) ]
			serviceString += "\n%s =  EventSelectorAthenaPool( '%s' )" % (self.__Selectors[ksample].name(), self.__Selectors[ksample].name())
			serviceString += "\nsvcMgr += %s" % self.__Selectors[ksample].name()
			serviceString += "\n%s.InputCollections= ['%s']" % \
					 (self.__Selectors[ksample].name(), ("', '").join(self.__Selectors[ksample].FilesToStage(soFar,perSFOnow)))
		return EventMixerList, serviceString	
	def ScaleFactor(self):
		totalLumi = 1
		return self.__ThisScaleFactor * totalLumi
	def PerSFOnow(self):
		return self.__ThisScaleFactor/mixStreamConfig.NumberOfSFO 		
	def SoFar(self):
		return self.__ScaleFactorSum + self.PerSFOnow() * (self.__ThisPartition % mixStreamConfig.NumberOfSFO)
	def ConfigureSelectors(self):		
		print("*** MixingPartitioner: INFO Partition ", self.__ThisPartition, " has a LB beginning after ", self.__ScaleFactorSum, "% of the input has been processed.")
		print("*** MixingPartitioner: INFO Partition ", self.__ThisPartition, " has a LB reading ", self.__ThisScaleFactor * 100, "% of the input. ")
		totalLumi = 1
		print(":::::::::: STREAMING JOB CONFIGURATION: LHC INSTANTANEOUS LUMINOSITY= %f x 10^%i cm^-2 s^-1" % (self.ScaleFactor(), 31))
		soFar = self.SoFar()
		perSFOnow = self.PerSFOnow()
		for ksample in self.__Selectors:
			sel = self.__Selectors[ksample]						
			if not sel.isSufficient():                   # prescale weighting is a TASK-GLOBAL! Read more, don't scale more.
				print("*** MixingPartitioner: WARNING not enough events for %s -- (%s) will be weighted." % \
				      (sel.name(), ','.join([str(ali) for ali in sel.Equivalents()]) ))
				for aliasedID in sel.Equivalents():
					self.__DatasetsToWeight[ aliasedID ] = sel.weight()					
			print("*** MixingPartitioner: INFO \t%s FirstFile=%s EvOffset=%i NeV=%f" % \
			      (sel.name(), sel.firstFileIndex(soFar), sel.firstEventInFile(soFar), sel.totalEventsThisJob(perSFOnow)))

	def preStageInputFiles(self,CastorOrDCache = 'Castor'):
		from PyJobTransformsCore.FilePreStager import theFileStagerRobot		
		if CastorOrDCache == 'Castor':
                        print("WARNING: Castor is not supported anymore")
		else:
			from StreamMix import DCachePreStager
		fileList = self.ListOfFilesToStage()
		# NOTE THAT FILES THAT DON'T START WITH PNFS OR DCACHE WILL NOT BE CHECKED.
		filesNeedingAction = theFileStagerRobot.addFilesToStagerIfNeeded( fileList )
		print("This job muust stage %i files" % len(filesNeedingAction))
		for f in filesNeedingAction: print(f)
		filesNotStaged = theFileStagerRobot.waitUntilAllFilesStaged()
		if filesNotStaged:
			problemFiles = ''
			for filename,status in iteritems(filesNotStaged):
				problemFiles += os.linesep + "%s:%s" % (filename,status)
			raise IOError("Could not stage following files from tape:%s" % problemFiles )
	
	def ListOfSamplesToWeight(self):		
		return [ k for k in self.__DatasetsToWeight ]
	def ListOfSampleFractions(self):
		return [ self.__DatasetsToWeight[k] for k in self.__DatasetsToWeight ]
	def ListOfSampleColonWeights(self):

		return [ str(k)+":"+("%f" % self.__DatasetsToWeight[k]) for k in self.__DatasetsToWeight ]
	def ListOfFilesToStage(self):
		tlst = []
		soFar = self.SoFar()
		perSFOnow = self.PerSFOnow()
		for ksample in self.__Selectors:
			sel = self.__Selectors[ksample]
			tlst += sel.FilesToStage(soFar,perSFOnow)
		return tlst
