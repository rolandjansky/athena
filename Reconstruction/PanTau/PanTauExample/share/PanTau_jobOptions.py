# Make custom jets for seeding
include("PanTauExample/PanTauGenericJetGetter.py")

kt6alg = make_eflowJet('Kt', 0.6).jetAlgorithmHandle()
kt6alg.JetCollectionName="EflowKt06Jets"

kt4alg = make_eflowJet('Kt', 0.4).jetAlgorithmHandle()
kt4alg.JetCollectionName="EflowKt04Jets"

kt2alg = make_eflowJet('Kt', 0.2).jetAlgorithmHandle()
kt2alg.JetCollectionName="EflowKt02Jets"

cone7alg = make_eflowJet('Cone',0.7).jetAlgorithmHandle()
cone7alg.JetCollectionName="EflowCone7Jets"

cone4alg = make_eflowJet('Cone',0.4).jetAlgorithmHandle()
cone4alg.JetCollectionName="EflowCone4Jets"

cone2alg = make_eflowJet('Cone',0.2).jetAlgorithmHandle()
cone2alg.JetCollectionName="EflowCone2Jets"

sisCone2alg = make_eflowJet('SISCone',0.2).jetAlgorithmHandle()
sisCone2alg.JetCollectionName="EflowSISCone2Jets"

sisCone4alg = make_eflowJet('SISCone',0.4).jetAlgorithmHandle()
sisCone4alg.JetCollectionName="EflowSISCone4Jets"

antiKt2alg = make_eflowJet('AntiKt',0.2).jetAlgorithmHandle()
antiKt2alg.JetCollectionName="EflowAntiKt02Jets"

antiKt4alg = make_eflowJet('AntiKt',0.4).jetAlgorithmHandle()
antiKt4alg.JetCollectionName="EflowAntiKt04Jets"

camKt2alg = make_eflowJet('CamKt',0.2).jetAlgorithmHandle();
camKt2alg.JetCollectionName="EflowCamKt02Jets"

camKt4alg = make_eflowJet('CamKt',0.4).jetAlgorithmHandle();
camKt4alg.JetCollectionName="EflowCamKt04Jets"

# Make eflowRec cone jet
#from eflowRec.ConeJetEFlowGetter import ConeJetEFlowGetter
#ConeJetEFlow=ConeJetEFlowGetter()

# PanTau options
#from PanTauAlgs.PanTauAlgsConf import PanTau__TauSeedBuilderAlg
#panTauSeedBuilder = PanTau__TauSeedBuilderAlg('TauSeedBuilderAlg')
#panTauSeedBuilder.EflowJetCollectionName = "EflowConeJets"
#panTauSeedBuilder.OutputLevel=1
#topSequence+=panTauSeedBuilder


#PanTauAOD_ItemList=["eflowObjectContainer#eflowObjects*"]
# add all cluster container to output list
# (not all eflow cluster container are written out by default)
#PanTauAOD_ItemList+=["CaloClusterContainer#*"]






