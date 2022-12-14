# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# import Hypo Algs/Tools
from AthenaConfiguration.ComponentFactory import CompFactory # tools are imported from the factory, (NewJO)
from TrigMuonHypo.TrigMuonHypoConf import (  # noqa: F401 (import all into this module)
    TrigMufastHypoAlg, TrigMufastHypoTool,
    TrigmuCombHypoAlg, TrigmuCombHypoTool,
    TrigMuonEFHypoAlg, TrigMuonEFHypoTool,
    TrigMuonEFTrackIsolationHypoAlg, TrigMuonEFTrackIsolationHypoTool,
    TrigMuonEFInvMassHypoTool,
    TrigMuonLateMuRoIHypoAlg, TrigMuonLateMuRoIHypoTool
)

# import monitoring
from TrigMuonHypo.TrigMuonHypoMonitoring import (
    TrigMufastHypoMonitoring,
    TrigmuCombHypoMonitoring,
    TrigMuonEFHypoMonitoring,
    TrigL2MuonOverlapRemoverMonitoringMufast,
    TrigL2MuonOverlapRemoverMonitoringMucomb,
    TrigMuonEFInvMassHypoMonitoring
)


# other imports
from AthenaCommon.SystemOfUnits import GeV

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigMuonHypoConfig')

muFastThresholds = {
    # added for Run3 low-mu 2mu3
    '3GeV_v22a'              : [ [0,9.9], [ 0.1 ] ],
    # 2011a tuning + 2015 tuning
    '4GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  3.38,  1.25,  3.17,  3.41] ],
    '4GeV_barrelOnly_v15a'   : [ [0,1.05,1.5,2.0,9.9], [  3.38, 1000., 1000., 1000.] ],
    '6GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  5.17,  3.25,  4.69,  5.14] ],
    '8GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  6.63,  5.17,  6.39,  6.81] ],
    '10GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  8.28,  6.35,  7.19,  8.58] ],
    #not optimized: ATR-20049
    '11GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  8.99,  6.62,  7.40,  9.32] ],
    '13GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 10.42,  7.16,  7.81, 10.80] ],
    '14GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 11.15,  7.58,  8.43, 11.61] ],
    '15GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 11.31, 10.52, 12.00, 13.24] ],
    '18GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 14.33,  9.45, 10.96, 14.35] ],
    '20GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  3.10,  3.25,  2.69,  5.14] ], # new
    '22GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  3.20,  3.25,  4.90,  5.00] ], # new
    '24GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  4.80,  3.25,  4.50,  5.10] ], # new
    '26GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  3.50,  3.25,  4.45,  4.40] ], # new
    '28GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  3.70,  3.25,  4.50,  5.40] ], # new
    '30GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  3.00,  4.00,  4.30,  5.55] ], # new
    '32GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  3.00,  6.50,  4.20,  6.00] ], # new
    '34GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  3.40,  7.20,  4.60,  9.00] ], # new
    '35GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  3.30,  5.40,  4.80,  5.00] ], # new
    '36GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  4.20,  3.90,  4.30,  5.00] ], # new
    '40GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  3.16,  6.20,  5.50, 10.00] ], # new
    '45GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  3.20,  9.80,  5.30, 10.70] ], # new
    '48GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  4.00,  7.30,  5.50,  9.50] ], # new
    '50GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  1.30,  8.10,  4.50,  9.50] ], # new
    '60GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 21.20, 25.38, 29.54] ],
    '80GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 21.20, 25.38, 29.54] ],
    '40GeV_uptoEC2_v15a'     : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 21.20, 25.38, 1000.] ],
    '40GeV_barrelOnly_v15a'  : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 1000., 1000., 1000.] ],
    '50GeV_barrelOnly_v15a'  : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 1000., 1000., 1000.] ],
    '60GeV_barrelOnly_v15a'  : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 1000., 1000., 1000.] ],
    }

