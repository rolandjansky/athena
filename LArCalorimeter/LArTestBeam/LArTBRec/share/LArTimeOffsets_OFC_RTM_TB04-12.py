#
# Timing Offset for OFC RTM at CTB2004
#
# Computed using OFC in /lar/LArElecCalibTB04/LArOFCPhys/RTM, tag TB04-12
#
# Missing periods: 7,  8-2 (25 ns)
#
# Last update: 15/12/2005 (Marco.Delmastro@cern.ch)
#

#LArTB04FolderTag_OFC_RTM = "TB04-12"

#import AthenaCommon.Logging.logging as logging
TimeOffsetsLog = logging.getLogger( "TimeOffsetsLog" )

TimeOffsetsLog.info( "Loading LAr RTM OFC Timing Offsets ..." )

theApp.TopAlg +=["FakeLArTimeOffset"]
FakeLArTimeOffset=Algorithm("FakeLArTimeOffset")
FakeLArTimeOffset.OutputLevel = INFO

#
# Period 1 - july 4th  to 12th - runs 1000055 to 1000460 - eta>0.8
#
if ( RunNumber >= 1000055 and RunNumber <= 1000460 ):
	TimeOffsetsLog.error( "==================================================" )
	TimeOffsetsLog.error( " *** There is no TDC information for this run *** " )
	TimeOffsetsLog.error( "     WARNING: you should NOT USE the LAr OFC!     " )
	TimeOffsetsLog.error( "              use LAr Parabola instead...         " )
	TimeOffsetsLog.error( "==================================================" )
	theApp.exit()
	
#	
# Period 2 - july 12th to 14th - runs 1000463 to 1000622 - 0.6<eta<1.2
#
if ( RunNumber >= 1000463 and RunNumber <= 1000622 ) :
	TimeOffsetsLog.error( "==================================================" )
	TimeOffsetsLog.error( " *** There is no TDC information for this run *** " )
	TimeOffsetsLog.error( "     WARNING: you should NOT USE the LAr OFC!     " )
	TimeOffsetsLog.error( "              use LAr Parabola instead...         " )
	TimeOffsetsLog.error( "==================================================" )
	theApp.exit()
	
#	
# Period 3 - july 26th to aug. 4th  - runs 1000623 to 1000797 - 0.6<eta<1.2
#
if ( RunNumber >= 1000623 and RunNumber <= 1000797 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 8.99629, 0, 0, 0, 10.3055, 21.3338, 16.9382, 0, 0, 21.7496, 0, 14.4824, 22.3707, 0, 7.6459, 0, 0, 0, 7.19743, 13.2964, 12.8002, 0, 0, 13.1924, 0, 0.633544, 14.6006, 0 ]

#
# Period 4 - aug. 4th  to 6th - runs 1000813 to 1000918 - eta<0.8
#
if ( RunNumber >= 1000813 and RunNumber <= 1000918 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 17.0738, 11.7326, 17.3288, 16.2998, 16.2484, 0, 0, 0, 18.6674, 0, 14.3392, 20.0127, 0, 0, 14.5702, 0, 12.9158, 10.1051, 0, 0, 0, 0, 12.0783, 0, 11.5289, 14.1051, 0, 0 ]

#	
# Period 5 - aug. 6th  to 9th - runs 1000919 to 1001056 - eta<0.8
#

#	
# Period 5-1
#
if ( RunNumber >= 1000919 and RunNumber <= 1000992 ) : 
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 12.5466, 7.54204, 12.7606, 12.3124, 0, 0, 0, 0, 15.06, 0, 8.50943, 15.9129, 0, 0, 11.7547, 7.58485, 9.73163, 7.7057, 0, 0, 0, 0, 9.24176, 0, 8.35578, 6.92025, 0, 0 ]

#	
# Period 5-2
#
if ( RunNumber >= 1000993 and RunNumber <= 1001024 ) : 
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 12.3673, 9.58359, 13.3502, 12.0841, 14.7058, 0, 0, 0, 14.7223, 0, 10.2922, 15.5664, 0, 0, 11.7706, 6.87566, 10.3017, 7.32381, 8.55533, 0, 0, 0, 9.10517, 0, 8.66712, 10.9894, 0, 0 ]

#	
# Period 5-3
#
if ( RunNumber >= 1001028 and RunNumber <= 1001056 ) : 
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 13.1829, 11.1593, 13.0061, 12.7552, 0, 0, 0, 0, 15.8154, 0, 8.89423, 16.6597, 0, 0, 12.4752, 9.44368, 9.91459, 8.18411, 0, 0, 0, 0, 9.68471, 0, 8.82161, 7.45051, 0, 0 ]

