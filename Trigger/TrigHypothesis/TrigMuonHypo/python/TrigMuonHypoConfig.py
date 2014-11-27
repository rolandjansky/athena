# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMuonHypo.TrigMuonHypoConf import *
from TrigMuonHypo.TrigMuonHypoMonitoring import *
from AthenaCommon.SystemOfUnits import GeV
from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
from TrigMuonBackExtrapolator.TrigMuonBackExtrapolatorConfig import *
from AthenaCommon.AppMgr import ToolSvc

ToolSvc += MuonBackExtrapolatorForAlignedDet()
ToolSvc += MuonBackExtrapolatorForMisalignedDet()

trigMuonEFSAThresholds = {
    '0GeV'             : [ [0,9.9],              [ 0.100 ] ],
    '2GeV'             : [ [0,9.9],              [ 2.000 ] ],
    '4GeV'             : [ [0,1.05,1.5,2.0,9.9], [  3.0,  2.5,  2.5,  2.5] ],
    '4GeV_barrelOnly'  : [ [0,1.05,1.5,2.0,9.9], [  3.0,1000.0,1000.0,1000.0]], 
    '5GeV'             : [ [0,1.05,1.5,2.0,9.9], [  4.6,  3.3,  4.0,  4.5] ],
    '6GeV'             : [ [0,1.05,1.5,2.0,9.9], [  5.4,  4.5,  4.9,  5.3] ],
    '7GeV'             : [ [0,1.05,1.5,2.0,9.9], [  6.3,  5.6,  5.6,  6.3] ],
    '8GeV'             : [ [0,1.05,1.5,2.0,9.9], [  7.2,  6.7,  6.4,  7.3] ],
    '10GeV'            : [ [0,1.05,1.5,2.0,9.9], [  8.9,  9.0,  8.4,  9.2] ],
    '11GeV'            : [ [0,1.05,1.5,2.0,9.9], [  9.8, 10.1,  9.3, 10.1] ],
    '12GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 10.6, 11.0, 10.2, 11.0] ], 
    '13GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 11.4, 12.0, 11.1, 12.0] ],
    '14GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 12.2, 13.0, 12.1, 13.0] ],
    '15GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 13.0, 14.0, 13.0, 14.0] ],
    '18GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 15.7, 16.6, 15.4, 16.3] ],
    '20GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 17.5, 18.5, 17.0, 18.0] ],
    '22GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 19.1, 20.0, 18.4, 19.6] ],
    '24GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 20.4, 20.8, 19.3, 21.0] ],
    '30GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 25.0, 24.5, 23.0, 26.0] ],
    '40GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 31.5, 30.0, 28.5, 32.5] ], 
    '40GeV_barrelOnly' : [ [0,1.05,1.5,2.0,9.9], [ 31.5,1000.0,1000.0,1000.0]], 
    '40GeV_uptoEC2'    : [ [0,1.05,1.5,2.0,9.9], [ 31.5, 30.0, 28.5,  1000.0]], 
    '50GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 45.0, 45.0, 45.0, 45.0] ], 
    '50GeV_barrelOnly' : [ [0,1.05,1.5,2.0,9.9], [ 45.0,1000.0,1000.0,1000.0]], 
    '60GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 54.0, 54.0, 54.0, 54.0] ], 
    '60GeV_barrelOnly' : [ [0,1.05,1.5,2.0,9.9], [ 54.0,1000.0,1000.0,1000.0]], 
    '70GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 63.0, 63.0, 63.0, 63.0] ], 
    '80GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 72.0, 72.0, 72.0, 72.0] ],
    '100GeV'           : [ [0,1.05,1.5,2.0,9.9], [ 90.0, 90.0, 90.0, 90.0] ],
   }


efCombinerThresholds = {
    '2GeV'             : [ [0,9.9], [2.000] ],
    '4GeV'             : [ [0,1.05,1.5,2.0,9.9], [  3.93,  3.91,  3.88,  3.88] ],
    '5GeV'             : [ [0,1.05,1.5,2.0,9.9], [  4.91,  4.86,  4.84,  4.83] ],
    '6GeV'             : [ [0,1.05,1.5,2.0,9.9], [  5.88,  5.81,  5.78,  5.76] ],
    '7GeV'             : [ [0,1.05,1.5,2.0,9.9], [  6.85,  6.77,  6.74,  6.74] ],
    '8GeV'             : [ [0,1.05,1.5,2.0,9.9], [  7.82,  7.74,  7.70,  7.72] ],
    '10GeV'            : [ [0,1.05,1.5,2.0,9.9], [  9.77,  9.67,  9.62,  9.57] ],
    '11GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 10.74, 10.64, 10.58, 10.53] ],
    '12GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 11.70, 11.59, 11.53, 11.49] ], 
    '13GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 12.67, 12.55, 12.49, 12.46] ],
    '14GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 13.65, 13.52, 13.46, 13.42] ],
    '15GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 14.63, 14.49, 14.42, 14.38] ],
    '18GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 17.53, 17.39, 17.34, 17.28] ],
    '20GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 19.47, 19.33, 19.30, 19.22] ],
    '22GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 21.40, 21.27, 21.25, 21.16] ],
    '24GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 23.34, 23.19, 23.14, 23.06] ],
    '26GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 25.29, 25.15, 25.14, 25.05] ],
    '27GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 26.26, 26.12, 26.11, 26.02] ],
    '28GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 27.23, 27.09, 27.07, 26.99] ],
    '30GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 29.17, 29.03, 29.00, 28.92] ],
    '32GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 31.10, 30.96, 30.91, 30.84] ],
    '34GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 33.04, 32.88, 32.81, 32.74] ],
    '36GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 34.96, 34.78, 34.69, 34.63] ],
    '38GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 36.87, 36.67, 36.55, 36.48] ],
    '40GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 38.76, 38.54, 38.38, 38.31] ],
    '50GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 45.00, 45.00, 45.00, 45.00] ],
    '60GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 54.00, 54.00, 54.00, 54.00] ],
    '70GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 63.00, 63.00, 63.00, 63.00] ],
    '80GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 72.00, 72.00, 72.00, 72.00] ],
    '100GeV'           : [ [0,1.05,1.5,2.0,9.9], [ 90.00, 90.00, 90.00, 90.00] ],
    }


