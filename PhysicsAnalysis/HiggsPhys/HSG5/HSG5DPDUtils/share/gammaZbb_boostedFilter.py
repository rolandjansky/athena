
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from AthenaCommon.AlgSequence import AlgSequence,AthSequencer

outputLevel=INFO

# create electron and muon selectors
sequencer = AthSequencer("HSG5GAMZ_Sequencer", StopOverride=False)

if not 'IS_SIMULATION' in inputFileSummary['evt_type']:
    print "applying trigger selection"
    from PrimaryDPDMaker.TriggerFilter import TriggerFilter
    sequencer += TriggerFilter( "HSG5GAMZ_triggerFilter",
                                OutputLevel = outputLevel,
                                trigger = "EF_g([2-9][0-9][0-9]|1[2-9][0-9]+).*") # 120 or higher

    HSG5D3PD_Stream.RequireAlgs.append("HSG5GAMZ_triggerFilter")
else:
    print "no trigger selection"

zbb_or_zllSequencer = AthSequencer("HSG5GAMZ_zbb_or_zllSequencer", StopOverride=True)
    
zbbSequencer = AthSequencer("HSG5GAMZ_zbbSequencer", StopOverride=False)

boostedJetCollectionName='HSG5GAMZ_boosted_LooseBoostedJetLinkCollection'
from D2PDMaker.D2PDMakerConf import D2PDJetSelector
zbbSequencer += D2PDJetSelector( "HSG5GAMZ_BoostedJetFilter",
                                 OutputLevel = outputLevel,
                                 inputCollection      = 'CamKt12LCTopoJets',
                                 outputLinkCollection = boostedJetCollectionName,
                                 minNumberPassed      = 1,
                                 ptMin                = 80.0*Units.GeV)

from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
ToolSvc += DeltaRAssociationTool( "HSG5GAMZ_subjetBoostedJetDeltaRAssociationTool",
                                  OutputLevel = outputLevel,
                                  inputAssociateToCollection = boostedJetCollectionName,
                                  deltaRMax = 1.3,
                                  writeUserData = False)

                                   
zbbSequencer += D2PDJetSelector( "HSG5GAMZ_btaggedSubjetFilter",
                                 OutputLevel = outputLevel,
                                 inputCollection      = 'CamKt12LCTopoSplitFiltSubjetsminSplitR0Jets',
                                 outputLinkCollection = 'HSG5GAMZ_btaggedSubjetLinkCollection',
                                 associationToolList = [ ToolSvc.HSG5GAMZ_subjetBoostedJetDeltaRAssociationTool ],
                                 outputAssociationContainerList = [ "HSG5GAMZ_subjetsMatchedToBoostedJets" ],
                                 numberOfAssociationsMinCutList = [ 1 ],
                                 jetFlavourTagName    = 'JetFitterCOMBNN',
                                 jetFlavourTagWeightMin = -1.25,
                                 minNumberPassed      = 1)
zbb_or_zllSequencer+=zbbSequencer

HSG5D3PD_Stream.AcceptAlgs.append("HSG5GAMZ_btaggedSubjetFilter")

# add muon and jet algs, but don't add to RequireAlgs (they're used for LeptonJetsFilter)
outputJetContainerName="HSG5GAMZ_jetLinkCollection"
zbb_or_zllSequencer += D2PDJetSelector( "HSG5GAMZ_jetFilter",
                                        OutputLevel = outputLevel,
                                        inputCollection      = 'AntiKt4TopoEMJets',        
                                        outputLinkCollection = outputJetContainerName,
                                        ptMin                = 19.0*Units.GeV,
                                        minNumberPassed      = 0)

from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
outputMuonContainerName="HSG5GAMZ_muonLinkCollection"
zbb_or_zllSequencer += D2PDMuonSelector( "HSG5GAMZ_muonFilter",
                                         OutputLevel = outputLevel,
                                         inputCollection      = 'StacoMuonCollection',
                                         outputLinkCollection = outputMuonContainerName,
                                         ptMin                = 19.0*Units.GeV,
                                         muonRequireIsLoose   = True,
                                         minNumberPassed      = 0)

# now add LeptonJetsSelector
from HSG5DPDUtils.HSG5DPDUtilsConf import HSG5__LeptonJetsFilter
zbb_or_zllSequencer += HSG5__LeptonJetsFilter("HSG5GAMZ_leptonJetsFilter",
                                              OutputLevel = outputLevel,
                                              SelectedJetContainerName = outputJetContainerName,
                                              SelectedMuonContainerName = outputMuonContainerName,
                                              MinSumJetsAndMuon=3)

sequencer+=zbb_or_zllSequencer
HSG5D3PD_Stream.AcceptAlgs.append("HSG5GAMZ_leptonJetsFilter")

topSequence+=sequencer

print "printing topSequence"
print topSequence
