# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import BackgroundD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
#from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
from D3PDMakerCoreComps.D3PDObject import DeferArg

import EventCommonD3PDMaker
import CaloD3PDMaker
import JetD3PDMaker


#
# Useful options:
#  Specify bbJetIndex_Target to fill indices of beam background jets in the jet collection.
#  Specify bbClusterIndex_Target to fill indices of beam background clusters in the cluster collection.
#


BeamBackgroundD3PDObject = \
    make_SG_D3PDObject ('BeamBackgroundData',
        'BeamBackgroundData',
        'bb_', 'BeamBackgroundD3PDObject')

BeamBackgroundD3PDObject.defineBlock (0, 'decision',
    BackgroundD3PDMaker.BeamBackgroundDecisionFillerTool)


ClusterAssoc = ContainedVectorMultiAssociation( BeamBackgroundD3PDObject,
    BackgroundD3PDMaker.BeamBackgroundClusterAssociationTool,
    blockname = "bbClusterInfo",
    prefix = "cl_" )

ClusterAssoc.defineBlock (1, "clus",
    BackgroundD3PDMaker.BeamBackgroundClusterFillerTool,
    BeamBackgroundKey = DeferArg ('sgkey'))

ClusterAssoc.defineBlock (1, 'bbClusterKinematics',
                               EventCommonD3PDMaker.FourMomFillerTool,
                               WriteE = False,
                               WriteM = False)
ClusterAssoc.defineBlock (1, 'bbClusterSamplingBasics',
                               CaloD3PDMaker.ClusterSamplingFillerTool)
from CaloD3PDMaker import ClusterMomentFillerTool as CMFT
ClusterAssoc.defineBlock (1, 'bbClusterMoments', CMFT,
                               Moments = [CMFT.FIRST_ENG_DENS,   'firstEdens',
                                          CMFT.ENG_FRAC_MAX,     'cellmaxfrac',
                                          CMFT.LONGITUDINAL,     'longitudinal',
                                          CMFT.SECOND_LAMBDA,    'secondlambda',
                                          CMFT.LATERAL,          'lateral',
                                          CMFT.SECOND_R,         'secondR',
                                          CMFT.CENTER_LAMBDA,    'centerlambda',
                                          CMFT.CENTER_MAG,       'centermag',
                                          CMFT.DELTA_THETA,      'deltaTheta',
                                          CMFT.DELTA_PHI,        'deltaPhi'
                                          ])
ClusterAssoc.defineBlock (1, 'bbClusterTime',
                               CaloD3PDMaker.ClusterTimeFillerTool)

ClusterAssoc.defineBlock (1, 'bbClusterIndex', D3PDMakerCoreComps.IndexFillerTool,
    Target = '')


JetAssoc = ContainedVectorMultiAssociation( BeamBackgroundD3PDObject,
    BackgroundD3PDMaker.BeamBackgroundJetAssociationTool,
    blockname = "bbJetInfo",
    prefix = "jet_")

JetAssoc.defineBlock( 1, "jet",
    BackgroundD3PDMaker.BeamBackgroundJetFillerTool,
    BeamBackgroundKey = DeferArg ('sgkey'))

JetAssoc.defineBlock (1, 'bbJetIndex', D3PDMakerCoreComps.IndexFillerTool,
    Target = '')

JetAssoc.defineBlock( 2, 'bbJetKinematics',
    EventCommonD3PDMaker.FourMomFillerTool,
    WriteE  = True)
JetAssoc.defineBlock( 2, 'bbJetQual',
    JetD3PDMaker.JetCaloUtilsFillerTool)
JetAssoc.defineBlock( 2, 'bbJetDQMoments',
    JetD3PDMaker.JetMomentFillerTool,
    Moments=['WIDTH',      'n90',
             'Timing',     'LArQuality',
             'nTrk',       'sumPtTrk',
             'OriginIndex',
             'HECQuality', 'NegativeE', 'AverageLArQF',
             'YFlip12',    'YFlip23',
             'BCH_CORR_CELL', 'BCH_CORR_DOTX','BCH_CORR_JET', 'BCH_CORR_JET_FORCELL', 
             'ENG_BAD_CELLS', 'N_BAD_CELLS', 'N_BAD_CELLS_CORR','BAD_CELLS_CORR_E',
             'NumTowers'
             ])
