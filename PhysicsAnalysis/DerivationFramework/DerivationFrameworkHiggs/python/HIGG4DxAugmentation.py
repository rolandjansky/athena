# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#################################################
# Common code used for the HIGG4 augmentation  	#
# Z.Zinonos					#
# zenon@cern.ch					#
# Nov 2015					#
#################################################

from AthenaCommon.GlobalFlags import globalflags
# running on data or MC
DFisMC = (globalflags.DataSource()=='geant4')

def setup(HIGG4DxName, ToolSvc):

    augmentationTools=[]

    # DELTA R TOOL
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__DeltaRTool
    HIGG4DxTauJetsElectronsDeltaRTool = DerivationFramework__DeltaRTool(  name                    = "HIGG4DxTauJetsElectronsDeltaRTool",
                                                                          ContainerName           = "Electrons",
                                                                          SecondContainerName     = "TauJets",
                                                                          StoreGateEntryName      = "HIGG4DxTauJetsElectronsDeltaR")
    ToolSvc += HIGG4DxTauJetsElectronsDeltaRTool
    augmentationTools.append(HIGG4DxTauJetsElectronsDeltaRTool)

    # TAU SELECTOR TOOL
    from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauSelectionWrapper
    HIGG4DxJetBDTSigMediumSelectionWrapper = DerivationFramework__TauSelectionWrapper( name                    = "HIGG4DxJetBDTSigMediumSelectionWrapper",
                                                                                       #IsTauFlag              = IsTauFlag.JetBDTSigMedium,
                                                                                       IsTauFlag               = 20,
                                                                                       CollectionName          = "TauJets",
                                                                                       StoreGateEntryName      = "HIGG4DxJetBDTSigMedium")
    ToolSvc += HIGG4DxJetBDTSigMediumSelectionWrapper
    augmentationTools.append(HIGG4DxJetBDTSigMediumSelectionWrapper)

    HIGG4DxJetBDTSigLooseSelectionWrapper = DerivationFramework__TauSelectionWrapper( name                   = "HIGG4DxJetBDTSigLooseSelectionWrapper",
                                                                                     IsTauFlag               = 19,
                                                                                     CollectionName          = "TauJets",
                                                                                     StoreGateEntryName      = "HIGG4DxJetBDTSigLoose")
    ToolSvc += HIGG4DxJetBDTSigLooseSelectionWrapper
    augmentationTools.append(HIGG4DxJetBDTSigLooseSelectionWrapper)



    # Tau primary vertex refit
    if HIGG4DxName == "HIGG4D3":
        from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauPVTrkSelectionTool
        HIGG4DxTauPVTrkSelectionTool = DerivationFramework__TauPVTrkSelectionTool( name = "HIGG4DxTauPVTrkSelectionTool",
                                                                                   #minPt = 15000,
                                                                                   #m_maxDeltaR = 0.2,
                                                                                   UseTrueTracks = DFisMC,
                                                                                   TauContainerName = "TauJets",
                                                                                   TauPVTracksContainerName = "TauPVTracks")
        ToolSvc += HIGG4DxTauPVTrkSelectionTool
        augmentationTools.append(HIGG4DxTauPVTrkSelectionTool)

        from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauPVRefitTool
        HIGG4DxTauPVRefittingTool = DerivationFramework__TauPVRefitTool( name = "HIGG4DxTauPVRefittingTool",
                                                                         PVLinkName = "PVLink",
                                                                         RefittedLinkName = "TauRefittedPVLink",
                                                                         PVContainerName = "PrimaryVertices",
                                                                         TauTrkContainerName = "TauPVTracks",
                                                                         PVRefContainerName = "TauRefittedPrimaryVertices")
        ToolSvc += HIGG4DxTauPVRefittingTool
        augmentationTools.append(HIGG4DxTauPVRefittingTool)

    if DFisMC:
        # Tau truth matching
        from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauTruthMatchingTool
        HIGG4DxTauTruthMatchingTool = TauAnalysisTools__TauTruthMatchingTool(name="HIGG4DxTauTruthMatchingTool",
                                                                             WriteTruthTaus = True)
        
        ToolSvc += HIGG4DxTauTruthMatchingTool

        # Tau truth matching wrapper
        from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauTruthMatchingWrapper
        HIGG4DxTauTruthMatchingWrapper = DerivationFramework__TauTruthMatchingWrapper( name = "HIGG4DxTauTruthMatchingWrapper",
                                                                                TauTruthMatchingTool = HIGG4DxTauTruthMatchingTool)
        
        ToolSvc += HIGG4DxTauTruthMatchingWrapper
        augmentationTools.append(HIGG4DxTauTruthMatchingWrapper)


    #=============
    # return tools
    #=============
    return augmentationTools

