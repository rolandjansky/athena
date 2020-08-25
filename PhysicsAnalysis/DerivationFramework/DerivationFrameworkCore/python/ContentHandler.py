# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from AthenaCommon.AlgSequence import AlgSequence
import PyUtils.Logging as L
msg = L.logging.getLogger('DerivationFramework__ContentHandler')
msg.setLevel(L.logging.INFO)
from DerivationFrameworkCore.ContainersForExpansion import ContainersForExpansion

class ContentHandler:
	def __init__(self,inputName,namesAndTypes):
		self.name = inputName
		self.AppendToDictionary = {}
		self.NamesAndTypes = namesAndTypes
		self.ContainersForExpansion = ContainersForExpansion
	
	def mainContainerLine(self,containerName):
		theDictionary = self.NamesAndTypes.copy()
		theDictionary.update (self.AppendToDictionary)
		line = ''
		if containerName in theDictionary.keys():
			line = theDictionary[containerName]+"#"+containerName
		else:
			msg.warning('Collection with name %s not found in input file or in definitions from user. No action will be taken for this collection.' % containerName)
		return line
		

	def expandAuxStore(self,auxContainerName):
		# Expand to dynamic variables via the AuxStoreWrapper
		wrapperName = auxContainerName+"Wrapper"
		if wrapperName.startswith( "HLT_xAOD__" ):
			wrapperName = wrapperName[ len( "HLT_xAOD__" ) : ]
			pass
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
			if len(components)==0:
				continue	
			mainItem = self.mainContainerLine(components[0])
			if mainItem=='':
				continue
			if len(components)==1:
				# deal with main containers
				if (mainItem not in mainOutput):
					mainOutput.append(mainItem)
			if len(components)>1:
				# Deal with the (few) containers that still need expanding
				if (mainItem.split('#')[0] in self.ContainersForExpansion):
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