muCombThresholds = {
    # added for Run3 low-mu 2mu3
    '3GeV_v22a'              : [ [0,1.05,1.5,2.0,9.9], [  2.82,  2.76,  2.64,  2.64] ],
    # original + 2015 tuning
    '2GeV_v15a'              : [ [0,9.9],              [ 2.000] ],
    '3GeV_v15a'              : [ [0,9.9],              [ 3.000] ],
    '4GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  3.86,  3.77,  3.69,  3.70] ],
    '5GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  4.9,  4.8,  4.8,  4.8] ],
    '6GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  5.87,  5.79,  5.70,  5.62] ],
    '7GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  6.8,  6.7,  6.7,  6.6] ],
    '8GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  7.80,  7.72,  7.59,  7.46] ],
    '10GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  9.73,  9.63,  9.45,  9.24] ],
    '11GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 10.8, 10.4, 10.6, 10.6] ],
    '12GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 11.7, 11.3, 11.4, 11.5] ],
    '13GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 12.62, 12.48, 12.24, 11.88] ],
    '14GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 13.57, 13.44, 13.21, 12.77] ],
    '15GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 14.50, 14.00, 14.00, 14.50] ],
    '16GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 15.47, 15.09, 14.98, 15.08] ], # Lidia - extrapolated not optimized
    '18GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 17.41, 17.27, 16.95, 16.25] ],
    '20GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 16.50, 19.19, 18.80, 17.00] ], # new 
    '22GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 18.70, 21.07, 19.13, 18.80] ], # new 
    '24GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 18.10, 21.07, 18.60, 19.35] ], # new 
    '25GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 24.20, 23.20, 23.20, 22.60] ],
    '26GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 21.50, 21.07, 20.22, 19.65] ], # new 
    '27GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 26.20, 25.10, 25.10, 24.40] ],
    '28GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 18.30, 21.07, 20.77, 20.00] ], # new 
    '30GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 18.80, 21.00, 21.00, 20.10] ], # new 
    '32GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 16.50, 22.50, 20.00, 21.50] ], # new 
    '34GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 18.40, 24.10, 21.45, 23.00] ], # new 
    '35GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 17.40, 25.90, 23.40, 23.80] ], # new 
    '36GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 16.90, 25.60, 24.20, 24.00] ], # new
    '38GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 35.80, 35.40, 35.40, 33.60] ],
    '40GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 13.20, 27.90, 27.30, 27.60] ], # new
    '40GeV_slow_v15a'        : [ [0,1.05,1.5,2.0,9.9], [ 40.00, 40.00, 40.00, 40.00] ],
    '45GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 17.70, 33.10, 31.60, 33.30] ], # new
    '48GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 14.00, 33.80, 34.20, 35.90] ], # new
    '50GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  8.30, 33.55, 32.40, 37.70] ], # new
    '60GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 45.0, 45.0, 45.0, 45.0] ],
    '60GeV_slow_v15a'        : [ [0,1.05,1.5,2.0,9.9], [ 47.0, 47.0, 47.0, 47.0] ],
    '70GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 49.0, 49.0, 49.0, 49.0] ],
    '80GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 56.0, 56.0, 56.0, 56.0] ],
    '100GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 70.0, 70.0, 70.0, 70.0] ],
    }

trigMuonEFSAThresholds = {
    # added for Run3 low-mu 2mu3
    '3GeV_v22a'              : [ [0,9.9], [ 1.0 ] ],
    #
    '0GeV'                   : [ [0,9.9],              [ 0.100 ] ],
    '2GeV'                   : [ [0,9.9],              [ 2.000 ] ],
    '3GeV'                   : [ [0,9.9],              [ 3.000 ] ],
    '4GeV'                   : [ [0,1.05,1.5,2.0,9.9], [  3.0,  2.5,  2.5,  2.5] ],
    '4GeV_barrelOnly'        : [ [0,1.05,1.5,2.0,9.9], [  3.0,1000.0,1000.0,1000.0]],
    '5GeV'                   : [ [0,1.05,1.5,2.0,9.9], [  4.6,  3.3,  4.0,  4.5] ],
    '6GeV'                   : [ [0,1.05,1.5,2.0,9.9], [  5.4,  4.5,  4.9,  5.3] ],
    '7GeV'                   : [ [0,1.05,1.5,2.0,9.9], [  6.3,  5.6,  5.6,  6.3] ],
    '8GeV'                   : [ [0,1.05,1.5,2.0,9.9], [  7.2,  6.7,  6.4,  7.3] ],
    '10GeV'                  : [ [0,1.05,1.5,2.0,9.9], [  8.9,  9.0,  8.4,  9.2] ],
    '11GeV'                  : [ [0,1.05,1.5,2.0,9.9], [  9.8, 10.1,  9.3, 10.1] ],
    '12GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 10.6, 11.0, 10.2, 11.0] ],
    '13GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 11.4, 12.0, 11.1, 12.0] ],
    '14GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 12.2, 13.0, 12.1, 13.0] ],
    '15GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 13.0, 14.0, 13.0, 14.0] ],
    '16GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 13.0, 14.0, 13.0, 14.0] ], # not optimized
    '18GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 15.7, 16.6, 15.4, 16.3] ],
    '20GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 18.20, 16.50, 17.00, 18.00] ], # new
    '22GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 20.20, 18.00, 17.00, 19.00] ], # new 
    '24GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 22.20, 19.50, 18.60, 20.20] ], # new 
    '26GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 23.50, 21.00, 20.15, 22.00] ], # new
    '28GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 23.40, 21.00, 20.55, 22.50] ], # new
    '30GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 25.20, 22.10, 21.70, 24.40] ], # new 
    '32GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 27.05, 20.50, 22.30, 26.12] ], # new 
    '34GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 28.90, 22.00, 24.00, 27.88] ], # new
    '35GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 30.00, 20.90, 24.90, 28.85] ], # new 
    '36GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 30.50, 23.77, 26.40, 28.00] ], # new
    '40GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 33.00, 27.30, 30.50, 32.90] ], # new
    '40GeV_barrelOnly'       : [ [0,1.05,1.5,2.0,9.9], [ 31.50,1000.0,1000.0,1000.0] ],
    '40GeV_uptoEC2'          : [ [0,1.05,1.5,2.0,9.9], [ 31.50, 30.00, 28.50, 1000.0] ],
    '45GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 41.20, 31.90, 35.10, 37.50] ], # new
    '48GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 43.00, 34.45, 38.00, 40.30] ], # new
    '50GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 40.00, 36.30, 40.00, 42.10] ], # new
    '50GeV_barrelOnly'       : [ [0,1.05,1.5,2.0,9.9], [ 45.0,1000.0,1000.0,1000.0]],
    '60GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 54.0, 54.0, 54.0, 54.0] ],
    '60GeV_barrelOnly'       : [ [0,1.05,1.5,2.0,9.9], [ 54.0,1000.0,1000.0,1000.0]],
    '70GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 63.0, 63.0, 63.0, 63.0] ],
    '80GeV'                  : [ [0,1.05,1.5,2.0,9.9], [ 72.0, 72.0, 72.0, 72.0] ],
    '100GeV'                 : [ [0,1.05,1.5,2.0,9.9], [ 90.0, 90.0, 90.0, 90.0] ],
   }

