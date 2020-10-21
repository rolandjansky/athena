# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwTransitionRegionCorr_v5.py
# Created: Dec, 2007, L. Carminati
# Purpose: Transition region correction, v5.
#


from CaloClusterCorrection.constants import CALOCORR_CLUSTER

#############################################################################

CaloTransitionEta00_v5_ele = [0.989223  ,  2164.26  ,  -0.0294058  ,  280.523  , -0.0294058,  2039.64, 0.75 ]  
CaloTransitionEta08_v5_ele = [0.99181  ,  6.60232e-06  ,  1.07589  ,  4.11989  , 1.07589,  -1.63854 , 0 ]

CaloTransitionEta00_v5_gam = [ 0.989434  ,  475.113  , -0.023772 ,  288.807  ,  -0.023772, 231.853, 0.75  ]
CaloTransitionEta08_v5_gam = [ 0.993671  ,  6.49907e-06  ,  1.06182  ,  5.51113  , 1.06182,  -1.53548  , 0 ]

CaloSwTransitionRegionsCorr_v5_ele = [ CaloTransitionEta00_v5_ele , CaloTransitionEta08_v5_ele ]
CaloSwTransitionRegionsCorr_v5_gam = [ CaloTransitionEta00_v5_gam , CaloTransitionEta08_v5_gam ]

#############################################################################

class CaloSwTransitionRegionsCorr_v5_parms:
    region = CALOCORR_CLUSTER
    etamin_TR08 = 0.785
    etamax_TR08 = 0.81
    etamin_TR00 = 0.002
    etamax_TR00 = 0.025    

    use_raw_eta = False
    correction = {'ele55' : CaloSwTransitionRegionsCorr_v5_ele,
                  'ele35' : CaloSwTransitionRegionsCorr_v5_ele,
                  'ele37' : CaloSwTransitionRegionsCorr_v5_ele,
                  'gam55' : CaloSwTransitionRegionsCorr_v5_gam,
                  'gam35' : CaloSwTransitionRegionsCorr_v5_gam,
                  'gam37' : CaloSwTransitionRegionsCorr_v5_gam,

                  # Use 5x5 for cluster sizes that aren't explicitly derived.
                  'ele33' : CaloSwTransitionRegionsCorr_v5_ele,
                  'ele57' : CaloSwTransitionRegionsCorr_v5_ele,
                  'ele77' : CaloSwTransitionRegionsCorr_v5_ele,
                  'gam33' : CaloSwTransitionRegionsCorr_v5_gam,
                  'gam57' : CaloSwTransitionRegionsCorr_v5_gam,
                  'gam77' : CaloSwTransitionRegionsCorr_v5_gam,
                  }
       
