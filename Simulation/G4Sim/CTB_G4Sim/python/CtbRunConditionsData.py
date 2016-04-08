# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:   CTB_G4Sim/share/CtbRunConditionsData.py
#
# Contents:
#         Run conditions to simulate the 'good validated' CTB data. 
#
#=======================================================================
"""
 Run conditions to simulate the 'good validated' CTB data in the 
 combined period. 

  The minimal information required per run is the 'SimLayout'. Energy, 
 type of particle or number of events SHOULD NOT BE WRITTEN HERE!!!. 

 Information collected by Thomas Koffas and Roberto Petti 
 that cover runs in the real-data range: 

   2101725 to 2102986 including the photon period that starts at 2102775 

 Note: 
 (1) Starting from revision version 1.15 the magnetic field for the ID 
     is taken from the  corresponding magnetic field map. The old constant 
     value stays here but it is not used.  
 (2) Starting from revision version 1.17 it is possible to use beam 
     divergence data per run. Data collected by Thomas Koffas and 
     Mayuko Kataoka.  

"""
__version__="$Revision: 1.2 $"
#
from G4AtlasApps.SimFlags import RunCondition 

#--- Data -------------------------------------------------------------
RunCond=RunCondition('Local')
BeamCond = RunCondition('Local')

RunCond.set_Run(2917,{'SimLayout':'ctbh8_photon','Eta':0.4971,
                     'IdetOracleTag':'InnerDetector-CTB-08-01',
                     'MagnetMBPSIDBz':0.0,'MagnetMBPL12Bz':-1.7,
                     'MagnetMBPL13By':-1.7})

RunCond.set_Run(2937,{'SimLayout':'ctbh8_photon','Eta':0.2998,
                     'IdetOracleTag':'InnerDetector-CTB-08-01',
                     'MagnetMBPSIDBz':0.0,'MagnetMBPL12Bz':-1.7,
                     'MagnetMBPL13By':-1.7})

RunCond.set_Run(2966,{'SimLayout':'ctbh8_photon','Eta':0.4125,
                     'IdetOracleTag':'InnerDetector-CTB-08-01',
                     'MagnetMBPSIDBz':0.0,'MagnetMBPL12Bz':-1.7,
                     'MagnetMBPL13By':1.25})

