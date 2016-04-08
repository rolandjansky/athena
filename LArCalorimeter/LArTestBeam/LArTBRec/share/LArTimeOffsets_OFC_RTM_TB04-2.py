#
# Timing Offset for OFC RTM at CTB2004
#
# Computed using OFC in /lar/LArElecCalibTB04/LArOFCPhys/RTM, tag TB04-2
#
# Last update: 26/08/2005 (Marco.Delmastro@cern.ch)
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
	FakeLArTimeOffset.GlobalTimeOffset = -0.4
	FakeLArTimeOffset.FEBids          += [0x39000000,0x39080000,0x39008000,0x39088000,0x39010000,0x39090000,0x39018000,0x39098000,0x39020000,0x390a0000,0x39050000,0x390d0000,0x390d8000,0x39058000,0x39040000,0x390c0000]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 6.8, 7.8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 6.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

#
# Period 4 - aug. 4th  to 6th - runs 1000813 to 1000918 - eta<0.8
#
if ( RunNumber >= 1000813 and RunNumber <= 1000918 ) :
	FakeLArTimeOffset.GlobalTimeOffset = -3.4
	FakeLArTimeOffset.FEBids          += [0x39000000,0x39080000,0x39008000,0x39088000,0x39010000,0x39090000,0x39018000,0x39098000,0x39020000,0x390a0000,0x39050000,0x390d0000,0x390d8000,0x39058000,0x39040000,0x390c0000]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 1.9,10.1, 0.0, 0.0,-4.5, 3.5, 0.1, 3.3, 0.0, 0.0,-0.6, 5.4, 0.4,-3.4, 8.0,13.3]

#	
# Period 5 - aug. 6th  to 9th - runs 1000919 to 1001056 - eta<0.8
#
if ( RunNumber >= 1000919 and RunNumber <= 1001056 ) : 
	FakeLArTimeOffset.GlobalTimeOffset =  1.1
	FakeLArTimeOffset.FEBids          += [0x39000000,0x39080000,0x39008000,0x39088000,0x39010000,0x39090000,0x39018000,0x39098000,0x39020000,0x390a0000,0x39050000,0x390d0000,0x390d8000,0x39058000,0x39040000,0x390c0000]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 2.5, 8.8, 0.0, 0.0,-4.3, 2.4, 0.5, 2.5, 0.0, 0.0,-0.3, 4.4,-0.4,-3.2, 7.9,12.6]

# Period 5 subperiod with FEBs exchange (to be checked)
if ( RunNumber >= 1000931 and RunNumber <= 1000969 ) : 
	FakeLArTimeOffset.GlobalTimeOffset =  2.0
	FakeLArTimeOffset.FEBids          += [0x39000000,0x39080000,0x39008000,0x39088000,0x39010000,0x39090000,0x39018000,0x39098000,0x39020000,0x390a0000,0x39050000,0x390d0000,0x390d8000,0x39058000,0x39040000,0x390c0000]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 2.0, 6.8, 0.0, 0.0,-3.9, 2.0,-0.5, 0.4, 0.0, 0.0,-0.6, 3.1, 2.0,-4.1, 0.0,11.5]

#
# Period 6 - aug. 23rd to sep. 15th - runs 1002007 to 1004234 & 2100024 to 2100482 - eta<0.8
#
if ( RunNumber >= 1002007 and RunNumber <= 1004234 ) : 
	FakeLArTimeOffset.GlobalTimeOffset = -1.3
	FakeLArTimeOffset.FEBids          += [0x39000000,0x39080000,0x39008000,0x39088000,0x39010000,0x39090000,0x39018000,0x39098000,0x39020000,0x390a0000,0x39050000,0x390d0000,0x390d8000,0x39058000,0x39040000,0x390c0000]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 3.2, 8.4,-0.8,-0.5,-3.3,-0.9, 0.0, 0.0, 0.0, 0.0,-1.0, 3.9, 0.0, 0.0, 5.4, 4.5]

if ( RunNumber >= 2100024 and RunNumber <= 2100132 ) : # computed with a single run (2100024)
	FakeLArTimeOffset.GlobalTimeOffset = -11.4
	FakeLArTimeOffset.FEBids          += [0x39000000,0x39080000,0x39008000,0x39088000,0x39010000,0x39090000,0x39018000,0x39098000,0x39020000,0x390a0000,0x39050000,0x390d0000,0x390d8000,0x39058000,0x39040000,0x390c0000]
	FakeLArTimeOffset.FEbTimeOffsets  += [-2.4, 3.0, 0.0, 0.0,-4.1, 7.0,-1.6, 4.8, 0.0, 0.0,-8.9,-2.8,-5.9,-10.4,-1.4,-1.6]

