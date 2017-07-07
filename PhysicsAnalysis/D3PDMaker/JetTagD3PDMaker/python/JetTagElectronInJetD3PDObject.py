# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagElectronInJetD3PDObject.py
## @brief D3PD object for electron in jets
## @author Georges Aad
## @date Nov, 2010
##

import JetTagD3PDMaker
import TrackD3PDMaker
import egammaD3PDMaker
import EventCommonD3PDMaker
##from egammaD3PDMaker.isem_version import isem_version ## use 16
import cppyy
cppyy.loadDictionary('egammaEnumsDict')
from ROOT import egammaPID

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags


from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags


def makeJetTagElectronInJetObject(name,  prefix, object_name,
                                  label=JetTagD3PDKeys.ElectronInJetGetterLabel(),
                                  jetCollections=[]):
    
    from JetTagD3PDMaker import JetTagElectronInJetGetterTool
    getter = JetTagElectronInJetGetterTool(name+'_Getter',
                                       Label = label,
                                       JetCollections=jetCollections)

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return  D3PDMakerCoreComps.VectorFillerTool (name, Prefix = prefix, Getter = getter,
                                                 ObjectName = object_name,
                                                 SaveMetadata = \
                                                 D3PDMakerFlags.SaveObjectMetadata())


JetTagElectronInJetD3PDObject = D3PDObject (makeJetTagElectronInJetObject,
                                            JetTagD3PDKeys.ElectronInJetPrefix(),
                                            'JetTagElectronInJetD3PDObject')


JetTagElectronInJetD3PDObject.defineBlock (0, 'Kinematics',
                                           EventCommonD3PDMaker.FourMomFillerTool,
                                           WriteRect = True,
                                           WriteE = True,
                                           WriteM = True)
 
JetTagElectronInJetD3PDObject.defineBlock (0, 'Authors',
                                           egammaD3PDMaker.egammaAuthorFillerTool)

JetTagElectronInJetD3PDObject.defineBlock (0, 'IsEMCuts',
                                           egammaD3PDMaker.egammaIsEMFillerTool,
                                           IsEM  = [(egammaPID.ElectronLoose,
                                                     None),
                                                    'loose',
                                                    (egammaPID.ElectronMedium,
                                                     None),
                                                    'medium',
                                                    (egammaPID.ElectronMediumIso,
                                                     None),
                                                    'mediumIso',
                                                    (egammaPID.ElectronTight,
                                                     None),
                                                    'tight',
                                                    (egammaPID.ElectronTightIso,
                                                     None),
                                                    'tightIso',
                                                    ])

if JetTagD3PDFlags.TrackAssocLabel() != "":

    ElecInDetTrackAssoc = IndexAssociation(JetTagElectronInJetD3PDObject,
                                           egammaD3PDMaker.egammaTrackParticleAssociationTool,
                                           JetTagD3PDFlags.TrackAssocLabel(),
                                           level=0,
                                           prefix=JetTagD3PDKeys.ElectronTrackAssocPrefix())

#
## get the ElectronInJetObject
## jet collections to be used should be specified
#

def getJetTagElectronInJetD3PDObject(level, jetCollections, **kw):

    return JetTagElectronInJetD3PDObject(level=level, jetCollections=jetCollections, **kw)
