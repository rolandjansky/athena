
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from AthenaCommon.AlgSequence import AlgSequence,AthSequencer

# create electron and muon selectors
sequencer = AthSequencer("HSG5ZBB_Sequencer", StopOverride=False)

from D2PDMaker.D2PDMakerConf import D2PDJetSelector
boostedJetCollectionName = 'HSG5ZBB_boosted_LooseBoostedJetLinkCollection'
sequencer += D2PDJetSelector( "HSG5ZBB_BoostedJetFilter",
                              inputCollection      = 'CamKt12LCTopoJets',
                              outputLinkCollection = boostedJetCollectionName,
                              minNumberPassed      = 1,
                              ptMin                = 150.0*Units.GeV)
HSG5D3PD_Stream.RequireAlgs.append("HSG5ZBB_BoostedJetFilter")

from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
ToolSvc += DeltaRAssociationTool( "HSG5ZBB_subjetBoostedJetDeltaRAssociationTool",
                                  OutputLevel = INFO,
                                  inputAssociateToCollection = boostedJetCollectionName,
                                  deltaRMax = 1.3,
                                  writeUserData = False)

if not 'IS_SIMULATION' in inputFileSummary['evt_type']:
    sequencer += D2PDJetSelector( "HSG5ZBB_btaggedSubjetFilter",
                                  OutputLevel = DEBUG,
                                  inputCollection      = 'CamKt12LCTopoSplitFiltSubjetsminSplitR0Jets',
                                  outputLinkCollection = 'HSG5ZBB_btaggedSubjetLinkCollection',
                                  associationToolList = [ ToolSvc.HSG5ZBB_subjetBoostedJetDeltaRAssociationTool ],
                                  outputAssociationContainerList = [ "HSG5ZBB_subjetsMatchedToBoostedJets" ],
                                  numberOfAssociationsMinCutList = [ 2 ],
                                  jetFlavourTagName    = 'JetFitterCOMBNN',
                                  jetFlavourTagWeightMin = -1.25,
                                  minNumberPassed      = 2)
    HSG5D3PD_Stream.RequireAlgs.append("HSG5ZBB_btaggedSubjetFilter")

topSequence+=sequencer
