# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

####################################################################
# SlimmingHelper.py
# James.Catmore@cern.ch
# This is the main class implementing "smart slimming"
# The code has to do four things:
# (1) build a list of the items that are to be smart-slimmed
# Items look like this:
# CollectionName - for the main container
# CollectionNameAux.var1.var2.var3.varN - for the variable lists
# (2) process each of these into strings (for the main containers)
# and dictionaries (for the variable lists) and simultaneously
# expand the aux store into dynamic variables for all collections
# being slimmed - these steps (2) are done by the ContentHandler
# (3) take the resulting list and dictionary and ensure that each
# item appears once and only once
# (4) Add each item to the output stream
#
# The items come from the following sources:
# - for smart variables: from the branch lists. In this case the user
# input is just a collection name which is then used to grab the
# appropriate list: SmartCollections
# - for extra variables added by the user: directly from the user.
# In these cases the user provides the items directly in the form listed
# above: ExtraVariables
# - where the full list of variables is needed the aux item is just of
# the form CollectionNameAux., so this can be internally generated using
# the collection name from the user
# In addition to this the user may also add items directly to the output
# stream, but no expansion-to-dynamic is done, so it is important that
# this feature is only used for collections that haven't been processed
# as described above. If the user attempts to do this the output data
# may be unreadable.
####################################################################

from DerivationFrameworkCore.CompulsoryContent import *
from DerivationFrameworkCore.ContentHandler import *
from DerivationFrameworkCore.ContainerNamesAndTypes import *
from AthenaCommon.BeamFlags import jobproperties

class SlimmingHelper:
	def __init__(self,inputName):
		self.name = inputName
		self.StaticContent = [] # Content added explicitly via old-style content lists
		self.ExtraVariables = [] # Content added by users via variable names (dictionary type:[item1,item,..,N])
		# Smart slimming (only variables needed for CP + kinematics)
		self.SmartCollections = [] 
		self.AllVariables = [] # Containers for which all branches should be kept
		self.AppendToDictionary = {} 
		self.theHandler = ContentHandler(self.name+"Handler")
		self.IncludeMuonTriggerContent = False
		self.IncludeEGammaTriggerContent = False
		self.IncludeJetTauEtMissTriggerContent = False
		self.IncludeBPhysTriggerContent = False

	# The main routine: called by all job options once.  
	def AppendContentToStream(self,Stream):
	        # Master item list: all items that must be passed to the ContentHandler for processing
	        # This will now be filled
		masterItemList = []
		# All variables list: where all variables are requested, no variable lists are needed
		# This list ensures that variables are not added individually in such cases
		allVariablesList = []	       
 
		# Add all-variable collections
		if len(self.AllVariables)>0:
			for item in self.AllVariables:
				masterItemList.extend(self.GetWholeContentItems(item))
		for item in masterItemList:
			if "Aux." in item:
				allVariablesList.append(item)

	        # Smart items
		if len(self.SmartCollections)>0:
			for collection in self.SmartCollections:
				masterItemList.extend(self.GetSmartItems(collection))
				masterItemList.extend(self.GetKinematicsItems(collection))
		
		# Add extra variables
		if len(self.ExtraVariables)>0:
			for item in self.ExtraVariables:
				masterItemList.extend(self.GetExtraItems(item))
		
		# Process the master list...

		# Main containers (this is a simple list of lines, one per container X collection)
		mainEntries = []
		# Aux items (this is a dictionary: collection name and list of aux variables)
		auxEntries = {}
		self.theHandler.AppendToDictionary = self.AppendToDictionary
		mainEntries,auxEntries = self.theHandler.GetContent(masterItemList,allVariablesList)
	        
		# Add processed items to the stream
		for item in mainEntries:
			Stream.AddItem(item)
		for item in auxEntries.keys():
			theDictionary = dict(ContainerNamesAndTypes.items() + self.AppendToDictionary.items())
			if (theDictionary[item]=='xAOD::JetAuxContainer'):
				entry = "xAOD::JetAuxContainer#"+item+"." 
			else:
				entry = "xAOD::AuxContainerBase#"+item+"."
			for element in auxEntries[item]:
				length = len(auxEntries[item])
				if (element==(auxEntries[item])[length-1]):
					entry += element
				else: 
					entry += element+"." 
			Stream.AddItem(entry)	
	
	        # Add compulsory items (not covered by smart slimming so no expansion)
		for item in CompulsoryContent:
			Stream.AddItem(item)
		# Add trigger item (not covered by smart slimming so no expansion)
		triggerContent = False
		# Issues with dictionaries, and should be replaced by smart slimming soon
		# ==> Commented out for now (but will not break derivations with some flag turned ON)
		#if (self.IncludeMuonTriggerContent == True):
		#	triggerContent = True
		#	from DerivationFrameworkCore.MuonTriggerContent import MuonTriggerContent
		#	for item in MuonTriggerContent:
		#		Stream.AddItem(item)
		#if (self.IncludeEGammaTriggerContent == True):
		#	triggerContent = True
		#	from DerivationFrameworkCore.EGammaTriggerContent import EGammaTriggerContent
		#	for item in EGammaTriggerContent:
		#		Stream.AddItem(item)
		#if (self.IncludeJetTauEtMissTriggerContent == True):
		#	triggerContent = True
		#	from DerivationFrameworkCore.JetTauEtMissTriggerContent import JetTauEtMissTriggerContent
		#	for item in JetTauEtMissTriggerContent:
		#		Stream.AddItem(item)
		#if (self.IncludeBPhysTriggerContent == True):
		#	triggerContent = True
		#	from DerivationFrameworkCore.BPhysTriggerContent import BPhysTriggerContent
		#	for item in BPhysTriggerContent:
		#		Stream.AddItem(item)
		#if (triggerContent):
		#	from DerivationFrameworkCore.CompulsoryTriggerContent import CompulsoryTriggerContent
		#	for item in CompulsoryTriggerContent:
		#		Stream.AddItem(item)

		# Add non-xAOD and on-the-fly content (not covered by smart slimming so no expansion)
		badItemsWildcards = []
		badItemsXAOD = []
		for item in self.StaticContent:
			if (self.ValidateStaticContent(item)=="OK"):
				Stream.AddItem(item)
			if (self.ValidateStaticContent(item)=="WILDCARD"):
				badItemsWildcards.append(item)
			if (self.ValidateStaticContent(item)=="XAOD"):
				badItemsXAOD.append(item)
		if (len(badItemsWildcards)>0):
			print "These static items contain wildcards: not permitted"
			print badItemsWildcards
			raise RuntimeError("Static content list contains wildcards")
		if (len(badItemsXAOD)>0):
			print "These static items are xAOD collections: not permitted"
			print badItemsXAOD
			raise RuntimeError("Static content list contains xAOD collections")		