efCombinerThresholds = {
    #
    # added for Run3 low-mu 2mu3
    '3GeV_v22a'              : [ [0,1.05,1.5,2.0,9.9], [  2.94,  2.91,  2.77,  2.72] ],
    # original + 2015 tuning
    '2GeV_v15a'              : [ [0,9.9], [2.000] ],
    '3GeV_v15a'              : [ [0,9.9], [3.000] ],
    '4GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  3.94,  3.91,  3.77,  3.72] ],
    '5GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  4.91,  4.86,  4.84,  4.83] ],
    '6GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  5.92,  5.86,  5.70,  5.64] ],
    '7GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  6.85,  6.77,  6.74,  6.74] ],
    '8GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  7.89,  7.81,  7.60,  7.53] ],
    '10GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  9.84,  9.77,  9.54,  9.47] ],
    '11GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 10.74, 10.64, 10.58, 10.53] ],
    '12GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 11.70, 11.59, 11.53, 11.49] ],
    '13GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 12.80, 12.67, 12.43, 12.38] ],
    '14GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 13.75, 13.62, 13.38, 13.36] ],
    '15GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 14.63, 14.49, 14.42, 14.38] ],
    '16GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 15.65, 15.50, 15.39, 15.37] ], # Lidia - extrapolated not optimized
    '18GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 17.68, 17.51, 17.34, 17.34] ],
    '20GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 19.65, 19.50, 19.20, 19.30] ], # new 
    '22GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 21.64, 21.32, 21.30, 21.30] ], # new 
    '24GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 23.55, 23.21, 23.20, 23.25] ], # new 
    '26GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 25.60, 25.15, 25.20, 25.20] ], # new
    '27GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 26.26, 26.12, 26.11, 26.02] ],
    '28GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 27.28, 27.09, 27.15, 27.20] ], # new 
    '30GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 29.20, 29.00, 29.00, 29.00] ], # new 
    '32GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 31.20, 30.95, 30.90, 30.84] ], # new 
    '34GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 33.16, 32.88, 32.80, 32.70] ], # new 
    '35GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 34.15, 33.82, 33.65, 33.65] ], # new 
    '36GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 35.23, 34.75, 34.50, 34.60] ], # new
    '38GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 36.87, 36.67, 36.55, 36.48] ],
    '40GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 38.80, 38.56, 38.40, 38.90] ], # new
    '45GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 43.75, 43.30, 43.10, 43.90] ], # new
    '48GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 46.50, 46.20, 46.00, 46.80] ], # new
    '50GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 48.00, 48.20, 48.00, 48.80] ], # new
    '60GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 54.00, 54.00, 54.00, 54.00] ],
    '70GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 63.00, 63.00, 63.00, 63.00] ],
    '80GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 72.00, 72.00, 72.00, 72.00] ],
    '100GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 90.00, 90.00, 90.00, 90.00] ],
    }


muFastThresholdsForECWeakBRegion = {
    #
    # added for Run3 low-mu 2mu3
    '3GeV_v22a'             : [  0.1,   0.1 ],
    # 2011a tuning + 2015 tuning
    '4GeV_v15a'             : [  2.72,  1.58],
    '4GeV_barrelOnly_v15a'  : [ 1000., 1000. ],
    '6GeV_v15a'             : [  3.91,  2.22],
    '8GeV_v15a'             : [  4.65,  3.26],
    '10GeV_v15a'            : [  5.96,  4.24],
    #not optimized: ATR-20049
    '11GeV_v15a'            : [  6.19,  4.37 ],
    '13GeV_v15a'            : [  6.65,  4.64 ],
    '14GeV_v15a'            : [  6.78,  5.03 ],
    '15GeV_v15a'            : [  7.61,  7.81 ],
    '18GeV_v15a'            : [  8.48,  7.26 ],
    '20GeV_v15a'            : [  8.63,  7.26 ],
    '22GeV_v15a'            : [  9.53,  7.77 ],
    '24GeV_v15a'            : [  9.02,  8.31 ],
    '26GeV_v15a'            : [  9.89,  8.77 ],
    '28GeV_v15a'            : [  9.89,  8.77 ], # copy of 26
    '30GeV_v15a'            : [ 14.41, 17.43 ],
    '32GeV_v15a'            : [ 14.41, 17.43 ], # copy of 30
    '34GeV_v15a'            : [ 14.41, 17.43 ], # copy of 30
    '35GeV_v15a'            : [ 14.41, 17.43 ], # copy of 30
    '36GeV_v15a'            : [ 10.78, 10.66 ],
    '40GeV_v15a'            : [ 15.07, 18.02 ],
    '45GeV_v15a'            : [ 15.07, 18.02 ], # copy of 40
    '48GeV_v15a'            : [ 15.07, 18.02 ], # copy of 50
    '50GeV_v15a'            : [ 15.07, 18.02 ], # lixia, not optimized
    '60GeV_v15a'            : [ 15.07, 18.02 ],
    '80GeV_v15a'            : [ 15.07, 18.02 ],
    '40GeV_uptoEC2_v15a'    : [ 15.07, 18.02 ],
    '40GeV_barrelOnly_v15a' : [ 1000., 1000. ],
    '50GeV_barrelOnly_v15a' : [ 1000., 1000. ],
    '60GeV_barrelOnly_v15a' : [ 1000., 1000. ],
    }


