# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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

from __future__ import print_function

from DerivationFrameworkCore.CompulsoryContent import *
from DerivationFrameworkCore.ContentHandler import *
from DerivationFrameworkCore.ContainersForExpansion import ContainersForExpansion
from DerivationFrameworkCore.ContainersOnTheFly import ContainersOnTheFly
from DerivationFrameworkCore.AllVariablesDisallowed import AllVariablesDisallowed
from DerivationFrameworkCore.FullListOfSmartContainers import FullListOfSmartContainers
from DerivationFrameworkCore.PreliminarySmartContainers import PreliminarySmartContainers
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
                self.IncludeTriggerNavigation = True
                self.IncludeAdditionalTriggerContent = False
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
                        formatName = Stream.Name.strip("Stream_DAOD")
                        for item in self.AllVariables:
                                # Block AllVariables for containers with smart slimming lists, for those formats for which it is disallowed
                                if (formatName in AllVariablesDisallowed) and (item in FullListOfSmartContainers):
                                    # We have a preliminary list of smart collections as a way to roll out new ones
                                    if item in PreliminarySmartContainers:
                                        msg.warning("Using AllVariables for a container with a smart slimming list ("+item+") will soon be disabled for format "+formatName+" - please use smart slimming and/or ExtraVariables, this will be promoted to an exception soon!")
                                    else:
                                        msg.error("Using AllVariables for a container with a smart slimming list ("+item+") is not permitted for the format "+formatName+" - please use smart slimming and/or ExtraVariables")
                                        raise RuntimeError("AllVariables not permitted for requested DAOD format")
                                masterItemList.extend(self.GetWholeContentItems(item))
                for item in masterItemList:
                        if "Aux." in item:
                                allVariablesList.append(item)

                # Trigger objects: add them by hand to the smart collection list (to keep the previous interface)
                triggerContent = False

                if (self.IncludeAdditionalTriggerContent == True):
                        triggerContent = True

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

                # Run some basic tests to prevent clashes with CompulsoryContent content
                self.CheckList(masterItemList)

                # Add extra variables
                if len(self.ExtraVariables)>0:
                        for item in self.ExtraVariables:
                                masterItemList.extend(self.GetExtraItems(item))

                #Add on-the-fly containers to the dictionary
                for _cont,_type in ContainersOnTheFly:
                        if _cont not in self.AppendToDictionary:
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
                        theDictionary = self.NamesAndTypes.copy()
                        theDictionary.update (self.AppendToDictionary)
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

                # non xAOD collections for MinBias
                if (self.IncludeMinBiasTriggerContent == True):
                        from DerivationFrameworkCore.MinBiasTrigger_nonxAOD_Content import MinBiasTrigger_nonxAOD_Content
                        for item in MinBiasTrigger_nonxAOD_Content:
                                Stream.AddItem(item)

                if (triggerContent and self.IncludeTriggerNavigation):
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
                        print (badItemsWildcards)
                        raise RuntimeError("Static content list contains wildcards")
                if (len(badItemsXAOD)>0):
                        msg.error("These static items are xAOD collections: not permitted")
                        print (badItemsXAOD)
                        raise RuntimeError("Static content list contains xAOD collections")
                #Prevent any more modifications As they will be completely ignored, and hard to debug
                print (self.ExtraVariables,dir(self.ExtraVariables))

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
                if collectionName not in FullListOfSmartContainers:
                        raise RuntimeError("Smart slimming container "+collectionName+" does not exist or does not have a smart slimming list")
                if collectionName=="Electrons":
                        from DerivationFrameworkEGamma.ElectronsCPContent import ElectronsCPContent
                        items.extend(ElectronsCPContent)
                elif collectionName=="Photons":
                        from DerivationFrameworkEGamma.PhotonsCPContent import PhotonsCPContent
                        items.extend(PhotonsCPContent)
                elif collectionName=="Muons":
                        from DerivationFrameworkMuons.MuonsCPContent import MuonsCPContent
                        items.extend(MuonsCPContent)
                elif collectionName=="TauJets":
                        from DerivationFrameworkTau.TauJetsCPContent import TauJetsCPContent
                        items.extend(TauJetsCPContent)
                elif collectionName=="TauMVATESJets":
                        from DerivationFrameworkTau.TauMVATESContent import TauMVATESContent
                        items.extend(TauMVATESContent)
                elif collectionName=="DiTauJets":
                        from DerivationFrameworkTau.DiTauJetsCPContent import DiTauJetsCPContent
                        items.extend(DiTauJetsCPContent)
                elif collectionName=="DiTauJetsLowPt":
                        from DerivationFrameworkTau.DiTauJetsLowPtCPContent import DiTauJetsLowPtCPContent
                        items.extend(DiTauJetsLowPtCPContent)
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
                elif collectionName=="MET_Baseline_AntiKt4EMTopo":
                        from DerivationFrameworkJetEtMiss.MET_Baseline_AntiKt4EMTopoCPContent import MET_Baseline_AntiKt4EMTopoCPContent
                        items.extend(MET_Baseline_AntiKt4EMTopoCPContent)
                elif collectionName=="MET_Baseline_AntiKt4EMPFlow":
                        from DerivationFrameworkJetEtMiss.MET_Baseline_AntiKt4EMPFlowCPContent import MET_Baseline_AntiKt4EMPFlowCPContent
                        items.extend(MET_Baseline_AntiKt4EMPFlowCPContent)
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
                        items.extend(AntiKt2LCTopoJetsCPContent)
                elif collectionName=="AntiKt4LCTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt4LCTopoJetsCPContent import AntiKt4LCTopoJetsCPContent
                        items.extend(AntiKt4LCTopoJetsCPContent)
                elif collectionName=="AntiKt6LCTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt6LCTopoJetsCPContent import AntiKt6LCTopoJetsCPContent
                        items.extend(AntiKt6LCTopoJetsCPContent)
                elif collectionName=="AntiKt4EMTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt4EMTopoJetsCPContent import AntiKt4EMTopoJetsCPContent
                        items.extend(AntiKt4EMTopoJetsCPContent)
                elif collectionName=="AntiKt4EMTopoJets_BTagging201810":
                        if "AntiKt4EMTopoJets_BTagging201810" not in  self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt4EMTopoJets_BTagging201810"]='xAOD::JetContainer'
                                self.AppendToDictionary["AntiKt4EMTopoJets_BTagging201810Aux"]='xAOD::ShallowAuxContainer'
                elif collectionName=="AntiKt10TruthJets":
                        from DerivationFrameworkJetEtMiss.AntiKt10TruthJetsCPContent import AntiKt10TruthJetsCPContent
                        items.extend(AntiKt10TruthJetsCPContent)
                elif collectionName=="AntiKt10TruthWZJets":
                        from DerivationFrameworkJetEtMiss.AntiKt10TruthWZJetsCPContent import AntiKt10TruthWZJetsCPContent
                        items.extend(AntiKt10TruthWZJetsCPContent)
                elif collectionName=="AntiKt10LCTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt10LCTopoJetsCPContent import AntiKt10LCTopoJetsCPContent
                        items.extend(AntiKt10LCTopoJetsCPContent)
                elif collectionName=="AntiKt10TrackCaloClusterJets":
                        from DerivationFrameworkJetEtMiss.AntiKt10TrackCaloClusterJetsCPContent import AntiKt10TrackCaloClusterJetsCPContent
                        items.extend(AntiKt10TrackCaloClusterJetsCPContent)
                elif collectionName=="AntiKt10UFOCSSKJets":
                        from DerivationFrameworkJetEtMiss.AntiKt10UFOCSSKJetsCPContent import AntiKt10UFOCSSKJetsCPContent
                        items.extend(AntiKt10UFOCSSKJetsCPContent)
                elif collectionName=="AntiKt10UFOCHSJets":
                        from DerivationFrameworkJetEtMiss.AntiKt10UFOCHSJetsCPContent import AntiKt10UFOCHSJetsCPContent
                        items.extend(AntiKt10UFOCHSJetsCPContent)
                elif collectionName=="AntiKt10TruthTrimmedPtFrac5SmallR20Jets":
                        from DerivationFrameworkJetEtMiss.AntiKt10TruthTrimmedPtFrac5SmallR20JetsCPContent import AntiKt10TruthTrimmedPtFrac5SmallR20JetsCPContent
                        if "AntiKt10TruthTrimmedPtFrac5SmallR20Jets" not in self:
                                self.AppendToDictionary["AntiKt10TruthTrimmedPtFrac5SmallR20Jets"]="xAOD::JetContainer"
                                self.AppendToDictionary["AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux"]='xAOD::JetAuxContainer'
                        items.extend(AntiKt10TruthTrimmedPtFrac5SmallR20JetsCPContent)
                elif collectionName=="AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets":
                        from DerivationFrameworkJetEtMiss.AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsCPContent import AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsCPContent
                        if "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]='xAOD::JetContainer'
                                self.AppendToDictionary["AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux"]='xAOD::JetAuxContainer'
                        if "AntiKt10TruthTrimmedPtFrac5SmallR20Jets" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10TruthTrimmedPtFrac5SmallR20Jets"]="xAOD::JetContainer"
                                self.AppendToDictionary["AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux"]='xAOD::JetAuxContainer'
                        items.extend(AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsCPContent)
                elif collectionName=="AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets":
                        from DerivationFrameworkJetEtMiss.AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20JetsCPContent import AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20JetsCPContent
                        if "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets"]='xAOD::JetContainer'
                                self.AppendToDictionary["AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20JetsAux"]='xAOD::JetAuxContainer'
                        if "AntiKt10TruthTrimmedPtFrac5SmallR20Jets" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10TruthTrimmedPtFrac5SmallR20Jets"]="xAOD::JetContainer"
                                self.AppendToDictionary["AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux"]='xAOD::JetAuxContainer'
                        items.extend(AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20JetsCPContent)
                elif collectionName=="AntiKt10EMPFlowTrimmedPtFrac5SmallR20Jets":
                        from DerivationFrameworkJetEtMiss.AntiKt10EMPFlowTrimmedPtFrac5SmallR20JetsCPContent import AntiKt10EMPFlowTrimmedPtFrac5SmallR20JetsCPContent
                        items.extend(AntiKt10EMPFlowTrimmedPtFrac5SmallR20JetsCPContent)
                elif collectionName=="AntiKt10UFOCHSTrimmedPtFrac5SmallR20Jets":
                        from DerivationFrameworkJetEtMiss.AntiKt10UFOCHSTrimmedPtFrac5SmallR20JetsCPContent import AntiKt10UFOCHSTrimmedPtFrac5SmallR20JetsCPContent
                        items.extend(AntiKt10UFOCHSTrimmedPtFrac5SmallR20JetsCPContent)
                elif collectionName=="AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets":
                        from DerivationFrameworkJetEtMiss.AntiKt10UFOCSSKTrimmedPtFrac5SmallR20JetsCPContent import AntiKt10UFOCSSKTrimmedPtFrac5SmallR20JetsCPContent
                        if "AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10UFOCSSKTrimmedPtFrac5SmallR20Jets"]='xAOD::JetContainer'
                                self.AppendToDictionary["AntiKt10UFOCSSKTrimmedPtFrac5SmallR20JetsAux"]='xAOD::JetAuxContainer'
                        if "AntiKt10TruthTrimmedPtFrac5SmallR20Jets" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10TruthTrimmedPtFrac5SmallR20Jets"]="xAOD::JetContainer"
                                self.AppendToDictionary["AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux"]='xAOD::JetAuxContainer'
                        items.extend(AntiKt10UFOCSSKTrimmedPtFrac5SmallR20JetsCPContent)
                elif collectionName=="AntiKt10TruthSoftDropBeta100Zcut10Jets":
                        from DerivationFrameworkJetEtMiss.AntiKt10TruthSoftDropBeta100Zcut10JetsCPContent import AntiKt10TruthSoftDropBeta100Zcut10JetsCPContent
                        if "AntiKt10TruthSoftDropBeta100Zcut10Jets" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10TruthSoftDropBeta100Zcut10Jets"]="xAOD::JetContainer"
                                self.AppendToDictionary["AntiKt10TruthSoftDropBeta100Zcut10JetsAux"]='xAOD::JetAuxContainer'
                        items.extend(AntiKt10TruthSoftDropBeta100Zcut10JetsCPContent)
                elif collectionName=="AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets":
                        from DerivationFrameworkJetEtMiss.AntiKt10LCTopoCSSKSoftDropBeta100Zcut10JetsCPContent import AntiKt10LCTopoCSSKSoftDropBeta100Zcut10JetsCPContent
                        items.extend(AntiKt10LCTopoCSSKSoftDropBeta100Zcut10JetsCPContent)
                elif collectionName=="AntiKt10UFOCHSSoftDropBeta100Zcut10Jets":
                        from DerivationFrameworkJetEtMiss.AntiKt10UFOCHSSoftDropBeta100Zcut10JetsCPContent import AntiKt10UFOCHSSoftDropBeta100Zcut10JetsCPContent
                        items.extend(AntiKt10UFOCHSSoftDropBeta100Zcut10JetsCPContent)
                elif collectionName=="AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets":
                        from DerivationFrameworkJetEtMiss.AntiKt10UFOCSSKSoftDropBeta100Zcut10JetsCPContent import AntiKt10UFOCSSKSoftDropBeta100Zcut10JetsCPContent
                        if "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets"]='xAOD::JetContainer'
                                self.AppendToDictionary["AntiKt10UFOCSSKSoftDropBeta100Zcut10JetsAux"]='xAOD::JetAuxContainer'
                        if "AntiKt10TruthSoftDropBeta100Zcut10Jets" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10TruthSoftDropBeta100Zcut10Jets"]="xAOD::JetContainer"
                                self.AppendToDictionary["AntiKt10TruthSoftDropBeta100Zcut10JetsAux"]='xAOD::JetAuxContainer'
                        items.extend(AntiKt10UFOCSSKSoftDropBeta100Zcut10JetsCPContent)
                elif collectionName=="AntiKt10TruthBottomUpSoftDropBeta100Zcut5Jets":
                        from DerivationFrameworkJetEtMiss.AntiKt10TruthBottomUpSoftDropBeta100Zcut5JetsCPContent import AntiKt10TruthBottomUpSoftDropBeta100Zcut5JetsCPContent
                        if "AntiKt10TruthBottomUpSoftDropBeta100Zcut5Jets" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10TruthBottomUpSoftDropBeta100Zcut5Jets"]="xAOD::JetContainer"
                                self.AppendToDictionary["AntiKt10TruthBottomUpSoftDropBeta100Zcut5JetsAux"]='xAOD::JetAuxContainer'
                        items.extend(AntiKt10TruthBottomUpSoftDropBeta100Zcut5JetsCPContent)
                elif collectionName=="AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets":
                        from DerivationFrameworkJetEtMiss.AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5JetsCPContent import AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5JetsCPContent
                        if "AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5Jets"]='xAOD::JetContainer'
                                self.AppendToDictionary["AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5JetsAux"]='xAOD::JetAuxContainer'
                        if "AntiKt10TruthBottomUpSoftDropBeta100Zcut5Jets" not in  self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10TruthBottomUpSoftDropBeta100Zcut5Jets"]="xAOD::JetContainer"
                                self.AppendToDictionary["AntiKt10TruthBottomUpSoftDropBeta100Zcut5JetsAux"]='xAOD::JetAuxContainer'
                        items.extend(AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5JetsCPContent)
                elif collectionName=="AntiKt10TruthRecursiveSoftDropBeta100Zcut5NinfJets":
                        from DerivationFrameworkJetEtMiss.AntiKt10TruthRecursiveSoftDropBeta100Zcut5NinfJetsCPContent import AntiKt10TruthRecursiveSoftDropBeta100Zcut5NinfJetsCPContent
                        if "AntiKt10TruthRecursiveSoftDropBeta100Zcut5NinfJets" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10TruthRecursiveSoftDropBeta100Zcut5NinfJets"]="xAOD::JetContainer"
                                self.AppendToDictionary["AntiKt10TruthRecursiveSoftDropBeta100Zcut5NinfJetsAux"]='xAOD::JetAuxContainer'
                        items.extend(AntiKt10TruthRecursiveSoftDropBeta100Zcut5NinfJetsCPContent)
                elif collectionName=="AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets":
                        from DerivationFrameworkJetEtMiss.AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJetsCPContent import AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJetsCPContent
                        if "AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJets"]='xAOD::JetContainer'
                                self.AppendToDictionary["AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJetsAux"]='xAOD::JetAuxContainer'
                        if "AntiKt10TruthRecursiveSoftDropBeta100Zcut5NinfJets" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt10TruthRecursiveSoftDropBeta100Zcut5NinfJets"]="xAOD::JetContainer"
                                self.AppendToDictionary["AntiKt10TruthRecursiveSoftDropBeta100Zcut5NinfJetsAux"]='xAOD::JetAuxContainer'
                        items.extend(AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5NinfJetsCPContent)
                elif collectionName=="AntiKt4EMPFlowJets":
                        from DerivationFrameworkJetEtMiss.AntiKt4EMPFlowJetsCPContent import AntiKt4EMPFlowJetsCPContent
                        #from DerivationFrameworkCore.AntiKt4EMPFlowJetsCPContent import AntiKt4EMPFlowJetsCPContent
                        items.extend(AntiKt4EMPFlowJetsCPContent)
                elif collectionName=="AntiKt4EMPFlowJets_BTagging201810":
                        if "AntiKt4EMPFlowJets_BTagging201810" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt4EMPFlowJets_BTagging201810"]='xAOD::JetContainer'
                                self.AppendToDictionary["AntiKt4EMPFlowJets_BTagging201810Aux"]='xAOD::ShallowAuxContainer'
                elif collectionName=="AntiKt4EMPFlowJets_BTagging201903":
                        if "AntiKt4EMPFlowJets_BTagging201903" not in self.AppendToDictionary:
                                self.AppendToDictionary["AntiKt4EMPFlowJets_BTagging201903"]='xAOD::JetContainer'
                                self.AppendToDictionary["AntiKt4EMPFlowJets_BTagging201903Aux"]='xAOD::ShallowAuxContainer'
                elif collectionName=="AntiKt2TruthJets":
                        from DerivationFrameworkJetEtMiss.AntiKt2TruthJetsCPContent import AntiKt2TruthJetsCPContent
                        items.extend(AntiKt2TruthJetsCPContent)
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
                elif collectionName=="AntiKt8EMTopoJets":
                        from DerivationFrameworkJetEtMiss.AntiKt8EMTopoJetsCPContent import AntiKt8EMTopoJetsCPContent
                        items.extend(AntiKt8EMTopoJetsCPContent)
                elif collectionName=="AntiKt8EMTopoExKt2SubJets":
                        from DerivationFrameworkJetEtMiss.AntiKt8EMTopoExKt2SubJetsCPContent import AntiKt8EMTopoExKt2SubJetsCPContent
                        items.extend(AntiKt8EMTopoExKt2SubJetsCPContent)
                elif collectionName=="AntiKt8EMTopoExKt3SubJets":
                        from DerivationFrameworkJetEtMiss.AntiKt8EMTopoExKt3SubJetsCPContent import AntiKt8EMTopoExKt3SubJetsCPContent
                        items.extend(AntiKt8EMTopoExKt3SubJetsCPContent)
                elif collectionName=="AntiKt8EMTopoExCoM2SubJets":
                        from DerivationFrameworkJetEtMiss.AntiKt8EMTopoExCoM2SubJetsCPContent import AntiKt8EMTopoExCoM2SubJetsCPContent
                        items.extend(AntiKt8EMTopoExCoM2SubJetsCPContent)
                elif collectionName=="AntiKt8EMPFlowJets":
                        from DerivationFrameworkJetEtMiss.AntiKt8EMPFlowJetsCPContent import AntiKt8EMPFlowJetsCPContent
                        items.extend(AntiKt8EMPFlowJetsCPContent)
                elif collectionName=="AntiKt8EMPFlowExKt2SubJets":
                        from DerivationFrameworkJetEtMiss.AntiKt8EMPFlowExKt2SubJetsCPContent import AntiKt8EMPFlowExKt2SubJetsCPContent
                        items.extend(AntiKt8EMPFlowExKt2SubJetsCPContent)
                elif collectionName=="AntiKt8EMPFlowExKt3SubJets":
                        from DerivationFrameworkJetEtMiss.AntiKt8EMPFlowExKt3SubJetsCPContent import AntiKt8EMPFlowExKt3SubJetsCPContent
                        items.extend(AntiKt8EMPFlowExKt3SubJetsCPContent)
                elif collectionName=="AntiKt8EMPFlowExKt2GASubJets":
                        from DerivationFrameworkJetEtMiss.AntiKt8EMPFlowExKt2GASubJetsCPContent import AntiKt8EMPFlowExKt2GASubJetsCPContent
                        items.extend(AntiKt8EMPFlowExKt2GASubJetsCPContent)
                elif collectionName=="AntiKt8EMPFlowExKt3GASubJets":
                        from DerivationFrameworkJetEtMiss.AntiKt8EMPFlowExKt3GASubJetsCPContent import AntiKt8EMPFlowExKt3GASubJetsCPContent
                        items.extend(AntiKt8EMPFlowExKt3GASubJetsCPContent)
                elif collectionName=="AntiKtVR30Rmax4Rmin02TrackGhostTagJets":
                        from DerivationFrameworkFlavourTag.AntiKtVR30Rmax4Rmin02TrackGhostTagJetsCPContent import AntiKtVR30Rmax4Rmin02TrackGhostTagJetsCPContent
                        items.extend(AntiKtVR30Rmax4Rmin02TrackGhostTagJetsCPContent)
                elif collectionName=="AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810":
                        from DerivationFrameworkFlavourTag.AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810CPContent import AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810CPContent
                        items.extend(AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810CPContent)
                elif collectionName=="AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810GhostTag":
                        from DerivationFrameworkFlavourTag.AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810GhostTagCPContent import AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810GhostTagCPContent
                        items.extend(AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810GhostTagCPContent)
                elif collectionName=="AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903":
                        from DerivationFrameworkFlavourTag.AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903CPContent import AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903CPContent
                        items.extend(AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903CPContent)
                elif collectionName=="BTagging_AntiKt4EMTopo_201810":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKt4EMTopoJets_BTagging201810"))
                elif collectionName=="BTagging_AntiKt4EMPFlow_201810":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKt4EMPFlowJets_BTagging201810"))
                elif collectionName=="BTagging_AntiKt4EMPFlow_201903":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKt4EMPFlowJets_BTagging201903"))
                elif collectionName=="BTagging_AntiKt4EMPFlow":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKt4EMPFlowJets"))
                elif collectionName=="BTagging_AntiKt2Track":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKt2PV0TrackJets"))
                elif collectionName=="BTagging_AntiKt3Track":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKt3PV0TrackJets"))
                elif collectionName=="BTagging_AntiKt4Track":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKt4PV0TrackJets"))
                elif collectionName=="BTagging_AntiKtVR30Rmax4Rmin02Track_201810":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810"))
                elif collectionName=="BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810GhostTag"))
                elif collectionName=="BTagging_AntiKtVR30Rmax4Rmin02Track_201810_expert":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810"))
                elif collectionName=="BTagging_AntiKtVR30Rmax4Rmin02Track_201903":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903"))
                elif collectionName=="BTagging_AntiKtVR30Rmax4Rmin02Track_201903_expert":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903"))
                elif collectionName=="BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKtVR30Rmax4Rmin02TrackGhostTagJets"))
                elif collectionName=="BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag_expert":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKtVR30Rmax4Rmin02TrackGhostTagJets"))
                elif collectionName=="BTagging_AntiKt8EMTopoExKt2Sub":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKt8EMTopoExKt2SubJets"))
                elif collectionName=="BTagging_AntiKt8EMTopoExKt3Sub":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKt8EMTopoExKt3SubJets"))
                elif collectionName=="BTagging_AntiKt8EMTopoExCoM2Sub":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKt8EMTopoExCoM2SubJets"))
                elif collectionName=="BTagging_AntiKt8EMPFlowExKt2Sub":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKt8EMPFlowExKt2SubJets"))
                elif collectionName=="BTagging_AntiKt8EMPFlowExKt3Sub":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKt8EMPFlowExKt3SubJets"))
                elif collectionName=="BTagging_AntiKt8EMPFlowExKt2GASub":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKt8EMPFlowExKt2GASubJets"))
                elif collectionName=="BTagging_AntiKt8EMPFlowExKt3GASub":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKt8EMPFlowExKt3GASubJets"))
                elif collectionName=="BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2Sub_expert":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubJets"))
                elif collectionName=="BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3Sub_expert":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubJets"))
                elif collectionName=="BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2GASub_expert":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2GASubJets"))
                elif collectionName=="BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3GASub_expert":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3GASubJets"))
                elif collectionName=="BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub_expert":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingExpertContent
                        items.extend(BTaggingExpertContent("AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJets"))
                elif collectionName=="BTagging_DFAntiKt4HI":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("DFAntiKt4HIJets"))
                elif collectionName=="BTagging_AntiKt4HI":
                        from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent
                        items.extend(BTaggingStandardContent("AntiKt4HIJets"))
                elif collectionName=="InDetTrackParticles":
                        from DerivationFrameworkInDet.InDetTrackParticlesCPContent import InDetTrackParticlesCPContent
                        items.extend(InDetTrackParticlesCPContent)
                elif collectionName=="PrimaryVertices":
                        from DerivationFrameworkInDet.PrimaryVerticesCPContent import PrimaryVerticesCPContent
                        items.extend(PrimaryVerticesCPContent)
                elif self.IncludeAdditionalTriggerContent == True:
                        from DerivationFrameworkCore.AdditionalTriggerContent import AdditionalTriggerContent
                        items.extend(AdditionalTriggerContent)

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
