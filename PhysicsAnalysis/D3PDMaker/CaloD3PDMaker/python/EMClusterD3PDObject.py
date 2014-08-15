# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import CaloD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from CaloD3PDMaker import CaloD3PDMakerConf


EMClusterD3PDObject = \
           make_SGDataVector_D3PDObject ('CaloClusterContainer',
                                         D3PDMakerFlags.EMTopoClusterSGKey(),
                                         'emcl_', 'EMClusterD3PDObject')



EMClusterD3PDObject.defineBlock (0, 'Kinematics',
                                 EventCommonD3PDMaker.FourMomFillerTool,
                                 WriteE  = True)

EMClusterD3PDObject.defineBlock (0, 'SamplingBasics',
                                 CaloD3PDMaker.ClusterSamplingFillerTool)

from CaloD3PDMaker import ClusterMomentFillerTool as CMFT
EMClusterD3PDObject.defineBlock (1, 'Moments', CMFT,
                                 Moments = [CMFT.FIRST_ENG_DENS,   'firstEdens',
                                            CMFT.ENG_FRAC_MAX,     'cellmaxfrac',
                                            CMFT.LONGITUDINAL,     'longitudinal',
                                            CMFT.SECOND_LAMBDA,    'secondlambda',
                                            CMFT.LATERAL,          'lateral',
                                            CMFT.SECOND_R,         'secondR',
                                            CMFT.CENTER_LAMBDA,    'centerlambda',
                                            CMFT.DELTA_THETA,      'deltaTheta',
                                            CMFT.DELTA_PHI,        'deltaPhi'
                                            ])

EMClusterD3PDObject.defineBlock (1, 'Time',
                                 CaloD3PDMaker.ClusterTimeFillerTool)

EMClusterD3PDObject.defineBlock (2, 'SamplingEnergies',
                                 CaloD3PDMaker.ClusterSamplingFillerTool,
                                 EmHadEnergies = False, # don't duplicate this one!
                                 SamplingEnergies = True,
                                 SamplingEtaPhi = False)

EMClusterD3PDObject.defineBlock (3, 'SamplingEtaPhi',
                                 CaloD3PDMaker.ClusterSamplingFillerTool,
                                 EmHadEnergies = False, # don't duplicate this one!
                                 SamplingEnergies = False, # don't duplicate this one!
                                 SamplingEtaPhi = True)

