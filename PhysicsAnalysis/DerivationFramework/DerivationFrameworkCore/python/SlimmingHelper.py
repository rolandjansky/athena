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
from AthenaCommon.BeamFlags import jobproperties

class SlimmingHelper:
	def __init__(self,inputName):
		self.name = inputName
		self.UserContent = [] # Content added explicitly by users via old-style content lists
		self.ExtraVariables = [] # Content added by users via variable names (dictionary type:[item1,item,..,N])
		# Smart slimming (only variables needed for CP + kinematics)
		self.SmartCollections = [] 
		self.AllVariables = [] # Containers for which all branches should be kept
		self.AppendToDictionary = {} 
		self.theHandler = ContentHandler(self.name+"Handler")

	# The main routine: called by all job options once.  
	def AppendContentToStream(self,Stream):
	        # Master item list: all items that must be passed to the ContentHandler for processing
	        # This will now be filled
		masterItemList = []
	        
	        # Smart items
		if len(self.SmartCollections)>0:
			for collection in self.SmartCollections:
				masterItemList.extend(self.GetSmartItems(collection))
				masterItemList.extend(self.GetKinematicsItems(collection))
		
		# Add extra variables
		if len(self.ExtraVariables)>0:
			for item in self.ExtraVariables:
				masterItemList.extend(self.GetExtraItems(item))
		
		# Add all-variable collections
		if len(self.AllVariables)>0:
			for item in self.AllVariables:
				masterItemList.extend(self.GetWholeContentItems(item))
		        
		# Process the master list...
		# Main containers (this is a simple list of lines, one per container X collection)
		mainEntries = []
		# Aux items (this is a dictionary: collection name and list of aux variables)
		auxEntries = {}
		self.theHandler.AppendToDictionary = self.AppendToDictionary
		mainEntries,auxEntries = self.theHandler.GetContent(masterItemList)
	        
		# Add processed items to the stream
		for item in mainEntries:
			Stream.AddItem(item)
		for item in auxEntries.keys():
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
		# Add user-defined content (not covered by smart slimming so no expansion)
		for item in self.UserContent:
			Stream.AddItem(item)

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
		if collectionName=="ElectronCollection":
			#from DerivationFrameworkEGamma.ElectronCollectionCPContent import ElectronCollectionCPContent
			from DerivationFrameworkCore.ElectronCollectionCPContent import ElectronCollectionCPContent
			items.extend(ElectronCollectionCPContent)
		elif collectionName=="PhotonCollection":
			#from DerivationFrameworkEGamma.PhotonCollectionCPContent import PhotonCollectionCPContent
			from DerivationFrameworkCore.PhotonCollectionCPContent import PhotonCollectionCPContent
			items.extend(PhotonCollectionCPContent)
		elif collectionName=="Muons":
			#from DerivationFrameworkMuons.MuonsCPContent import MuonsCPContent
			from DerivationFrameworkCore.MuonsCPContent import MuonsCPContent
			items.extend(MuonsCPContent)
		elif collectionName=="TauRecContainer":
			#from DerivationFrameworkTau.TauRecContainerCPContent import TauRecContainerCPContent
			from DerivationFrameworkCore.TauRecContainerCPContent import TauRecContainerCPContent
			items.extend(TauRecContainerCPContent)
		elif collectionName=="MET_RefFinal":
			#from DerivationFrameworkJetEtMiss.MET_RefFinalCPContent import MET_RefFinalCPContent
			from DerivationFrameworkCore.MET_RefFinalCPContent import MET_RefFinalCPContent
			items.extend(MET_RefFinalCPContent)
		elif collectionName=="AntiKt4LCTopoJets":
			#from DerivationFrameworkJetEtMiss.AntiKt4LCTopoJetsCPContent import AntiKt4LCTopoJetsCPContent
			from DerivationFrameworkCore.AntiKt4LCTopoJetsCPContent import AntiKt4LCTopoJetsCPContent
			items.extend(AntiKt4LCTopoJetsCPContent)
		elif collectionName=="BTagging_AntiKt4LCTopo":
			if jobproperties.Beam.energy()==4000000.0:
				#from DerivationFrameworkFlavourTag.BTagging_AntiKt4LCTopoCPContent import BTagging_AntiKt4LCTopoCPContent
				from DerivationFrameworkCore.BTagging_AntiKt4LCTopoCPContent import BTagging_AntiKt4LCTopoCPContent
				items.extend(BTagging_AntiKt4LCTopoCPContent)
			else:
				#from DerivationFrameworkFlavourTag.BTagging_AntiKt4LCTopo_DC14Run2CPContent import BTagging_AntiKt4LCTopoCPContent
				from DerivationFrameworkCore.BTagging_AntiKt4LCTopo_DC14Run2CPContent import BTagging_AntiKt4LCTopoCPContent
				items.extend(BTagging_AntiKt4LCTopoCPContent)
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