#
# Period 6a - aug. 23rd to sep. 1st - runs 1002007 to 1004234 - eta<0.8
#
if ( RunNumber >= 1002007 and RunNumber <= 1004234 ) : 
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 15.4554, 13.4202, 15.9593, 14.8999, 0, 0, 0, 0, 18.1364, 0, 13.1591, 17.6836, 0, 0, 14.259, 13.0813, 14.164, 9.88885, 0, 0, 0, 0, 14.3171, 0, 9.24348, 8.62648, 0, 0 ]


#
# Period 6b - aug. 23rd to sep.15th - runs 2100024 to 2100482 - eta<0.8
#

#
# Period 6b-1
#
if ( RunNumber >= 2100024 and RunNumber <= 2100110 ) :
	FakeLArTimeOffset.GlobalTimeOffset =   0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 6.15979, 12.3529, 4.95277, 4.0376, 0, 0, 0, 0, 7.08687, 0, 2.97156, 0, 0, 0, 4.26945, 17.7187, 4.18178, 9.22706, 0, 0, 0, 0, 0, 0, 14.9176, 4.63276, 0, 0 ]

#
# Period 6b-2
#	
if ( RunNumber >= 2100132 and RunNumber <= 2100253 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 5.42388, 11.6275, 5.29973, 4.44121, 0, 0, 0, 0, 6.94017, 0, 2.84483, 8.05193, 0, 0, 4.04627, 15.0061, 2.59872, 0, 0, 0, 0, 0, 2.25797, 0, 4.91218, 0, 0, 0 ]


#
# Period 6b-3
#	
if ( RunNumber >= 2100254 and RunNumber <= 2100510 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 8.88423, 17.9795, 10.4802, 10.4386, 12.4275, 0, 0, 0, 10.078, 0, 8.10476, 12.167, 0, 0, 7.85931, 7.10123, 8.38278, 5.37082, 6.25998, 0, 0, 0, 5.60678, 0, 9.74422, 12.8086, 0, 0 ]

	
#
# Period 6b-4
#	
if ( RunNumber >= 2100513 and RunNumber <= 2100540 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 8.98606, 17.6986, 10.6528, 11.1947, 11.4479, 0, 0, 0, 8.05384, 0, 8.2363, 12.3221, 0, 0, 7.89682, 7.09154, 7.43999, 6.85566, 5.49555, 0, 0, 0, 5.90347, 0, 9.57245, 13.2378, 0, 0 ]


#
# Period 7 - sep. 15th to 24th - runs 2100558 to 2100975 - eta>0.8
#
if ( RunNumber >= 2100558 and RunNumber <= 2100975 ) :
	TimeOffsetsLog.error( "=========================================================" )
	TimeOffsetsLog.error( " There are no LAr RTM OFC with tag "+LArTB04FolderTag_OFC_RTM+" for this run!" )
	TimeOffsetsLog.error( "=========================================================" )
	theApp.exit()

#
# Period 8 - sep. 24th to nov. 15th - runs 2101022 to 2102992 - eta<0.8
#

#
# Period 8-1 (Low energy Period 8 subperiod, with known timing problems)
# 
if ( RunNumber >= 2101022 and RunNumber <= 2101475 ) : 
	TimeOffsetsLog.warning( "====================================================" )
	TimeOffsetsLog.warning( " *** WARNING: this run has known LAr timing problems" )
	TimeOffsetsLog.warning( "====================================================" )
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 6.31955, 0, 5.1824, 7.23507, 6.75694, 0, 0, 0, 7.71951, 0, 0, 8.80756, 0, 0, 4.40908, 0, 8.63216, 9.55948, 8.00058, 0, 0, 0, 2.46427, 0, 0.204178, 0, 0, 0 ]

#
# Period 8-2 ("25 ns" subperiod)
#
if ( RunNumber >= 2101571 and RunNumber <= 2101737 ) :
	TimeOffsetsLog.error( "================================================" )
	TimeOffsetsLog.error( " *** WARNING: you are attempting to reconstruct " )
	TimeOffsetsLog.error( " ***          a 25-ns run using LAr RTM OFC...  " )
	TimeOffsetsLog.error( " ***          ---> Please contact a LAr expert  " )
	TimeOffsetsLog.error( "================================================" )
	theApp.exit()

#
# Period 8-3
#	
# Note: for all the rest of Period8 apparently the TDC calibration did not change.

if ( RunNumber >= 2102095 and RunNumber <= 2102992 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 15.0485, 10.3885, 14.963, 13.6596, 0, 0, 0, 0, 16.6076, 0, 12.4465, 17.1272, 0, 0, 13.8096, 8.74077, 13.9138, 9.35184, 0, 0, 0, 0, 11.5829, 0, 9.00505, 8.78157, 0, 0 ]
