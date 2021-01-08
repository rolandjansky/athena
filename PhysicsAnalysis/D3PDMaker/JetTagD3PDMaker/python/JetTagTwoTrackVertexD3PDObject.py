# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagTwoTrackVertexD3PDObject.py
## @brief D3PD object for jet fitter TwoTrackVertex information
## @author Georges Aad
## @date Nov, 2010
##

import JetTagD3PDMaker

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from AthenaCommon.AlgSequence             import AlgSequence
from RecExConfig.ObjKeyStore                  import cfgKeyStore

from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags

from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags



#
## hook function to add TwoTrackVertex information to storegate
## run algorithm in the D3PD presequence
#

def _jetTwoTrackVertexTagAlgHook (c, prefix, sgkey,
                                  jetCollections,
                                  InfoName=JetTagD3PDFlags.JetFitterVertexFinderObjects(),
                                  **kw):

    preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())

    # Is the container already in SG?
    if cfgKeyStore.isInInput ('JetTagTwoTrackVertexDataHolder', sgkey):
        return

    # Is the algorithm already in the sequence?
    algname = sgkey + JetTagD3PDKeys.JFTwoTrackVertexMakerAlgPostfix()
    if hasattr (preseq, algname):
        return

    filterargs = kw
    for k in filterargs.keys():
        if not hasattr(JetTagD3PDMaker.JetTagJetFitterDataToSGAlg, k):
            del filterargs[k]

    del filterargs['name']
    preseq += JetTagD3PDMaker.JetTagJetFitterDataToSGAlg(algname,
                                                         TwoTrackVertexName = sgkey,
                                                         FillTwoTrackVertex=True,
                                                         JetCollections=jetCollections,  
                                                         InfoName=InfoName,
                                                         **filterargs)
    
    return


def make_TwoTrackVertex_D3PDObject(name, prefix, object_name, sgkey,
                     label=JetTagD3PDKeys.JFTwoTrackVertexGetterLabel(),
                     allowMissing=False,
                     **kw):

        
    getter = D3PDMakerCoreComps.SGDataVectorGetterTool(name + '_Getter',
                                                       SGKey = sgkey,
                                                       TypeName = 'JetTagTwoTrackVertexDataHolder',
                                                       Label = label)
    
    defs = kw

    for k in defs.keys():
        if not hasattr(D3PDMakerCoreComps.VectorFillerTool, k):
            del defs[k]

    if not defs.has_key ("SaveMetadata"):
        defs["SaveMetadata"]=D3PDMakerFlags.SaveObjectMetadata()

    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name,
                                                AllowMissing=allowMissing,
                                                **defs)




JetTagTwoTrackVertexD3PDObject= D3PDObject(make_TwoTrackVertex_D3PDObject,
                                           JetTagD3PDKeys.JFTwoTrackVertexPrefix(),
                                           'JetTagTwoTrackVertexD3PDObject')

JetTagTwoTrackVertexD3PDObject.defineHook(_jetTwoTrackVertexTagAlgHook)

JetTagTwoTrackVertexD3PDObject.defineBlock (0, '',
                                            JetTagD3PDMaker.JetTagTwoTrackVertexFillerTool)

if JetTagD3PDFlags.TrackAssocLabel() != "":


    JetTagTwoTrackVertexTrackAssoc = IndexMultiAssociation(\
        JetTagTwoTrackVertexD3PDObject,
        JetTagD3PDMaker.JetTagTwoTrackVertexTrackAssociationTool,
        JetTagD3PDFlags.TrackAssocLabel(),
        level=0,
        prefix=JetTagD3PDKeys.TwoTrackVertexTrackAssocPrefix())

#
## get the TwoTrackVertexD3PDObject
## jet collections to be used should be specified
#

def getJetTagTwoTrackVertexD3PDObject(level, jetCollections, sgkey=JetTagD3PDKeys.JFTwoTrackVertexDataSGKey(),**kw):

    return JetTagTwoTrackVertexD3PDObject(level=level,jetCollections=jetCollections, sgkey=sgkey, **kw)
  




