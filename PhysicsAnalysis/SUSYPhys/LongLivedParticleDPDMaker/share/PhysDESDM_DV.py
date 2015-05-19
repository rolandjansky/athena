#
#
# ##########################################################################################
# Electron/photon/muon augmentation
# Use the derivation framework tools directly
# ##########################################################################################

# Cache the container names (used throughout)
jetContainer = primRPVLLDESDM.DV_containerFlags.jetCollectionName
METContainer= primRPVLLDESDM.DV_containerFlags.METCollectionName
photonContainer = primRPVLLDESDM.DV_containerFlags.photonCollectionName
muonContainer = primRPVLLDESDM.DV_containerFlags.muonCollectionName
electronContainer = primRPVLLDESDM.DV_containerFlags.electronCollectionName


# ##########################################################################################
# Selection algorithms setup
# ##########################################################################################

def DVSelectionString(flags, container):
    """Construct object selection string based on the flags provided (from DVFlags)
    """

    cutList = []
    if flags.cutEtMin:  cutList.append('%s.pt > %s'%(container,flags.cutEtMin))

    cutString = 'count(' + ' && '.join(cutList) + ') > %i'%(flags.nPassed-1)

    return cutString

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool as skimtool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel

def DVTriggerSelectionString(flags):
    cutString=""
    if flags.triggers.__len__() >=1:
        cutString+=flags.triggers[0]
        if flags.triggers.__len__() >1:
            for trigger in flags.triggers[1:]:
                cutString+=" || "+trigger
                pass
            pass
        pass
    return cutString
    pass

############################################################
## Tools for combining filters
###########################################################
 ## needs package PhysicsAnalysis/DerivationFramework/DerivationFrameworkTools-00-00-20 or later..
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR


############################################################
## Trackless jet filter
###########################################################
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__DVTracklessJetFilterTool

DVTracklessJetFilterTool = DerivationFramework__DVTracklessJetFilterTool( name = "DVTracklessJetFilterTool",
                                                                          JetContainerKey = jetContainer,
                                                                          JetPtCut= primRPVLLDESDM.DV_TracklessJetFilterFlags.cutEtMin,
                                                                          JetEtaCut= primRPVLLDESDM.DV_TracklessJetFilterFlags.cutEtaMax,
                                                                          sumPtTrkCut = primRPVLLDESDM.DV_TracklessJetFilterFlags.cutSumPtTrkMax )
ToolSvc+=DVTracklessJetFilterTool

#======================================================================================================================================
#######################################################################################################################################
## should have all the pieces now - combine them into actual filters for the DerivationKernel #########################################
#######################################################################################################################################
#======================================================================================================================================


#############################################################################################
##  DV+muon filter
#############################################################################################



# Muon d0
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__RpvMuonD0Tool
DVMuonD0 = DerivationFramework__RpvMuonD0Tool( name = "DVMuonD0",
                                               CollectionName = muonContainer,
                                               SGPrefix = "DV"+muonContainer,
                                               )
ToolSvc += DVMuonD0

# Kernel for the augmentation tools
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
topSequence += DerivationFramework__DerivationKernel(
    "RPVLL_DVAugmentationKernel",
    AugmentationTools = [DVMuonD0],
    )

DVMuonTriggerFilter = skimtool( name = "DVMuonTriggerFilter",
                                expression = DVTriggerSelectionString(primRPVLLDESDM.DV_MuonFilterFlags)
                                )
ToolSvc+=DVMuonTriggerFilter
muonFilterExpression = "count("+muonContainer+".pt > "+str(primRPVLLDESDM.DV_MuonFilterFlags.cutEtMin)+" && (DV"+muonContainer+"isCombined==0 || abs(DV"+muonContainer+"D0)>1.5) )>0"

DVMuonFilterTool = skimtool( name = "DVMuonFilterTool",
                             expression = muonFilterExpression)
ToolSvc+=DVMuonFilterTool

DV_MuonFinalFilter = DerivationFramework__FilterCombinationAND( name = "DV_MuonFinalFilter",
                                                                FilterList=[DVMuonFilterTool,DVMuonTriggerFilter],
                                                                OutputLevel=INFO
                                                                )
ToolSvc+=DV_MuonFinalFilter
topSequence += kernel("RPVLL_DVMuonFilterKernel",
                      SkimmingTools = [DV_MuonFinalFilter])
RPVLLfilterNames.extend(["RPVLL_DVMuonFilterKernel"])

########################################
## photon filter for DV+electron
#######################################

