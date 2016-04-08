# Call as UserAlgs

# Imports
from JetRec.JetRecConf import *
from JetRecTools.JetRecToolsConf import *
from JetRec.JetGetters import *
from JetCalibTools.JetCalibToolsConf import *
from AthenaCommon.SystemOfUnits import GeV
from RecExConfig.RecFlags import rec
from AthenaCommon.AlgSequence import AlgSequence
seq = AlgSequence()

if rec.doTruth():
  # Needed for release 15 AntiKt6 truth jets
  # make_StandardJetGetter('AntiKt', 0.6, 'Truth')
  print "jason"
  import D3PDMakerCoreComps
  from D3PDMakerCoreComps.resolveSGKey                   import testSGKey
  print testSGKey ('JetCollection', 'AntiKt6TruthJets')
  if not testSGKey ('JetCollection', 'AntiKt6TruthJets'):
    make_StandardJetGetter('AntiKt', 0.6, 'Truth')

  # Add truth jet collection that includes all final state particles (including muons and neutrinos)
  antikt4truthAlgAll = make_StandardJetGetter('AntiKt',0.4,'Truth',disable=False,
                                               outputCollectionName='AntiKt4TruthJets_ALL',
                                               useInteractingOnly=False,
                                               includeMuons=True
                                             ).jetAlgorithmHandle()
  antikt6truthAlgAll = make_StandardJetGetter('AntiKt',0.6,'Truth',disable=False,
                                               outputCollectionName='AntiKt6TruthJets_ALL',
                                               useInteractingOnly=False,
                                               includeMuons=True
                                             ).jetAlgorithmHandle()

  # Configuration of this jetalgorithm adapted from TruthD3PDMaker/evgenD3PD.py
  from TruthD3PDMaker.TruthJetFilterConfig             import TruthJetFilterConfig
  # Build Particle list
  TruthJetFilterConfig (seq, writePartons = False, writeHadrons = True, excludeWZdecays = True, photonCone=0.1)
  from ParticleBuilderOptions.AODFlags import AODFlags
  AODFlags.MissingEtTruth = True
  AODFlags.TruthParticleJet = True
  AODFlags.McEventKey="GEN_EVENT"
  # Add truth jet collection that includes all final state particles (including muons and neutrinos)
  antikt4truthAlgWZ = make_StandardJetGetter('AntiKt',0.4,'Truth',disable=False,
                                            inputCollectionNames=['FilteredD3PDTruth'],
                                            outputCollectionName='AntiKt4TruthJets_WZ',
                                            useInteractingOnly=False,
                                            includeMuons=True
                                            ).jetAlgorithmHandle()
  antikt6truthAlgWZ = make_StandardJetGetter('AntiKt',0.6,'Truth',disable=False,
                                            inputCollectionNames=['FilteredD3PDTruth'],
                                            outputCollectionName='AntiKt6TruthJets_WZ',
                                            useInteractingOnly=False,
                                            includeMuons=True
                                            ).jetAlgorithmHandle()