muCombThresholds = {
    '2GeV'             : [ [0,9.9],              [ 2.000] ],
    '4GeV'             : [ [0,1.05,1.5,2.0,9.9], [  3.0,  2.5,  2.5,  2.5] ],  
    '5GeV'             : [ [0,1.05,1.5,2.0,9.9], [  4.9,  4.8,  4.8,  4.8] ], 
    '6GeV'             : [ [0,1.05,1.5,2.0,9.9], [  5.8,  5.8,  5.8,  5.6] ], 
    '7GeV'             : [ [0,1.05,1.5,2.0,9.9], [  6.8,  6.7,  6.7,  6.6] ], 
    '8GeV'             : [ [0,1.05,1.5,2.0,9.9], [  7.8,  7.7,  7.7,  7.7] ], 
    '10GeV'            : [ [0,1.05,1.5,2.0,9.9], [  9.8,  9.5,  9.6,  9.7] ], 
    '11GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 10.8, 10.4, 10.6, 10.6] ], 
    '12GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 11.7, 11.3, 11.4, 11.5] ],  
    '13GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 12.6, 12.2, 12.2, 12.4] ], 
    '14GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 13.6, 13.1, 13.1, 13.5] ], 
    '15GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 14.5, 14.0, 14.0, 14.5] ], 
    '18GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 17.5, 16.6, 16.6, 16.8] ], 
    '20GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 19.5, 18.5, 18.5, 18.5] ], 
    '22GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 21.4, 20.3, 20.3, 20.1] ],
    '24GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 23.2, 22.2, 22.2, 21.8] ], 
    '26GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 25.3, 24.1, 24.1, 23.5] ], 
    '27GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 26.2, 25.1, 25.1, 24.4] ], 
    '28GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 27.1, 26.0, 26.0, 25.2] ], 
    '30GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 29.0, 28.0, 28.0, 27.0] ], 
    '32GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 30.7, 29.9, 29.9, 28.7] ], 
    '34GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 32.5, 31.8, 31.8, 30.4] ], 
    '36GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 34.2, 33.6, 33.6, 32.1] ], 
    '38GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 35.8, 35.4, 35.4, 33.6] ], 
    '40GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 37.5, 37.0, 37.0, 35.0] ], 
    '40GeV_slow'      : [ [0,1.05,1.5,2.0,9.9], [ 40.0, 40.0, 40.0, 40.0] ], 
    '50GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 40.0, 40.0, 40.0, 40.0] ], 
    '60GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 45.0, 45.0, 45.0, 45.0] ], 
    '60GeV_slow'      : [ [0,1.05,1.5,2.0,9.9], [ 47.0, 47.0, 47.0, 47.0] ], 
    '70GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 49.0, 49.0, 49.0, 49.0] ], 
    '80GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 56.0, 56.0, 56.0, 56.0] ], 
    '100GeV'           : [ [0,1.05,1.5,2.0,9.9], [ 70.0, 70.0, 70.0, 70.0] ],
    }


muFastThresholds = {
    #
    # original + 2010 tuning
    '2GeV'             : [ [0,9.9],              [ 2.000] ],               
    '4GeV'             : [ [0,1.05,1.5,2.0,9.9], [  3.0,  2.5,  2.5,  2.5] ], 
    '4GeV_V2'          : [ [0,1.05,1.5,2.0,9.9], [  3.0,  1.8,  2.0,  2.5] ], 
    '5GeV'             : [ [0,1.05,1.5,2.0,9.9], [  4.6,  3.3,  4.0,  4.5] ], 
    '6GeV'             : [ [0,1.05,1.5,2.0,9.9], [  5.4,  4.5,  4.9,  5.3] ], 
    '6GeV_V2'          : [ [0,1.05,1.5,2.0,9.9], [  5.2,  4.0,  4.0,  4.5] ], 
    '7GeV'             : [ [0,1.05,1.5,2.0,9.9], [  6.3,  5.6,  5.6,  6.3] ], 
    '8GeV'             : [ [0,1.05,1.5,2.0,9.9], [  7.2,  6.7,  6.4,  7.3] ], 
    '10GeV'            : [ [0,1.05,1.5,2.0,9.9], [  8.9,  9.0,  8.4,  9.2] ],
    '10GeV_V2'         : [ [0,1.05,1.5,2.0,9.9], [  8.5,  7.0,  7.0,  8.0] ],
    '11GeV'            : [ [0,1.05,1.5,2.0,9.9], [  9.8, 10.1,  9.3, 10.1] ],
    '12GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 10.6, 11.0, 10.2, 11.0] ], 
    '13GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 11.4, 12.0, 11.1, 12.0] ],
    '14GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 12.2, 13.0, 12.1, 13.0] ],
    '15GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 13.0, 14.0, 13.0, 14.0] ],
    '18GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 15.7, 16.6, 15.4, 16.3] ],
    '20GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 17.5, 18.5, 17.0, 18.0] ],
    '20GeV_V2'         : [ [0,1.05,1.5,2.0,9.9], [ 16.2, 12.9, 13.2, 15.5] ],
    '22GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 19.1, 20.0, 18.4, 19.6] ],
    '30GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 25.0, 24.5, 23.0, 26.0] ],
    '30GeV_V2'         : [ [0,1.05,1.5,2.0,9.9], [ 22.3, 17.1, 18.6, 22.2] ],
    '40GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 31.5, 30.0, 28.5, 32.5] ],
    '40GeV_barrelOnly' : [ [0,1.05,1.5,2.0,9.9], [ 31.5,1000.0,1000.0,1000.0] ],
    '40GeV_uptoEC2'    : [ [0,1.05,1.5,2.0,9.9], [ 31.5, 30.0, 28.5,  1000.0] ],
    '40GeV_V2'         : [ [0,1.05,1.5,2.0,9.9], [ 27.1, 23.6, 24.2, 26.4] ],
    '40GeV_slow'      : [ [0,1.05,1.5,2.0,9.9], [ 40.0, 40.0, 40.0, 40.0] ],
    '50GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 31.5, 30.0, 28.5, 32.5] ],
    '60GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 37.8, 36., 34.2, 39.] ],
    '60GeV_slow'      : [ [0,1.05,1.5,2.0,9.9], [ 40.0, 40.0, 45.0, 40.0] ],
    '70GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 37.8, 36.0, 34.2, 39.0] ],
    '80GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 37.8, 36.0, 34.2, 39.0] ],
    '100GeV'           : [ [0,1.05,1.5,2.0,9.9], [ 37.8, 36.0, 34.2, 39.0] ],
    #
    # 2011a tuning (95%, based on 2010 repro data)
    '4GeV_v11a'              : [ [0,1.05,1.5,2.0,9.9], [  3.41,  3.21,  3.39,  3.53] ], 
    '4GeV_barrelOnly_v11a'   : [ [0,1.05,1.5,2.0,9.9], [  3.41, 1000., 1000., 1000.] ],
    '6GeV_v11a'              : [ [0,1.05,1.5,2.0,9.9], [  5.04,  4.81,  5.01,  5.25] ], 
    '10GeV_v11a'             : [ [0,1.05,1.5,2.0,9.9], [  8.08,  7.68,  8.28,  8.90] ],
    '13GeV_v11a'             : [ [0,1.05,1.5,2.0,9.9], [ 10.00,  9.33, 10.28, 11.42] ],
    '15GeV_v11a'             : [ [0,1.05,1.5,2.0,9.9], [ 11.31, 10.52, 12.00, 13.24] ],
    '18GeV_v11a'             : [ [0,1.05,1.5,2.0,9.9], [ 12.95, 12.32, 13.92, 15.51] ],
    '20GeV_v11a'             : [ [0,1.05,1.5,2.0,9.9], [ 13.94, 13.50, 15.28, 16.79] ],
    '22GeV_v11a'             : [ [0,1.05,1.5,2.0,9.9], [ 14.85, 14.48, 16.36, 18.45] ],
    '24GeV_v11a'             : [ [0,1.05,1.5,2.0,9.9], [ 15.68, 15.51, 17.49, 19.82] ],
    '30GeV_v11a'             : [ [0,1.05,1.5,2.0,9.9], [ 17.83, 18.32, 20.46, 23.73] ],
    '40GeV_v11a'             : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 21.20, 25.38, 29.54] ],
    '40GeV_uptoEC2_v11a'     : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 21.20, 25.38, 1000.] ],
    '40GeV_barrelOnly_v11a'  : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 1000., 1000., 1000.] ],
    '50GeV_barrelOnly_v11a'  : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 1000., 1000., 1000.] ],
    '60GeV_barrelOnly_v11a'  : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 1000., 1000., 1000.] ],
     # 2011b tuning (90%, based on 2010 repro data) 
    '40GeV_v11b'             : [ [0,1.05,1.5,2.0,9.9], [ 26.88, 26.54, 29.22, 32.18] ],
    '40GeV_uptoEC2_v11b'     : [ [0,1.05,1.5,2.0,9.9], [ 26.88, 26.54, 29.22, 1000.] ],
    '40GeV_barrelOnly_v11b'  : [ [0,1.05,1.5,2.0,9.9], [ 26.88, 1000., 1000., 1000.] ],
    }


