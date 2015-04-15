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
		# Expand to dynamic variables via the AuxStoreWrapper
		wrapperName = auxContainerName+"Wrapper"
		seq = AlgSequence()
		for item in seq:
			if item.name() == "AuxStoreWrapperSequence":
				if not hasattr( item, wrapperName ):
					alg = CfgMgr.xAODMaker__AuxStoreWrapper( wrapperName, SGKeys = [ auxContainerName+"." ] )
					#seq.insert( 0, alg )
					item += alg
					pass
				break

	def GetContent(self,contentList,wholeContentList):
		mainOutput = []
		auxOutput = {}
		for item in contentList:
			components = item.split(".")
			if len(components)==1:
				# deal with main containers
				mainItem = self.mainContainerLine(components[0])
				if (mainItem not in mainOutput):
					mainOutput.append(self.mainContainerLine(components[0]))
			if len(components)>1:
				# Deal with Aux containers
				if (self.mainContainerLine(components[0]).split('#')[0]!='xAOD::JetAuxContainer'):
					# expand to dynamic, not needed for jets as dynamic already
					self.expandAuxStore(components[0])
				if (components[0]+"." in wholeContentList):
					if (components[0] not in auxOutput.keys()):
						auxOutput[components[0]] = ""
					continue
				if (components[0] not in auxOutput.keys()):
					auxOutput[components[0]] = components[1:]
				if (components[0] in auxOutput.keys()):
					auxOutput[components[0]] = list(set(auxOutput[components[0]]) | set(components[1:]))
		return mainOutput,auxOutput 
