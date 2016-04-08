#
# Timing Offset for OFC RTM at CTB2004
#
# Computed using OFC in /lar/LArElecCalibTB04/LArOFCPhys/RTM, tag TB04-9
#
# Last update: 07/09/2005 (Marco.Delmastro@cern.ch)
#

#import AthenaCommon.Logging.logging as logging
TimeOffsetsLog = logging.getLogger( 'TimeOffsetsLog' )

TimeOffsetsLog.info( 'Loading LAr RTM OFC Timing Offsets ...' )

theApp.TopAlg +=["FakeLArTimeOffset"]
FakeLArTimeOffset=Algorithm("FakeLArTimeOffset")
FakeLArTimeOffset.OutputLevel = INFO

#
# Period 1 - july 4th  to 12th - runs 1000055 to 1000460 - eta>0.8
#
if ( RunNumber >= 1000055 and RunNumber <= 1000460 ):
	TimeOffsetsLog.error( '==================================================' )
	TimeOffsetsLog.error( ' *** There is no TDC information for this run *** ' )
	TimeOffsetsLog.error( '     WARNING: you should NOT USE the LAr OFC!     ' )
	TimeOffsetsLog.error( '              use LAr Parabola instead...         ' )
	TimeOffsetsLog.error( '==================================================' )

#	
# Period 2 - july 12th to 14th - runs 1000463 to 1000622 - 0.6<eta<1.2
#
if ( RunNumber >= 1000463 and RunNumber <= 1000622 ) :
	TimeOffsetsLog.error( '==================================================' )
	TimeOffsetsLog.error( ' *** There is no TDC information for this run *** ' )
	TimeOffsetsLog.error( '     WARNING: you should NOT USE the LAr OFC!     ' )
	TimeOffsetsLog.error( '              use LAr Parabola instead...         ' )
	TimeOffsetsLog.error( '==================================================' )

#	
# Period 3 - july 26th to aug. 4th  - runs 1000623 to 1000797 - 0.6<eta<1.2
#
if ( RunNumber >= 1000623 and RunNumber <= 1000797 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 19.4427, 0, 0, 0, 10.2147, 21.2868, 16.8594, 0, 0, 21.7697, 0, 14.6511, 22.4133, 0, 18.1232, 0, 0, 0, 6.81441, 13.3438, 12.5902, 0, 0, 13.2133, 0, 2.46791, 14.7413, 0 ]

#
# Period 4 - aug. 4th  to 6th - runs 1000813 to 1000918 - eta<0.8
#
if ( RunNumber >= 1000813 and RunNumber <= 1000918 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 23.4932, 0, 17.3312, 16.0127, 0, 0, 0, 0, 17.0232, 0, 14.4834, 19.7267, 0, 0, 23.3909, 0, 12.9645, 9.91301, 0, 0, 0, 0, 12.0417, 0, 11.5591, 12.6266, 0, 0 ]

#	
# Period 5 - aug. 6th  to 9th - runs 1000919 to 1001056 - eta<0.8
#

#	
# Period 5-1
#
if ( RunNumber >= 1000919 and RunNumber <= 1001056 ) : 
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 22.9869, 0, 12.5342, 12.2391, 16.1674, 0, 0, 0, 0, 0, 8.47013, 16.032, 0, 0, 22.2306, 0, 9.56283, 7.62281, 0, 0, 0, 0, 9.2557, 0, 8.49248, 6.84468, 0, 0 ]

#	
# Period 5-2
#
if ( RunNumber >= 1000993 and RunNumber <= 1001024 ) : 
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 22.376, 8.09512, 13.3656, 11.819, 14.8602, 0, 0, 0, 14.8316, 0, 10.4405, 15.5254, 0, 0, 22.0311, 0, 10.3485, 7.1548, 8.55533, 0, 0, 0, 9.04074, 0, 8.73463, 10.2558, 0, 0 ]

#	
# Period 5-3
#
if ( RunNumber >= 1001028 and RunNumber <= 1001056 ) : 
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 23.5603, 10.6813, 12.8007, 12.7022, 0, 0, 0, 0, 15.6225, 0, 8.91575, 16.6833, 0, 0, 22.9988, 9.14665, 9.70747, 8.06627, 0, 0, 0, 0, 9.69955, 0, 8.89786, 7.4618, 0, 0 ]