muFastThresholdsForECWeakBRegion = {
    #
    # original + 2010 tuning
    '2GeV'             : [  2.0,  2.0 ],   
    '4GeV'             : [  2.5,  2.5 ],
    '4GeV_V2'          : [  1.8,  2.0 ],
    '5GeV'             : [  3.3,  4.0 ],
    '6GeV'             : [  4.5,  4.9 ],
    '6GeV_V2'          : [  4.0,  4.0 ],
    '7GeV'             : [  5.6,  5.6 ],
    '8GeV'             : [  6.7,  6.4 ],
    '10GeV'            : [  9.0,  8.4 ],
    '10GeV_V2'         : [  7.0,  7.0 ],
    '11GeV'            : [ 10.1,  9.3 ],
    '12GeV'            : [ 11.0, 10.2 ], 
    '13GeV'            : [ 12.0, 11.1 ],
    '14GeV'            : [ 13.0, 12.1 ],
    '15GeV'            : [ 14.0, 13.0 ],
    '18GeV'            : [ 16.6, 15.4 ],
    '20GeV'            : [ 18.5, 17.0 ],
    '20GeV_V2'         : [ 12.9, 13.2 ],
    '22GeV'            : [ 20.0, 18.4 ],
    '30GeV'            : [ 24.5, 23.0 ],
    '30GeV_V2'         : [ 17.1, 18.6 ],
    '40GeV'            : [ 30.0, 28.5 ],
    '40GeV_barrelOnly' : [ 1000.0, 1000.0 ],
    '40GeV_uptoEC2'    : [   30.0,   28.5 ],
    '40GeV_V2'         : [ 23.6, 24.2 ],
    '40GeV_slow'      : [ 40.0, 40.0 ],
    '50GeV'            : [ 30.0, 28.5 ],
    '60GeV'            : [ 30.0, 28.5 ],
    '60GeV_slow'      : [ 60.0, 60.0 ],
    '70GeV'            : [ 30.0, 28.5 ],
    '80GeV'            : [ 30.0, 28.5 ],
    '100GeV'           : [ 30.0, 28.5 ],
    #
    # 2011a tuning (95%, based on 2010 repro data)
    '4GeV_v11a'             : [  2.11,  2.97 ],
    '4GeV_barrelOnly_v11a'  : [ 1000., 1000. ],
    '6GeV_v11a'             : [  4.37,  3.77 ],
    '10GeV_v11a'            : [  6.61,  5.34 ],
    '13GeV_v11a'            : [  6.81,  6.97 ],
    '15GeV_v11a'            : [  7.61,  7.81 ],
    '18GeV_v11a'            : [  9.01,  9.83 ],
    '20GeV_v11a'            : [ 10.15, 11.88 ],
    '22GeV_v11a'            : [ 10.72, 12.54 ],
    '24GeV_v11a'            : [ 11.66, 13.97 ],
    '30GeV_v11a'            : [ 14.41, 17.43 ],
    '40GeV_v11a'            : [ 15.07, 18.02 ],
    '40GeV_uptoEC2_v11a'    : [ 15.07, 18.02 ],
    '40GeV_barrelOnly_v11a' : [ 1000., 1000. ],
    '50GeV_barrelOnly_v11a' : [ 1000., 1000. ],
    '60GeV_barrelOnly_v11a' : [ 1000., 1000. ],
    # 2011b tuning (90%, based on 2010 repro data)
    '40GeV_v11b'            : [ 26.54, 29.22 ],
    '40GeV_uptoEC2_v11b'    : [ 26.54, 29.22 ],
    '40GeV_barrelOnly_v11b' : [ 1000., 1000. ],
    '50GeV_barrelOnly_v11b' : [ 1000., 1000. ],
    '60GeV_barrelOnly_v11b' : [ 1000., 1000. ],
    }

