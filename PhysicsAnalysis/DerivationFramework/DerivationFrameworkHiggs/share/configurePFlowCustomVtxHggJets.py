#============================================================================
# Configuration for custom building of PFlow jets, with b-tagging and fJVT
#   *** to be used only in HIGG1D1 ***
#============================================================================
SuffixForHggCollection   = "Hgg"
HggVertexContainerName   = SuffixForHggCollection+"PrimaryVertices"
CustomPFJetContainerName = 'AntiKt4PFlowCustomVtxHgg'

from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += [ CustomPFJetContainerName+'->AntiKt4EMPFlow_BTagging201903' ]

# Modifiying the btagging flag for the PV container; This will only affect the declaration of tools below this line
BTaggingFlags.PrimaryVertexCollectionName = HggVertexContainerName
print 'HIGG1D1.py INFO Attention...modifying BTaggingFlags.PrimaryVertexCollectionName to',HggVertexContainerName

# This is to get the latest FTag tuning for the custom pflow jets
from BTagging.JetCollectionToTrainingMaps import preTagDL2JetToTrainingMap,postTagDL2JetToTrainingMap
preTagDL2JetToTrainingMap[CustomPFJetContainerName]  = preTagDL2JetToTrainingMap['AntiKt4EMPFlow']
postTagDL2JetToTrainingMap[CustomPFJetContainerName] = postTagDL2JetToTrainingMap['AntiKt4EMPFlow']

# Build custom pflow jets
addConstModJets("AntiKt", 0.4, "PFlow", [], HIGG1D1Seq, "HIGG1D1Jets", customVxColl=SuffixForHggCollection, ptmin=5000, ptminFilter=10000, ghostArea=0.01, calibOpt="arj:pflow")
topSequence += defineEDAlg(0.4,"PFlowCustomVtx")

# Augment PFlow fJVT (+Jvt)
getPFlowfJVT(jetalg=CustomPFJetContainerName,algname="JetForwardPFlowCustomVtxHggJvtToolAlg",sequence=HIGG1D1Seq,primaryVertexCont="HggPrimaryVertices",overlapLabel="passOR")

# Updating jet moments
customJetTool = getattr(ToolSvc,CustomPFJetContainerName+'Jets')
customJetTool.unlock()
newList = []
for cjm in customJetTool.JetModifiers:
    cjmName = cjm.getName()
    if cjmName in ['trksummoms','jvf','jvt','jetorigin_setpv']:
        print 'HIGG1D1.py Updating jet moments, cloning the tool',cjmName
        nt = getattr(ToolSvc,cjmName).clone(cjmName+'Custom')
        nt.VertexContainer = HggVertexContainerName
        ToolSvc += nt
        newList.append(nt)
    else:
        newList.append(cjm)
customJetTool.JetModifiers = newList

# Recomputing the track IP with respect to the Hgg vertex; keep the same prefix as the AODFix one, because it is hardcoded for DL2 purpose
from BTagging.BTaggingConf import Analysis__BTagTrackAugmenterAlg
if hasattr(ToolSvc,'AODFixBTagIPETool'):
    print 'HIGG1D1.py Reusing the IPEstimator tool'
    BTagIPETool = ToolSvc.AODFixBTagIPETool
else:
    print 'HIGG1D1.py Defining an IPEestimator tool'
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator as IPEstimator
    BTagIPETool = IPEstimator(name="BTagIPETool")
    ToolSvc += BTagIPETool
HggBTagTrkAug = Analysis__BTagTrackAugmenterAlg(name                     = 'CustomVtxHggBTagTrackAugmenter',
                                                prefix                   = 'btagIp_',
                                                primaryVertexContainer   = HggVertexContainerName,
                                                TrackToVertexIPEstimator = BTagIPETool)

# Quark-gluon jet tagging for custom PFlow jets
addQGTaggerTool(jetalg=CustomPFJetContainerName,sequence=HIGG1D1Seq,algname="QGTaggerToolPFHggAlg",truthjetalg=truthjetalg) 

# Flavour-tagging for standard and custom PFlow jet collections
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets',CustomPFJetContainerName+'Jets'], Sequencer = HIGG1D1Seq)

# Adding the new IPE alg just before the custom pf jet tagging; Then the default collection is still tagged with the "default" ip estimator
indexCustomVtxPFJetsBTag = HIGG1D1Seq.getSequence().index('Analysis::StandAloneJetBTaggerAlg/standalone_btagging_'+CustomPFJetContainerName.lower())
HIGG1D1Seq.insert(indexCustomVtxPFJetsBTag,HggBTagTrkAug)

# Overriding the global flag for the vertex container to the default for standard pflow jets tagging
for empfbttN in ['thisBTagSecVertexing_AntiKt4EMPFlow','myBTagTool_AntiKt4EMPFlow']:
    if hasattr(ToolSvc,empfbttN):
        empfbtt = getattr(ToolSvc,empfbttN)
        empfbtt.PrimaryVertexName = "PrimaryVertices"
        if empfbttN == 'thisBTagSecVertexing_AntiKt4EMPFlow' and hasattr(ToolSvc,'NewJetFitterVxFinder_AODFix'):
            svtxTHList = []
            for svtxTH in empfbtt.SecVtxFinderList:
                if svtxTH.getName().find('NewJetFitterVxFinder') < 0:
                    svtxTHList.append(svtxTH)
                else:
                    print 'Will replace NewJetFitterVxFinder with NewJetFitterVxFinder_AODFix for AntiKt4EMPFlow vertexing to use the default ID vertex in for btagging those jets'
                    svtxTHList.append(ToolSvc.NewJetFitterVxFinder_AODFix)
            empfbtt.SecVtxFinderList = svtxTHList
                
