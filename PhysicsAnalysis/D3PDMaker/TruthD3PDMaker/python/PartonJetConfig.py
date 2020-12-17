# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file TruthD3PDMaker/python/PartonJetConfig.py
# @author Renaud Bruneliere <Renaud.Bruneliere@cern.ch>
# @date Apr, 2010
# @brief Configure algorithms to build parton-level jets
#        information on algorithm configuration can be found on
#        Reconstruction/Jet/JetSimTools/PartonTruthJets_jobOptions.py
#

from AthenaCommon.SystemOfUnits import GeV
from RecExConfig.ObjKeyStore                  import cfgKeyStore
from RecExConfig.RecFlags                     import rec

from JetRec.JetGetters import make_StandardJetGetter
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
    ToolSvc += jetPartonSelectorTool

    # Configure jets builder
    if inputCollections is not None:
        partonJetAlg = make_StandardJetGetter(finder,size,'Truth',inputSuff='Parton'+suffix,
                                              inputCollectionNames=inputCollections,
                                              inputTools=[jetPartonSelectorTool]).jetAlgorithmHandle()
        partonJetAlg.AlgTools['JetPartonSelectorTool'].InputCollectionKeys = []
        partonJetAlg.AlgTools['JetPartonSelectorTool'].InputCollectionKeys.append(inputCollections[0])
    else:
        partonJetAlg = make_StandardJetGetter(finder,size,'Truth',inputSuff='Parton'+suffix).jetAlgorithmHandle()
    partonJetAlg.AlgTools['JetFinalPtCut'].MinimumSignal = minJetPt

    return
