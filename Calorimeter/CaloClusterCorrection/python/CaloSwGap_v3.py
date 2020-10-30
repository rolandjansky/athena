# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwGap_v3.py
# Created: Nov 2006, sss
# Purpose: Gap correction, v3 and v3_1.
#
# These corrections were derived by Stathes Paganis.
# The electron corrections were derived using 11.0.41 simulation
# and 12.0.0 reconstruction; the photon corrections were derived using
# 11.0.42 simulation and 12.0.2 reconstruction.
#
# There are two versions exported by this file.
# `v3' has electrons only (the electron corrections are reused for photon
# clusters).  This was added in CaloClusterCorrection-00-02-38,
# in 12.0.0.
# `v3_1' has both electrons and photons.  This was added in
# CaloClusterCorrection-00-02-59, in 12.0.3.
#


from CaloClusterCorrection.constants import CALOCORR_CLUSTER

#############################################################################
# Gap energy correction for versions 12.x.x and later
#  (calculated with 11.0.41 runs)
#  SP: since May/06 it is cluster size dependent and includes the scintillator.

CaloSwGap_v3_ele55 = [
            # escale    wgapSci   eoffset 
[ 1.437500,   1.1498,   0.9545,   2972.83  ],
[ 1.462500,   1.2299,   1.0339,   1624.10  ],
[ 1.487500,   1.0880,   1.0825,   3795.40  ],
[ 1.512500,   0.9159,   1.0438,   4195.66  ],
[ 1.537500,   0.9117,   0.9788,   1253.47  ]
] 

CaloSwGap_v3_ele35 = [
            # escale    wgapSci   eoffset 
[ 1.437500,   1.1745,   0.9115,   1903.38  ],
[ 1.462500,   1.2119,   1.0519,   1949.33  ],
[ 1.487500,   1.2003,   1.0679,   2402.09  ],
[ 1.512500,   0.9087,   1.0533,   4849.28  ],
[ 1.537500,   0.9067,   0.9731,   2600.90  ]
]

CaloSwGap_v3_ele37 = [
            # escale    wgapSci   eoffset 
[ 1.437500,   1.1546,   0.9099,   1771.53  ],
[ 1.462500,   1.1984,   1.0602,   1611.72  ],
[ 1.487500,   1.1767,   1.0181,   2359.71  ],
[ 1.512500,   0.9048,   1.0079,   4507.59  ],
[ 1.537500,   0.9095,   0.9669,   1751.92  ]
] 

CaloSwGap_v3_gam55 = [
            # escale    wgapSci   eoffset 
[ 1.437500,   1.1379,   1.1252,   1402.07  ],
[ 1.462500,   1.1900,   1.1362,    531.62  ],
[ 1.487500,   1.1673,   1.0685,   3112.60  ],
[ 1.512500,   1.0159,   1.0766,    677.68  ],
[ 1.537500,   1.0263,   1.0011,      0.00  ]
] 

CaloSwGap_v3_gam35 = [
            # escale    wgapSci   eoffset 
[ 1.437500,   1.1555,   1.1208,    911.61  ],
[ 1.462500,   1.2645,   1.1105,    623.80  ],
[ 1.487500,   1.1801,   1.1676,   3272.09  ],
[ 1.512500,   1.0255,   1.0583,   1261.91  ],
[ 1.537500,   1.0463,   1.0175,      0.00  ]
]

CaloSwGap_v3_gam37 = [
            # escale    wgapSci   eoffset 
[ 1.437500,   1.1384,   1.1418,   1355.28  ],
[ 1.462500,   1.2120,   1.1211,   1894.89  ],
[ 1.487500,   1.1407,   1.2279,   2662.74  ],
[ 1.512500,   1.0316,   1.0785,    464.67  ],
[ 1.537500,   1.0368,   1.0009,    104.17  ]
] 



#############################################################################


# For this version, the photon corrections are the same as the electrons.
class CaloSwGap_v3_parms:
    region = CALOCORR_CLUSTER
    etamin_crack = 1.425
    etamax_crack = 1.55
    degree = 3
    use_raw_eta = False
    correction = {'ele55' : CaloSwGap_v3_ele55,
                  'ele35' : CaloSwGap_v3_ele35,
                  'ele37' : CaloSwGap_v3_ele37,
                  'gam55' : CaloSwGap_v3_ele55,
                  'gam35' : CaloSwGap_v3_ele35,
                  'gam37' : CaloSwGap_v3_ele37}
    

# This version has separate electron and photon corrections.
class CaloSwGap_v3_1_parms:
    region = CALOCORR_CLUSTER
    etamin_crack = 1.425
    etamax_crack = 1.55
    degree = 3
    use_raw_eta = False
    correction = {'ele55' : CaloSwGap_v3_ele55,
                  'ele35' : CaloSwGap_v3_ele35,
                  'ele37' : CaloSwGap_v3_ele37,
                  'gam55' : CaloSwGap_v3_gam55,
                  'gam35' : CaloSwGap_v3_gam35,
                  'gam37' : CaloSwGap_v3_gam37,

                  # Use 5x5 for cluster sizes that aren't explicitly derived.
                  'ele33' : CaloSwGap_v3_ele55,
                  'ele57' : CaloSwGap_v3_ele55,
                  'ele77' : CaloSwGap_v3_ele55,
                  'gam33' : CaloSwGap_v3_gam55,
                  'gam57' : CaloSwGap_v3_gam55,
                  'gam77' : CaloSwGap_v3_gam55,
                  }
    
