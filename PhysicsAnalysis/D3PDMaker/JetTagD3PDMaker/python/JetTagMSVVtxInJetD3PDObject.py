# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagMSVVtxInJetD3PDObject.py
## @brief D3PD object for MSVVtxInJet 
## @author Georges Aad
## @date Apr, 2011
##


import JetTagD3PDMaker

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation

from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags


def makeJetTagMSVVtxInJetObject(name, prefix, object_name,
                              label=JetTagD3PDKeys.MSVVtxInfoInJetGetterLabel(),
                              jetCollections=[]):
    
    from JetTagD3PDMaker import JetTagMSVVtxInfoGetterTool
    getter = JetTagMSVVtxInfoGetterTool(name+'_Getter',
                                        Label = label,
                                        JetCollections=jetCollections)

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return  D3PDMakerCoreComps.VectorFillerTool (name, Prefix = prefix, Getter = getter,
                                                 ObjectName = object_name,
                                                 SaveMetadata = \
                                                 D3PDMakerFlags.SaveObjectMetadata())



JetTagMSVVtxInfoInJetD3PDObject = D3PDObject (makeJetTagMSVVtxInJetObject,
                                              JetTagD3PDKeys.MSVVtxInfoInJetPrefix(),
                                              'JetTagMSVVtxInfoInJetD3PDObject')


JetTagMSVVtxInfoInJetD3PDObject.defineBlock(0, 'MSVVtxInfo',
                                            JetTagD3PDMaker.JetTagMSVVtxInfoFillerTool)



def _jetTagMultiSVAssocLevel (reqlev, args):
    if not args.has_key ('target') : 
        return False
    return True


MSVVtxInfoAssoc = IndexMultiAssociation(JetTagMSVVtxInfoInJetD3PDObject,
                                        JetTagD3PDMaker.JetTagMSVVtxInfoTrackAssociationTool,
                                        '',
                                        level = _jetTagMultiSVAssocLevel,
                                        prefix=JetTagD3PDKeys.MSVVtxInfoTrackAssocPrefix(),
                                        blockname="JetTag_JetMultiSVVtxAssoc"
                                        )

#
## get the JetTagMSVVtxInfoInJetD3PDObject
## jet collections to be used should be specified
#

def _updateDict(name, value, kwin):
    kw = kwin.copy()
    if not kw.has_key (name+'_target'):
        kw[name+'_target']= value

    return kw

def getJetTagMSVVtxInfoInJetD3PDObject(level, jetCollections, **kw):

    if JetTagD3PDFlags.TrackAssocLabel() != "":
        kw = _updateDict("JetTag_JetMultiSVVtxAssoc",
                         JetTagD3PDFlags.TrackAssocLabel(),
                         kw)


    return JetTagMSVVtxInfoInJetD3PDObject(level=level, jetCollections=jetCollections, **kw)
