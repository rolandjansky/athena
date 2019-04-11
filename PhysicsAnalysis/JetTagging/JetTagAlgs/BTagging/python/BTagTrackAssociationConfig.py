# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTagTrackToJetAssociatorConfig import BTagTrackToJetAssociatorCfg
from BTagging.BTagMuonToJetAssociatorConfig import BTagMuonToJetAssociatorCfg
from BTagging.BTaggingFlags import BTaggingFlags

from BTagging.BTaggingConf import Analysis__BTagTrackAssociation

def BTagTrackAssociationCfg(flags, name, JetCollection, TaggerList, Verbose = False, options={}):

    acc=ComponentAccumulator()

    # Setup associators
    BTagTrackToJetAssociatorList = []
    BTagTrackToJetAssocNameList = []
    if 'IP2D' in TaggerList or 'IP3D' in TaggerList:
        # calling simplified version of TrackAssociator to be checked with BTaggingConfiguration.py
        accTrackAssociator = BTagTrackToJetAssociatorCfg(flags, 'BTagTrackToJetAssociator', **options)
        tracktojetassociator = accTrackAssociator.popPrivateTools()
        #tracktojetassociator = setupTrackAssociator('BTagTrackToJetAssociator', **options)
        BTagTrackToJetAssociatorList.append(tracktojetassociator)
        BTagTrackToJetAssocNameList.append('BTagTrackToJetAssociator')
        acc.merge(accTrackAssociator)
        
    if 'MultiSVbb1' in TaggerList or 'MultiSVbb2' in TaggerList:
        # calling simplified version of TrackAssociator to be checked with BTaggingConfiguration.py
        accTrackAssociatorBB = BTagTrackToJetAssociatorCfg(flags, 'BTagTrackToJetAssociatorBB',
                                          options={'shareTracks': False,
                                             'useVariableSizedTrackCone' : True,
                                             'coneSizeFitPar1' : 3.15265e-01,
                                             'coneSizeFitPar2' : -3.66502e-01,
                                             'coneSizeFitPar3' : -1.56387e-05})
        tracktojetassociatorbb = accTrackAssociatorBB.popPrivateTools()
        #tracktojetassociatorbb = setupTrackAssociator('BTagTrackToJetAssociatorBB',  
        #                                  options={'shareTracks': False,
        #                                     'useVariableSizedTrackCone' : True,
        #                                     'coneSizeFitPar1' : 3.15265e-01,
        #                                     'coneSizeFitPar2' : -3.66502e-01,
        #                                     'coneSizeFitPar3' : -1.56387e-05})
        BTagTrackToJetAssociatorList.append(tracktojetassociatorbb)
        BTagTrackToJetAssocNameList.append('BTagTrackToJetAssociatorBB')
        acc.merge(accTrackAssociatorBB)

    MuonToJetAssociatorList = []
    MuonToJetAssocNameList = []
    MuonContainerNameList = []
    if 'SoftMu' in TaggerList:
        accMuonAssociator = BTagMuonToJetAssociatorCfg(flags, 'BTagMuonToJetAssociator', JetCollection)
        tool = accMuonAssociator.popPrivateTools()
        MuonToJetAssociatorList.append(tool)
        MuonToJetAssocNameList.append('BTagMuonToJetAssociator')
        MuonContainerNameList.append('Muons')
        acc.merge(accMuonAssociator)

        
  # NOTE: The secondary vertex finders etc. don't need to be set up here depending on the flags; they are set up when needed by the
    options = dict(options)
    options.setdefault('TrackToJetAssociatorList', BTagTrackToJetAssociatorList)
    options.setdefault('TrackToJetAssocNameList', BTagTrackToJetAssocNameList)
    options.setdefault('MuonToJetAssociatorList', MuonToJetAssociatorList)
    options.setdefault('MuonToJetAssocNameList', MuonToJetAssocNameList)
    options.setdefault('MuonContainerNameList', MuonContainerNameList)
    options.setdefault('BTagAssociation', BTaggingFlags.doStandardAssoc)
    options['name'] = name
    tool = Analysis__BTagTrackAssociation(**options)

    acc.setPrivateTools(tool)

    return acc