#
# Period 6a - aug. 23rd to sep. 1st - runs 1002007 to 1004234 - eta<0.8
#
if ( RunNumber >= 1002007 and RunNumber <= 1004234 ) : 
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 25.9361, 13.4142, 15.9979, 15.3053, 0, 0, 0, 0, 17.932, 0, 13.1987, 17.4238, 0, 0, 24.6033, 13.2451, 14.3499, 9.51308, 0, 0, 0, 0, 13.3091, 0, 9.46887, 8.96047, 0, 0 ]

#
# Period 6b - aug. 23rd to sep.15th - runs 2100024 to 2100482 - eta<0.8
#

#
# Period 6b-1
#
if ( RunNumber >= 2100024 and RunNumber <= 2100110 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 0, 6.90114, 9.77279, 0, 0, 0, 0, 0, 11.9382, 0, 8.21135, 0, 0, 0, 20.0183, 6.85694, 9.1029, 4.94817, 0, 0, 0, 0, 6.98346, 0, 5.02334, 4.63276, 0, 0 ]

#
# Period 6b-2
#	
if ( RunNumber >= 2100132 and RunNumber <= 2100253 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 26.4399, 11.6694, 15.4146, 14.8167, 0, 0, 0, 0, 17.154, 0, 13.2441, 18.4041, 0, 0, 24.8534, 9.62866, 12.6976, 10.2844, 0, 0, 0, 0, 12.7083, 0, 10.2313, 9.29522, 0, 0 ]

#
# Period 6b-3
#	
if ( RunNumber >= 2100254 and RunNumber <= 2100510 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 24.6674, 12.5752, 15.3694, 15.6419, 17.7339, 0, 0, 0, 15.2435, 0, 13.3441, 17.5065, 0, 0, 23.194, 12.1856, 13.4339, 10.4044, 10.9062, 0, 0, 0, 10.515, 0, 10.1634, 7.87344, 0, 0 ]
	
#
# Period 6b-4
#	
if ( RunNumber >= 2100513 and RunNumber <= 2100540 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 24.3439, 12.2535, 15.5004, 16.2448, 16.6779, 0, 0, 0, 13.2382, 0, 13.5527, 17.5423, 0, 0, 23.3187, 12.1365, 12.4644, 11.9723, 10.4353, 0, 0, 0, 11.0151, 0, 9.82882, 8.34833, 0, 0 ]

#
# Period 7 - sep. 15th to 24th - runs 2100558 to 2100975 - eta>0.8
#
if ( RunNumber >= 2100558 and RunNumber <= 2100975 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 21.4216, 0, 0, 0, 0, 19.4041, 18.3163, 18.9614, 0, 19.7443, 0, 0, 22.801, 18.1505, 19.4247, 0, 0, 0, 0, 16.7444, 15.5908, 14.6577, 0, 14.2851, 0, 0, 17.016, 20.8809 ]

#
# Period 8 - sep. 24th to nov. 15th - runs 2101022 to 2102992 - eta<0.8
#

#
# Period 8-1 (Low energy Period 8 subperiod, with known timing problems)
# 
if ( RunNumber >= 2101022 and RunNumber <= 2101475 ) : 
	TimeOffsetsLog.warning( '====================================================' )
	TimeOffsetsLog.warning( ' *** WARNING: this run has known LAr timing problems' )
	TimeOffsetsLog.warning( '====================================================' )
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 16.7433, 0, 5.01925, 7.11307, 6.913, 0, 0, 0, 7.75064, 0, 0, 8.86492, 0, 0, 14.7937, 0, 1.03717, 0, 3.04786, 0, 0, 0, 2.4098, 0, 0.343579, 0, 0, 0 ]

#
# Period 8-2 ("25 ns" subperiod)
#
if ( RunNumber >= 2101571 and RunNumber <= 2101737 ) :
	TimeOffsetsLog.warning( '================================================' )
	TimeOffsetsLog.warning( ' *** WARNING: you are attempting to reconstruct ' )
	TimeOffsetsLog.warning( ' ***          a 25-ns run using LAr RTM OFC...  ' )
	TimeOffsetsLog.warning( '================================================' )

#
# Period 8-3
#	
# Note: for all the rest of Period8 apparently the TDC calibration did not change.

if ( RunNumber >= 2102095 and RunNumber <= 2102992 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 0, 27.2448, 29.4475, 29.6399, 29.1952, 0, 0, 0, 29.6627, 0, 28.1106, 29.841, 0, 0, 0, 26.5562, 27.8388, 26.1255, 25.4838, 0, 0, 0, 26.4303, 0, 25.3316, 23.6475, 0, 0 ]
