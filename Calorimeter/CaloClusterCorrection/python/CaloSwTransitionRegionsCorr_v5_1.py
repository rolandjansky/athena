# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwTransitionRegionCorr_v5_1.py
# Created: Dec, 2007, L. Carminati
#          
# Purpose: Transition region correction, v5_1.
#          This version implement some fixes for eta=0 and eta=0.8 


from CaloClusterCorrection.constants import CALOCORR_CLUSTER

#############################################################################

CaloTransitionEta00_v5_1_ele = [ 0.993293  ,  10.2368  ,  -0.00699482  ,  348.749  ,  -0.00699482  , 15.5551 , 0.68 ]
CaloTransitionEta08_v5_1_ele = [ 0.992672  ,  5.46887e-06  ,  1.07283  ,  4.52516  ,1.07283  ,  -1.60459     , 0 ]

CaloTransitionEta00_v5_1_gam = [ 0.997326  ,  6.07154  ,  -0.0100278  ,  270.107  ,-0.0100278,  -4.16365 , 0.68  ]
CaloTransitionEta08_v5_1_gam = [ 0.994129  ,  3.19538e-06  ,  1.07219  ,  2.86697  ,1.07219,  -1.77106 , 0  ]

CaloSwTransitionRegionsCorr_v5_1_ele = [ CaloTransitionEta00_v5_1_ele , CaloTransitionEta08_v5_1_ele ]
CaloSwTransitionRegionsCorr_v5_1_gam = [ CaloTransitionEta00_v5_1_gam , CaloTransitionEta08_v5_1_gam ]

#############################################################################

class CaloSwTransitionRegionsCorr_v5_1_parms:
    region = CALOCORR_CLUSTER
    etamin_TR08 = 0.78
    etamax_TR08 = 0.82
    etamin_TR00 = 0.001
    etamax_TR00 = 0.015    

    use_raw_eta = True
    correction = {'ele55' : CaloSwTransitionRegionsCorr_v5_1_ele,
                  'ele35' : CaloSwTransitionRegionsCorr_v5_1_ele,
                  'ele37' : CaloSwTransitionRegionsCorr_v5_1_ele,
                  'gam55' : CaloSwTransitionRegionsCorr_v5_1_gam,
                  'gam35' : CaloSwTransitionRegionsCorr_v5_1_gam,
                  'gam37' : CaloSwTransitionRegionsCorr_v5_1_gam,

                  # Use 5x5 for cluster sizes that aren't explicitly derived.
                  'ele33' : CaloSwTransitionRegionsCorr_v5_1_ele,
                  'ele57' : CaloSwTransitionRegionsCorr_v5_1_ele,
                  'ele77' : CaloSwTransitionRegionsCorr_v5_1_ele,
                  'gam33' : CaloSwTransitionRegionsCorr_v5_1_gam,
                  'gam57' : CaloSwTransitionRegionsCorr_v5_1_gam,
                  'gam77' : CaloSwTransitionRegionsCorr_v5_1_gam,
                  }
       