# Working points for EF track isolation algorithm
# syntax is:
# 'WPname' : cut on 0.3 cone
# put < 0 for no cut
trigMuonEFTrkIsoThresholds = {
    'ivarloose'       : 0.16, #ivarloose
    'ivarmedium'      : 0.07, #ivarmedium
    'ivartight'       : 0.06, #ivartight
    'ivarverytight'   : 0.04,   #ivarverytight
    'iloosems'        : 3000.0 #ms-only iso
    }

trigMuonLrtd0Cut = {
    'd0loose' : 2.0,
    'd0medium' : 3.0,
    'd0tight' : 5.0
    }

#Possible dimuon mass window cuts
#Fomat is [lower bound, upper bound] in GeV
# <0 for no cut
trigMuonEFInvMassThresholds = {
    'invmJPsiOS' : [2.5, 4.3],
    'invmDimu'   : [1.5, 14.]
}

# Monitoring groups to monitor
muonHypoMonGroups = ['muonMon:online']

def getThresholdsFromDict( chainDict ):
    cparts = [i for i in chainDict['chainParts'] if i['signature']=='Muon' or i['signature']=='Bphysics']
    return sum( [ [part['threshold']]*int(part['multiplicity']) for part in cparts ], [])


def TrigMufastHypoToolFromDict( chainDict ):
    if 'mucombTag' in chainDict['chainParts'][0]['extra']:
        thresholds = ['passthrough']
    else:   
        thresholds = getThresholdsFromDict( chainDict )
    config = TrigMufastHypoConfig()
    tool = config.ConfigurationHypoTool( chainDict['chainName'], thresholds )

    if any(group in muonHypoMonGroups for group in chainDict['monGroups']):
        tool.MonTool = TrigMufastHypoMonitoring('TrigMufastHypoTool/'+chainDict['chainName'])
    
    return tool

def TrigMufastHypoToolwORFromDict( chainDict ):

    thresholds = getThresholdsFromDict( chainDict )
    config = TrigMufastHypoConfig()
    tool = config.ConfigurationHypoTool( chainDict['chainName'], thresholds )

    if any(group in muonHypoMonGroups for group in chainDict['monGroups']):
        tool.MonTool = TrigL2MuonOverlapRemoverMonitoringMufast('TrigMufastHypoTool/'+chainDict['chainName'])

    # Overlap Removal
    tool.ApplyOR = True
    # cut defintion
    tool.RequireDR       = True
    tool.RequireMass     = True
    tool.RequireSameSign = True
    # BB
    tool.DRThresBB       = 0.05
    tool.MassThresBB     = 0.20
    # BE
    tool.DRThresBE       = 0.05
    tool.MassThresBE     = 0.20
    # EE
    tool.EtaBinsEC       = [0, 1.9, 2.1, 9.9]
    tool.DRThresEC       = [0.06, 0.05, 0.05]
    tool.MassThresEC     = [0.20, 0.15, 0.10]

    return tool

# muFast Hypo for L2 multi-track SA mode
def Trigl2mtSAHypoToolwORFromDict( chainDict ):

    thresholds = getThresholdsFromDict( chainDict )
    config = TrigMufastHypoConfig()
    tool=config.ConfigurationHypoTool( chainDict['chainName'], thresholds )
    #tight = False # can be probably decoded from some of the proprties of the chain, expert work
    #acceptAll = False
    
    # Overlap Removal
    tool.ApplyOR = True
    tool.RequireDR       = True
    tool.RequireMass     = True
    tool.RequireSameSign = True
    # BB
    tool.DRThresBB       = 0.05
    tool.MassThresBB     = 0.20
    # BE
    tool.DRThresBE       = 0.05
    tool.MassThresBE     = 0.20
    # EE
    tool.EtaBinsEC       = [0, 1.9, 2.1, 9.9]
    tool.DRThresEC       = [0.06, 0.05, 0.05]
    tool.MassThresEC     = [0.20, 0.15, 0.10]
    
    return tool


