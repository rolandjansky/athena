# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file TruthD3PDMaker/python/PartonJetConfig.py
# @author Renaud Bruneliere <Renaud.Bruneliere@cern.ch>
# @date Apr, 2010
# @brief Configure algorithms to build parton-level jets
#        information on algorithm configuration can be found on
#        Reconstruction/Jet/JetSimTools/PartonTruthJets_jobOptions.py
#


import EventCommonD3PDMaker
from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from AthenaCommon.AlgSequence                 import AlgSequence
from RecExConfig.ObjKeyStore                  import cfgKeyStore
from RecExConfig.RecFlags                     import rec

from JetRec.JetMomentGetter import make_JetMomentGetter
from JetRec.JetGetters import *  

from AthenaCommon.SystemOfUnits import MeV, GeV
from JetRec.JetRecConf import JetAlgorithm

from JetSimTools.JetSimToolsConf import JetPartonSelectorTool

def PartonJetConfig (finder    = 'AntiKt',
                     size      = 0.4,
                     doPythia  = False,
                     doHerwig  = False,
                     minJetPt  = 7.*GeV,
                     absEtaMax = 10.,
                     suffix    = '',
                     inputCollections = None):

    if not rec.doTruth():
        return

    # Is the container already in SG?
    import JetRec.JetAlgConfiguration                                                        
    nameBuilder = JetRec.JetAlgConfiguration.nameBuilder                                     
    editParm    = JetRec.JetAlgConfiguration.editParm 
    sgkey = nameBuilder(finder,editParm(size),'','Truth','Parton'+suffix,'',)+'Jets'
    if cfgKeyStore.isInInput ('JetCollection', sgkey):
        return

    # Configure parton selector tool
    from AthenaCommon.AppMgr import ToolSvc
    jetPartonSelectorTool = JetPartonSelectorTool("JetPartonSelectorTool")
    jetPartonSelectorTool.DoPythia = doPythia
    jetPartonSelectorTool.DoHerwig = doHerwig
    jetPartonSelectorTool.max_absEta = absEtaMax
    #jetPartonSelectorTool.OutputLevel = INFO
    ToolSvc += jetPartonSelectorTool

    # Configure jets builder
    if inputCollections != None:
        partonJetAlg = make_StandardJetGetter(finder,size,'Truth',inputSuff='Parton'+suffix,
                                              inputCollectionNames=inputCollections,
                                              inputTools=[jetPartonSelectorTool]).jetAlgorithmHandle()
        partonJetAlg.AlgTools['JetPartonSelectorTool'].InputCollectionKeys = []
        partonJetAlg.AlgTools['JetPartonSelectorTool'].InputCollectionKeys.append(inputCollections[0])
        print 'partonJetAlg',partonJetAlg
    else:
        partonJetAlg = make_StandardJetGetter(finder,size,'Truth',inputSuff='Parton'+suffix).jetAlgorithmHandle()
    partonJetAlg.AlgTools['JetFinalPtCut'].MinimumSignal = minJetPt
    #partonJetAlg.AlgTools['InputToJet'].InputSelector = jetPartonSelectorTool
    #partonJetAlg.OutputLevel = INFO

    return
