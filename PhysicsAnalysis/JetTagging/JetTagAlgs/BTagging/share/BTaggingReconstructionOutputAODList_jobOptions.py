include.block('BTagging/BTaggingReconstructionOutputAODList_jobOptions.py')
#**************   AOD list  ************************************************
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingAODList =  BTaggingFlags.btaggingAODList

printfunc ("List of containers")
printfunc (BTaggingAODList)
if len(BTaggingAODList) == 0:
    # The list is empty if b-tagging was not set up; the typical use case for this is the ESDtoAOD step.
    # Apparently we are not allowed to use wildcards in the container names... (and they cause problems for HLT).
    # Therefore, we duplicate here some code from BTagging/share/BTagging_jobOptions.py to specify the relevant Jet collections.
    # Clearly this duplication is undesirable and should be replaced with a single function.
    #JetCollectionList = ['AntiKt4LCTopoJets', 'AntiKt10LCTopoJets', 'AntiKt4EMTopoJets', 'AntiKt4TrackJets', 'AntiKt3TrackJets']
    JetCollectionList = ['AntiKt4LCTopoJets', 'AntiKt4EMTopoJets', 'AntiKt4TrackJets', 'AntiKt4EMPFlowJets', 'AntiKt2TrackJets', 'AntiKt4HIJets']
    from JetRec.JetRecFlags import jetFlags
    # VD: disbling b-tagging on TruthJets
    #if jetFlags.useTruth():
    #  JetCollectionList += [ 'AntiKt10TruthWZJets', 'AntiKt4TruthWZJets' ]
    from BTagging.BTaggingConfiguration import getConfiguration
    BTagConf = getConfiguration()
    for coll in JetCollectionList:
      BTagConf.RegisterOutputContainersForJetCollection(coll[:-4])
    BTaggingAODList = BTaggingFlags.btaggingAODList if jetFlags.writeJetsToAOD() else []
    BTaggingESDList = BTaggingFlags.btaggingAODList

#    BTaggingAODList += ["xAOD::BTaggingAuxContainer#*"]
#    BTaggingAODList += ["xAOD::BTaggingContainer#*"]
#    BTaggingAODList += ["xAOD::BTagVertexContainer#*"]
#    BTaggingAODList += ["xAOD::BTagVertexAuxContainer#*"]
#    BTaggingAODList += ["xAOD::VertexContainer#BTagging*"]
#    BTaggingAODList += ["xAOD::VertexAuxContainer#BTagging*"]

if BTaggingFlags.DoJetHitAssociation:
    BTaggingAODList += ['xAOD::TrackMeasurementValidationContainer#JetAssociatedPixelClusters',
                        'xAOD::TrackMeasurementValidationAuxContainer#JetAssociatedPixelClustersAux.']
    BTaggingAODList += ['xAOD::TrackMeasurementValidationContainer#JetAssociatedSCTClusters',
                        'xAOD::TrackMeasurementValidationAuxContainer#JetAssociatedSCTClustersAux.']
    
printfunc ("#BTAG# ESD output container list: " + str(BTaggingESDList))
printfunc ("#BTAG# AOD output container list: " + str(BTaggingAODList))