class TrigMufastHypoConfig(object):

    log = logging.getLogger('TrigMufastHypoConfig')

    def ConfigurationHypoTool( self, toolName, thresholds ):

        tool = CompFactory.TrigMufastHypoTool( toolName )

        if "muoncalib" in toolName:
            tool.DoCalib = True
            nt = len(thresholds)
            tool.PtBins = [ [ 0, 2.5 ] ] * nt
            try:
                tool.AcceptAll = False

            except LookupError:
                raise Exception('MuFast Hypo for muoncalib chain  Misconfigured')
        else:
            nt = len(thresholds)
            log.debug('Set %d thresholds', nt)
            tool.PtBins = [ [ 0, 2.5 ] ] * nt
            tool.PtThresholds = [ [ 5.49 * GeV ] ] * nt
            tool.PtThresholdForECWeakBRegionA = [ 3. * GeV ] * nt
            tool.PtThresholdForECWeakBRegionB = [ 3. * GeV ] * nt

            for th, thvalue in enumerate(thresholds):
                if (thvalue == 'passthrough'):
                    tool.PtBins[th] = [-10000.,10000.]
                    tool.PtThresholds[th] = [ -1. * GeV ]
                    tool.AcceptAll = True
                else:
                    if "idperf" in toolName or int(thvalue) < 5 or "3layersEC" in toolName:
                        thvaluename =  thvalue + 'GeV_v15a'
                        if int(thvalue)==3:
                            thvaluename = thvalue + 'GeV_v22a'
                    elif "0eta105" in toolName:
                        thvaluename = thvalue+ "GeV_barrelOnly_v15a"
                    # 15.03.2022: added to allow to use the new HLT algo thresholds
                    elif int(thvalue) >= 20:
                        thvaluename = thvalue + 'GeV_v15a'
                    else:
                        thvaluename = '6GeV_v15a'



                    log.debug('Number of threshold = %d, Value of threshold = %s', th, thvaluename)

                    try:
                        tool.AcceptAll = False
                        values = muFastThresholds[thvaluename]
                        tool.PtBins[th] = values[0]
                        tool.PtThresholds[th] = [ x * GeV for x in values[1] ]
                        log.debug('Configration of threshold[%d] %s', th, tool.PtThresholds[th])
                        log.debug('Configration of PtBins[%d] %s', th, tool.PtBins[th])
                        if thvaluename in muFastThresholdsForECWeakBRegion:
                            spThres = muFastThresholdsForECWeakBRegion[thvaluename]
                            tool.PtThresholdForECWeakBRegionA[th] = spThres[0] * GeV
                            tool.PtThresholdForECWeakBRegionB[th] = spThres[1] * GeV
                        else:
                            log.debug('No special thresholds for EC weak Bfield regions for %s. Copy EC1 for region A, EC2 for region B.', thvaluename)
                            spThres = values[0][1]
                            if thvaluename == '2GeV' or thvaluename == '3GeV':
                                tool.PtThresholdForECWeakBRegionA[th] = spThres[0] * GeV
                                tool.PtThresholdForECWeakBRegionB[th] = spThres[0] * GeV
                            else:
                                tool.PtThresholdForECWeakBRegionA[th] = spThres[1] * GeV
                                tool.PtThresholdForECWeakBRegionB[th] = spThres[2] * GeV

                            log.debug('Thresholds for A[%d]/B[%d] = %d/%d', th, th, tool.PtThresholdForECWeakBRegionA[th], tool.PtThresholdForECWeakBRegionB[th])

                    except LookupError:
                        raise Exception('MuFast Hypo Misconfigured: threshold %r not supported' % thvaluename)

        return tool


def TrigmuCombHypoToolFromDict( chainDict ):

    if 'idperf' in chainDict['chainParts'][0]['addInfo']:
        thresholds = ['passthrough']
    else:
        thresholds = getThresholdsFromDict( chainDict )

    config = TrigmuCombHypoConfig()

    tight = False # can be probably decoded from some of the proprties of the chain, expert work

    acceptAll = False
    if chainDict['chainParts'][0]['signature'] == 'Bphysics':
        acceptAll = True

    tool = config.ConfigurationHypoTool( chainDict['chainName'], thresholds, tight, acceptAll )

    if any(group in muonHypoMonGroups for group in chainDict['monGroups']):
        tool.MonTool = TrigmuCombHypoMonitoring("TrigmuCombHypoTool/"+chainDict['chainName'])

    d0cut=0.
    if 'd0loose' in chainDict['chainParts'][0]['lrtInfo']:
        d0cut=trigMuonLrtd0Cut['d0loose']
    elif 'd0medium' in chainDict['chainParts'][0]['lrtInfo']:
        d0cut=trigMuonLrtd0Cut['d0medium']
    elif 'd0tight' in chainDict['chainParts'][0]['lrtInfo']:
        d0cut=trigMuonLrtd0Cut['d0tight']
    tool.MinimumD0=d0cut  
    
    return tool


def TrigmuCombHypoToolwORFromDict( chainDict ):

    if 'idperf' in chainDict['chainParts'][0]['addInfo']:
       thresholds = ['passthrough']
    else:
       thresholds = getThresholdsFromDict( chainDict )

    config = TrigmuCombHypoConfig()

    tight = False # can be probably decoded from some of the proprties of the chain, expert work

    acceptAll = False

    tool = config.ConfigurationHypoTool( chainDict['chainName'], thresholds, tight, acceptAll )

    if any(group in muonHypoMonGroups for group in chainDict['monGroups']):
        tool.MonTool = TrigL2MuonOverlapRemoverMonitoringMucomb("TrigmuCombHypoTool/"+chainDict['chainName'])

    # Overlap Removal
    tool.ApplyOR = True
    tool.RequireDR       = True
    tool.RequireMufastDR = True
    tool.RequireMass     = True
    tool.RequireSameSign = True
    tool.EtaBins         = [0, 0.9, 1.1, 1.9, 2.1, 9.9]
    tool.DRThres         = [0.002, 0.001, 0.002, 0.002, 0.002]
    tool.MufastDRThres   = [0.4,   0.4,   0.4,   0.4,   0.4]
    tool.MassThres       = [0.004, 0.002, 0.006, 0.006, 0.006]

    return tool


