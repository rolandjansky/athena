# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwTransitionRegionCorr_v1.py
# Created: Dec, 2007, L. Carminati
# Purpose: Transition region correction, v1.
#


from CaloClusterCorrection.constants import CALOCORR_CLUSTER

#############################################################################

CaloTransitionEta08_v1 = [ 9.97907e-01 ,1.95940e-06 , 1.06848e+00 , 2.70599e+00 , 1.06848e+00 , 1.79156e+00 , 0    ]
CaloTransitionEta00_v1 = [ 1.01300e+00 ,8.64627e-03 , 2.34858e-01, 5.17198e+00,  2.34858e-01 , 4.16339e+00 , 0.78 ]

CaloSwTransitionRegionsCorr_v1 = [ CaloTransitionEta00_v1 , CaloTransitionEta08_v1 ]

#############################################################################

class CaloSwTransitionRegionsCorr_v1_parms:
    region = CALOCORR_CLUSTER
    etamin_TR08 = 0.78
    etamax_TR08 = 0.82
    etamin_TR00 = 0.0015
    etamax_TR00 = 0.025    

    use_raw_eta = False
    correction = CaloSwTransitionRegionsCorr_v1