###################################################################################
###################################################################################

# Get full content (e.g. whole aux store) for this container
	def GetWholeContentItems(self,collection):
		items = [collection,collection+"Aux."]
		return items

	# Get all branches associated with all tools needed for this container
	def GetSmartItems(self,collectionName):
		# Look up what is needed for this container type
		items = []
		if collectionName=="Electrons":
			#from DerivationFrameworkEGamma.ElectronsCPContent import ElectronsCPContent
			from DerivationFrameworkCore.ElectronsCPContent import ElectronsCPContent
			items.extend(ElectronsCPContent)
		elif collectionName=="Photons":
			#from DerivationFrameworkEGamma.PhotonsCPContent import PhotonsCPContent
			from DerivationFrameworkCore.PhotonsCPContent import PhotonsCPContent
			items.extend(PhotonsCPContent)
		elif collectionName=="Muons":
			#from DerivationFrameworkMuons.MuonsCPContent import MuonsCPContent
			from DerivationFrameworkCore.MuonsCPContent import MuonsCPContent
			items.extend(MuonsCPContent)
		#elif collectionName=="MuonsTriggerMatching":
		#	from DerivationFrameworkCore.MuonsTriggerMatchingContent import MuonsTriggerMatchingContent
		#	items.extend(MuonsTriggerMatchingContent)
		elif collectionName=="TauJets":
			#from DerivationFrameworkTau.TauJetsCPContent import TauJetsCPContent
			from DerivationFrameworkCore.TauJetsCPContent import TauJetsCPContent
			items.extend(TauJetsCPContent)
		elif collectionName=="MET_Reference_AntiKt4LCTopo":
			#from DerivationFrameworkJetEtMiss.MET_Reference_AntiKt4LCTopoCPContent import MET_Reference_AntiKt4LCTopoCPContent
			from DerivationFrameworkCore.MET_Reference_AntiKt4LCTopoCPContent import MET_Reference_AntiKt4LCTopoCPContent
			items.extend(MET_Reference_AntiKt4LCTopoCPContent)
		elif collectionName=="MET_Reference_AntiKt4EMTopo":
			#from DerivationFrameworkJetEtMiss.MET_Reference_AntiKt4EMTopoCPContent import MET_Reference_AntiKt4EMTopoCPContent
			from DerivationFrameworkCore.MET_Reference_AntiKt4EMTopoCPContent import MET_Reference_AntiKt4EMTopoCPContent
			items.extend(MET_Reference_AntiKt4EMTopoCPContent)	
		elif collectionName=="AntiKt4LCTopoJets":
			#from DerivationFrameworkJetEtMiss.AntiKt4LCTopoJetsCPContent import AntiKt4LCTopoJetsCPContent
			from DerivationFrameworkCore.AntiKt4LCTopoJetsCPContent import AntiKt4LCTopoJetsCPContent
			items.extend(AntiKt4LCTopoJetsCPContent)
		elif collectionName=="AntiKt4EMTopoJets":
			#from DerivationFrameworkJetEtMiss.AntiKt4EMTopoJetsCPContent import AntiKt4EMTopoJetsCPContent
			from DerivationFrameworkCore.AntiKt4EMTopoJetsCPContent import AntiKt4EMTopoJetsCPContent
			items.extend(AntiKt4EMTopoJetsCPContent)
		elif collectionName=="BTagging_AntiKt4LCTopo":
			#from DerivationFrameworkFlavourTag.BTagging_AntiKt4LCTopoCPContent import BTagging_AntiKt4LCTopoCPContent
			from DerivationFrameworkCore.BTagging_AntiKt4LCTopoCPContent import BTagging_AntiKt4LCTopoCPContent
			items.extend(BTagging_AntiKt4LCTopoCPContent)
		elif collectionName=="BTagging_AntiKt4EMTopo":
			from DerivationFrameworkCore.BTagging_AntiKt4EMTopoCPContent import BTagging_AntiKt4EMTopoCPContent
			items.extend(BTagging_AntiKt4EMTopoCPContent)
		elif collectionName=="InDetTrackParticles":
			#from DerivationFrameworkInDet.InDetTrackParticlesCPContent import InDetTrackParticlesCPContent
			from DerivationFrameworkCore.InDetTrackParticlesCPContent import InDetTrackParticlesCPContent
			items.extend(InDetTrackParticlesCPContent)
		elif collectionName=="PrimaryVertices":
			#from DerivationFrameworkInDet.PrimaryVerticesCPContent import PrimaryVerticesCPContent
			from DerivationFrameworkCore.PrimaryVerticesCPContent import PrimaryVerticesCPContent
			items.extend(PrimaryVerticesCPContent)
		return items

	# Kinematics content only
	def GetKinematicsItems(self,collectionName):
		# Content lines in the same style as is produced by the PrintStats
		kinematicsLine = collectionName+"Aux."+"pt.eta.phi.m"
		items = [collectionName,kinematicsLine]
		return items

	# Extra content, listed via container
	def GetExtraItems(self,userInput):
		# Build up a content list in the same style as is produced by the PrintStats
		splitup = userInput.split(".")
		auxContainerName = splitup[0]+"Aux"
		items = []
		items.append(splitup[0])
		auxLine = ""		
		length = len(splitup)
		for string in splitup:
			if string==splitup[0]:
				auxLine = auxContainerName+"."
				continue
			if string==splitup[length-1]:
				auxLine = auxLine+string
			else:
				auxLine = auxLine+string+"."
		items.append(auxLine)
		return items
	
	# Check that static content is legit
	def ValidateStaticContent(self,item):
		# No wildcards
		if ("*" in item):
			return "WILDCARD"
		# No xAOD containers
		sep = item.split("#") 
		collection = sep[1]
		if (sep[1] in ContainerNamesAndTypes.keys()):
			return "XAOD"
		return "OK"	


  