# muComb Hypo for L2 inside-out
def Trigl2IOHypoToolwORFromDict( chainDict ):

    thresholds = getThresholdsFromDict( chainDict )

    config = TrigmuCombHypoConfig()

    tight = False # can be probably decoded from some of the proprties of the chain, expert work

    acceptAll = False

    tool=config.ConfigurationHypoTool( chainDict['chainName'], thresholds, tight, acceptAll )

    # Overlap Removal
    tool.ApplyOR = True
    tool.RequireDR       = True
    tool.RequireMufastDR = False
    tool.RequireMass     = True
    tool.RequireSameSign = True
    tool.EtaBins         = [0, 0.9, 1.1, 1.9, 2.1, 9.9]
    tool.DRThres         = [0.002, 0.001, 0.002, 0.002, 0.002]
    tool.MufastDRThres   = [0]
    tool.MassThres       = [0.004, 0.002, 0.006, 0.006, 0.006]

    return tool


# muComb Hypo for L2 multi-track SA mode
def Trigl2mtCBHypoToolwORFromDict( chainDict ):

    if 'idperf' in chainDict['chainParts'][0]['addInfo']:
       thresholds = ['passthrough']
    else:
       thresholds = getThresholdsFromDict( chainDict )

    config = TrigmuCombHypoConfig()

    tight = False # can be probably decoded from some of the proprties of the chain, expert work

    acceptAll = False

    tool=config.ConfigurationHypoTool( chainDict['chainName'], thresholds, tight, acceptAll )

    # Overlap Removal
    tool.ApplyOR = True
    tool.RequireDR       = True
    tool.RequireMufastDR = True
    tool.RequireMass     = True
    tool.RequireSameSign = True
    tool.EtaBins         = [0, 0.9, 1.1, 1.9, 2.1, 9.9]
    tool.DRThres         = [0.002, 0.001, 0.002, 0.002, 0.002]
    tool.MufastDRThres   = [0.4,   0.4,   0.4,   0.4,   0.4]
    tool.MassThres       = [0.004, 0.002, 0.006, 0.006, 0.006]

    return tool


class TrigmuCombHypoConfig(object):

    log = logging.getLogger('TrigmuCombHypoConfig')

    def ConfigurationHypoTool( self, thresholdHLT, thresholds, tight, acceptAll ):

        tool = CompFactory.TrigmuCombHypoTool( thresholdHLT )
        tool.AcceptAll = acceptAll

        nt = len(thresholds)
        log.debug('Set %d thresholds', nt)
        tool.PtBins = [ [ 0, 2.5 ] ] * nt
        tool.PtThresholds = [ [ 5.83 * GeV ] ] * nt

        for th, thvalue in enumerate(thresholds):
            if thvalue == 'passthrough':
                tool.AcceptAll = True
                tool.PtBins[th] = [-10000.,10000.]
                tool.PtThresholds[th] = [ -1. * GeV ]
            else:
                # 15.03.2022: original flow, commented to allow use of the new HLT algo thresholds
                #if int(thvalue) >= 24:
                #    thvaluename = '22GeV_v15a'
                #else:
                #    thvaluename = thvalue + 'GeV_v15a'
                thvaluename = thvalue + 'GeV_v15a'
                if int(thvalue)==3:
                    thvaluename = thvalue + 'GeV_v22a'
                log.debug('Number of threshold = %d, Value of threshold = %s', th, thvaluename)

                try:
                    values = muCombThresholds[thvaluename]
                    tool.PtBins[th] = values[0]
                    tool.PtThresholds[th] = [ x * GeV for x in values[1] ]
                    if (tight is True):
                        tool.ApplyPikCuts        = True
                        tool.MaxPtToApplyPik      = 25.
                        tool.MaxChi2IDPik         = 3.5
                except LookupError:
                    raise Exception('MuComb Hypo Misconfigured: threshold %r not supported' % thvaluename)

        return tool



def TrigMuonEFMSonlyHypoToolFromDict( chainDict ) :
    thresholds = getThresholdsFromDict( chainDict )
    config = TrigMuonEFMSonlyHypoConfig()
    doOverlap=False
    if 'msonly' in chainDict['chainParts'][0]['msonlyInfo'] and 'noL1' not in chainDict['chainParts'][0]['extra']:
        doOverlap=True
    tool = config.ConfigurationHypoTool( chainDict['chainName'], thresholds, doOverlap )

    if any(group in muonHypoMonGroups for group in chainDict['monGroups']):
        tool.MonTool = TrigMuonEFHypoMonitoring("TrigMuonEFMSonlyHypoTool/"+chainDict['chainName'])
    
    return tool

def TrigMuonEFMSonlyHypoToolFromName(chainDict):
    #For full scan chains, we need to configure the thresholds based on all muons
    #in the chain to get the counting correct. 
    thresholds=[]
    chainName = chainDict["chainName"]
    hltChainName = chainName.rsplit("_L1",1)[0]
    cparts = hltChainName.split("_")

    if 'HLT' in hltChainName:
        cparts.remove('HLT')
    for part in cparts:
        if 'mu' in part:
            thrPart = part.split('mu')
            if not thrPart[0]:
                mult = 1
            else:
                mult=thrPart[0]
            thr = thrPart[1]
            if 'noL1' in part:
                thr =thr.replace('noL1','')
            for i in range(1,int(mult)+1):
                thresholds.append(thr)
    config = TrigMuonEFMSonlyHypoConfig()
    tool = config.ConfigurationHypoTool(chainDict['chainName'], thresholds, False)

    if any(group in muonHypoMonGroups for group in chainDict['monGroups']):
        tool.MonTool = TrigMuonEFHypoMonitoring("TrigMuonEFMSonlyHypoTool/"+chainDict['chainName'])

    return tool

