####################################################################################
#
# Timing Offset for OFC RTM at CTB2004 - TAG = TB04-XTC-01
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
	FakeLArTimeOffset.FEbTimeOffsets += [ 9.12457, 0, 0, 0, 6.4031, 17.937, 13.4905, 0, 0, 21.7917, 0, 14.4521, 22.9186, 0, 7.66525, 0, 0, 0, 3.53704, 10.3822, 10.0713, 0, 0, 13.2385, 0, 0.856282, 14.6482, 0 ]

#
# Period 4 - aug. 4th  to 6th - runs 1000813 to 1000918 - eta<0.8
#
if ( RunNumber >= 1000813 and RunNumber <= 1000918 ) :
	FakeLArTimeOffset.FEbTimeOffsets += [ 17.208, 8.9668, 13.5626, 12.4822, 13.5481, 0, 0, 0, 18.6778, 0, 14.3852, 20.0047, 0, 0, 14.5839, 0, 9.57931, 6.39737, 0, 0, 0, 0, 12.1581, 0, 11.4929, 14.1324, 0, 0 ]
	
#	
# Period 5 - aug. 6th  to 9th - runs 1000919 to 1001056 - eta<0.8
#

#	
# Period 5-1
#
if ( RunNumber >= 1000919 and RunNumber <= 1000992 ) : 
	FakeLArTimeOffset.FEbTimeOffsets += [ 12.6125, 5.01215, 8.91651, 8.35728, 0, 0, 0, 0, 15.4795, 0, 8.56977, 15.9266, 0, 0, 11.7587, 3.30198, 6.55056, 3.73762, 0, 0, 0, 0, 9.23579, 0, 10.0965, 6.90427, 0, 0 ]

#	
# Period 5-2
#
if ( RunNumber >= 1000993 and RunNumber <= 1001024 ) : 
	FakeLArTimeOffset.FEbTimeOffsets += [ 12.4709, 6.0713, 9.38452, 8.14928, 10.9463, 0, 0, 0, 14.7753, 0, 10.326, 15.7085, 0, 0, 11.6949, 2.5867, 6.82234, 3.48904, 4.94293, 0, 0, 0, 9.15069, 0, 8.61919, 11.019, 0, 0 ]
	
#	
# Period 5-3
#
if ( RunNumber >= 1001028 and RunNumber <= 1001056 ) : 
	FakeLArTimeOffset.FEbTimeOffsets += [ 13.2707, 8.15598, 9.10758, 8.70479, 0, 0, 0, 0, 15.8411, 0, 8.95327, 16.6244, 0, 0, 12.4724, 5.89879, 6.72635, 4.16617, 0, 0, 0, 0, 9.71259, 0, 10.3519, 7.48193, 0, 0 ]
	
#
# Period 6a - aug. 23rd to sep. 1st - runs 1002007 to 1004234 - eta<0.8
#
if ( RunNumber >= 1002007 and RunNumber <= 1004234 ) : 
	FakeLArTimeOffset.FEbTimeOffsets += [ 15.5743, 10.1416, 12.13, 10.9995, 0, 0, 0, 0, 18.183, 0, 13.2211, 17.519, 0, 0, 14.2902, 9.45329, 10.5642, 6.19042, 0, 0, 0, 0, 14.3618, 0, 9.30381, 8.87977, 0, 0 ]

#
# Period 6b - aug. 23rd to sep.15th - runs 2100024 to 2100482 - eta<0.8
#

#
# Period 6b-1
#
if ( RunNumber >= 2100024 and RunNumber <= 2100110 ) :
	FakeLArTimeOffset.FEbTimeOffsets += [ 6.37493, 8.9486, 0, -0.719012, 0, 0, 0, 0, 7.14147, 0, 3.01042, 0, 0, 0, 4.3145, 14.3674, 0, 5.56709, 0, 0, 0, 0, 0, 0, 14.9534, 4.86584, 0, 0 ]
#
# Period 6b-2
#	
if ( RunNumber >= 2100132 and RunNumber <= 2100253 ) :
	FakeLArTimeOffset.FEbTimeOffsets += [ 5.59378, 8.36946, 2.19354, 0, 0, 0, 0, 0, 7.01639, 0, 2.84425, 8.11718, 0, 0, 4.10071, 11.5669, 0, -3.98934, 0, 0, 0, 0, 2.29326, 0, 4.95207, 0, 0, 0 ]

#
# Period 6b-3
#	
if ( RunNumber >= 2100254 and RunNumber <= 2100510 ) :
	FakeLArTimeOffset.FEbTimeOffsets += [ 9.00597, 14.3744, 6.80893, 6.77274, 8.33551, 0, 0, 0, 10.0902, 0, 8.14654, 12.2106, 0, 0, 7.85525, 3.32489, 4.95314, 2.08016, 2.75046, 0, 0, 0, 5.57, 0, 9.77459, 12.8408, 0, 0 ]

#
# Period 6b-4
#	
if ( RunNumber >= 2100513 and RunNumber <= 2100540 ) :
	FakeLArTimeOffset.FEbTimeOffsets += [ 9.20266, 14.6736, 7.03955, 7.16821, 7.84435, 0, 0, 0, 8.14448, 0, 8.27385, 12.4297, 0, 0, 7.94724, 3.7521, 3.663, 2.66425, 2.31169, 0, 0, 0, 5.94724, 0, 9.58, 13.2758, 0, 0 ]

#
# Period 7 - sep. 15th to 24th - runs 2100558 to 2100975 - eta>0.8
#
if ( RunNumber >=  2100558 and RunNumber <= 2100975 ) :
	FakeLArTimeOffset.FEbTimeOffsets += [ 12.0864, 0, 0, 0, 0, 5.07748, 5.99226, 5.13119, 0, 10.2989, 0, 0, 17.8728, 8.86842, 9.71111, 0, 0, 0, 0, 2.89255, 2.88127, 1.85509, 0, 5.15832, 0, 0, 6.9436, 0 ]
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
	FakeLArTimeOffset.FEbTimeOffsets += [ 6.45155, 0, 1.64139, 3.69562, 3.31206, 0, 0, 0, 7.73846, 0, 0, 8.8356, 0, 0, 4.45888, 0, 5.11078, 6.33597, 4.61353, 0, 0, 0, 2.44879, 0, 1.73488, 0, 0, 0 ]

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
	FakeLArTimeOffset.FEbTimeOffsets += [ 14.5649, 8.38324, 10.7725, 10.5281, 11.9622, 0, 0, 0, 15.0243, 0, 12.5471, 16.6507, 0, 0, 13.3245, 7.68256, 9.8261, 6.93204, 6.21907, 0, 0, 0, 10.9302, 0, 9.49009, 8.05296, 0, 0 ]

####################################################################################
# End of LArTimeOffsets_OFC_RTM_TB04-XTC-01.py
####################################################################################
