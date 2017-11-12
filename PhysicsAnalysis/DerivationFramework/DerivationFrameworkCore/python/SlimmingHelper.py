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
from DerivationFrameworkCore.ContainersForExpansion import ContainersForExpansion
from DerivationFrameworkCore.ContainersOnTheFly import ContainersOnTheFly
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.GlobalFlags  import globalflags
import PyUtils.Logging as L
msg = L.logging.getLogger('DerivationFramework__SlimmingHelper')
msg.setLevel(L.logging.INFO)
#from TrigEDMConfig.TriggerEDMAnalysis import *

# This list base class allows the slimming helper to be locked after calling AppendContentToStream
class lockable_list(list):
        def __init__(self,data=[]):
                list.__init__(self,data)
                self.__dict__["_locked"] = False                
        def append(self,name):
                if self._locked == True:
                        msg.error("Attempting to Modify SlimmingHelper after AppendContentToStream has Been Called")
                        raise RuntimeError("Late Modification to SlimmingHelper do not modify after calling AppendContentToStream")
                else:
                        return list.append(self, name)
        def __setattr__(self,name,value):
                if self._locked==True:
                        msg.error("Attempting to Modify SlimmingHelper after AppendContentToStream has Been Called")
                        raise RuntimeError("Late Modification to SlimmingHelper do not modify after calling AppendContentToStream")
                else:
                        self.__dict__[name] = value
        def lock(self):
                self.__dict__["_locked"] = True

def buildNamesAndTypes():
        from RecExConfig.InputFilePeeker import inputFileSummary
        namesAndTypes = {}
        if inputFileSummary['eventdata_items'] is not None:
                for item in inputFileSummary['eventdata_items']:
                        namesAndTypes[item[1].strip('.')] = item[0]
        else:
                from DerivationFrameworkCore.StaticNamesAndTypes import StaticNamesAndTypes
                namesAndTypes = StaticNamesAndTypes
        return namesAndTypes    