class MufastHypoConfig(MufastHypo) :

    __slots__ = []
    
    def __new__( cls, *args, **kwargs ):
        if len(args) == 2:
            newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        if len(args) == 4:
            newargs = ['%s_%s_%s_%s_%s' % (cls.getType(),args[0],args[1],args[2],args[3])] + list(args)
        return super( MufastHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( MufastHypoConfig, self ).__init__( name )

        threshold = args[1]

        try:
            values = muFastThresholds[threshold]
            self.PtBins = values[0]
            self.PtThresholds = [ x * GeV for x in values[1] ]
            if threshold in muFastThresholdsForECWeakBRegion:
                spThres = muFastThresholdsForECWeakBRegion[threshold]
                self.PtThresholdForECWeakBRegionA = spThres[0] * GeV
                self.PtThresholdForECWeakBRegionB = spThres[1] * GeV
            else:
                print 'MufastHypoConfig: No special thresholds for EC weak Bfield regions for',threshold
                print 'MufastHypoConfig: -> Copy EC1 for region A, EC2 for region B'
                spThres = values[0][1]
                if threshold == '2GeV':
                    self.PtThresholdForECWeakBRegionA = spThres[0] * GeV
                    self.PtThresholdForECWeakBRegionB = spThres[0] * GeV
                else:
                    self.PtThresholdForECWeakBRegionA = spThres[1] * GeV
                    self.PtThresholdForECWeakBRegionB = spThres[2] * GeV
                print 'MufastHypoConfig: -> Thresholds for A/B=',self.PtThresholdForECWeakBRegionA,'/',self.PtThresholdForECWeakBRegionB
            
        except LookupError:
            if (threshold=='passthrough'):
                self.PtBins = [-10000.,10000.]
                self.PtThresholds = [ -1. * GeV ]
            else:
                raise Exception('MuFast Hypo Misconfigured: threshold %r not supported' % threshold)

        validation = MufastHypoValidationMonitoring()
        online     = MufastHypoOnlineMonitoring()
        cosmic     = MufastHypoCosmicMonitoring()
	
        self.AthenaMonTools = [ validation, online, cosmic ]
        #if len(args) == 4:
        #    if args[2] != 9999:
        #        
        #        self.SelectPV = True
        #        self.Z_PV_Bins = args[2]
        #        self.R_PV_Bins = args[3]
        #    else:
        #        self.SelectPV = False
        #        self.Z_PV_Bins = 99999
        #        self.R_PV_Bins = 99999
    
    def setDefaults(cls,handle):
        if hasattr(handle,'PtThresholds') and hasattr(handle,'PtBins'):
            if len(handle.PtThresholds)!=len(handle.PtBins)-1:
                print handle.name," eta bins doesn't match the Pt thresholds!"

class MufastStauHypoConfig(MufastStauHypo) :

    __slots__ = []
    
    def __new__( cls, *args, **kwargs ):
        if len(args) == 2:
            newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        if len(args) == 4:
            newargs = ['%s_%s_%s_%s_%s' % (cls.getType(),args[0],args[1],args[2],args[3])] + list(args)
        return super( MufastStauHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( MufastStauHypoConfig, self ).__init__( name )

        threshold = args[1]

        try:
            values = muFastThresholds[threshold]
            self.PtBins = values[0]
            self.PtThresholds = [ x * GeV for x in values[1] ] 
        except LookupError:
            if (threshold=='passthrough'):
                self.PtBins = [-10000.,10000.]
                self.PtThresholds = [ -1. * GeV ]
            else:
                raise Exception('MuFast Hypo Misconfigured: threshold %r not supported' % threshold)

        validation = MufastStauHypoValidationMonitoring()
        online     = MufastStauHypoOnlineMonitoring()
        cosmic     = MufastStauHypoCosmicMonitoring()
	
        self.AthenaMonTools = [ validation, online, cosmic ]
        #if len(args) == 4:
        #    if args[2] != 9999:
        #        
        #        self.SelectPV = True
        #        self.Z_PV_Bins = args[2]
        #        self.R_PV_Bins = args[3]
        #    else:
        #        self.SelectPV = False
        #        self.Z_PV_Bins = 99999
        #        self.R_PV_Bins = 99999
        if (args[0]=='Tight'):
            print "sofia OK"
            self.EtaCut = True
 
    def setDefaults(cls,handle):
        if hasattr(handle,'PtThresholds') and hasattr(handle,'PtBins'):
            if len(handle.PtThresholds)!=len(handle.PtBins)-1:
                print handle.name," eta bins doesn't match the Pt thresholds!"


class MufastPEBHypoConfig(MufastPEBHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s' % (cls.getType(),args[0])] + list(args)
        return super( MufastPEBHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( MufastPEBHypoConfig, self ).__init__( name )

        validation = MufastPEBHypoValidationMonitoring()
        online     = MufastPEBHypoOnlineMonitoring()
        cosmic     = MufastPEBHypoCosmicMonitoring()

        self.AthenaMonTools = [ validation, online, cosmic ]


class MufastCALHypoConfig(MufastCALHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        if len(args) == 2:
            newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        #newargs = ['%s_%s' % (cls.getType(),args[0])] + list(args)
        return super( MufastCALHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( MufastCALHypoConfig, self ).__init__( name )

        validation = MufastCALHypoValidationMonitoring()
        online     = MufastCALHypoOnlineMonitoring()
        cosmic     = MufastCALHypoCosmicMonitoring()

        self.AthenaMonTools = [ validation, online, cosmic ]


class MufastOTRHypoConfig(MufastOTRHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        if len(args) == 2:
            newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        if len(args) == 4:
            newargs = ['%s_%s_%s_%s_%s' % (cls.getType(),args[0],args[1],args[2],args[3])] + list(args)
        return super( MufastOTRHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( MufastOTRHypoConfig, self ).__init__( name )

        threshold = int(args[1])

        self.SegmentsTh = threshold

        validation = MufastOTRHypoValidationMonitoring()
        online     = MufastOTRHypoOnlineMonitoring()
        cosmic     = MufastOTRHypoCosmicMonitoring()

        self.AthenaMonTools = [ validation, online, cosmic ]



class MucombHypoConfig(MucombHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( MucombHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( MucombHypoConfig, self ).__init__( name )

        threshold = args[1]
        print 'MucombHypoConfig configured for threshold: ',threshold

        try:
            values = muCombThresholds[threshold]
            self.PtBins = values[0]
            self.PtThresholds = [ x * GeV for x in values[1] ]
        except LookupError:
            if (threshold=='passthrough'):
                #self.AcceptAll = True
                #self.PtBins = [-10000.,10000.]
                #self.PtThresholds = [ -1. * GeV ]
                self.PtBins = [-10000.,10000.]
                self.PtThresholds = [ -1. * GeV ]
                self.ApplyStrategyDependentCuts = True
                self.Apply_pik_Cuts = False
            else:
                raise Exception('MuComb Hypo Misconfigured: threshold %r not supported' % threshold)

        conf = args[0]
        if (conf.find("tight")!=-1): 
            self.Apply_pik_Cuts        = True
            self.MaxPtToApply_pik      = 25.
            self.MaxChi2ID_pik         = 3.5

        validation = MucombHypoValidationMonitoring()
        online     = MucombHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online ]

    def setDefaults(cls,handle):
        if hasattr(handle,'PtThresholds') and hasattr(handle,'PtBins'):
            if len(handle.PtThresholds)!=len(handle.PtBins)-1:
                print handle.name," eta bins doesn't match the Pt thresholds!"


class MucombStauHypoConfig(MucombStauHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( MucombStauHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( MucombStauHypoConfig, self ).__init__( name )

        threshold = args[1]

        try:
            values = muCombThresholds[threshold]
            self.PtBins = values[0]
            self.PtThresholds = [ x * GeV for x in values[1] ]
        except LookupError:
            raise Exception('MuCombStau Hypo Misconfigured: threshold %r not supported' % threshold)


        if ( muonByteStreamFlags.RpcDataType()=='atlas' ):
            self.BackExtrapolator = MuonBackExtrapolatorForMisalignedDet()
        else:
            self.BackExtrapolator = MuonBackExtrapolatorForAlignedDet()	   

        conf = args[0]
        if (conf.find("tight")!=-1): 
            self.Apply_pik_Cuts = True

        validation = MucombStauHypoValidationMonitoring()
        online     = MucombStauHypoOnlineMonitoring()
	
        self.AthenaMonTools = [ validation, online ]

    def setDefaults(cls,handle):
        if hasattr(handle,'PtThresholds') and hasattr(handle,'PtBins'):
            if len(handle.PtThresholds)!=len(handle.PtBins)-1:
                print handle.name," eta bins doesn't match the Pt thresholds!"


class MuisoHypoConfig(MuisoHypo):
    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s' % (cls.getType())] + list(args)
        if len(args) == 1:
            newargs = ['%s_%s' % (cls.getType(),args[0])] + list(args)
        if len(args) == 2:
            newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( MuisoHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( MuisoHypoConfig, self ).__init__( name )

        validation = MuisoHypoValidationMonitoring()
        online     = MuisoHypoOnlineMonitoring()
        cosmic     = MuisoHypoCosmicMonitoring()

        self.AthenaMonTools = [ validation, online, cosmic]

        # If configured with passthrough, set AcceptAll flag on
        self.AcceptAll           = False
        if len(args) == 2:
            if (args[1]=='passthrough'):
                self.AcceptAll = True
                print 'MuisoHypoConfig configured in pasthrough mode'

        # Isolation Hypothesis configuration and cuts
        self.UseCalo    = True
        self.UseAbsCalo = True
        self.UseID      = True
        self.UseAbsID   = False

        #Default cuts (more than 95% eff on Z->mumu) tuned on 2011 run 178044 data <beta>~6
        self.CaloConeSize = 2;
        self.IDConeSize   = 2;
        self.MaxCaloIso_1 = 5200.0
        self.MaxCaloIso_2 = 4800.0
        self.MaxCaloIso_3 = 4000.0
        self.MaxIDIso_1   = 0.05
        self.MaxIDIso_2   = 0.05
        self.MaxIDIso_3   = 0.05

        # Specific configurations
        if len(args) == 2:
            print 'MuisoHypoConfig configuration for: ', args[1]
            if (args[1]=='OLD'):
               print self.name,"OLD configuration not supported anymore, standard cuts used insetad)"
            if (args[1]=='10GeV'):
               print self.name,"Very LowPt muIsoHypo configuration set (thight to keep rate low)"
               self.MaxCaloIso_1 = 4400.0
               self.MaxCaloIso_2 = 4000.0
               self.MaxCaloIso_3 = 3200.0
               self.MaxIDIso_1   = 0.04
               self.MaxIDIso_2   = 0.04
               self.MaxIDIso_3   = 0.04
            if (args[1]=='15GeV'):
               print self.name,"LowPt muIsoHypo configuration set (tau/W/Z/top)"
            if (args[1]=='18GeV'):
               print self.name,"IntermediatePt muIsoHypo configuration set (tau/W/Z/top)"
            if (args[1]=='20GeV'):
               print self.name,"HighPt muIsoHypo configuration set (W/Z) (default)"
            if (args[1]=='4GeV'):
               print self.name,"Passthrough special muIsoHypo configuration for 4GeV muons test)"
               self.AcceptAll = True
            if (args[1]=='6GeV'):
               print self.name,"Passthrough special muIsoHypo configuration for 6GeV muons test)"
               self.AcceptAll = True
            if (args[1]=='top'):
               print self.name,"top physics muIsoHypo configuration set (dummy yet)"
            if (args[1]=='ExoticLJ'):
               print self.name,"Special Exotic non prompt LeptonJets configuration set"
               self.UseHollowConeCalo = True;
               self.HollowConeCaloInner = 2;
               self.HollowConeCaloOuter = 4;
               self.MaxCaloIso_1 = 5000.0;
               self.MaxCaloIso_2 = 5000.0;
               self.MaxCaloIso_3 = 5000.0;
               self.MaxIDIso_1   = 5000.0;
               self.MaxIDIso_2   = 5000.0;
               self.MaxIDIso_3   = 5000.0;
               self.UseCalo    = True
               self.UseID      = True
               self.UseAbsID   = True
               self.UseAbsCalo = True
            if (args[1]=='DY'):
               print self.name,"Special configuration set for DY dimuon isolation (Eram)"
               self.MaxCaloIso_1 = 99999999999.0
               self.MaxCaloIso_2 = 99999999999.0
               self.MaxCaloIso_3 = 99999999999.0
               self.MaxIDIso_1   = 0.25
               self.MaxIDIso_2   = 0.25
               self.MaxIDIso_3   = 0.25
               self.UseCalo = False
               self.UseID = True
            if (args[1]=='tight'):
               print self.name,"Tight muIsoHypo configuration set for 3e33 chains (absolute cuts)"
               self.MaxCaloIso_1 = 1400.0
               self.MaxCaloIso_2 = 1400.0
               self.MaxCaloIso_3 = 1400.0
               self.MaxIDIso_1   = 5700.0
               self.MaxIDIso_2   = 5700.0
               self.MaxIDIso_3   = 5700.0
               self.UseCalo = True
               self.UseID = True
               self.UseAbsID = True
            if (args[1]=='tightRel'):
               print self.name,"Tight muIsoHypo configuration set for 3e33 chains (relative cuts)"
               self.MaxCaloIso_1 = 0.1
               self.MaxCaloIso_2 = 0.1
               self.MaxCaloIso_3 = 0.1
               self.MaxIDIso_1   = 0.3
               self.MaxIDIso_2   = 0.3
               self.MaxIDIso_3   = 0.3
               self.UseCalo = True
               self.UseID = True
               self.UseAbsID = False
               self.UseAbsCalo = False
            if (args[1]=='loose'):
               print self.name,"Loose muIsoHypo configuration set for 3e33 chains (absolute cuts)" 
               self.MaxCaloIso_1 = 2700.0
               self.MaxCaloIso_2 = 2700.0
               self.MaxCaloIso_3 = 2700.0
               self.MaxIDIso_1   = 99999999999.0
               self.MaxIDIso_2   = 99999999999.0
               self.MaxIDIso_3   = 99999999999.0
               self.UseCalo = True
               self.UseID = False
               self.UseAbsID = True
            if (args[1]=='tauCP'):
               print self.name,"muIsoHypo configuration set for Tau CP (absolute cuts)" 
               self.MaxCaloIso_1 = 1400.0
               self.MaxCaloIso_2 = 1400.0
               self.MaxCaloIso_3 = 1400.0
               self.MaxIDIso_1   = 99999999999.0
               self.MaxIDIso_2   = 99999999999.0
               self.MaxIDIso_3   = 99999999999.0
               self.UseCalo = True
               self.UseID = False
               self.UseAbsID = True
            if (args[1]=='looseRel'):
               print self.name,"Loose muIsoHypo configuration set for 3e33 chains (relative cuts)" 
               self.MaxCaloIso_1 = 0.1
               self.MaxCaloIso_2 = 0.1
               self.MaxCaloIso_3 = 0.1
               self.MaxIDIso_1   = 99999999999.0
               self.MaxIDIso_2   = 99999999999.0
               self.MaxIDIso_3   = 99999999999.0
               self.UseCalo = True
               self.UseID = False
               self.UseAbsID = False
               self.UseAbsCalo = False

        print 'MuisoHypoConfig configuration done'


class TrigMooreHypoConfig(TrigMooreHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( TrigMooreHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigMooreHypoConfig, self ).__init__( name )

        threshold = args[1]

        try:
            values = trigMuonEFSAThresholds[threshold]
            self.PtBins = values[0]
            self.PtThresholds = [ x * GeV for x in values[1] ]
            #values = dirThresholdValues[threshold]
            #self.PtBins = values[2][0]
            #self.PtThresholds = [ x * GeV for x in values[2][1] ]
        except LookupError:
            raise Exception('TrigMoore Hypo Misconfigured!')

        validation = TrigMooreHypoValidationMonitoring()
        online     = TrigMooreHypoOnlineMonitoring()
	
        self.AthenaMonTools = [ validation, online ]

    def setDefaults(cls,handle):
        if hasattr(handle,'PtThresholds') and hasattr(handle,'PtBins'):
            if len(handle.PtThresholds)!=len(handle.PtBins)-1:
                print handle.name," eta bins doesn't match the Pt thresholds!"


class TrigMuonEFSegmentFinderHypoConfig(TrigMuonEFSegmentFinderHypo):
    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( TrigMuonEFSegmentFinderHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigMuonEFSegmentFinderHypoConfig, self ).__init__( name )

#        from TrigMuonHypo.TrigMuonHypoMonitoring import TrigMuonEFSegmentFinderHypoOnlineMonitoring,TrigMuonEFSegmentFinderHypoValidationMonitoring
#        validation = TrigMuonEFSegmentFinderHypoValidationMonitoring()
#        online     = TrigMuonEFSegmentFinderHypoOnlineMonitoring()
#        cosmic     = TrigMuonEFSegmentFinderHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TrigMuonEFSegmentFinderHypo_Time")

#        self.AthenaMonTools = [ validation, online, time, cosmic]

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # If configured with passthrough, set AcceptAll flag on
        if (args[1]=='passthrough'):
            self.AcceptAll = True

        # Hypothesis configuration and cuts
        # std. cuts SG-10-11-08)
        self.Nseg = 1



class TrigMuonEFTrackBuilderHypoConfig(TrigMuonEFTrackBuilderHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( TrigMuonEFTrackBuilderHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigMuonEFTrackBuilderHypoConfig, self ).__init__( name )

        threshold = args[1]

        try:
            values = trigMuonEFSAThresholds[threshold]
            self.PtBins = values[0]
            self.PtThresholds = [ x * GeV for x in values[1] ]
            self.AcceptAll = False
        except LookupError:
            if (threshold=='passthrough'):
                self.PtBins = [-10000.,10000.]
                self.PtThresholds = [ -1. * GeV ]
                self.AcceptAll = True
            else:
                raise Exception('TrigMuonEFTrackBuilder Hypo Misconfigured: threshold %r not supported' % threshold)

        validation = TrigMuonEFTrackBuilderHypoValidationMonitoring()
        online     = TrigMuonEFTrackBuilderHypoOnlineMonitoring()
	
        self.AthenaMonTools = [ validation, online ]

    def setDefaults(cls,handle):
        if hasattr(handle,'PtThresholds') and hasattr(handle,'PtBins'):
            if len(handle.PtThresholds)!=len(handle.PtBins)-1:
                print handle.name," eta bins doesn't match the Pt thresholds!"



class TrigMuonEFExtrapolatorHypoConfig(TrigMuonEFExtrapolatorHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( TrigMuonEFExtrapolatorHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigMuonEFExtrapolatorHypoConfig, self ).__init__( name )

        threshold = args[1]

        try:
            values = trigMuonEFSAThresholds[threshold]
            self.PtBins = values[0]
            self.PtThresholds = [ x * GeV for x in values[1] ]
            self.AcceptAll = False
        except LookupError:
            if (threshold=='passthrough'):
                self.PtBins = [-10000.,10000.]
                self.PtThresholds = [ -1. * GeV ]
                self.AcceptAll = True
            else:
                raise Exception('TrigMuonEFExtrapolator Hypo Misconfigured: threshold %r not supported' % threshold)

        validation = TrigMuonEFExtrapolatorHypoValidationMonitoring()
        online     = TrigMuonEFExtrapolatorHypoOnlineMonitoring()
	
        self.AthenaMonTools = [ validation, online ]

        def setDefaults(cls,handle):
            if hasattr(handle,'PtThresholds') and hasattr(handle,'PtBins'):
                if len(handle.PtThresholds)!=len(handle.PtBins)-1:
                    print handle.name," eta bins doesn't match the Pt thresholds!"             



class TrigMuonEFCombinerHypoConfig(TrigMuonEFCombinerHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( TrigMuonEFCombinerHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigMuonEFCombinerHypoConfig, self ).__init__( name )

        threshold = args[1]

        try:
            values = efCombinerThresholds[threshold]
            self.PtBins = values[0]
            self.PtThresholds = [ x * GeV for x in values[1] ]
            self.AcceptAll = False
        except LookupError:
            if (threshold=='passthrough'):
                self.PtBins = [-10000.,10000.]
                self.PtThresholds = [ -1. * GeV ]
                self.AcceptAll = True
            else:
                raise Exception('TrigMuonEFCombiner Hypo Misconfigured: threshold %r not supported' % threshold)

        validation = TrigMuonEFCombinerHypoValidationMonitoring()
        online     = TrigMuonEFCombinerHypoOnlineMonitoring()
	
        self.AthenaMonTools = [ validation, online ]

    def setDefaults(cls,handle):
        if hasattr(handle,'PtThresholds') and hasattr(handle,'PtBins'):
            if len(handle.PtThresholds)!=len(handle.PtBins)-1:
                print handle.name," eta bins doesn't match the Pt thresholds!"


#AOH
#
# Multi Muon hypothesis.
# Thresholds must be inclusive, but no ordering required.
#
# arg[0] = "Muon"
# arg[1] = [1st muon threshold]
# arg[2] = [2nd muon threshold]
# arg[3] = [3rd muon threshold]
# ...
# arg[N] = {Nth muon threshold]

class TrigMuonEFCombinerMultiHypoConfig(TrigMuonEFCombinerMultiHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        #newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        newargs = ['%s_%s' % (cls.getType(),reduce(lambda s1,s2: s1+"_"+s2, args))] + list(args)
        return super( TrigMuonEFCombinerMultiHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigMuonEFCombinerMultiHypoConfig, self ).__init__( name )

        try:
            import itertools as it
            PtMultiplicity = list()
            PtBins = list()
            PtThresholds = list()
            i = 0
            for threshold in list(it.islice(args,1,None)) :
                values = efCombinerThresholds[threshold]
                PtMultiplicity += [ i for x in values[1] ] 
                PtBins         += values[0]
                PtThresholds   += [ x * GeV for x in values[1] ]                
                i                   += 1                    
            self.PtMultiplicity = PtMultiplicity
            self.PtBins         = PtBins
            self.PtThresholds   = PtThresholds                
            self.AcceptAll      = False
        except LookupError:
            if (threshold=='passthrough'):
                self.PtBins = [-10000.,10000.]
                self.PtThresholds = [ -1. * GeV ]
                self.AcceptAll = True
            else:
                raise Exception('TrigMuonEFCombiner Multi Hypo Misconfigured: threshold %r not supported' % threshold)

        validation = TrigMuonEFCombinerMultiHypoValidationMonitoring()
        online     = TrigMuonEFCombinerMultiHypoOnlineMonitoring()
	
        self.AthenaMonTools = [ validation, online ]
        
    def setDefaults(cls,handle):
        if hasattr(handle,'PtThresholds') and hasattr(handle,'PtBins') and hasattr(handle,'PtMultiplicity'):
            nmult = max(handle.PtMultiplicity) - min(handle.PtMultiplicity) + 1
            if len(handle.PtThresholds)!=len(handle.PtBins)-nmult:
                print handle.name," eta bins doesn't match the Pt thresholds!"


class TrigMuonEFExtrapolatorMultiHypoConfig(TrigMuonEFExtrapolatorMultiHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        #newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        newargs = ['%s_%s' % (cls.getType(),reduce(lambda s1,s2: s1+"_"+s2, args))] + list(args)
        return super( TrigMuonEFExtrapolatorMultiHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigMuonEFExtrapolatorMultiHypoConfig, self ).__init__( name )

        try:
            import itertools as it
            PtMultiplicity = list()
            PtBins = list()
            PtThresholds = list()
            i = 0
            for threshold in list(it.islice(args,1,None)) :
                values = trigMuonEFSAThresholds[threshold]
                PtMultiplicity += [ i for x in values[1] ] 
                PtBins         += values[0]
                PtThresholds   += [ x * GeV for x in values[1] ]                
                i                   += 1                    
            self.PtMultiplicity = PtMultiplicity
            self.PtBins         = PtBins
            self.PtThresholds   = PtThresholds                
            self.AcceptAll      = False
        except LookupError:
            if (threshold=='passthrough'):
                self.PtBins = [-10000.,10000.]
                self.PtThresholds = [ -1. * GeV ]
                self.AcceptAll = True
            else:
                raise Exception('TrigMuonEFExtrapolator Multi Hypo Misconfigure: threshold %r not supported' % threshold)

        validation = TrigMuonEFExtrapolatorMultiHypoValidationMonitoring()
        online     = TrigMuonEFExtrapolatorMultiHypoOnlineMonitoring()
	
        self.AthenaMonTools = [ validation, online ]
        
    def setDefaults(cls,handle):
        if hasattr(handle,'PtThresholds') and hasattr(handle,'PtBins') and hasattr(handle,'PtMultiplicity'):
            nmult = max(handle.PtMultiplicity) - min(handle.PtMultiplicity) + 1
            if len(handle.PtThresholds)!=len(handle.PtBins)-nmult:
                print handle.name," eta bins doesn't match the Pt thresholds!"


class TrigMuonEFTrackBuilderMultiHypoConfig(TrigMuonEFTrackBuilderMultiHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        #newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        newargs = ['%s_%s' % (cls.getType(),reduce(lambda s1,s2: s1+"_"+s2, args))] + list(args)
        return super( TrigMuonEFTrackBuilderMultiHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigMuonEFTrackBuilderMultiHypoConfig, self ).__init__( name )

        try:
            import itertools as it
            PtMultiplicity = list()
            PtBins = list()
            PtThresholds = list()
            i = 0
            for threshold in list(it.islice(args,1,None)) :
                values = trigMuonEFSAThresholds[threshold]
                PtMultiplicity += [ i for x in values[1] ] 
                PtBins         += values[0]
                PtThresholds   += [ x * GeV for x in values[1] ]                
                i                   += 1                    
            self.PtMultiplicity = PtMultiplicity
            self.PtBins         = PtBins
            self.PtThresholds   = PtThresholds                
            self.AcceptAll      = False
        except LookupError:
            if (threshold=='passthrough'):
                self.PtBins = [-10000.,10000.]
                self.PtThresholds = [ -1. * GeV ]
                self.AcceptAll = True
            else:
                raise Exception('TrigMuonEFTrackBuilder Multi Hypo Misconfigured: threshold %r not supported' % threshold)

        validation = TrigMuonEFTrackBuilderMultiHypoValidationMonitoring()
        online     = TrigMuonEFTrackBuilderMultiHypoOnlineMonitoring()
	
        self.AthenaMonTools = [ validation, online ]
        
    def setDefaults(cls,handle):
        if hasattr(handle,'PtThresholds') and hasattr(handle,'PtBins') and hasattr(handle,'PtMultiplicity'):
            nmult = max(handle.PtMultiplicity) - min(handle.PtMultiplicity) + 1
            if len(handle.PtThresholds)!=len(handle.PtBins)-nmult:
                print handle.name," eta bins doesn't match the Pt thresholds!"


class StauHypoConfig(StauHypo) :

    __slots__ = []
    
    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s' % (cls.getType(),args[0])] + list(args)
        return super( StauHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( StauHypoConfig, self ).__init__( name )

        # Isolation Hypothesis configuration and cuts
        self.PtThreshold = 30000.0
        self.BetaMax     = 0.97
        self.MMin        = 40000.0


class TileMuHypoConfig(TileMuHypo) :

    __slots__ = [] 
            
    def __new__( cls, *args, **kwargs ):
        if len(args) == 2:            
            newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        elif len(args) == 3:
            newargs = ['%s_%s_%s_%s' % (cls.getType(),args[0],args[1],args[2])] + list(args)
        else:
            newargs = ['%s_%s' % (cls.getType(),args[0])] + list(args)

        return super( TileMuHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TileMuHypoConfig, self ).__init__( name )
            
        # Isolation Hypothesis configuration and cuts
        #self.AcceptAll = True
        self.AcceptAll = False
        #self.IOptIDScan = 0
        self.IOptIDScan = 1
        if len(args) > 2:
          TrackType = args[2]
          if (TrackType=='NoTrack'):
            self.IOptIDScan = 0
          if (TrackType=='SITRACK'):
            self.IOptIDScan = 2
          if (TrackType=='TRTXK'):
            self.IOptIDScan = 3
          if (TrackType=='TRTSEG'):
            self.IOptIDScan = 4

        #self.UseIDScan = True  # Not use! (2008/Jan/18)
        #self.DelPhi_Cut = 0.1
        #self.DelEta_Cut = 0.1
        
        self.Pt_Cut = 2000.0
                            # Unit(Pt):MeV! It is the default value!
        if len(args) == 2:
          threshold = args[1]
          if (threshold=='6GeV'):
            self.Pt_Cut = 6000.0
          if (threshold=='4GeV'):
            self.Pt_Cut = 4000.0

        # Add for histogram
        validation = TileMuHypoValidationMonitoring()
        online     = TileMuHypoOnlineMonitoring()
        cosmic     = TileMuHypoCosmicMonitoring()

        self.AthenaMonTools = [ validation, online, cosmic ]

class TrigMuGirlHypoConfig(TrigMuGirlHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( TrigMuGirlHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigMuGirlHypoConfig, self ).__init__( name )

        threshold = args[1]

        try:
            values = efCombinerThresholds[threshold]
            self.PtBins = values[0]
            self.PtThresholds = [ x * GeV for x in values[1] ]
            self.AcceptAll = False
        except LookupError:
            raise Exception('TrigMuGirl Hypo Misconfigured: threshold %r not supported' % threshold)

#        validation = TrigMuGirlHypoValidationMonitoring()
#        online     = TrigMuGirlHypoOnlineMonitoring()
#        self.AthenaMonTools = [ validation, online ]


        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 100]
        time.NumberOfHistBins = 100
        self.AthenaMonTools = [ time ]

    def setDefaults(cls,handle):
        if hasattr(handle,'PtThresholds') and hasattr(handle,'PtBins'):
            if len(handle.PtThresholds)!=len(handle.PtBins)-1:
                print handle.name," eta bins doesn't match the Pt thresholds!"


class TrigMuGirlStauHypoConfig(TrigMuGirlStauHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( TrigMuGirlStauHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigMuGirlStauHypoConfig, self ).__init__( name )

        threshold = args[1]

        try:
            values = efCombinerThresholds[threshold]
            self.PtBins = values[0]
            self.PtThresholds = [ x * GeV for x in values[1] ]
            self.AcceptAll = False
        except LookupError:
            raise Exception('TrigMuGirlStau Hypo Misconfigured: threshold %r not supported' % threshold)

#        validation = TrigMuGirlHypoValidationMonitoring()
#        online     = TrigMuGirlHypoOnlineMonitoring()
#        self.AthenaMonTools = [ validation, online ]


        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 100]
        time.NumberOfHistBins = 100
        self.AthenaMonTools = [ time ]

    def setDefaults(cls,handle):
        if hasattr(handle,'PtThresholds') and hasattr(handle,'PtBins'):
            if len(handle.PtThresholds)!=len(handle.PtBins)-1:
                print handle.name," eta bins doesn't match the Pt thresholds!"



class TrigMuTagIMOHypoConfig(TrigMuTagIMOHypo) :
    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( TrigMuTagIMOHypoConfig, cls ).__new__( cls, *newargs, **kwargs )
    
    def __init__( self, name = "TrigMuTagIMOHypoConfig", *args, **kwargs):
        super( TrigMuTagIMOHypoConfig, self ).__init__( name )

        threshold = args[1]

        try:
            # borrow muFast threshold
            values = muFastThresholds[threshold]
            self.PtBins = values[0]
            self.PtThresholds = [ x * GeV for x in values[1] ]
            self.AcceptAll = False
        except LookupError:
            raise Exception('TrigMuTagIMOHypo Misconfigured!')
        
        validation = TrigMuTagIMOHypoValidationMonitoring()
        online     = TrigMuTagIMOHypoOnlineMonitoring()
        cosmic     = TrigMuTagIMOHypoCosmicMonitoring()
        self.AthenaMonTools = [ validation, online, cosmic ]

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 10]
        time.NumberOfHistBins = 20
        self.AthenaMonTools = [ time ]

    def setDefaults(cls,handle):
        if hasattr(handle,'PtThresholds') and hasattr(handle,'PtBins'):
            if len(handle.PtThresholds)!=len(handle.PtBins)-1:
                print handle.name," eta bins doesn't match the Pt thresholds!"


class MuonRoiFexConfig(MuonRoiFex) :

    __slots__ = []
    
    def __new__( cls, *args, **kwargs ):
        if len(args) == 1:
            newargs = ['%s_%s' % (cls.getType(),args[0])] + list(args)
        return super( MuonRoiFexConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( MuonRoiFexConfig, self ).__init__( name )

        mode = args[0]

        if (mode=='passthrough'):
            self.AcceptAll     = True
            self.RpcOnly       = False
            self.RpcHorizontal = False
        elif (mode=='RpcOnly'):
            self.AcceptAll     = False
            self.RpcOnly       = True
            self.RpcHorizontal = False
        elif (mode=='RpcHorizontal'):
            self.AcceptAll            = False
            self.RpcOnly              = False
            self.RpcHorizontal        = True
            self.dPhiForRpcHorizontal = 0.78
        else:
             raise Exception('MuonRoiFex Misconfigured!')

# Working points for EF track isolation algorithm
# syntax is:
# 'WPname' : [cut on 0.2 cone, cut on 0.3 cone]
# put < 0 for no cut
# For a relative cut the WPname should contain 'Rel'
trigMuonEFTrkIsoThresholds = {
    'EFOnlyLoose'             : [ 4200.0,  -1.0 ],
    'EFOnlyTight'             : [ 1500.0,  -1.0 ],
    'Loose'                   : [ 2800.0,  -1.0 ],
    'Tight'                   : [ -1.0,   1000.0],
    'TauCP'                   : [ -1.0,      1.0],

    'RelEFOnlyLoose'          : [ 0.2,  -1.0 ],
    'RelEFOnlyMedium'         : [ 0.12, -1.0 ],
    'RelEFOnlyTight'          : [ 0.06, -1.0 ],
    
    'EFOnlyLooseWide'         : [ -1.0,   4200.0],
    'EFOnlyMediumWide'        : [ -1.0,   2100.0],
    'EFOnlyTightWide'         : [ -1.0,   1500.0],
    
    'RelEFOnlyLooseWide'      : [-1.0 ,  0.2  ],
    'RelEFOnlyMediumWide'     : [-1.0 ,  0.12 ],
    'RelEFOnlyTightWide'      : [-1.0 ,  0.06 ] 
    }

"""
Class for hypothesis cuts on EF track isolation algorithm.
arg[0] = Muon
arg[1] = working point
See trigMuonEFTrkIsoThresholds for available working points
Put passthrough in arg[1] for passthrough
"""
class TrigMuonEFTrackIsolationHypoConfig(TrigMuonEFTrackIsolationHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( TrigMuonEFTrackIsolationHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigMuonEFTrackIsolationHypoConfig, self ).__init__( name )

        try:
            cuts = trigMuonEFTrkIsoThresholds[ args[1] ]
            ptcone02 = cuts[0]
            ptcone03 = cuts[1]

            self.PtCone02Cut = ptcone02
            self.PtCone03Cut = ptcone03
            self.AcceptAll = False

            if 'Rel' in args[1] :
                self.DoAbsCut = False
            else :
                self.DoAbsCut = True
                                            
        except LookupError:
            if(args[1]=='passthrough') :
                print 'Setting passthrough'
                self.AcceptAll = True
            else:
                print 'args[1] = ', args[1]
                raise Exception('TrigMuonEFTrackIsolation Hypo Misconfigured')
        

        validation = TrigMuonEFTrackIsolationHypoValidationMonitoring()
        online     = TrigMuonEFTrackIsolationHypoOnlineMonitoring()
	
        self.AthenaMonTools = [ validation, online ]


class TrigMuonEFCaloIsolationHypoConfig(TrigMuonEFCaloIsolationHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        #newargs = ['%s_%s' % (cls.getType(),args[0])] + list(args)

        return super( TrigMuonEFCaloIsolationHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigMuonEFCaloIsolationHypoConfig, self ).__init__( name )

        try:
            #cuts = trigMuonEFCaloIsoThresholds[ args[1] ]

            #validation = MuisoHypoValidationMonitoring()
            #online     = MuisoHypoOnlineMonitoring()
            #cosmic     = MuisoHypoCosmicMonitoring()

            # If configured with passthrough, set AcceptAll flag on
            self.AcceptAll = False
            if len(args) == 2:
                if (args[1]=='passthrough'):
                    self.AcceptAll = True
                    print 'TrigMuonEFCaloIsolationConfig configured in passthrough mode'

            # Isolation Hypothesis configuration and cuts
            self.UseCalo    = True
            self.UseAbsCalo = True
            #self.UseID      = True
            #self.UseAbsID   = False

            #Default cuts (more than 95% eff on Z->mumu) tuned on 2011 run 178044 data <beta>~6
            #These probably should be updated!
            self.CaloConeSize = 2;
            #self.IDConeSize   = 2;
            self.MaxCaloIso_1 = 5200.0
            self.MaxCaloIso_2 = 4800.0
            self.MaxCaloIso_3 = 4000.0
            #self.MaxIDIso_1   = 0.05
            #self.MaxIDIso_2   = 0.05
            #self.MaxIDIso_3   = 0.05

            if 'Rel' in args[1] :
                self.UseAbsCalo = False
            else :
                self.UseAbsCalo = True
                                            
        except LookupError:
            if(args[1]=='passthrough') :
                print 'Setting passthrough'
                self.AcceptAll = True
            else:
                print 'args[1] = ', args[1]
                raise Exception('TrigMuonEFCaloIsolation Hypo Misconfigured')
        

        #validation = TrigMuonEFCaloIsolationHypoValidationMonitoring()
        #online     = TrigMuonEFCaloIsolationHypoOnlineMonitoring()
	
        #self.AthenaMonTools = [ validation, online ]



"""
Class for hypothesis cuts on EF combiner dimuon mass hypo
arg[0] = mass working point
arg[1] = sign (NS/OS/SS)
Put passthrough in arg[1] for passthrough
"""
trigMuonEFCombinerDiMuonMassThresholds = {
     'Jpsi'    : [  2.5,    4.3 ],
     'Upsi'    : [  8.0,   12.0 ],
     'Z'       : [ 80.0,  100.0 ],
    }

class TrigMuonEFCombinerDiMuonMassHypoConfig(TrigMuonEFCombinerDiMuonMassHypo) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( TrigMuonEFCombinerDiMuonMassHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigMuonEFCombinerDiMuonMassHypoConfig, self ).__init__( name )

        try:
            cuts = trigMuonEFCombinerDiMuonMassThresholds[ args[0] ]
            massThresLow  = cuts[0]
            massThresHigh = cuts[1]

            self.massThresLow  = massThresLow
            self.massThresHigh = massThresHigh
            self.AcceptAll = False

            if 'NS' in args[1] :
                self.signRequirement =  0
            elif 'OS' in args[1] :
                self.signRequirement = -1
            elif 'SS' in args[1] :
                self.signRequirement =  1
            else :
                print 'args[1] = ', args[1]
                raise Exception('TrigMuonEFCombinerDiMuonMass Hypo Misconfigured')
                                            
        except LookupError:
            if(args[0]=='passthrough') :
                print 'Setting passthrough'
                self.AcceptAll = True
            else:
                print 'args[0] = ', args[0]
                raise Exception('TrigMuonEFCombinerDiMuonMass Hypo Misconfigured')
        

        validation = TrigMuonEFCombinerDiMuonMassHypoValidationMonitoring()
        online     = TrigMuonEFCombinerDiMuonMassHypoOnlineMonitoring()
	
        self.AthenaMonTools = [ validation, online ]


class MufastNSWHypoConfig(MufastNSWHypo) :

    __slots__ = []
    
    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s' % (cls.getType(),args[0])] + list(args)
        return super( MufastNSWHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( MufastNSWHypoConfig, self ).__init__( name )

        self.AcceptAll = False

        validation = MufastNSWHypoValidationMonitoring()
        online     = MufastNSWHypoOnlineMonitoring()
	
        self.AthenaMonTools = [ validation, online ]


class TrigMuonEFExtrapolatorNSWHypoConfig(TrigMuonEFExtrapolatorNSWHypo) :

    __slots__ = []
    
    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s' % (cls.getType(),args[0])] + list(args)
        return super( TrigMuonEFExtrapolatorNSWHypoConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigMuonEFExtrapolatorNSWHypoConfig, self ).__init__( name )

        self.AcceptAll = False

        validation = TrigMuonEFExtrapolatorNSWHypoValidationMonitoring()
        online     = TrigMuonEFExtrapolatorNSWHypoOnlineMonitoring()
	
        self.AthenaMonTools = [ validation, online ]