class TrigMuonEFMSonlyHypoConfig(object):

    log = logging.getLogger('TrigMuonEFMSonlyHypoConfig')

    def ConfigurationHypoTool( self, toolName, thresholds, overlap ):

        log = logging.getLogger(self.__class__.__name__)
        tool = CompFactory.TrigMuonEFHypoTool( toolName )

        nt = len(thresholds)
        log.debug('Set %d thresholds', nt)
        tool.PtBins = [ [ 0, 2.5 ] ] * nt
        tool.PtThresholds = [ [ 5.49 * GeV ] ] * nt
        tool.RequireSAMuons=True
        tool.RemoveOverlaps=overlap
        if '3layersEC' in toolName:
            tool.RequireThreeStations=True
        for th, thvalue in enumerate(thresholds):
            if "0eta105" in toolName:
                thvaluename = thvalue+ "GeV_barrelOnly"
            else:
                thvaluename = thvalue + 'GeV'
                if int(thvalue)==3:
                    thvaluename = thvalue + 'GeV_v22a'
            log.debug('Number of threshold = %d, Value of threshold = %s', th, thvaluename)

            try:
                tool.AcceptAll = False
                values = trigMuonEFSAThresholds[thvaluename]
                tool.PtBins[th] = values[0]
                tool.PtThresholds[th] = [ x * GeV for x in values[1] ]

            except LookupError:
                if (thvalue=='passthrough'):
                    tool.AcceptAll = True
                    tool.PtBins[th] = [-10000.,10000.]
                    tool.PtThresholds[th] = [ -1. * GeV ]
                else:
                    raise Exception('MuonEFMSonly Hypo Misconfigured: threshold %r not supported' % thvaluename)

        return tool


def TrigMuonEFCombinerHypoToolFromDict( chainDict ) :
    if 'idperf' in chainDict['chainParts'][0]['addInfo']:
       thresholds = ['passthrough']
    else:
       thresholds = getThresholdsFromDict( chainDict )

    if 'muonqual' in chainDict['chainParts'][0]['addInfo']:
       muonquality = True
    else:
       muonquality = False

    if 'nscan' in chainDict['chainParts'][0]['addInfo']:
       narrowscan = True
    else:
       narrowscan = False

    if 'noL1' not in chainDict['chainParts'][0]['extra']:
        overlap=True
    else:
        overlap=False

    config = TrigMuonEFCombinerHypoConfig()
    tool = config.ConfigurationHypoTool( chainDict['chainName'], thresholds , muonquality, narrowscan, overlap)

    if any(group in muonHypoMonGroups for group in chainDict['monGroups']):
        tool.MonTool = TrigMuonEFHypoMonitoring("TrigMuonEFCombinerHypoTool/"+chainDict['chainName'])

    d0cut=0.
    if 'd0loose' in chainDict['chainParts'][0]['lrtInfo']:
        d0cut=trigMuonLrtd0Cut['d0loose']
    elif 'd0medium' in chainDict['chainParts'][0]['lrtInfo']:
        d0cut=trigMuonLrtd0Cut['d0medium']
    elif 'd0tight' in chainDict['chainParts'][0]['lrtInfo']:
        d0cut=trigMuonLrtd0Cut['d0tight']
    tool.MinimumD0=d0cut  
    return tool

def TrigMuonEFCombinerHypoToolFromName(chainDict):
    #For full scan chains, we need to configure the thresholds based on all muons
    #in the chain to get the counting correct. Currently a bit convoluted as
    #the chain dict is (improperly) overwritten when merging for FS chains.
    #Can probably improve this once serial merging is officially implemented
    thresholds=[]
    chainName = chainDict["chainName"]
    hltChainName = chainName.rsplit("_L1",1)[0]
    cparts = hltChainName.split("_")
    if 'HLT' in hltChainName:
        cparts.remove('HLT')
    for part in cparts:
        if 'mu' in part:
            thrPart = part.split('mu')
            if not thrPart[0]:
                mult = 1
            else:
                mult=thrPart[0]
            thr = thrPart[1]
            if 'noL1' in part:
                thr =thr.replace('noL1','')
            for i in range(1,int(mult)+1):
                thresholds.append(thr)

    if 'nscan' in chainDict['chainParts'][0]['addInfo']:
        narrowscan=True
    else:
        narrowscan=False

    if 'muonqual' in chainDict['chainParts'][0]['addInfo']:
       muonquality = True
    else:
       muonquality = False

    config = TrigMuonEFCombinerHypoConfig()

    tool = config.ConfigurationHypoTool(chainDict['chainName'], thresholds, muonquality, narrowscan, False)

    if any(group in muonHypoMonGroups for group in chainDict['monGroups']):
        tool.MonTool = TrigMuonEFHypoMonitoring("TrigMuonEFCombinerHypoTool/"+chainDict['chainName'])

    return tool