class SlimmingHelper:
        def __init__(self,inputName):
                self.__dict__["_locked"] = False
                self.name = inputName
                self.StaticContent = lockable_list() # Content added explicitly via old-style content lists
                self.ExtraVariables = lockable_list() # Content added by users via variable names (dictionary type:[item1,item,..,N])
                # Smart slimming (only variables needed for CP + kinematics)
                self.SmartCollections = lockable_list() 
                self.AllVariables = lockable_list() # Containers for which all branches should be kept
                self.AppendToDictionary = {} 
                self.NamesAndTypes = buildNamesAndTypes()
                self.theHandler = ContentHandler(self.name+"Handler",self.NamesAndTypes)
                self.IncludeMuonTriggerContent = False
                self.IncludeEGammaTriggerContent = False
                self.IncludeJetTauEtMissTriggerContent = False
                self.IncludeJetTriggerContent = False
                self.IncludeTauTriggerContent = False
                self.IncludeEtMissTriggerContent = False
                self.IncludeBJetTriggerContent = False
                self.IncludeBPhysTriggerContent = False
                self.IncludeMinBiasTriggerContent = False
                
        # This hack prevents any members from being modified after lock is set to true, this happens in AppendContentToStream
        def __setattr__(self,name,value):
                if self._locked==True:
                        msg.error("Attempting to Modify SlimmingHelper "+self.name+" After AppendContentToStream has Been Called")
                        raise RuntimeError("Late Modification to SlimmingHelper, do not modifiy after calling AppendContentToStream")
                elif type(value)==list:
                        self.__dict__[name] = lockable_list(value)
                else:
                        self.__dict__[name] = value

        # Function to check the configuration of the Smart Slimming List
        def CheckList(self,masterList):
                conflicted_items=[]
                for item in CompulsoryContent:          
                        if item.endswith("#*"):
                                compare_str=item[:-2].replace("xAOD::","")
                                for m_item in masterList:                                       
                                        if m_item.startswith(compare_str):
                                                conflicted_items.append(m_item)
                if len(conflicted_items)!=0:
                        msg.error("Smart Slimming lists attempting to add " +str(conflicted_items)+" which are already included in Compulsory content please remove these items from Smart Slimming List")
                        raise RuntimeError("Conflict in Smart Slimming List and Compulsory Content")

                                        
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

                # Trigger objects: add them by hand to the smart collection list (to keep the previous interface)
                triggerContent = False
                if (self.IncludeMuonTriggerContent == True):
                        triggerContent = True
                        self.SmartCollections.append("HLT_xAOD__MuonContainer_MuonEFInfo")

                if (self.IncludeEGammaTriggerContent == True):
                        triggerContent = True
                        self.SmartCollections.append("HLT_xAOD__PhotonContainer_egamma_Photons")

                if (self.IncludeJetTriggerContent == True):
                        triggerContent = True
                        self.SmartCollections.append("HLT_xAOD__JetContainer_a4tcemsubjesFS")
                        from DerivationFrameworkCore.JetTriggerFixContent import JetTriggerFixContent
                        for item in JetTriggerFixContent:
                                Stream.AddItem(item)
                
                if (self.IncludeEtMissTriggerContent == True):
                        triggerContent = True
                        self.SmartCollections.append("HLT_xAOD__TrigMissingETContainer_TrigEFMissingET")
                        from DerivationFrameworkCore.EtMissTriggerFixContent import EtMissTriggerFixContent
                        for item in EtMissTriggerFixContent:
                                Stream.AddItem(item)
                        
                if (self.IncludeTauTriggerContent == True):
                        triggerContent = True
                        self.SmartCollections.append("HLT_xAOD__TauJetContainer_TrigTauRecMerged")
                        
                if (self.IncludeBJetTriggerContent == True):
                        triggerContent = True
                        self.SmartCollections.append("HLT_xAOD__BTaggingContainer_HLTBjetFex")
                        
                if (self.IncludeBPhysTriggerContent == True):
                        triggerContent = True
                        self.SmartCollections.append("HLT_xAOD__TrigBphysContainer_EFBMuMuFex")
                                
                if (self.IncludeMinBiasTriggerContent == True):
                        triggerContent = True
                        self.SmartCollections.append("HLT_xAOD__TrigVertexCountsContainer_vertexcounts")
                        
                # Smart items
                if len(self.SmartCollections)>0:
                        for collection in self.SmartCollections:
                                masterItemList.extend(self.GetSmartItems(collection))
                                #masterItemList.extend(self.GetKinematicsItems(collection))

                # Run some basic tests to prevent clashes with CompulsoryContent content                
                self.CheckList(masterItemList)
                
                # Add extra variables
                if len(self.ExtraVariables)>0:
                        for item in self.ExtraVariables:
                                masterItemList.extend(self.GetExtraItems(item))
                
                #Add on-the-fly containers to the dictionary
                for _cont,_type in ContainersOnTheFly:
                        if not self.AppendToDictionary.has_key(_cont):
                                self.AppendToDictionary[_cont]=_type

                # Process the master list...
                                                                       
                # Main containers (this is a simple list of lines, one per container X collection)
                mainEntries = []
                # Aux items (this is a dictionary: collection name and list of aux variables)
                auxEntries = {}
                self.theHandler.AppendToDictionary = self.AppendToDictionary
                mainEntries,auxEntries = self.theHandler.GetContent(masterItemList,allVariablesList)
                
                # Add processed items to the stream
                excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation" #  From https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetExample/InDetRecExample/trunk/share/WriteInDetAOD.py#L41
                excludedAuxEntries= [entry.strip("-") for entry in excludedAuxData.split(".")]

                for item in mainEntries:
                        Stream.AddItem(item)
                for item in auxEntries.keys():
                        theDictionary = dict(self.NamesAndTypes.items() + self.AppendToDictionary.items())
                        if item in theDictionary.keys():
                                if (theDictionary[item]=='xAOD::JetAuxContainer'):
                                        entry = "xAOD::JetAuxContainer#"+item+"." 
                                elif (theDictionary[item]=='xAOD::ShallowAuxContainer'):
                                        entry = "xAOD::ShallowAuxContainer#"+item+"."
                                elif ("AuxInfo" in theDictionary[item]):
                                        entry = "xAOD::AuxInfoBase#"+item+"."
                                elif (theDictionary[item]=='xAOD::MissingETAuxAssociationMap'):
                                        entry = "xAOD::MissingETAuxAssociationMap#"+item+"."
                                elif (theDictionary[item]=='xAOD::EventInfo'):
                                        entry = "xAOD::AuxInfoBase!#"+item+"."
                                elif (theDictionary[item]=='xAOD::EventShape'):
                                        entry = "xAOD::AuxInfoBase!#"+item+"." 
                                # Next two lines - remaining containers 
                                # that still need to be expanded with AuxStoreWrapper
                                elif (theDictionary[item] in ContainersForExpansion):
                                        entry = "xAOD::AuxContainerBase#"+item+"."
                                else:
                                        entry = "xAOD::AuxContainerBase!#"+item+"."   
                                for element in auxEntries[item]:
                                        if (theDictionary[item.replace("Aux","")]=='xAOD::TrackParticleContainer') and element in excludedAuxEntries:continue #Skip anything that shouldn't be written out to a DAOD for tracks
                                        length = len(auxEntries[item])
                                        if (element==(auxEntries[item])[length-1]):
                                                entry += element
                                        else: 
                                                entry += element+"."
                                if theDictionary[item.replace("Aux","")]=='xAOD::TrackParticleContainer' and auxEntries[item]=="":
                                        entry+=excludedAuxData
                                Stream.AddItem(entry)   

                # Add compulsory items not covered by smart slimming (so no expansion)
                for item in CompulsoryContent:
                        Stream.AddItem(item)

                # Add trigger item (not covered by smart slimming so no expansion)
                # Old, will be removed (kept just to not break some deriavtions)
                if (self.IncludeJetTauEtMissTriggerContent == True):
                        from DerivationFrameworkCore.JetTauEtMissTriggerContent import JetTauEtMissTriggerContent
                        for item in JetTauEtMissTriggerContent:
                                Stream.AddItem(item)
                        
                # JetTrigger: not slimmed for now because of CLID issue 
                #if (self.IncludeJetTriggerContent == True):
                #       triggerContent = True
                #       from DerivationFrameworkCore.JetTriggerContent import JetTriggerContent
                #       for item in JetTriggerContent:
                #               Stream.AddItem(item)
                                
                # Same issue for BJetTrigger
                #if (self.IncludeBJetTriggerContent == True):
                #       triggerContent = True
                #       from DerivationFrameworkFlavourTag.BJetTriggerContent import BJetTriggerContent
                #       for item in BJetTriggerContent:
                #       Stream.AddItem(item)

                # non xAOD collections for MinBias      
                if (self.IncludeMinBiasTriggerContent == True):
                        from DerivationFrameworkCore.MinBiasTrigger_nonxAOD_Content import MinBiasTrigger_nonxAOD_Content
                        for item in MinBiasTrigger_nonxAOD_Content:
                                Stream.AddItem(item)
                                
                if (triggerContent):
                        for item in CompulsoryTriggerNavigation:
                                Stream.AddItem(item)
                                
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
                        msg.error("These static items contain wildcards: not permitted")
                        print badItemsWildcards
                        raise RuntimeError("Static content list contains wildcards")
                if (len(badItemsXAOD)>0):
                        msg.error("These static items are xAOD collections: not permitted")
                        print badItemsXAOD
                        raise RuntimeError("Static content list contains xAOD collections")             
                #Prevent any more modifications As they will be completely ignored, and hard to debug
                print self.ExtraVariables,dir(self.ExtraVariables)
               
                self.StaticContent.lock()
                self.ExtraVariables.lock()
                self.SmartCollections.lock()
                self.AllVariables.lock()
                self._locked=True 

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
                        from DerivationFrameworkEGamma.ElectronsCPContent import ElectronsCPContent
                        #from DerivationFrameworkCore.ElectronsCPContent import ElectronsCPContent
                        items.extend(ElectronsCPContent)
                elif collectionName=="Photons":
                        from DerivationFrameworkEGamma.PhotonsCPContent import PhotonsCPContent
                        #from DerivationFrameworkCore.PhotonsCPContent import PhotonsCPContent
                        items.extend(PhotonsCPContent)
                elif collectionName=="Muons":
                        from DerivationFrameworkMuons.MuonsCPContent import MuonsCPContent
