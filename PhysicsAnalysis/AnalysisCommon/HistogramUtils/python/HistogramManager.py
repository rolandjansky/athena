# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    HistogramManager.py
# @purpose: A python class to manage histogram booking
# @author:  Carsten Burgard <cburgard@cern.ch>

__doc__     = 'A python class to manage histogram booking'
__version__ = '$Revision: 1.0 $'
__author__  = 'Carsten Burgard <cburgard@cern.ch>'

import AthenaPython.PyAthena as PyAthena
from ROOT import TH1, TH2, TH3
from HistogramUtils.HistogramTool import HistogramTool
import HistogramUtils
from AthenaCommon.Logging import logging
from AthenaCommon.Configurable import ConfigurableAlgTool


histogramManagerLogger = logging.getLogger( 'histogramManagerLogger' )

class HistogramManager:
	def __init__(self, Name, RootStreamName, RootDirName, OnlyAtCutStages = None, OutputLevel = 3, HistToolClasses = [HistogramUtils.HistogramUtilsConf.HistogramUtils__HistogramToolTH1, HistogramUtils.HistogramUtilsConf.HistogramUtils__HistogramToolTH2, HistogramUtils.HistogramUtilsConf.HistogramUtils__HistogramToolTH3]):
		self.m_name = Name
		self.m_rootStreamName = RootStreamName
		self.m_rootDirName = RootDirName
		self.m_onlyAtCutStages = OnlyAtCutStages
		self.m_histogramTools = {}
		self.m_outputLevel = OutputLevel
		self.m_histogramToolClasses = HistToolClasses
	
	def isAcceptedToolClass(self,obj):
		"""return true if the type of this object is among the accepted tool classes, false otherwise"""
		if type(obj) in self.m_histogramToolClasses:
			return True
		return False

	def acceptToolClass(self,s):
		"""add a type to the accepted tool classes"""
		self.m_histogramToolClasses.append(s)

	def rejectToolClass(self,s):
		"""remove a type from the accepted tool classes"""
		l = len(self.m_histogramToolClasses)
		for i in range(0,l):
			if self.m_histogramToolClasses[l-i-1] == s:
				self.m_histogramToolClasses.remove(l-i-1)

	def printToolClasses(self):
		"""print the list of accepted tool classes"""
		l = len(self.m_histogramToolClasses)
		for i in range(0,l):
			print(self.m_histogramToolClasses[i])

	def __iadd__(self, other):
		self.add(other)
		return self

	def add(self,other,arg1=None,arg2=None,arg3=None):
		"""add a histogram tool to the list
		this method will do it's very best to interpret your input in a sensible way
		the most well-defined way of booking a histogram is manually constructing the tool, i.e.
		  myHistMgr.add( HistogramTool( TH*("name","title",...) , "expression" ) )
		if something has been added, this function returns true, false otherwise.
		"""
		if self.isAcceptedToolClass(other):
			# if this is a tool, add it to the list
			if other.name() not in self.m_histogramTools.keys():
				if self.m_outputLevel < 3: # 3 = INFO
					histogramManagerLogger.verbose("adding histogram tool '{:s}' to the manager".format(other.name()))
				self.m_histogramTools[other.name()] = other
				return True
			return False
		elif isinstance(other,ConfigurableAlgTool):
			histogramManagerLogger.warn("an instance of ConfigurableAlgTool named '{:s}' was passed to the HistogramManager, but its type is not listed as an accepted tool class. this HistogramManager will reject and ignore this instance. if you want this HistogramManager to accept this instance, you should add its type to the list of accepted tool classes by calling 'myHistogramManager.acceptToolClass(type(myToolObject))'.")
			return False
		elif isinstance(other, TH1):
			# if this is a histogram, instantiate a tool and add it to the list
			if other.GetName() not in self.m_histogramTools.keys():
				if self.m_outputLevel < 3: # 3 = INFO
					histogramManagerLogger.verbose("adding histogram '{:s}' to the manager".format(other.GetName()))
				if isinstance(other,TH3):
					self.m_histogramTools[other.GetName()] = HistogramTool(other,arg1,arg2,arg3,
																		   OutputLevel = self.m_outputLevel,
																		   RootDirName = self.m_rootDirName,
																		   RootStreamName = self.m_rootStreamName
					)
					return True
				elif isinstance(other,TH2):
					self.m_histogramTools[other.GetName()] = HistogramTool(other,arg1,arg2,
																		   OutputLevel = self.m_outputLevel,
																		   RootDirName = self.m_rootDirName,
																		   RootStreamName = self.m_rootStreamName
					)
					return True
				else:
					self.m_histogramTools[other.GetName()] = HistogramTool(other,arg1,
																		   OutputLevel = self.m_outputLevel,
																		   RootDirName = self.m_rootDirName,
																		   RootStreamName = self.m_rootStreamName
					)
					return True
			return False
		else:
			# if this is iterable, dispatch the items and recall the function
			first = None
			second = None
			third = None
			fourth = None
			try:
				iterator = iter(other)
				first = iterator.next()
				second = iterator.next()
				third = iterator.next()
				fourth = iterator.next()
				raise StopIteration
			except StopIteration:
				self.add(first,second,third,fourth)
				return True
			except TypeError:
				return False

	def __isub__(self, other):
		self.remove(other)
		return self

	def remove(self,other):
		"""remove a histogram tool from the list - matching is done by name"""
		if isinstance(other, HistTool):
			if other.name() not in self.m_histogramTools.keys():
				del self.m_histogramTools[other.name()]
				return True
		elif isinstance(other, TH1):
			if other.GetName() not in self.m_histogramTools.keys():
				del self.m_histogramTools[other.GetName()]
				return True
		elif isinstance(other,basestring):
			if other not in self.m_histogramTools.keys():
				del self.m_histogramTools[other]
				return True
		return False		

	def updateToolProperty(self,tool,key,value):
		"""update a property of a tool, only setting it if it was undefined previously"""
		if not key in tool.getProperties().keys() or tool.getProperties()[key] == "<no value>":
			setattr(tool,key,value)
			
	def updateTools(self):
		"""update the properties of all tool according to the defaults"""
		for tool in self.m_histogramTools.values():
			self.updateToolProperty(tool,"RootStreamName", self.m_rootStreamName )
			self.updateToolProperty(tool,"RootDirName",    self.m_rootDirName    )
			self.updateToolProperty(tool,"OutputLevel",    self.m_outputLevel    )
			self.updateToolProperty(tool,"OnlyAtCutStages",self.m_onlyAtCutStages)

	def ToolList(self):
		"""retrieve the list of histogram tools"""
		self.updateTools()
		return self.m_histogramTools.values()

	def printToolList(self):
		"""print the list of histogram tools"""
		list = self.ToolList()
		for tool in list:
			print("{: <10} {: <20} >> {:s}:{: <20}".format(tool.ClassName,tool.name(),tool.getProperties()["RootStreamName"],tool.getProperties()["RootDirName"]))

	def __str__(self):
		return "HistogramManager '{:s}' with ROOT stream '{:s}' to directory '{:s}'".format(self.m_name,self.m_rootStreamName,self.m_rootDirName)

	def __repr__(self):
		print(self.__str__())
		self.printToolList()

	
		
	
		
	
