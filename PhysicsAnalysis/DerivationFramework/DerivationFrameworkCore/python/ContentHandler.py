# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkCore.ContainerNamesAndTypes import *
from AthenaCommon import CfgMgr
from AthenaCommon.AlgSequence import AlgSequence

class ContentHandler:
	def __init__(self,inputName):
		self.name = inputName
		self.AppendToDictionary = {}
	
	def mainContainerLine(self,containerName):
		theDictionary = dict(ContainerNamesAndTypes.items() + self.AppendToDictionary.items())
		line = theDictionary[containerName]+"#"+containerName
		return line

	def expandAuxStore(self,auxContainerName):
		wrapperName = auxContainerName+"Wrapper"
		seq = AlgSequence()
		if not hasattr( seq, wrapperName ):
			alg = CfgMgr.xAODMaker__AuxStoreWrapper( wrapperName, SGKeys = [ auxContainerName+"." ] )
			seq.insert( 0, alg )
			pass

	def GetContent(self,contentList):
		mainOutput = []
		auxOutput = {}
		for item in contentList:
			components = item.split(".")
			if len(components)==1:
				mainItem = self.mainContainerLine(components[0])
				if (mainItem not in mainOutput):
					mainOutput.append(self.mainContainerLine(components[0]))
			if len(components)>1:
				self.expandAuxStore(components[0])
				if (components[0] not in auxOutput.keys()):
					auxOutput[components[0]] = components[1:]
				if (components[0] in auxOutput.keys()):
					auxOutput[components[0]] = list(set(auxOutput[components[0]]) | set(components[1:]))
		return mainOutput,auxOutput 
