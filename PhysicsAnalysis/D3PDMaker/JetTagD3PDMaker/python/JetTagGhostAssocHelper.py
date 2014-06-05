# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDKeys
from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags


def addToIncludeBlocks(jetname, list):

    key = jetname+"_include"
    dict = JetTagD3PDFlags.D3PDPropDict()
    if key in dict.keys():
        list+=dict[key]
    JetTagD3PDFlags.D3PDPropDict().update({key:list})


def addTrackGhostAssociation(jetcollection):

    ### assoc name is TrackAssoc
    ### using the default assoc otherwise use setupGhostAssociator to get a custom assoc
    if JetTagD3PDFlags.TrackAssocLabel() != "":
        JetTagD3PDFlags.D3PDPropDict().update(
            {jetcollection+"_"+JetTagD3PDKeys.JetTrackGhostAssocBlockName()+
             "_target":JetTagD3PDFlags.TrackAssocLabel()})


def addBHadronGhostAssociation(jetcollection):

    if TruthD3PDFlags.GenParticleAssocLabel() != "":
        addToIncludeBlocks(jetcollection,[JetTagD3PDKeys.JetBHadronGhostAssocBlockName()+"tmp"])
        JetTagD3PDFlags.D3PDPropDict().update(
            {jetcollection+"_"+JetTagD3PDKeys.JetBHadronGhostAssocBlockName()+
             "_target":TruthD3PDFlags.GenParticleAssocLabel()})



def addTruthGhostAssociation(jetcollection):

    if TruthD3PDFlags.GenParticleAssocLabel() != "":
        addToIncludeBlocks(jetcollection,[JetTagD3PDKeys.JetTruthGhostAssocBlockName()+"tmp"])
        JetTagD3PDFlags.D3PDPropDict().update(
            {jetcollection+"_"+JetTagD3PDKeys.JetTruthGhostAssocBlockName()+
             "_target":TruthD3PDFlags.GenParticleAssocLabel()})



def createAssocObjects(jetcollection, assocList):
    from JetMomentTools.GhostAssociation import addGhostAssociation
    return addGhostAssociation(jetcollection, assocList)
#     finder=None
#     input=None
#     mainParam=None

#     if jetcollection.endswith("MuonNonInteractingTruthJets"):
#         finder='AntiKt'
#         input='Truth'

#     if jetcollection.startswith('AntiKt4'):
#         mainParam=0.4
#     if jetcollection.startswith('AntiKt6'):
#         mainParam=0.6
        
#     return addGhostAssociation(jetcollection,["TrackAssoc"], finder,input, mainParam)