if ( RunNumber >= 2100133 and RunNumber <= 2100482 ) : # computed with a single run (2100133)
	FakeLArTimeOffset.GlobalTimeOffset = 12.8
	FakeLArTimeOffset.FEBids          += [0x39000000,0x39080000,0x39008000,0x39088000,0x39010000,0x39090000,0x39018000,0x39098000,0x39020000,0x390a0000,0x39050000,0x390d0000,0x390d8000,0x39058000,0x39040000,0x390c0000]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 0.0, 5.8,-2.7, 0.6,-4.6,-0.8, 0.0, 0.0, 0.0, 0.0,-3.8, 0.7, 0.0, 0.0, 0.0, 7.8]

#
# Period 7 - sep. 15th to 24th - runs 2100558 to 2100975 - eta>0.8
#
if ( RunNumber >= 2100558 and RunNumber <= 2101021 ) : # computed with a single run (2100558)
	FakeLArTimeOffset.GlobalTimeOffset =  6.3
	FakeLArTimeOffset.FEBids          += [0x39000000,0x39080000,0x39008000,0x39088000,0x39010000,0x39090000,0x39018000,0x39098000,0x39020000,0x390a0000,0x39050000,0x390d0000,0x390d8000,0x39058000,0x39040000,0x390c0000]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 5.5, 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

#
# Period 8 - sep. 24th to nov. 15th - runs 2101022 to 2102992 - eta<0.8
#

#
# Low energy Period 8 subperiod, with known timing problems!
# 
if ( RunNumber >= 2101022 and RunNumber <= 2101475 ) : # computed with a single run (2101238)
	TimeOffsetsLog.warning( '====================================================' )
	TimeOffsetsLog.warning( ' *** WARNING: this run has known LAr timing problems' )
	TimeOffsetsLog.warning( '====================================================' )
	FakeLArTimeOffset.GlobalTimeOffset = 17.6
	FakeLArTimeOffset.FEBids          += [0x39000000,0x39080000,0x39008000,0x39088000,0x39010000,0x39090000,0x39018000,0x39098000,0x39020000,0x390a0000,0x39050000,0x390d0000,0x390d8000,0x39058000,0x39040000,0x390c0000]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 2.1, 9.4, 0.0, 0.0, 0.0, 0.0,-2.2, 2.3,-1.0, 3.4, 0.0, 0.0, 4.9,-2.8, 3.3, 6.0]
	
#
# "25 ns" Period 8 subperiod
#
if ( RunNumber >= 2101571 and RunNumber <= 2101737 ) : # computed with a single run (2101571)
	TimeOffsetsLog.warning( '================================================' )
	TimeOffsetsLog.warning( ' *** WARNING: you are attempting to reconstruct ' )
	TimeOffsetsLog.warning( ' ***          a 25-ns run using LAr RTM OFC...  ' )
	TimeOffsetsLog.warning( '================================================' )
	FakeLArTimeOffset.GlobalTimeOffset = -7.2
	FakeLArTimeOffset.FEBids          += [0x39000000,0x39080000,0x39008000,0x39088000,0x39010000,0x39090000,0x39018000,0x39098000,0x39020000,0x390a0000,0x39050000,0x390d0000,0x390d8000,0x39058000,0x39040000,0x390c0000]
	FakeLArTimeOffset.FEbTimeOffsets  += [-5.9,-1.3,-6.6,-4.0,-11.3,-6.7,-8.8,-7.4,-11.0,-6.1,-9.3,-3.7,-5.6,-11.5,-2.3, 2.1]

	
# for all the rest of Period8 apparently the TDC calibration did not changed.
# If the LAr timing stayed the same (did it?) a single set of timeng offsets should work... 

if ( RunNumber >= 2002095 ) :
	FakeLArTimeOffset.GlobalTimeOffset = -5.2
	FakeLArTimeOffset.FEBids          += [0x39000000,0x39080000,0x39008000,0x39088000,0x39010000,0x39090000,0x39018000,0x39098000,0x39020000,0x390a0000,0x39050000,0x390d0000,0x390d8000,0x39058000,0x39040000,0x390c0000]
	FakeLArTimeOffset.FEbTimeOffsets  += [ 1.6, 7.7,-1.3,-0.5,-4.5,-1.4,-2.5, 0.4,-3.8, 2.3,-1.5, 3.0, 2.6,-3.4, 5.7,10.4]
