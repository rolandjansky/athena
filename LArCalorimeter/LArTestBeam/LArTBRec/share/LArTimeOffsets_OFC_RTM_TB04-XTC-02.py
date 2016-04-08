####################################################################################
#
# Timing Offset for OFC RTM at CTB2004 - TAG = TB04-XTC-02
#
# Last update: 07/11/08 (Marco.Delmastro@cern.ch)
#
####################################################################################

#import AthenaCommon.Logging.logging as logging
TimeOffsetsLog = logging.getLogger( "TimeOffsetsLog" )

TimeOffsetsLog.info( "Loading LAr RTM OFC Timing Offsets ..." )

theApp.Dlls += ["LArEventTest"]
theApp.TopAlg +=["FakeLArTimeOffset"]
FakeLArTimeOffset=Algorithm("FakeLArTimeOffset")
FakeLArTimeOffset.OutputLevel = INFO

FakeLArTimeOffset.GlobalTimeOffset =  0.
FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]

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
	

#
# Period 4 - aug. 4th  to 6th - runs 1000813 to 1000918 - eta<0.8
#
if ( RunNumber >= 1000813 and RunNumber <= 1000918 ) :
	
	
#	
# Period 5 - aug. 6th  to 9th - runs 1000919 to 1001056 - eta<0.8
#

#	
# Period 5-1
#
if ( RunNumber >= 1000919 and RunNumber <= 1000992 ) : 
	

#	
# Period 5-2
#
if ( RunNumber >= 1000993 and RunNumber <= 1001024 ) : 
	
	
#	
# Period 5-3
#
if ( RunNumber >= 1001028 and RunNumber <= 1001056 ) : 
	
	
#
# Period 6a - aug. 23rd to sep. 1st - runs 1002007 to 1004234 - eta<0.8
#
if ( RunNumber >= 1002007 and RunNumber <= 1004234 ) : 
	

#
# Period 6b - aug. 23rd to sep.15th - runs 2100024 to 2100482 - eta<0.8
#

#
# Period 6b-1
#
if ( RunNumber >= 2100024 and RunNumber <= 2100110 ) :
	
#
# Period 6b-2
#	
if ( RunNumber >= 2100132 and RunNumber <= 2100253 ) :
	

#
# Period 6b-3
#	
if ( RunNumber >= 2100254 and RunNumber <= 2100510 ) :
	

#
# Period 6b-4
#	
if ( RunNumber >= 2100513 and RunNumber <= 2100540 ) :
	

#
# Period 7 - sep. 15th to 24th - runs 2100558 to 2100975 - eta>0.8
#
if ( RunNumber >=  2100558 and RunNumber <= 2100975 ) :
	
	if ( ( RunNumber >= 2100683 and RunNumber <= 2100750 ) or ( RunNumber >= 2100767 and RunNumber <= 2100975 ) ) :	
		TimeOffsetsLog.warning( "=========================================================" )
		TimeOffsetsLog.warning( " *** WARNING: There are known LAr reconstruction problems" )
		TimeOffsetsLog.warning( "              for this run ("+str(RunNumber)+") using RTM OFCs" )
		TimeOffsetsLog.warning( "=========================================================" )
		
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
	

####################################################################################
# End of LArTimeOffsets_OFC_RTM_TB04-XTC-02.py
####################################################################################