RunCond.set_Run(2860,{'SimLayout':'ctbh8_photon','Eta':0.54,
                     'IdetOracleTag':'InnerDetector-CTB-08-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.5,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2859,{'SimLayout':'ctbh8_photon','Eta':0.54,
                     'IdetOracleTag':'InnerDetector-CTB-08-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.5,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2857,{'SimLayout':'ctbh8_photon','Eta':0.54,
                     'IdetOracleTag':'InnerDetector-CTB-08-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.5,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2856,{'SimLayout':'ctbh8_photon','Eta':0.539,
                     'IdetOracleTag':'InnerDetector-CTB-08-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.5,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2851,{'SimLayout':'ctbh8_photon','Eta':0.537,
                     'IdetOracleTag':'InnerDetector-CTB-08-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.5,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2845,{'SimLayout':'ctbh8_photon','Eta':0.537,
                     'IdetOracleTag':'InnerDetector-CTB-08-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.5,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2841,{'SimLayout':'ctbh8_photon','Eta':0.537,
                     'IdetOracleTag':'InnerDetector-CTB-08-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.5,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2824,{'SimLayout':'ctbh8_photon','Eta':0.537,
                     'IdetOracleTag':'InnerDetector-CTB-08-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.5,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2819,{'SimLayout':'ctbh8_photon','Eta':0.537,
                     'IdetOracleTag':'InnerDetector-CTB-08-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.5,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2816,{'SimLayout':'ctbh8_photon','Eta':0.537,
                     'IdetOracleTag':'InnerDetector-CTB-08-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.5,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2813,{'SimLayout':'ctbh8_photon','Eta':0.537,
                     'IdetOracleTag':'InnerDetector-CTB-08-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.6,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2801,{'SimLayout':'ctbh8_photon','Eta':0.53,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.0,
                     'MagnetMBPL13By':1.034,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2800,{'SimLayout':'ctbh8_photon','Eta':0.53,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.0,
                     'MagnetMBPL13By':1.034,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2788,{'SimLayout':'ctbh8_photon','Eta':0.523,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.0,
                     'MagnetMBPL13By':1.034,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2787,{'SimLayout':'ctbh8_photon','Eta':0.523,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.0,
                     'MagnetMBPL13By':1.034,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2783,{'SimLayout':'ctbh8_photon','Eta':0.523,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.0,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2782,{'SimLayout':'ctbh8_photon','Eta':0.523,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.0,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2780,{'SimLayout':'ctbh8_photon','Eta':0.523,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-1.0,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2779,{'SimLayout':'ctbh8_photon','Eta':0.514,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-0.65,
                     'MagnetMBPL13By':0.836,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2778,{'SimLayout':'ctbh8_photon','Eta':0.517,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'MagnetMBPL12Bz':-0.9,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2777,{'SimLayout':'ctbh8_photon','Eta':0.517,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'MagnetMBPL12Bz':-0.9,
                     'MagnetMBPL13By':1.350,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2775,{'SimLayout':'ctbh8_photon','Eta':0.517,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'MagnetMBPL12Bz':-0.9,
                     'MagnetMBPL13By':1.350})

RunCond.set_Run(2734,{'SimLayout':'ctbh8_combined','Eta':0.2,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':-0.3294,
                     'MagFieldMap':'minus-mbps1-200-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2733,{'SimLayout':'ctbh8_combined','Eta':0.55,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.3294,
                     'MagFieldMap':'mbps1-200-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2731,{'SimLayout':'ctbh8_combined','Eta':0.2,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2730,{'SimLayout':'ctbh8_combined','Eta':0.2,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':-0.3294,
                     'MagFieldMap':'minus-mbps1-200-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2728,{'SimLayout':'ctbh8_combined','Eta':0.2,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.3294,
                     'MagFieldMap':'mbps1-200-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2727,{'SimLayout':'ctbh8_combined','Eta':0.2,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2710,{'SimLayout':'ctbh8_combined','Eta':0.2,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2554,{'SimLayout':'ctbh8_combined','Eta':0.2,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':-0.1647,
                     'MagFieldMap':'minus-mbps1-100-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2552,{'SimLayout':'ctbh8_combined','Eta':0.2,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':-0.1647,
                     'MagFieldMap':'minus-mbps1-100-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2550,{'SimLayout':'ctbh8_combined','Eta':0.2,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':-0.1647,
                     'MagFieldMap':'minus-mbps1-100-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2549,{'SimLayout':'ctbh8_combined','Eta':0.2,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':-0.1647,
                     'MagFieldMap':'minus-mbps1-100-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2545,{'SimLayout':'ctbh8_combined','Eta':0.2,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':-0.1647,
                     'MagFieldMap':'minus-mbps1-100-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2541,{'SimLayout':'ctbh8_combined','Eta':0.2,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':-0.1647,
                     'MagFieldMap':'minus-mbps1-100-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2538,{'SimLayout':'ctbh8_combined','Eta':0.575,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2537,{'SimLayout':'ctbh8_combined','Eta':0.575,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2530,{'SimLayout':'ctbh8_combined','Eta':0.575,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2528,{'SimLayout':'ctbh8_combined','Eta':0.682,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2527,{'SimLayout':'ctbh8_combined','Eta':0.682,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2526,{'SimLayout':'ctbh8_combined','Eta':0.682,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2525,{'SimLayout':'ctbh8_combined','Eta':0.682,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2523,{'SimLayout':'ctbh8_combined','Eta':0.682,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2521,{'SimLayout':'ctbh8_combined','Eta':0.682,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2519,{'SimLayout':'ctbh8_combined','Eta':0.682,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2517,{'SimLayout':'ctbh8_combined','Eta':0.682,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2516,{'SimLayout':'ctbh8_combined','Eta':0.682,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2514,{'SimLayout':'ctbh8_combined','Eta':0.682,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':-1.4,
                     'MagFieldMap':'minus-mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2512,{'SimLayout':'ctbh8_combined','Eta':0.682,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2511,{'SimLayout':'ctbh8_combined','Eta':0.484,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2510,{'SimLayout':'ctbh8_combined','Eta':0.484,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2507,{'SimLayout':'ctbh8_combined','Eta':0.484,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2506,{'SimLayout':'ctbh8_combined','Eta':0.484,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2504,{'SimLayout':'ctbh8_combined','Eta':0.484,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2500,{'SimLayout':'ctbh8_combined','Eta':0.484,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2499,{'SimLayout':'ctbh8_combined','Eta':0.484,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2498,{'SimLayout':'ctbh8_combined','Eta':0.484,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2497,{'SimLayout':'ctbh8_combined','Eta':0.388,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2494,{'SimLayout':'ctbh8_combined','Eta':0.388,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2493,{'SimLayout':'ctbh8_combined','Eta':0.388,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2492,{'SimLayout':'ctbh8_combined','Eta':0.388,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2491,{'SimLayout':'ctbh8_combined','Eta':0.388,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2489,{'SimLayout':'ctbh8_combined','Eta':0.388,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2488,{'SimLayout':'ctbh8_combined','Eta':0.388,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2487,{'SimLayout':'ctbh8_combined','Eta':0.292,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2484,{'SimLayout':'ctbh8_combined','Eta':0.292,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2481,{'SimLayout':'ctbh8_combined','Eta':0.292,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2480,{'SimLayout':'ctbh8_combined','Eta':0.292,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2478,{'SimLayout':'ctbh8_combined','Eta':0.292,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2477,{'SimLayout':'ctbh8_combined','Eta':0.193,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2474,{'SimLayout':'ctbh8_combined','Eta':0.193,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2473,{'SimLayout':'ctbh8_combined','Eta':0.193,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2472,{'SimLayout':'ctbh8_combined','Eta':0.193,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2470,{'SimLayout':'ctbh8_combined','Eta':0.193,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2467,{'SimLayout':'ctbh8_combined','Eta':0.083,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2464,{'SimLayout':'ctbh8_combined','Eta':0.083,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2463,{'SimLayout':'ctbh8_combined','Eta':0.083,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2462,{'SimLayout':'ctbh8_combined','Eta':0.083,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2461,{'SimLayout':'ctbh8_combined','Eta':0.083,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2459,{'SimLayout':'ctbh8_combined','Eta':0.083,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2458,{'SimLayout':'ctbh8_combined','Eta':0.083,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2457,{'SimLayout':'ctbh8_combined','Eta':0.083,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2455,{'SimLayout':'ctbh8_combined','Eta':0.083,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2454,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2452,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2449,{'SimLayout':'ctbh8_combined','Eta':0.439,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2446,{'SimLayout':'ctbh8_combined','Eta':0.439,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2445,{'SimLayout':'ctbh8_combined','Eta':0.434,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2444,{'SimLayout':'ctbh8_combined','Eta':0.434,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2442,{'SimLayout':'ctbh8_combined','Eta':0.434,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2441,{'SimLayout':'ctbh8_combined','Eta':0.434,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2440,{'SimLayout':'ctbh8_combined','Eta':0.437,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2439,{'SimLayout':'ctbh8_combined','Eta':0.437,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2438,{'SimLayout':'ctbh8_combined','Eta':0.437,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2435,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':-0.1647})

RunCond.set_Run(2434,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':-0.1647})

RunCond.set_Run(2433,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':-0.1647})

RunCond.set_Run(2423,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':-0.1647})

RunCond.set_Run(2419,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2413,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2410,{'SimLayout':'ctbh8_combined','Eta':0.440,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2404,{'SimLayout':'ctbh8_combined','Eta':0.440,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2400,{'SimLayout':'ctbh8_combined','Eta':0.440,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2399,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2398,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2397,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2396,{'SimLayout':'ctbh8_combined','Eta':0.434,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'LeadUpstreamAbs':True})

RunCond.set_Run(2395,{'SimLayout':'ctbh8_combined','Eta':0.434,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'LeadUpstreamAbs':True})

RunCond.set_Run(2393,{'SimLayout':'ctbh8_combined','Eta':0.434,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'LeadUpstreamAbs':True})

RunCond.set_Run(2392,{'SimLayout':'ctbh8_combined','Eta':0.434,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'LeadUpstreamAbs':True})

RunCond.set_Run(2391,{'SimLayout':'ctbh8_combined','Eta':0.434,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2390,{'SimLayout':'ctbh8_combined','Eta':0.434,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2389,{'SimLayout':'ctbh8_combined','Eta':0.434,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2381,{'SimLayout':'ctbh8_combined','Eta':0.437,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2375,{'SimLayout':'ctbh8_combined','Eta':0.437,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2372,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2371,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2367,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2365,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2356,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2355,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'LeadUpstreamAbs':True})

RunCond.set_Run(2354,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'LeadUpstreamAbs':True})

RunCond.set_Run(2353,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'LeadUpstreamAbs':True})

RunCond.set_Run(2352,{'SimLayout':'ctbh8_combined','Eta':0.438,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'LeadUpstreamAbs':True})

RunCond.set_Run(2351,{'SimLayout':'ctbh8_combined','Eta':0.440,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'LeadUpstreamAbs':True})

RunCond.set_Run(2350,{'SimLayout':'ctbh8_combined','Eta':0.440,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':1.4,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2349,{'SimLayout':'ctbh8_combined','Eta':0.440,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'LeadUpstreamAbs':True})

RunCond.set_Run(2348,{'SimLayout':'ctbh8_combined','Eta':0.440,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'LeadUpstreamAbs':True})

RunCond.set_Run(2347,{'SimLayout':'ctbh8_combined','Eta':0.439,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'LeadUpstreamAbs':True})

RunCond.set_Run(2345,{'SimLayout':'ctbh8_combined','Eta':0.439,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0,'LeadUpstreamAbs':True})

RunCond.set_Run(2343,{'SimLayout':'ctbh8_combined','Eta':0.439,
                     'IdetOracleTag':'InnerDetector-CTB-04-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2311,{'SimLayout':'ctbh8_combined','Eta':0.428,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2310,{'SimLayout':'ctbh8_combined','Eta':0.428,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2309,{'SimLayout':'ctbh8_combined','Eta':0.428,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2307,{'SimLayout':'ctbh8_combined','Eta':0.428,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2306,{'SimLayout':'ctbh8_combined','Eta':0.428,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2305,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2302,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2301,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2300,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2297,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2296,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2295,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2294,{'SimLayout':'ctbh8_combined','Eta':0.432,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2292,{'SimLayout':'ctbh8_combined','Eta':0.432,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2291,{'SimLayout':'ctbh8_combined','Eta':0.432,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2290,{'SimLayout':'ctbh8_combined','Eta':0.429,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2289,{'SimLayout':'ctbh8_combined','Eta':0.429,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2288,{'SimLayout':'ctbh8_combined','Eta':0.429,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2287,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2286,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2283,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2280,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2279,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2274,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2273,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2272,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2270,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2261,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2249,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2247,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2246,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2245,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2244,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2242,{'SimLayout':'ctbh8_combined','Eta':0.431,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2241,{'SimLayout':'ctbh8_combined','Eta':0.431,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2238,{'SimLayout':'ctbh8_combined','Eta':0.431,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2227,{'SimLayout':'ctbh8_combined','Eta':0.431,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2225,{'SimLayout':'ctbh8_combined','Eta':0.431,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2224,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2219,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2217,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-06-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2198,{'SimLayout':'ctbh8_combined','Eta':0.565,
                     'IdetOracleTag':'InnerDetector-CTB-05-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2195,{'SimLayout':'ctbh8_combined','Eta':0.565,
                     'IdetOracleTag':'InnerDetector-CTB-05-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2182,{'SimLayout':'ctbh8_combined','Eta':0.430,
                     'IdetOracleTag':'InnerDetector-CTB-05-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2164,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2163,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2162,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2156,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2154,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2152,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':0.49412,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-300-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2151,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':0.82353,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-500-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2150,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':1.4,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2149,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':0.3294,
                     'MagFieldMap':'mbps1-200-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2147,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':0.49412,
                     'MagFieldMap':'mbps1-300-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2146,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':0.49412,
                     'MagFieldMap':'mbps1-300-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2145,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':0.82353,
                     'MagFieldMap':'mbps1-500-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2143,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':0.82353,
                     'MagFieldMap':'mbps1-500-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2140,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2139,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2137,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':0.49412,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-300-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2134,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':1.4,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2130,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':1.4,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2128,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':0.82353,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-500-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2126,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-09-01',
                     'MagnetMBPSIDBz':0.82353,'LeadUpstreamAbs':True,
                     'MagFieldMap':'mbps1-500-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2118,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.3294,
                     'MagFieldMap':'mbps1-200-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2117,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2115,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2114,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.49412,
                     'MagFieldMap':'mbps1-300-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2113,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.49412,
                     'MagFieldMap':'mbps1-300-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2110,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.49412,
                     'MagFieldMap':'mbps1-300-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2109,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2107,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2106,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.82353,
                     'MagFieldMap':'mbps1-500-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2105,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':1.4,
                     'MagFieldMap':'mbps1-850-source.data',
                     'MagFieldMapCalculation':1})

RunCond.set_Run(2104,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2103,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2102,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2101,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2098,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2097,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2096,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(2009,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(1815,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.0})

RunCond.set_Run(1814,{'SimLayout':'ctbh8_combined','Eta':0.442,
                     'IdetOracleTag':'InnerDetector-CTB-03-01',
                     'MagnetMBPSIDBz':0.0})

BeamCond.set_Run(2734 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2733 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2731 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2730 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2728 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2727 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2710 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2554 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2552 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2550 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2549 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2545 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2541 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2538 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2537 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2530 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2528 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2527 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2526 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2525 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2523 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2521 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2519 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2517 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2516 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2514 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2512 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2511 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2510 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2507 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2506 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2504 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2500 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2499 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2498 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10. ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568050 0.000968 ','phi:' : ' gauss -0.001997 0.000372'}})
 
BeamCond.set_Run(2497 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2494 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2493 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2492 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2491 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2489 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2488 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2487 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2484 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2481 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2480 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
#BeamCond.set_Run(2478 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
BeamCond.set_Run(2478 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' gauss  1.39 5.94 ','vertZ:' : ' gauss 5.307 5.57','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2477 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2474 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2473 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2472 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2470 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2467 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2464 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
#BeamCond.set_Run(2463 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
BeamCond.set_Run(2463 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' gauss  1.39 5.94 ','vertZ:' : ' gauss 5.307 5.57','t:' :' constant -1100.0','theta:' : ' gauss 1.5677 0.000788 ','phi:' : ' gauss -0.00154 0.000320'}})
 
BeamCond.set_Run(2462 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2461 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2459 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2458 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2457 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2455 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
#BeamCond.set_Run(2454 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568120 0.000912 ','phi:' : ' gauss -0.002343 0.000305'}})
BeamCond.set_Run(2454 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -14.5 14 ','vertZ:' : ' flat -9. 23.','t:' :' constant -1100.0','theta:' : ' gauss 1.568120 0.000912 ','phi:' : ' gauss -0.002343 0.000305'}})
 
#BeamCond.set_Run(2452 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568120 0.000912 ','phi:' : ' gauss -0.002343 0.000305'}})
BeamCond.set_Run(2452 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -14.5 14 ','vertZ:' : ' flat -9. 23.','t:' :' constant -1100.0','theta:' : ' gauss 1.5678 0.00143 ','phi:' : ' gauss -0.00160 0.000248'}})
 
BeamCond.set_Run(2449 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
 
BeamCond.set_Run(2446 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
 
BeamCond.set_Run(2445 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2444 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
#BeamCond.set_Run(2442 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
BeamCond.set_Run(2442 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -14.5 13.5 ','vertZ:' : ' flat -8.5 24.5','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.00183 0.00114'}})
 
BeamCond.set_Run(2441 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2440 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
 
#BeamCond.set_Run(2439 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
BeamCond.set_Run(2439 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 8. ','vertZ:' : ' flat -8. 24.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.00217 0.000464'}})
 
BeamCond.set_Run(2438 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
 
BeamCond.set_Run(2435 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2434 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2433 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2423 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2419 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
 
#BeamCond.set_Run(2413 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
BeamCond.set_Run(2413 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' gauss  1.96 6.40 ','vertZ:' : ' flat -7.5 25.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000147 ','phi:' : ' gauss -0.001943 0.000461'}})
 
#BeamCond.set_Run(2410 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568107 0.000947 ','phi:' : ' gauss -0.002304 0.000357'}})
BeamCond.set_Run(2410 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -14.5 12.5 ','vertZ:' : ' flat -7. 23.5','t:' :' constant -1100.0','theta:' : ' gauss 1.568107 0.000947 ','phi:' : ' gauss -0.002304 0.000357'}})
 
BeamCond.set_Run(2404 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568107 0.000947 ','phi:' : ' gauss -0.002304 0.000357'}})
 
#BeamCond.set_Run(2400 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568107 0.000947 ','phi:' : ' gauss -0.002304 0.000357'}})
BeamCond.set_Run(2400 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -14.5 12.5 ','vertZ:' : ' flat -7. 23.5','t:' :' constant -1100.0','theta:' : ' gauss 1.568107 0.000947 ','phi:' : ' gauss -0.00197 0.00021'}})
 
#BeamCond.set_Run(2399 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568120 0.000912 ','phi:' : ' gauss -0.002343 0.000305'}})
BeamCond.set_Run(2399 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -14.5 13.5 ','vertZ:' : ' flat -7. 24.','t:' :' constant -1100.0','theta:' : ' gauss 1.568120 0.000912 ','phi:' : ' gauss -0.00201 0.000289'}})
 
#BeamCond.set_Run(2398 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568120 0.000912 ','phi:' : ' gauss -0.002343 0.000305'}})
BeamCond.set_Run(2398 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -14.5 14 ','vertZ:' : ' flat -9. 23.','t:' :' constant -1100.0','theta:' : ' gauss 1.568120 0.000912 ','phi:' : ' gauss -0.00198 0.000394'}})
 
#BeamCond.set_Run(2397 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
BeamCond.set_Run(2397 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 8. ','vertZ:' : ' flat -8. 24.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
 
#BeamCond.set_Run(2396 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
BeamCond.set_Run(2396 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -14.5 13.5 ','vertZ:' : ' flat -8.5 24.5','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2395 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2393 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2392 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2391 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2390 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2389 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2381 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2375 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2372 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2371 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
BeamCond.set_Run(2367 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
#BeamCond.set_Run(2365 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
BeamCond.set_Run(2365 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 13 ','vertZ:' : ' flat -8. 24.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.00194 0.00040'}})
 
BeamCond.set_Run(2356 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
#BeamCond.set_Run(2355 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
BeamCond.set_Run(2355 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 13 ','vertZ:' : ' flat -8. 24.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
BeamCond.set_Run(2354 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
BeamCond.set_Run(2353 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
BeamCond.set_Run(2352 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2351 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568053 0.000940 ','phi:' : ' gauss -0.002436 0.000550'}})
 
BeamCond.set_Run(2349 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568053 0.000940 ','phi:' : ' gauss -0.002436 0.000550'}})
 
BeamCond.set_Run(2348 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568053 0.000940 ','phi:' : ' gauss -0.002436 0.000550'}})
 
BeamCond.set_Run(2347 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568053 0.000940 ','phi:' : ' gauss -0.002436 0.000550'}})
 
BeamCond.set_Run(2345 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568053 0.000940 ','phi:' : ' gauss -0.002436 0.000550'}})
 
BeamCond.set_Run(2343 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568053 0.000940 ','phi:' : ' gauss -0.002436 0.000550'}})
 
BeamCond.set_Run(2311 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2310 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2309 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2307 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2306 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.567987 0.001334 ','phi:' : ' gauss -0.002059 0.001043'}})
 
BeamCond.set_Run(2305 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
 
BeamCond.set_Run(2302 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
 
BeamCond.set_Run(2301 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
 
BeamCond.set_Run(2300 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
 
BeamCond.set_Run(2297 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
 
BeamCond.set_Run(2296 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
 
BeamCond.set_Run(2295 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568315 0.000947 ','phi:' : ' gauss -0.002343 0.000538'}})
 
BeamCond.set_Run(2294 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568107 0.000947 ','phi:' : ' gauss -0.002304 0.000357'}})
 
BeamCond.set_Run(2292 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568107 0.000947 ','phi:' : ' gauss -0.002304 0.000357'}})
 
BeamCond.set_Run(2291 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568107 0.000947 ','phi:' : ' gauss -0.002304 0.000357'}})
 
BeamCond.set_Run(2290 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568053 0.000940 ','phi:' : ' gauss -0.002436 0.000550'}})
 
BeamCond.set_Run(2289 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568053 0.000940 ','phi:' : ' gauss -0.002436 0.000550'}})
 
BeamCond.set_Run(2288 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10. ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568053 0.000940 ','phi:' : ' gauss -0.002436 0.000550'}})
 
BeamCond.set_Run(2287 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
BeamCond.set_Run(2286 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
BeamCond.set_Run(2283 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
BeamCond.set_Run(2280 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
BeamCond.set_Run(2279 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
BeamCond.set_Run(2274 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
BeamCond.set_Run(2273 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
BeamCond.set_Run(2272 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
BeamCond.set_Run(2270 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
BeamCond.set_Run(2261 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568093 0.000813 ','phi:' : ' gauss -0.002327 0.000358'}})
 
BeamCond.set_Run(2249 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568120 0.000912 ','phi:' : ' gauss -0.002343 0.000305'}})
 
BeamCond.set_Run(2247 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568120 0.000912 ','phi:' : ' gauss -0.002343 0.000305'}})
 
BeamCond.set_Run(2246 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568120 0.000912 ','phi:' : ' gauss -0.002343 0.000305'}})
 
BeamCond.set_Run(2245 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568120 0.000912 ','phi:' : ' gauss -0.002343 0.000305'}})
 
BeamCond.set_Run(2244 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 10 ','vertZ:' : ' flat -5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568120 0.000912 ','phi:' : ' gauss -0.002343 0.000305'}})
 
BeamCond.set_Run(2242 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2241 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2238 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2227 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2225 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2224 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2219 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2217 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2198 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2195 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2182 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -10. 10 ','vertZ:' : ' flat 0. 20.','t:' :' constant -1100.0','theta:' : ' gauss 1.568055 0.000955 ','phi:' : ' gauss -0.001995 0.000287'}})
 
BeamCond.set_Run(2164 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569548 0.003593 ','phi:' : ' gauss -0.000734 0.003906'}})
 
BeamCond.set_Run(2163 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569126 0.001974 ','phi:' : ' gauss -0.001013 0.003478'}})
 
BeamCond.set_Run(2162 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569228 0.002264 ','phi:' : ' gauss -0.001373 0.003274'}})
 
BeamCond.set_Run(2156 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569126 0.001974 ','phi:' : ' gauss -0.001013 0.003478'}})
 
BeamCond.set_Run(2154 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569126 0.001974 ','phi:' : ' gauss -0.001013 0.003478'}})
 
BeamCond.set_Run(2152 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569228 0.002264 ','phi:' : ' gauss -0.001373 0.003274'}})
 
BeamCond.set_Run(2151 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569126 0.001974 ','phi:' : ' gauss -0.001013 0.003478'}})
 
BeamCond.set_Run(2150 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569180 0.002202 ','phi:' : ' gauss -0.001107 0.003111'}})
 
BeamCond.set_Run(2149 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569548 0.003593 ','phi:' : ' gauss -0.000734 0.003906'}})
 
BeamCond.set_Run(2147 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569228 0.002264 ','phi:' : ' gauss -0.001373 0.003274'}})
 
BeamCond.set_Run(2146 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569228 0.002264 ','phi:' : ' gauss -0.001373 0.003274'}})
 
BeamCond.set_Run(2145 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569126 0.001974 ','phi:' : ' gauss -0.001013 0.003478'}})
 
BeamCond.set_Run(2143 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569126 0.001974 ','phi:' : ' gauss -0.001013 0.003478'}})
 
BeamCond.set_Run(2140 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569180 0.002202 ','phi:' : ' gauss -0.001107 0.003111'}})
 
BeamCond.set_Run(2139 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569180 0.002202 ','phi:' : ' gauss -0.001107 0.003111'}})
 
BeamCond.set_Run(2137 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569228 0.002264 ','phi:' : ' gauss -0.001373 0.003274'}})
 
BeamCond.set_Run(2134 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569180 0.002202 ','phi:' : ' gauss -0.001107 0.003111'}})
 
BeamCond.set_Run(2130 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569180 0.002202 ','phi:' : ' gauss -0.001107 0.003111'}})
 
BeamCond.set_Run(2128 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569126 0.001974 ','phi:' : ' gauss -0.001013 0.003478'}})
 
BeamCond.set_Run(2126 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569126 0.001974 ','phi:' : ' gauss -0.001013 0.003478'}})
 
BeamCond.set_Run(2118 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569548 0.003593 ','phi:' : ' gauss -0.000734 0.003906'}})
 
BeamCond.set_Run(2117 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568391 0.002687 ','phi:' : ' gauss -0.000849 0.003675'}})
 
BeamCond.set_Run(2115 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569548 0.003593 ','phi:' : ' gauss -0.000734 0.003906'}})
 
BeamCond.set_Run(2114 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569228 0.002264 ','phi:' : ' gauss -0.001373 0.003274'}})
 
BeamCond.set_Run(2113 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569228 0.002264 ','phi:' : ' gauss -0.001373 0.003274'}})
 
BeamCond.set_Run(2110 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568736 0.002543 ','phi:' : ' gauss -0.001301 0.003448'}})
 
BeamCond.set_Run(2109 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569228 0.002264 ','phi:' : ' gauss -0.001373 0.003274'}})
 
BeamCond.set_Run(2107 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569180 0.002202 ','phi:' : ' gauss -0.001107 0.003111'}})
 
BeamCond.set_Run(2106 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569126 0.001974 ','phi:' : ' gauss -0.001013 0.003478'}})
 
BeamCond.set_Run(2105 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569126 0.001974 ','phi:' : ' gauss -0.001013 0.003478'}})
 
BeamCond.set_Run(2104 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569126 0.001974 ','phi:' : ' gauss -0.001013 0.003478'}})
 
BeamCond.set_Run(2103 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569180 0.002202 ','phi:' : ' gauss -0.001107 0.003111'}})
 
BeamCond.set_Run(2102 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 5. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.568647 0.001578 ','phi:' : ' gauss -0.001351 0.002784'}})
 
BeamCond.set_Run(2101 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569548 0.003593 ','phi:' : ' gauss -0.000734 0.003906'}})
 
BeamCond.set_Run(2098 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569228 0.002264 ','phi:' : ' gauss -0.001373 0.003274'}})
 
BeamCond.set_Run(2097 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569126 0.001974 ','phi:' : ' gauss -0.001013 0.003478'}})
 
BeamCond.set_Run(2096 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 0. 30.','t:' :' constant -1100.0','theta:' : ' gauss 1.569180 0.002202 ','phi:' : ' gauss -0.001107 0.003111'}})
 
BeamCond.set_Run(2009 ,{'ParticleGeneratorOrders':{'vertX:':' constant -1100.0','vertY:' :' flat  -15. 5. ','vertZ:' : ' flat 5. 35.','t:' :' constant -1100.0','theta:' : ' gauss 1.568701 0.001912 ','phi:' : ' gauss -0.001111 0.002873'}})
 
#=======================================================================