#                       from DerivationFrameworkCore.MuonsCPContent import MuonsCPContent
                        items.extend(MuonsCPContent)
                elif collectionName=="TauJets":
                        from DerivationFrameworkTau.TauJetsCPContent import TauJetsCPContent
                        items.extend(TauJetsCPContent)
                elif collectionName=="TauMVATESJets":
                        from DerivationFrameworkTau.TauMVATESContent import TauMVATESContent
                        items.extend(TauMVATESContent)
                elif collectionName=="MET_Reference_AntiKt4LCTopo":
                        from DerivationFrameworkJetEtMiss.MET_Reference_AntiKt4LCTopoCPContent import MET_Reference_AntiKt4LCTopoCPContent
                        #from DerivationFrameworkCore.MET_Reference_AntiKt4LCTopoCPContent import MET_Reference_AntiKt4LCTopoCPContent
                        items.extend(MET_Reference_AntiKt4LCTopoCPContent)
                elif collectionName=="MET_Reference_AntiKt4EMTopo":
                        from DerivationFrameworkJetEtMiss.MET_Reference_AntiKt4EMTopoCPContent import MET_Reference_AntiKt4EMTopoCPContent
                        #from DerivationFrameworkCore.MET_Reference_AntiKt4EMTopoCPContent import MET_Reference_AntiKt4EMTopoCPContent
                        items.extend(MET_Reference_AntiKt4EMTopoCPContent)
                elif collectionName=="MET_Reference_AntiKt4EMPFlow":
                        from DerivationFrameworkJetEtMiss.MET_Reference_AntiKt4EMPFlowCPContent import MET_Reference_AntiKt4EMPFlowCPContent
                        items.extend(MET_Reference_AntiKt4EMPFlowCPContent)
                elif collectionName=="AntiKt4TruthJets":
                        from DerivationFrameworkJetEtMiss.AntiKt4TruthJetsCPContent import AntiKt4TruthJetsCPContent
                        items.extend(AntiKt4TruthJetsCPContent)
                elif collectionName=="AntiKt4TruthWZJets":
                        from DerivationFrameworkJetEtMiss.AntiKt4TruthWZJetsCPContent import AntiKt4TruthWZJetsCPContent
                        items.extend(AntiKt4TruthWZJetsCPContent)
                elif collectionName=="AntiKt4TruthDressedWZJets":
                        from DerivationFrameworkJetEtMiss.AntiKt4TruthDressedWZJetsCPContent import AntiKt4TruthDressedWZJetsCPContent
                        items.extend(AntiKt4TruthDressedWZJetsCPContent)
                elif collectionName=="AntiKt2LCTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt2LCTopoJetsCPContent import AntiKt2LCTopoJetsCPContent
                        #from DerivationFrameworkCore.AntiKt2LCTopoJetsCPContent import AntiKt2LCTopoJetsCPContent
                        items.extend(AntiKt2LCTopoJetsCPContent)
                elif collectionName=="AntiKt4LCTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt4LCTopoJetsCPContent import AntiKt4LCTopoJetsCPContent
                        #from DerivationFrameworkCore.AntiKt4LCTopoJetsCPContent import AntiKt4LCTopoJetsCPContent
                        items.extend(AntiKt4LCTopoJetsCPContent)
                elif collectionName=="AntiKt6LCTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt6LCTopoJetsCPContent import AntiKt6LCTopoJetsCPContent
                        #from DerivationFrameworkCore.AntiKt6LCTopoJetsCPContent import AntiKt6LCTopoJetsCPContent
                        items.extend(AntiKt6LCTopoJetsCPContent)
                elif collectionName=="AntiKt4EMTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt4EMTopoJetsCPContent import AntiKt4EMTopoJetsCPContent
                        #from DerivationFrameworkCore.AntiKt4EMTopoJetsCPContent import AntiKt4EMTopoJetsCPContent
                        items.extend(AntiKt4EMTopoJetsCPContent)
                elif collectionName=="AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets":
                        from DerivationFrameworkJetEtMiss.AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsCPContent import AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsCPContent
                        #from DerivationFrameworkCore.AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsCPContent import AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsCPContent
                        if not self.AppendToDictionary.has_key("AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"):
                                self.AppendToDictionary["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]='xAOD::JetContainer'
                                self.AppendToDictionary["AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux"]='xAOD::JetAuxContainer'
                        if not self.AppendToDictionary.has_key("AntiKt10TruthTrimmedPtFrac5SmallR20Jets"):
                                self.AppendToDictionary["AntiKt10TruthTrimmedPtFrac5SmallR20Jets"]="xAOD::JetContainer"
                                self.AppendToDictionary["AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux"]='xAOD::JetAuxContainer'
                        items.extend(AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsCPContent)
                elif collectionName=="AntiKt4EMPFlowJets":
                        from DerivationFrameworkJetEtMiss.AntiKt4EMPFlowJetsCPContent import AntiKt4EMPFlowJetsCPContent
                        #from DerivationFrameworkCore.AntiKt4EMPFlowJetsCPContent import AntiKt4EMPFlowJetsCPContent
                        items.extend(AntiKt4EMPFlowJetsCPContent)
                elif collectionName=="AntiKt2LCTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt2LCTopoJetsCPContent import AntiKt2LCTopoJetsCPContent
                        items.extend(AntiKt2LCTopoJetsCPContent)
                elif collectionName=="AntiKt3LCTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt3LCTopoJetsCPContent import AntiKt3LCTopoJetsCPContent
                        items.extend(AntiKt3LCTopoJetsCPContent)
                elif collectionName=="AntiKt5LCTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt5LCTopoJetsCPContent import AntiKt5LCTopoJetsCPContent
                        items.extend(AntiKt5LCTopoJetsCPContent)
                elif collectionName=="AntiKt6LCTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt6LCTopoJetsCPContent import AntiKt6LCTopoJetsCPContent
                        items.extend(AntiKt6LCTopoJetsCPContent)
                elif collectionName=="AntiKt7LCTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt7LCTopoJetsCPContent import AntiKt7LCTopoJetsCPContent
                        items.extend(AntiKt7LCTopoJetsCPContent)
                elif collectionName=="AntiKt8LCTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt8LCTopoJetsCPContent import AntiKt8LCTopoJetsCPContent
                        items.extend(AntiKt8LCTopoJetsCPContent)
                elif collectionName=="BTagging_AntiKt4LCTopo":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKt4LCTopoJets"))
                elif collectionName=="BTagging_AntiKt4EMTopo":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKt4EMTopoJets"))
                elif collectionName=="BTagging_AntiKt4EMPFlow":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKt4EMPFlowJets"))
                elif collectionName=="BTagging_AntiKt2Track":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKt2TrackJets"))
                elif collectionName=="BTagging_AntiKt3Track":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKt3TrackJets"))
                elif collectionName=="BTagging_AntiKt4Track":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKt4TrackJets"))
                elif collectionName=="BTagging_AntiKtVR30Rmax4Rmin02Track":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKtVR30Rmax4Rmin02TrackJets"))
                elif collectionName=="InDetTrackParticles":
                        #from DerivationFrameworkInDet.InDetTrackParticlesCPContent import InDetTrackParticlesCPContent
                        from DerivationFrameworkCore.InDetTrackParticlesCPContent import InDetTrackParticlesCPContent
                        items.extend(InDetTrackParticlesCPContent)
                elif collectionName=="PrimaryVertices":
                        #from DerivationFrameworkInDet.PrimaryVerticesCPContent import PrimaryVerticesCPContent
                        from DerivationFrameworkCore.PrimaryVerticesCPContent import PrimaryVerticesCPContent
                        items.extend(PrimaryVerticesCPContent)
                elif collectionName=="HLT_xAOD__MuonContainer_MuonEFInfo":
                        from DerivationFrameworkMuons.MuonTriggerContent import MuonTriggerContent
                        items.extend(MuonTriggerContent)
                elif collectionName=="HLT_xAOD__PhotonContainer_egamma_Photons":
                        from DerivationFrameworkCore.EGammaTriggerContent import EGammaTriggerContent
                        items.extend(EGammaTriggerContent)
                elif collectionName=="HLT_xAOD__JetContainer_a4tcemsubjesFS":
                        from DerivationFrameworkCore.JetTriggerContent import JetTriggerContent 
                        items.extend(JetTriggerContent)
                elif collectionName=="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET":
                        from DerivationFrameworkCore.EtMissTriggerContent import EtMissTriggerContent 
                        items.extend(EtMissTriggerContent)
                elif collectionName=="HLT_xAOD__TauJetContainer_TrigTauRecMerged":
                        from DerivationFrameworkCore.TauTriggerContent import TauTriggerContent 
                        items.extend(TauTriggerContent)
                elif collectionName=="HLT_xAOD__BTaggingContainer_HLTBjetFex":
                        from DerivationFrameworkFlavourTag.BJetTriggerContent import BJetTriggerContent
                        items.extend(BJetTriggerContent)
                elif collectionName=="HLT_xAOD__TrigBphysContainer_EFBMuMuFex":
                        from DerivationFrameworkCore.BPhysTriggerContent import BPhysTriggerContent 
                        items.extend(BPhysTriggerContent)
                elif collectionName=="HLT_xAOD__TrigVertexCountsContainer_vertexcounts":
                        from DerivationFrameworkCore.MinBiasTriggerContent import MinBiasTriggerContent 
                        items.extend(MinBiasTriggerContent)
                else:
                        raise RuntimeError("Smart slimming container "+collectionName+" does not exist or does not have a smart slimming list") 
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
                if ("xAOD::" in item and sep[1] in self.NamesAndTypes.keys()):
                        return "XAOD"
                return "OK"     


  
