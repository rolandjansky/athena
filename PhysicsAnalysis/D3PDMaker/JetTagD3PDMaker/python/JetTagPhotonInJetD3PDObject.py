# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagPhotonInJetD3PDObject.py
## @brief D3PD object for photon in jets
## @author Georges Aad
## @date Nov, 2010
##

import egammaD3PDMaker
import EventCommonD3PDMaker

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject

from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys

def makeJetTagPhotonInJetObject(name, prefix, object_name,
                                label=JetTagD3PDKeys.PhotonInJetGetterLabel(),
                                jetCollections=[]):
    
    from JetTagD3PDMaker import JetTagPhotonInJetGetterTool
    getter = JetTagPhotonInJetGetterTool(name+'_Getter',
                                       Label = label,
                                       JetCollections=jetCollections)

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return  D3PDMakerCoreComps.VectorFillerTool (name, Prefix = prefix, Getter = getter,
                                                 ObjectName = object_name,
                                                 SaveMetadata = \
                                                 D3PDMakerFlags.SaveObjectMetadata())




JetTagPhotonInJetD3PDObject = D3PDObject (makeJetTagPhotonInJetObject,
                                          JetTagD3PDKeys.PhotonInJetPrefix(),
                                          'JetTagPhotonInJetD3PDObject')

JetTagPhotonInJetD3PDObject.defineBlock (0, 'Kinematics',
                                       EventCommonD3PDMaker.FourMomFillerTool,
                                       WriteRect = True,
                                       WriteE = True,
                                       WriteM = True)

 
JetTagPhotonInJetD3PDObject.defineBlock (0, 'Authors',
                                           egammaD3PDMaker.egammaAuthorFillerTool)
 


#
## get the PhotonInJetObject
## jet collections to be used should be specified
#


def getJetTagPhotonInJetD3PDObject(level, jetCollections, **kw):

    return JetTagPhotonInJetD3PDObject(level=level, jetCollections=jetCollections, **kw)
