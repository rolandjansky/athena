# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

# #
# # Useful options:
# #  Specify ConstitIndex_Target to fill a list of indices of jet constituents.
# #  Target should be target collection for the association.
# #
# #  Specify JetVertexFraction_FromUD=True to get JVF information
# #  from UserData rather than from jet moments.
# #
# #  Specify JetVertexFraction_FillFullJVF=True to fill JVF info for
# #  all vertices (and force taking JVF from UserData).
# #


def getJetD3PDObject(objectname='JetD3PDObject', prefix='jet_', btagleveloffset=7):

    object = make_SGDataVector_D3PDObject ('DataVector<xAOD::Jet_v1>',
                                           D3PDMakerFlags.JetSGKey(),
                                           prefix, objectname)

    object.defineBlock(0, 'Kinematics',
                       EventCommonD3PDMaker.FourMomFillerTool,
                       WriteE  = True)

    object.defineBlock (
        1, 'DQMoments',
        D3PDMakerCoreComps.AuxDataFillerTool,
        Vars = ['n90 = N90Cells < int: 0',
                'Timing < float: 0',
                'LArQuality < float: 0',
                'OriginIndex < float: 0',
                'HECQuality < float: 0',
                'NegativeE < float: 0',
                'AverageLArQF < float: 0',
                'BCH_CORR_CELL = BchCorrCell < float:0',
                'BCH_CORR_DOTX = BchCorrDotx < int:0',
                'BCH_CORR_JET  = BchCorrJet < int:0',
                'BCH_CORR_JET_FORCELL = BchCorrJetForCell < int:0',
                'NumTowers < float: 0',
                'ootFracCells5 < float: 0',
                'ootFracCells10 < float: 0',
                'ootFracClusters5 < float: 0',
                'ootFracClusters10 < float: 0',
        ])
  
    object.defineBlock (
        1, 'JetQual',
        D3PDMakerCoreComps.AuxDataFillerTool,
        Vars = ['isBadLoose < int: 0',
                'isBadMedium < int: 0',
                'isBadTight < int: 0',
                'isUgly < int: 0',
                'hecf = HECFrac < float: 0',
                ])

    return object

JetD3PDObject = getJetD3PDObject()
