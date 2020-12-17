# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file TruthD3PDMaker/python/evgenD3PD.py
# @author Renaud Bruneliere <Renaud.Bruneliere@cern.ch>
# @date Apr, 2010
# @brief Construct an evgen D3PD.
#


import D3PDMakerCoreComps

from EventCommonD3PDMaker.EventInfoD3PDObject        import EventInfoD3PDObject
from TruthD3PDMaker.TruthJetD3PDObject               import TruthJetD3PDObject
from TruthD3PDMaker.Atlfast1MissingETD3PDObject      import TruthMETD3PDObject

from TruthD3PDMaker.GenEventD3PDObject               import GenEventD3PDObject
from TruthD3PDAnalysis.truthParticleConfig           import truthParticleConfig
from TruthD3PDMaker.TruthJetFilterConfig             import TruthJetFilterConfig
from TruthD3PDMaker.TruthParticleD3PDObject          import TruthParticleD3PDObject
from TruthD3PDMaker.PartonJetConfig                  import PartonJetConfig
from RecExConfig.RecFlags                            import rec
from JetRec.JetGetters                               import make_StandardJetGetter

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


def evgenD3PD (file,
               tuplename = 'evgen',
               seq = topSequence,
               D3PDSvc = 'D3PD::RootD3PDSvc'):

    #--------------------------------------------------------------------------
    # Configuration
    #--------------------------------------------------------------------------
    if rec.doTruth():
        # compatibility with jets
        from JetRec.JetRecFlags import jobproperties as jobpropjet
        jobpropjet.JetRecFlags.inputFileType = "GEN"

        # Build list of particles stored in D3PD
        truthParticleConfig (seq)

        # Build list of particles used to jet building
        # Be careful, if doExcludeWZdecays == True, 
        # then W and Z decays (including electrons, QCD FSRs) are excluded from jet building
        doExcludeWZdecays = True
        mysuffix = 'WZ' if doExcludeWZdecays else ''
        TruthJetFilterConfig (seq, writePartons = True, writeHadrons = True,
                              excludeWZdecays = doExcludeWZdecays)

        # PartonJetConfig is used to build parton-level jets
        # PartonJetConfig requires JetSimTools-00-01-22 or higher
        PartonJetConfig (doPythia = True, doHerwig = False,
                         finder = 'AntiKt', size = 0.4, suffix = mysuffix,
                         inputCollections = ['FilteredD3PDTruth'])
        PartonJetConfig (doPythia = True, doHerwig = False,
                         finder = 'AntiKt', size = 0.6, suffix = mysuffix,
                         inputCollections = ['FilteredD3PDTruth'])

        # Build truth particle (hadron-level) jets
        # flags for AOD
        from ParticleBuilderOptions.AODFlags import AODFlags
        AODFlags.MissingEtTruth = True
        AODFlags.TruthParticleJet = True
        AODFlags.McEventKey="GEN_EVENT"
        # The function that makes the truth jets, with appropriate arguments
        antikt4truthAlg = make_StandardJetGetter('AntiKt',0.4,'Truth',globalSuff=mysuffix,disable=False,includeMuons=True,useInteractingOnly=False).jetAlgorithmHandle()
        antikt4truthAlg.AlgTools['InputToJet'].InputCollectionKeys = ['FilteredD3PDTruth']
        antikt6truthAlg = make_StandardJetGetter('AntiKt',0.6,'Truth',globalSuff=mysuffix,disable=False,includeMuons=True,useInteractingOnly=False).jetAlgorithmHandle()
        antikt6truthAlg.AlgTools['InputToJet'].InputCollectionKeys = ['FilteredD3PDTruth']
        if doExcludeWZdecays:
            # Reconstruct standard ATLAS truth jets
            antikt4truthAlgStd = make_StandardJetGetter('AntiKt',0.4,'Truth',disable=False).jetAlgorithmHandle()  # noqa: F841
            antikt6truthAlgStd = make_StandardJetGetter('AntiKt',0.6,'Truth',disable=False).jetAlgorithmHandle()  # noqa: F841


    #--------------------------------------------------------------------------
    # Make the D3PD
    #--------------------------------------------------------------------------
    alg = D3PDMakerCoreComps.MakerAlg(tuplename, seq,
                                      file = file, D3PDSvc = D3PDSvc)
    alg += EventInfoD3PDObject (10)

    if rec.doTruth():
        alg += GenEventD3PDObject (1)
        alg += TruthParticleD3PDObject (1)
        alg += TruthMETD3PDObject (level=10)
        alg += TruthJetD3PDObject (level=10, sgkey='AntiKt4Truth'+mysuffix+'Jets', prefix='jet_antikt4truth'+mysuffix+'jets_')
        alg += TruthJetD3PDObject (level=10, sgkey='AntiKt6Truth'+mysuffix+'Jets', prefix='jet_antikt6truth'+mysuffix+'jets_')
        if doExcludeWZdecays:
            alg += TruthJetD3PDObject (level=10, sgkey='AntiKt4TruthJets', prefix='jet_antikt4truthjets_')
            alg += TruthJetD3PDObject (level=10, sgkey='AntiKt6TruthJets', prefix='jet_antikt6truthjets_')
        alg += TruthJetD3PDObject (level=10, sgkey='AntiKt4TruthParton'+mysuffix+'Jets', prefix='jet_antikt4truthparton'+mysuffix+'jets_')
        alg += TruthJetD3PDObject (level=10, sgkey='AntiKt6TruthParton'+mysuffix+'Jets', prefix='jet_antikt6truthparton'+mysuffix+'jets_')

    return alg
