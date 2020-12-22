# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagMuonInJetD3PDObject.py
## @brief D3PD object for muon in jets
## @author Georges Aad
## @date Nov, 2010
##


import JetTagD3PDMaker
import TrackD3PDMaker
import InDetD3PDMaker
import MuonD3PDMaker
import EventCommonD3PDMaker

import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation

from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags


def makeJetTagMuonInJetObject(name, prefix, object_name,
                              label=JetTagD3PDKeys.MuonInJetGetterLabel(),
                              jetCollections=[],
                              muonsName="Muons"):
    
    from JetTagD3PDMaker import JetTagMuonInJetGetterTool
    getter = JetTagMuonInJetGetterTool(name+'_Getter',
                                       Label = label,
                                       JetCollections=jetCollections,
                                       MuonsName=muonsName)

    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return  D3PDMakerCoreComps.VectorFillerTool (name, Prefix = prefix, Getter = getter,
                                                 ObjectName = object_name,
                                                 SaveMetadata = \
                                                 D3PDMakerFlags.SaveObjectMetadata())



JetTagMuonInJetD3PDObject = D3PDObject (makeJetTagMuonInJetObject,
                                        JetTagD3PDKeys.MuonInJetPrefix(),
                                        'JetTagMuonInJetD3PDObject')


JetTagMuonInJetD3PDObject.defineBlock (0, 'Kinematics',
                                       EventCommonD3PDMaker.FourMomFillerTool,
                                       WriteRect = True,
                                       WriteE = True,
                                       WriteM = True)

 
JetTagMuonInJetD3PDObject.defineBlock (0, 'Authors',
                            MuonD3PDMaker.MuonAuthorFillerTool)
 
JetTagMuonInJetD3PDObject.defineBlock (0, 'TrkMatchQuality',
                            MuonD3PDMaker.MuonMatchQualityFillerTool)

JetTagMuonInJetD3PDObject.defineBlock (1, 'AcceptMethods',
                                       MuonD3PDMaker.MuonAcceptMethodFillerTool)

JetTagMuonInJetD3PDObject.defineBlock (0, 'Charge',
                                       EventCommonD3PDMaker.ChargeFillerTool)



from AthenaCommon.AppMgr import ToolSvc
if hasattr(ToolSvc, "MuonScatteringSigTool"):
    JetTagMuonInJetD3PDObject.defineBlock (1, "MuonScatteringAngleSignificance",
                                JetTagD3PDMaker.MuonScatteringAngleSignificanceFillerTool,
                                ScatteringSigTool=ToolSvc.MuonScatteringSigTool)
# else:
#     JetTagMuonInJetD3PDObject.defineBlock (1, "MuonScatteringAngleSignificance",
#                                 JetTagD3PDMaker.MuonScatteringAngleSignificanceFillerTool,
#                                 ScatteringSigTool=None)


JetTagMuonInJetD3PDObject.defineBlock (1, 'Isolation', 
                            MuonD3PDMaker.MuonParameterFillerTool, 
                            Parameters = ['1', 'etcone20',
                                          '2', 'etcone30',
                                          '3', 'etcone40'])


JetTagMuonInJetD3PDObject.defineBlock (1, 'CaloEnergyLoss',
                                       MuonD3PDMaker.MuonELossFillerTool)

JetTagMuonInJetD3PDObject.defineBlock (2, 'MuonHitSummary',
                                       MuonD3PDMaker.MuonTrkHitFillerTool,
                                       SaveIDMuonHitSummary=False)


MuonPrimaryTrackAssoc = SimpleAssociation(JetTagMuonInJetD3PDObject,
                                          MuonD3PDMaker.MuonTrackParticleAssociationTool,
                                          level = 0,
                                          matched = 'hastrack',
                                          blockname = "MuonFitQuality",
                                          prefix=JetTagD3PDKeys.MuonPrimaryTrackAssocPrefix(),
                                          Type =  'Primary')


MuonFitQualityAssoc = SimpleAssociation (MuonPrimaryTrackAssoc,
                                         InDetD3PDMaker.TrackParticleFitQualityAssociationTool,
                                         level = 0)

MuonFitQualityAssoc.defineBlock (0, 'TrkFitQuality',
                                 InDetD3PDMaker.TrackFitQualityFillerTool)

MuonExtrapolatedTrackAssoc = SimpleAssociation(JetTagMuonInJetD3PDObject,
                                               MuonD3PDMaker.MuonTrackParticleAssociationTool,
                                               level = 1,
                                               matched = 'hastrack',
                                               blockname = "",
                                               prefix="me_",
                                               Type =  'MuonExtrapolated')


ExTrackParticlePerigeeAssoc = SimpleAssociation(MuonExtrapolatedTrackAssoc,
                                                TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
                                                level=1,
                                                prefix = '')

ExTrackParticlePerigeeAssoc.defineBlock (1, 'ExTrkParameters',
                                         TrackD3PDMaker.PerigeeFillerTool)




if JetTagD3PDFlags.TrackAssocLabel() != "":

    MuonInDetTrackAssoc = IndexAssociation(JetTagMuonInJetD3PDObject,
                                           MuonD3PDMaker.MuonTrackParticleAssociationTool,
                                           JetTagD3PDFlags.TrackAssocLabel(),
                                           level = 0,
                                           prefix=JetTagD3PDKeys.MuonInDetTrackAssocPrefix(),
                                           Type =  'InDet')

#
## get the MuonInJetObject
## jet collections to be used should be specified
#

def getJetTagMuonInJetObject(level, jetCollections, **kw):

    return JetTagMuonInJetD3PDObject(level=level, jetCollections=jetCollections, **kw)