class TrigMuonEFCombinerHypoConfig(object):

    log = logging.getLogger('TrigMuonEFCombinerHypoConfig')

    def ConfigurationHypoTool( self, thresholdHLT, thresholds, muonquality, narrowscan, overlap):

        tool = CompFactory.TrigMuonEFHypoTool( thresholdHLT )  
        nt = len(thresholds)
        log.debug('Set %d thresholds', nt)
        tool.PtBins = [ [ 0, 2.5 ] ] * nt
        tool.PtThresholds = [ [ 5.49 * GeV ] ] * nt

        tool.MuonQualityCut = muonquality
        tool.RequireSAMuons=False
        tool.NarrowScan=narrowscan
        tool.RemoveOverlaps=overlap
        for th, thvalue in enumerate(thresholds):
            thvaluename = thvalue + 'GeV_v15a'
            if thvalue != 'passthrough' and int(thvalue) == 3:
                thvaluename = thvalue + 'GeV_v22a'
            log.debug('Number of threshold = %d, Value of threshold = %s', th, thvaluename)

            try:
                tool.AcceptAll = False
                values = efCombinerThresholds[thvaluename]
                tool.PtBins[th] = values[0]
                tool.PtThresholds[th] = [ x * GeV for x in values[1] ]

            except LookupError:
                if (thvalue=='passthrough'):
                    tool.AcceptAll = True
                    tool.PtBins[th] = [-10000.,10000.]
                    tool.PtThresholds[th] = [ -1. * GeV ]
                else:
                    raise Exception('MuonEFCB Hypo Misconfigured: threshold %r not supported' % thvaluename)

        return tool



def TrigMuonEFTrackIsolationHypoToolFromDict( chainDict ) :
    cparts = [i for i in chainDict['chainParts'] if i['signature']=='Muon']
    if 'ivarperf' in chainDict['chainParts'][0]['isoInfo']:
        thresholds = 'passthrough'
    else:
        thresholds = cparts[0]['isoInfo']
    config = TrigMuonEFTrackIsolationHypoConfig()
    tool = config.ConfigurationHypoTool( chainDict['chainName'], thresholds )
    return tool

class TrigMuonEFTrackIsolationHypoConfig(object) :

    log = logging.getLogger('TrigMuonEFTrackIsolationHypoConfig')

    def ConfigurationHypoTool(self, toolName, isoCut):

        tool = CompFactory.TrigMuonEFTrackIsolationHypoTool(toolName)

        try:
            if(isoCut=='passthrough') :
                tool.AcceptAll = True

            else:
                ptcone03 = trigMuonEFTrkIsoThresholds[ isoCut ]

                tool.PtCone02Cut = 0.0
                tool.PtCone03Cut = ptcone03
                tool.AcceptAll = False

                if 'ms' in isoCut:
                    tool.RequireCombinedMuon = False
                    tool.DoAbsCut = True
                else:
                    tool.RequireCombinedMuon = True
                    tool.DoAbsCut = False


                if 'var' in isoCut :
                    tool.useVarIso = True
                else :
                    tool.useVarIso = False
        except LookupError:
            if(isoCut=='passthrough') :
                log.debug('Setting passthrough')
                tool.AcceptAll = True
            else:
                log.error('isoCut = ', isoCut)
                raise Exception('TrigMuonEFTrackIsolation Hypo Misconfigured')
        return tool

def TrigMuonEFInvMassHypoToolFromDict( chainDict ) :
    cparts = [i for i in chainDict['chainParts'] if i['signature']=='Muon']
    #The invariant mass is specified at end of chain, so only shows up in the last chainPart
    thresholds = cparts[-1]['invMassInfo']
    if "os" in cparts[-1]['addInfo']:
        osCut=True
    else:
        osCut = False
    config = TrigMuonEFInvMassHypoConfig()
    tool = config.ConfigurationHypoTool( chainDict['chainName'], thresholds, osCut )

    if any(group in muonHypoMonGroups for group in chainDict['monGroups']):
        tool.MonTool = TrigMuonEFInvMassHypoMonitoring("TrigMuonEFInvMassHypoTool/"+chainDict['chainName'])

    return tool

class TrigMuonEFInvMassHypoConfig(object) :

    log = logging.getLogger('TrigMuonEFInvMassHypoConfig')

    def ConfigurationHypoTool(self, toolName, thresholds, osCut):

        tool = CompFactory.TrigMuonEFInvMassHypoTool(toolName)

        try:
            massWindow = trigMuonEFInvMassThresholds[thresholds]

            tool.InvMassLow = massWindow[0]
            tool.InvMassHigh = massWindow[1]
            tool.AcceptAll = False
            tool.SelectOppositeSign = osCut

        except LookupError:
            if(thresholds=='passthrough') :
                log.debug('Setting passthrough')
                tool.AcceptAll = True
            else:
                log.error('thresholds = ', thresholds)
                raise Exception('TrigMuonEFInvMass Hypo Misconfigured')
        return tool

def TrigMuonLateMuRoIHypoToolFromDict( chainDict ) :
    config = TrigMuonLateMuRoIHypoConfig()
    tool = config.ConfigurationHypoTool( chainDict['chainName'] )
    return tool

class TrigMuonLateMuRoIHypoConfig(object) :

    log = logging.getLogger('TrigMuonLateMuRoIHypoConfig')

    def ConfigurationHypoTool(self, toolName):

        tool = CompFactory.TrigMuonLateMuRoIHypoTool(toolName)

        try:
            tool.AcceptAll = False

        except LookupError:
            raise Exception('TrigMuonLateMuRoI Hypo Misconfigured')
        return tool
