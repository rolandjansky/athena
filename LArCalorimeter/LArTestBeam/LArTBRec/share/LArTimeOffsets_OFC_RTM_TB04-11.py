#
# Timing Offset for OFC RTM at CTB2004
#
# Computed using OFC in /lar/LArElecCalibTB04/LArOFCPhys/RTM, tag TB04-11
#
# Missing periods: 6b, 7,  8-2 (25 ns)
#
# Last update: 28/11/2005 (Marco.Delmastro@cern.ch)
#

#LArTB04FolderTag_OFC_RTM = "TB04-11"

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
	FakeLArTimeOffset.FEbTimeOffsets  += [ 20.392, 0, 0, 0, 10.9085, 22.3675, 17.5928, 0, 0, 22.12, 0, 14.9584, 23.2108, 0, 18.9465, 0, 0, 0, 7.79481, 14.2085, 13.5365, 0, 0, 13.5697, 0, 1.23259, 15.4494, 0 ]

#
# Period 4 - aug. 4th  to 6th - runs 1000813 to 1000918 - eta<0.8
#
if ( RunNumber >= 1000813 and RunNumber <= 1000918 ) :
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 28.6552, 13.4678, 18.7311, 17.2435, 16.7116, 0, 0, 0, 19.2121, 0, 15.6011, 20.8719, 0, 0, 26.1681, 0, 14.3194, 10.9471, 0, 0, 0, 0, 12.5805, 0, 12.8502, 15.4983, 0, 0 ]

#	
# Period 5 - aug. 6th  to 9th - runs 1000919 to 1001056 - eta<0.8
#

#	
# Period 5-1
#
if ( RunNumber >= 1000919 and RunNumber <= 1001056 ) : 
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 23.8034, 8.38618, 14.1214, 13.2987, 0, 0, 0, 0, 15.4504, 0, 9.9187, 16.81, 0, 0, 23.4156, 8.08891, 11.1009, 8.78125, 0, 0, 0, 0, 9.65008, 0, 9.65014, 7.85293, 0, 0 ]

#	
# Period 5-2
#
if ( RunNumber >= 1000993 and RunNumber <= 1001024 ) : 
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 23.7926, 10.4319, 14.8677, 13.0287, 15.9449, 0, 0, 0, 15.241, 0, 11.6289, 16.5077, 0, 0, 23.2989, 8.16476, 11.8144, 8.20559, 9.18529, 0, 0, 0, 9.56531, 0, 10.0636, 12.5121, 0, 0 ]

#	
# Period 5-3
#
if ( RunNumber >= 1001028 and RunNumber <= 1001056 ) : 
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 24.4631, 11.8878, 14.2471, 13.7446, 0, 0, 0, 0, 16.191, 0, 10.1576, 17.6169, 0, 0, 24, 10.0835, 11.165, 9.19526, 0, 0, 0, 0, 10.027, 0, 10.0021, 8.50511, 0, 0 ]

#
# Period 6a - aug. 23rd to sep. 1st - runs 1002007 to 1004234 - eta<0.8
#
if ( RunNumber >= 1002007 and RunNumber <= 1004234 ) : 
	FakeLArTimeOffset.GlobalTimeOffset =  0.
	FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 26.958, 15.0801, 17.5979, 15.925, 0, 0, 0, 0, 18.6033, 0, 14.8044, 18.3033, 0, 0, 26.1635, 14.7238, 15.6399, 10.9162, 0, 0, 0, 0, 15.8183, 0, 10.8217, 9.77186, 0, 0 ]

#
# Period 6b - aug. 23rd to sep.15th - runs 2100024 to 2100482 - eta<0.8
#

#
# Period 6b-1
#
if ( RunNumber >= 2100024 and RunNumber <= 2100110 ) :
	TimeOffsetsLog.error( "=========================================================" )
	TimeOffsetsLog.error( " There are no LAr RTM OFC with tag "+LArTB04FolderTag_OFC_RTM+" for this run!" )
	TimeOffsetsLog.error( "=========================================================" )
	theApp.exit()
#
# Period 6b-2
#	
if ( RunNumber >= 2100132 and RunNumber <= 2100253 ) :
	TimeOffsetsLog.error( "=========================================================" )
	TimeOffsetsLog.error( " There are no LAr RTM OFC with tag "+LArTB04FolderTag_OFC_RTM+" for this run!" )
	TimeOffsetsLog.error( "=========================================================" )
	theApp.exit()

#
# Period 6b-3
#	
if ( RunNumber >= 2100254 and RunNumber <= 2100510 ) :
	TimeOffsetsLog.error( "=========================================================" )
	TimeOffsetsLog.error( " There are no LAr RTM OFC with tag "+LArTB04FolderTag_OFC_RTM+" for this run!" )
	TimeOffsetsLog.error( "=========================================================" )
	theApp.exit()
	
#
# Period 6b-4
#	
if ( RunNumber >= 2100513 and RunNumber <= 2100540 ) :
	TimeOffsetsLog.error( "=========================================================" )
	TimeOffsetsLog.error( " There are no LAr RTM OFC with tag "+LArTB04FolderTag_OFC_RTM+" for this run!" )
	TimeOffsetsLog.error( "=========================================================" )
	theApp.exit()

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
	FakeLArTimeOffset.FEbTimeOffsets  += [ 7.29871, 0, 5.72954, 8.49265, 7.68776, 0, 0, 0, 8.23094, 0, 0, 10.0111, 0, 0, 5.63829, 0, 9.46414, 10.7751, 8.94151, 0, 0, 0, 2.98527, 0, 1.07913, 0, 0, 0 ]

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
	FakeLArTimeOffset.FEbTimeOffsets  += [ 26.5789, 11.2139, 16.6311, 14.567, 0, 0, 0, 0, 17.0088, 0, 13.6337, 17.935, 0, 0, 25.7079, 9.57543, 15.1865, 10.1253, 0, 0, 0, 0, 11.99, 0, 10.1301, 9.58999, 0, 0 ]
