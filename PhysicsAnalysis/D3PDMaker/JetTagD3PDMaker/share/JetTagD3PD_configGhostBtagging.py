JetTagD3PD_ListOfJetsToGhostTag = []

if JetTagD3PD_AntiKt4TopoEM:
  JetTagD3PD_ListOfJetsToGhostTag+=['AntiKt4TopoEMJets']
if JetTagD3PD_AntiKt6TopoEM:
  JetTagD3PD_ListOfJetsToGhostTag+=['AntiKt6TopoEMJets']
if JetTagD3PD_AntiKt4LCTopo:
  JetTagD3PD_ListOfJetsToGhostTag+=['AntiKt4LCTopoJets']
if JetTagD3PD_AntiKt6LCTopo:
  JetTagD3PD_ListOfJetsToGhostTag+=['AntiKt6LCTopoJets']
if JetTagD3PD_AntiKt4Truth:
  JetTagD3PD_ListOfJetsToGhostTag+=['AntiKt4TruthJets']
if JetTagD3PD_AntiKt4MuonNonInteractingTruth:
  JetTagD3PD_ListOfJetsToGhostTag+=['AntiKt4MuonNonInteractingTruthJets']
if JetTagD3PD_AntiKt6MuonNonInteractingTruth:
  JetTagD3PD_ListOfJetsToGhostTag+=['AntiKt6MuonNonInteractingTruthJets']
### track ghost assoc not supported for now for these collections
# if JetTagD3PD_AntiKtZ4TrackJets:
#   JetTagD3PD_ListOfJetsToGhostTag+=['AntiKtZ4TrackJets']


for ijet in JetTagD3PD_ListOfJetsToGhostTag:
    from JetTagD3PDMaker.JetTagGhostAssocHelper import createAssocObjects
    createAssocObjects(ijet,["TrackAssoc"])
    JetTagD3PDFlags.JetCollections+=[ijet+"ReTaggedGhost"]
    JetTagD3PDFlags.D3PDPropDict().update({ijet+"ReTaggedGhost_level":0})

include( "BTagging/BTagging_LoadTools.py" )


from BTagging.BTaggingConf import Analysis__BJetBuilder
for key in JetTagD3PD_ListOfJetsToGhostTag:
    buildername = "GhostBJetBuilder"+key
    jetBasis = "Cells"
    if key.find('Track') >= 0:
        jetBasis = "Tracks"
    if BTaggingFlags.OutputLevel < 3:
        print '### creating ',buildername,' ',jetBasis
    ghostAlg = Analysis__BJetBuilder(
        name                       = buildername,
        Runmodus                   = BTaggingFlags.Runmodus,
        TrackParticleContainerName = BTaggingFlags.TrackParticleCollectionName,
        OutputLevel                = BTaggingFlags.OutputLevel,
        BTagTool                   = myBTagTool,
        KeepTruthInfo              = not BTaggingFlags.ConvertParticleJets,
        jetFinderBasedOn           = jetBasis,
        BTagAssociation            = BTaggingFlags.doStandardAssoc,
        MuonToJetAssociator        = getattr (ToolSvc, 'BTagMuonToJetAssociator', None),     # OK: always the same
        ElectronToJetAssociator    = getattr (ToolSvc, 'BTagElectronToJetAssociator', None), # will also not be duplicated in memory
        JetCollectionName          = key)
  # -- association of tracks to jet (for the time being same one for all jet collections):
    if jetBasis == "Tracks" and not BTaggingFlags.doStandardAssoc:
        if BTaggingFlags.OutputLevel < 3:
            print '### jetBasis tracks, and use the track constituents for tagging'
        ghostAlg.ParticleToJetAssociator = None
    else:
        if BTaggingFlags.OutputLevel < 3:
            print '### jetBasis cells or tracks but using standard btagging track-jet association'
        ghostAlg.ParticleToJetAssociator =  CfgMgr.Analysis__ParticleToJetAssociator("BTagTrackToJetAssociator")
            # -- configure leptons:
    if BTaggingFlags.SoftMu:
        ghostAlg.MuonContainerName       = BTaggingFlags.MuonCollectionName
    if BTaggingFlags.SoftMuChi2:
        ghostAlg.MuonContainerName       = BTaggingFlags.MuonCollectionName
    if BTaggingFlags.SecondSoftMu:
        ghostAlg.SecondMuonContainerName = BTaggingFlags.SecondMuonCollectionName
    if BTaggingFlags.SecondSoftMuChi2:
        ghostAlg.SecondMuonContainerName = BTaggingFlags.SecondMuonCollectionName
    if BTaggingFlags.SoftEl:
        ghostAlg.ElectronContainerName   = BTaggingFlags.ElectronCollectionName
        ghostAlg.PhotonContainerName     = BTaggingFlags.PhotonCollectionName
    if BTaggingFlags.MultiSV:
        ghostAlg.AdditionalTrackToJetAssocName = [ "TracksBB" ]
        ghostAlg.AdditionalTrackToJetAssociator = [ToolSvc.BTagTrackToJetAssociatorBB ] 
    ghostAlg.UseTrackAssoc="TrackAssoc"
    ghostAlg.CollectionAppendix="ReTaggedGhost"

  # -- add the algo:
    topSequence += ghostAlg
    if BTaggingFlags.OutputLevel < 3:
        print ghostAlg

