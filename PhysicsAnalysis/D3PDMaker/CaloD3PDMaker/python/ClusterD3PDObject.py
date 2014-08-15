# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import CaloD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from CaloD3PDMaker import CaloD3PDMakerConf
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation



ClusterD3PDObject = \
           make_SGDataVector_D3PDObject ('CaloClusterContainer',
                                         D3PDMakerFlags.ClusterSGKey(),
                                         'cl_', 'ClusterD3PDObject')


ClusterD3PDObject.defineBlock (0, 'Kinematics',
                               EventCommonD3PDMaker.FourMomFillerTool,
                               WriteE = False,
                               WriteM = False)

ClusterD3PDObject.defineBlock (0, 'SamplingBasics',
                               CaloD3PDMaker.ClusterSamplingFillerTool)

from CaloD3PDMaker import ClusterMomentFillerTool as CMFT
ClusterD3PDObject.defineBlock (1, 'Moments', CMFT,
                               Moments = [CMFT.FIRST_ENG_DENS,   'firstEdens',
                                          CMFT.ENG_FRAC_MAX,     'cellmaxfrac',
                                          CMFT.LONGITUDINAL,     'longitudinal',
                                          CMFT.SECOND_LAMBDA,    'secondlambda',
                                          CMFT.LATERAL,          'lateral',
                                          CMFT.SECOND_R,         'secondR',
                                          CMFT.CENTER_LAMBDA,    'centerlambda',
                                          CMFT.DELTA_THETA,      'deltaTheta',
                                          CMFT.DELTA_PHI,        'deltaPhi',
                                          CMFT.ENG_BAD_CELLS,    'eng_bad_cells',
                                          CMFT.N_BAD_CELLS,      'n_bad_cells',
                                          CMFT.BADLARQ_FRAC,     'badlarq_frac',
                                          CMFT.ENG_POS,          'eng_pos',
                                          CMFT.SIGNIFICANCE,     'significance',
                                          CMFT.AVG_LAR_Q ,       'avg_lar_q',
                                          CMFT.AVG_TILE_Q,       'avg_tile_q',
                                          CMFT.VERTEX_FRACTION,  'vertexfraction',
                                          CMFT.NVERTEX_FRACTION,  'nvertexfraction',
                                          CMFT.ISOLATION,        'isolation'
                                          ])
ClusterD3PDObject.defineBlock (1, 'CenterMagMoment', CMFT,
                               Moments = [CMFT.CENTER_MAG,       'centermag',
                                          ])

ClusterD3PDObject.defineBlock (1, 'Time',
                               CaloD3PDMaker.ClusterTimeFillerTool)

ClusterD3PDObject.defineBlock (2, 'SamplingEnergies',
                               CaloD3PDMaker.ClusterSamplingFillerTool,
                               EmHadEnergies = False, # don't duplicate this one!
                               SamplingEnergies = True,
                               SamplingEtaPhi = False,
                               WriteRecoStatus = True)

ClusterD3PDObject.defineBlock (3, 'SamplingEtaPhi',
                               CaloD3PDMaker.ClusterSamplingFillerTool,
                               EmHadEnergies = False, # don't duplicate this one!
                               SamplingEnergies = False, # don't duplicate this one!
                               SamplingEtaPhi = True,
                               WriteRecoStatus = False) # don't duplicate this one!


CaloCellInCluster =ContainedVectorMultiAssociation(ClusterD3PDObject,CaloD3PDMaker.CaloClusterCellAssociationTool,"cell_",4)

CaloCellInCluster.defineBlock (4, 'CellKinematics',
                                EventCommonD3PDMaker.FourMomFillerTool,
                                WriteE  = True,  WriteM = False)


CaloCellInCluster.defineBlock (5, 'Detail1',
                                CaloD3PDMaker.CaloCellDetailsFillerTool,
                                SaveCellQuality=True,
                                SaveTimeInfo=True,
                                SaveDetInfo=True,
                                SaveCellGain=True,
                                SaveBadCellStatus=False,
                                SaveId =False,
                                SavePositionInfo=False,
                                )


CaloCellInCluster.defineBlock (6, 'Detail2',
                                CaloD3PDMaker.CaloCellDetailsFillerTool,
                                SaveCellQuality=False,
                                SaveTimeInfo=False,
                                SaveDetInfo=False,
                                SaveCellGain=False,
                                SaveBadCellStatus=True,
                                SaveId =True,
                                SavePositionInfo=True,
                                )
