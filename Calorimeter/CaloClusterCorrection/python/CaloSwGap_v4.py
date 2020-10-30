# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwGap_v4.py
# Created: Nov 2006, sss
# Purpose: Gap correction, v3 and v3_1.
#
# These corrections were derived by Stathes Paganis, using
# 12.0.3 simulation and reconstruction with ideal (calib0) geometry.
# This was added in CaloClusterCorrection-00-02-61, in 12.0.4.
#


from CaloClusterCorrection.constants import CALOCORR_CLUSTER

#############################################################################

CaloSwGap_v4_ele55 = [
            # escale    wgapSci   eoffset 
[ 1.437500,   1.1748,   1.3471,    988.84  ],
[ 1.462500,   1.2016,   1.3796,   2888.49  ],
[ 1.487500,   1.1609,   1.3653,   3627.43  ],
[ 1.512500,   0.8912,   1.6771,   4681.53  ],
[ 1.537500,   0.9472,   1.1663,   1723.72  ]
] 

CaloSwGap_v4_ele35 = [
            # escale    wgapSci   eoffset 
[ 1.437500,   1.2082,   1.3252,    847.23  ],
[ 1.462500,   1.2221,   1.4600,   2645.10  ],
[ 1.487500,   1.2682,   1.3119,   2107.51  ],
[ 1.512500,   0.9003,   1.5640,   5460.82  ],
[ 1.537500,   0.9661,   1.1784,   1528.92  ]
]

CaloSwGap_v4_ele37 = [
            # escale    wgapSci   eoffset 
[ 1.437500,   1.1858,   1.3586,    893.54  ],
[ 1.462500,   1.2167,   1.4474,   2136.88  ],
[ 1.487500,   1.2049,   1.3005,   2691.53  ],
[ 1.512500,   0.9205,   1.5863,   3665.26  ],
[ 1.537500,   0.9617,   1.1393,   1577.98  ]
] 

CaloSwGap_v4_gam55 = [
            # escale    wgapSci   eoffset 
[ 1.437500,   1.1415,   1.3961,   1399.41  ],
[ 1.462500,   1.1873,   1.4093,   1998.63  ],
[ 1.487500,   1.1409,   1.4237,   2391.79  ],
[ 1.512500,   0.8947,   1.5816,   4199.27  ],
[ 1.537500,   0.9560,   1.1320,    968.75  ]
] 

CaloSwGap_v4_gam35 = [
            # escale    wgapSci   eoffset 
[ 1.437500,   1.1962,   1.4423,    590.26  ],
[ 1.462500,   1.2123,   1.4681,   1707.78  ],
[ 1.487500,   1.2245,   1.3677,   1482.52  ],
[ 1.512500,   0.9051,   1.5283,   4930.62  ],
[ 1.537500,   0.9756,   1.1137,    989.47  ]
]

CaloSwGap_v4_gam37 = [
            # escale    wgapSci   eoffset 
[ 1.437500,   1.1681,   1.3994,    824.20  ],
[ 1.462500,   1.2081,   1.4540,   1423.95  ],
[ 1.487500,   1.1376,   1.4125,   2683.33  ],
[ 1.512500,   0.9383,   1.5156,   1832.49  ],
[ 1.537500,   0.9702,   1.0988,   1014.11  ]
] 



#############################################################################


class CaloSwGap_v4_parms:
    region = CALOCORR_CLUSTER
    etamin_crack = 1.425
    etamax_crack = 1.55
    degree = 3
    use_raw_eta = False
    correction = {'ele55' : CaloSwGap_v4_ele55,
                  'ele35' : CaloSwGap_v4_ele35,
                  'ele37' : CaloSwGap_v4_ele37,
                  'gam55' : CaloSwGap_v4_gam55,
                  'gam35' : CaloSwGap_v4_gam35,
                  'gam37' : CaloSwGap_v4_gam37,

                  # Use 5x5 for cluster sizes that aren't explicitly derived.
                  'ele33' : CaloSwGap_v4_ele55,
                  'ele57' : CaloSwGap_v4_ele55,
                  'ele77' : CaloSwGap_v4_ele55,
                  'gam33' : CaloSwGap_v4_gam55,
                  'gam57' : CaloSwGap_v4_gam55,
                  'gam77' : CaloSwGap_v4_gam55,
                  }
    
