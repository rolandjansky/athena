####################################################################################
#
# Timing Offset for OFC RTM at CTB2004 - TAG = TB04-XTC-03
#
# Last update: 01/12/07 (Marco.Delmastro@cern.ch)
#
####################################################################################

#import AthenaCommon.Logging.logging as logging
TimeOffsetsLog = logging.getLogger( "TimeOffsetsLog" )

TimeOffsetsLog.info( "Loading LAr RTM OFC Timing Offsets ..." )

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
	FakeLArTimeOffset.FEbTimeOffsets += [ 9.12539, 0, 0, 0, 7.91883, 19.6264, 15.5514, 0, 0, 21.7924, 0, 14.6025, 22.9186, 0, 7.66504, 0, 0, 0, 3.53, 10.3821, 10.0719, 0, 0, 13.2382, 0, 0.876778, 14.6485, 0 ]

#
# Period 4 - aug. 4th  to 6th - runs 1000813 to 1000918 - eta<0.8
#
if ( RunNumber >= 1000813 and RunNumber <= 1000918 ) :
	FakeLArTimeOffset.FEbTimeOffsets += [ 17.2838, 9.94863, 15.5254, 13.4016, 14.3601, 0, 0, 0, 18.6863, 0, 14.3852, 20.0132, 0, 0, 14.5789, 0, 9.57873, 6.37547, 0, 0, 0, 0, 12.1587, 0, 11.4927, 14.1324, 0, 0 ]
	
#	
# Period 5 - aug. 6th  to 9th - runs 1000919 to 1001056 - eta<0.8
#

#	
# Period 5-1
#
if ( RunNumber >= 1000919 and RunNumber <= 1000992 ) : 
	FakeLArTimeOffset.FEbTimeOffsets += [ 12.6164, 5.5948, 10.4943, 9.22017, 0, 0, 0, 0, 15.4842, 0, 8.56935, 15.9092, 0, 0, 11.7587, 3.30198, 6.55061, 3.73609, 0, 0, 0, 0, 9.23426, 0, 10.0991, 6.90674, 0, 0 ]

#	
# Period 5-2
#
if ( RunNumber >= 1000993 and RunNumber <= 1001024 ) : 
	FakeLArTimeOffset.FEbTimeOffsets += [ 12.4709, 7.17179, 11.4236, 9.07052, 12.137, 0, 0, 0, 14.7753, 0, 10.3256, 15.7069, 0, 0, 11.6949, 2.5867, 6.82242, 3.48893, 5.01379, 0, 0, 0, 9.14836, 0, 8.61878, 11.0192, 0, 0 ]
	
#	
# Period 5-3
#
if ( RunNumber >= 1001028 and RunNumber <= 1001056 ) : 
	FakeLArTimeOffset.FEbTimeOffsets += [ 13.2732, 8.80068, 10.7307, 9.59033, 0, 0, 0, 0, 15.8411, 0, 8.95331, 16.6077, 0, 0, 12.4708, 5.89879, 6.72615, 4.16328, 0, 0, 0, 0, 9.71193, 0, 10.3524, 7.48013, 0, 0 ]
	
#
# Period 6a - aug. 23rd to sep. 1st - runs 1002007 to 1004234 - eta<0.8
#
if ( RunNumber >= 1002007 and RunNumber <= 1004234 ) : 
	FakeLArTimeOffset.FEbTimeOffsets += [ 12.6164, 5.5948, 10.4943, 9.22017, 0, 0, 0, 0, 15.4842, 0, 8.56935, 15.9092, 0, 0, 11.7587, 3.30198, 6.55061, 3.73609, 0, 0, 0, 0, 9.23426, 0, 10.0991, 6.90674, 0, 0 ]

#
# Period 6b - aug. 23rd to sep.15th - runs 2100024 to 2100482 - eta<0.8
#

#
# Period 6b-1
#
if ( RunNumber >= 2100024 and RunNumber <= 2100110 ) :
	FakeLArTimeOffset.FEbTimeOffsets += [ 12.4709, 7.17179, 11.4236, 9.07052, 12.137, 0, 0, 0, 14.7753, 0, 10.3256, 15.7069, 0, 0, 11.6949, 2.5867, 6.82242, 3.48893, 5.01379, 0, 0, 0, 9.14836, 0, 8.61878, 11.0192, 0, 0 ]
#
# Period 6b-2
#	
if ( RunNumber >= 2100132 and RunNumber <= 2100253 ) :
	FakeLArTimeOffset.FEbTimeOffsets += [ 13.2732, 8.80068, 10.7307, 9.59033, 0, 0, 0, 0, 15.8411, 0, 8.95331, 16.6077, 0, 0, 12.4708, 5.89879, 6.72615, 4.16328, 0, 0, 0, 0, 9.71193, 0, 10.3524, 7.48013, 0, 0 ]

#
# Period 6b-3
#	
if ( RunNumber >= 2100254 and RunNumber <= 2100510 ) :
	FakeLArTimeOffset.FEbTimeOffsets += [ 12.6164, 5.5948, 10.4943, 9.22017, 0, 0, 0, 0, 15.4842, 0, 8.56935, 15.9092, 0, 0, 11.7587, 3.30198, 6.55061, 3.73609, 0, 0, 0, 0, 9.23426, 0, 10.0991, 6.90674, 0, 0 ]

#
# Period 6b-4
#	
if ( RunNumber >= 2100513 and RunNumber <= 2100540 ) :
	FakeLArTimeOffset.FEbTimeOffsets += [ 12.4709, 7.17179, 11.4236, 9.07052, 12.137, 0, 0, 0, 14.7753, 0, 10.3256, 15.7069, 0, 0, 11.6949, 2.5867, 6.82242, 3.48893, 5.01379, 0, 0, 0, 9.14836, 0, 8.61878, 11.0192, 0, 0 ]

#
# Period 7 - sep. 15th to 24th - runs 2100558 to 2100975 - eta>0.8
#
if ( RunNumber >=  2100558 and RunNumber <= 2100975 ) :
	FakeLArTimeOffset.FEbTimeOffsets += [ 13.2732, 8.80068, 10.7307, 9.59033, 0, 0, 0, 0, 15.8411, 0, 8.95331, 16.6077, 0, 0, 12.4708, 5.89879, 6.72615, 4.16328, 0, 0, 0, 0, 9.71193, 0, 10.3524, 7.48013, 0, 0 ]
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
	FakeLArTimeOffset.FEbTimeOffsets += [ 6.45159, 0, 2.9785, 4.83405, 4.43354, 0, 0, 0, 7.7403, 0, 0, 8.83592, 0, 0, 4.45855, 0, 5.11078, 6.33593, 4.6212, 0, 0, 0, 2.45235, 0, 1.73488, 0, 0, 0 ]

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
	FakeLArTimeOffset.FEbTimeOffsets += [ 14.5621, 9.37032, 12.4158, 11.5953, 13.547, 0, 0, 0, 15.0243, 0, 12.5471, 16.651, 0, 0, 13.3241, 7.68261, 9.8251, 6.88415, 6.15711, 0, 0, 0, 10.9306, 0, 9.49019, 8.05319, 0, 0 ]

####################################################################################
# End of LArTimeOffsets_OFC_RTM_TB04-XTC-03.py
####################################################################################