DVPhotonTriggerFilter = skimtool( name = "DVPhotonTriggerFilter",
                                expression = DVTriggerSelectionString(primRPVLLDESDM.DV_PhotonFilterFlags)
                                )
ToolSvc += DVPhotonTriggerFilter

DVPhotonFilterTool = skimtool( name = "DVPhotonFilterTool",
                               expression = DVSelectionString(primRPVLLDESDM.DV_PhotonFilterFlags, photonContainer),
                               )
ToolSvc += DVPhotonFilterTool
DV_PhotonFinalFilter = DerivationFramework__FilterCombinationAND( name = "DV_PhotonFinalFilter",
                                                                  FilterList=[DVPhotonFilterTool,DVPhotonTriggerFilter],
                                                                  OutputLevel=INFO
                                                                  )

ToolSvc+=DV_PhotonFinalFilter

topSequence += kernel( "RPVLL_DV_PhotonFilterKernel",
                       SkimmingTools = [DV_PhotonFinalFilter]
                       )
RPVLLfilterNames.extend(["RPVLL_DV_PhotonFilterKernel"])


###########################################################################################
# DV+jets filter
###########################################################################################

DVMultiJetTriggerFilter = skimtool( name = "DVMultiJetTriggerFilter",
                                  expression = DVTriggerSelectionString(primRPVLLDESDM.DV_MultiJetTriggerFlags)
                                  )
ToolSvc += DVMultiJetTriggerFilter

DV4JetFilterTool = skimtool( name = "DV4JetFilterTool",
                                 expression = DVSelectionString(primRPVLLDESDM.DV_4JetFilterFlags, jetContainer),
                                        )
ToolSvc += DV4JetFilterTool


DV5JetFilterTool = skimtool( name = "DV5JetFilterTool",
                             expression = DVSelectionString(primRPVLLDESDM.DV_5JetFilterFlags, jetContainer),
                             )
ToolSvc += DV5JetFilterTool


DV6JetFilterTool = skimtool( name = "DV6JetFilterTool",
                             expression = DVSelectionString(primRPVLLDESDM.DV_6JetFilterFlags, jetContainer),
                             )
ToolSvc += DV6JetFilterTool

DV7JetFilterTool = skimtool( name = "DV7JetFilterTool",
                             expression = DVSelectionString(primRPVLLDESDM.DV_7JetFilterFlags, jetContainer),
                             )
ToolSvc += DV7JetFilterTool




DV_multiJet_offlineJetFilter = DerivationFramework__FilterCombinationOR( name = "DV_multiJet_offlineJetFilter",
                                                                        FilterList=[DV4JetFilterTool,DV5JetFilterTool,DV6JetFilterTool,DV7JetFilterTool],
                                                                        OutputLevel=INFO
                                                                        )
ToolSvc += DV_multiJet_offlineJetFilter



DV_MultiJetFinalFilter = DerivationFramework__FilterCombinationAND( name = "DV_MultiJetFinalFilter",
                                                                    FilterList=[DV_multiJet_offlineJetFilter,DVTracklessJetFilterTool,DVMultiJetTriggerFilter],
                                                                    OutputLevel=INFO
                                                                        )
ToolSvc+= DV_MultiJetFinalFilter

topSequence += kernel( "RPVLL_DV_MultiJetFilterKernel",
                       SkimmingTools = [DV_MultiJetFinalFilter],
                       )
RPVLLfilterNames.extend(["RPVLL_DV_MultiJetFilterKernel"])


############################################################
## DV+MET filter
###########################################################

DVMETTriggerFilter = skimtool( name = "DVMETTriggerFilter",
                               expression = DVTriggerSelectionString(primRPVLLDESDM.DV_METFilterFlags)
                               )

ToolSvc+=DVMETTriggerFilter

from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__DVMissingETFilterTool


DVMETFilterTool = DerivationFramework__DVMissingETFilterTool(name = "DVMETFilterTool",
                                                             METContainerKey = METContainer,
                                                             METCut=primRPVLLDESDM.DV_METFilterFlags.cutMetMin)
ToolSvc+=DVMETFilterTool

DV_METFinalFilter = DerivationFramework__FilterCombinationAND( name = "DV_METFinalFilter",
                                                               FilterList=[DVMETFilterTool,DVTracklessJetFilterTool,DVMETTriggerFilter],
                                                               OutputLevel=INFO
                                                               )
ToolSvc += DV_METFinalFilter

topSequence += kernel( "RPVLL_DV_METFilterKernel",
                       SkimmingTools = [DV_METFinalFilter],
                       )
RPVLLfilterNames.extend(["RPVLL_DV_METFilterKernel"])




