# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagVxOnJetAxisD3PDObject.py
## @brief D3PD object for jet fitter VxOnJetAxis information
## @author Georges Aad
## @date Nov, 2010
##


import JetTagD3PDMaker

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from AthenaCommon.AlgSequence             import AlgSequence
from RecExConfig.ObjKeyStore              import cfgKeyStore
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags

from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags


#
## hook function to add VxOnJetAxis information to storegate
## run algorithm in the D3PD presequence
#

def _jetTagVxOnJetAxisAlgHook (c, prefix, sgkey,
                               jetCollections,
                               InfoName=JetTagD3PDFlags.JetFitterVertexFinderObjects(),
                               **kw):

    preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())

    # Is the container already in SG?
    if cfgKeyStore.isInInput ('JetTagVxOnJetAxisDataHolder', sgkey):
        return

    # Is the algorithm already in the sequence?
    algname = sgkey + JetTagD3PDKeys.JFVxOnJetAxisMakerAlgPostfix()
    if hasattr (preseq, algname):
        return

    filterargs = kw
    for k in filterargs.keys():
        if not hasattr(JetTagD3PDMaker.JetTagJetFitterDataToSGAlg, k):
            del filterargs[k]

    del filterargs['name']
    preseq += JetTagD3PDMaker.JetTagJetFitterDataToSGAlg(algname,
                                                         VxOnJetAxisName = sgkey,
                                                         FillVxOnJetAxis = True,     
                                                         JetCollections=jetCollections,
                                                         InfoName=InfoName,
                                                         **filterargs)
    
    return


def make_VxOnJetAxis_D3PDObject(name, prefix, object_name, sgkey,
                     label=JetTagD3PDKeys.JFVxOnJetAxisGetterLabel(),
                     allowMissing=False,
                     **kw):

    getter = D3PDMakerCoreComps.SGDataVectorGetterTool(name + '_Getter',
                                                       SGKey = sgkey,
                                                       TypeName = 'JetTagVxOnJetAxisDataHolder',
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




VxOnJetAxisD3PDObject= D3PDObject (make_VxOnJetAxis_D3PDObject,
                                   JetTagD3PDKeys.JFVxOnJetAxisPrefix(),
                                   'VxOnJetAxisD3PDObject')

VxOnJetAxisD3PDObject.defineHook(_jetTagVxOnJetAxisAlgHook)

VxOnJetAxisD3PDObject.defineBlock (0, '',
                                   JetTagD3PDMaker.JetTagVxOnJetAxisFillerTool)

if JetTagD3PDFlags.TrackAssocLabel() != "":

    VxOnJetAxisTrackAssoc = IndexMultiAssociation(VxOnJetAxisD3PDObject,
                                                  JetTagD3PDMaker.JetTagVxOnJetAxisTrackAssociationTool,
                                                  JetTagD3PDFlags.TrackAssocLabel(),
                                                  level=0,
                                                  prefix=JetTagD3PDKeys.VxOnJetAxisTrackAssocPrefix())


#
## get the VxOnJetAxisD3PDObject
## jet collections to be used should be specified
#

def getJetTagVxOnJetAxisD3PDObject(level, jetCollections, sgkey=JetTagD3PDKeys.JFVxOnJetAxisDataSGKey(), **kw):

    return VxOnJetAxisD3PDObject(level=level, jetCollections=jetCollections, sgkey=sgkey, **kw)




