#
# Timing Offset for OFC RTM at CTB2004
#
# Computed using OFC in /lar/LArElecCalibTB04/LArOFCPhys/RTM, tag TB04-01
# should work fine with OFC RTM tags = TB04-01, TB04-02, TB04-03, TB04-04
#
# Last update: 28/7/2005 (Marco.Delmastro@cern.ch)
#

theApp.TopAlg +=["FakeLArTimeOffset"]
FakeLArTimeOffset=Algorithm("FakeLArTimeOffset")
FakeLArTimeOffset.OutputLevel = INFO
FakeLArTimeOffset.FEBids += [0x39000000,0x39080000,0x39008000,0x39088000,0x39010000,0x39090000,0x39018000,0x39098000,0x39020000,0x390a0000,0x39050000,0x390d0000,0x390d8000,0x39058000,0x39040000,0x390c0000]


# Periode 1 - july 4th  to 12th      - runs 1000055 to 1000460 - eta>0.8
if ( RunNumber >= 1000055 and RunNumber <= 1000460 ):
	print "=================================================="
	print " *** There is no TDC information for this run *** "
	print "     WARNING: you should NOT USE the LAr OFC!     "
	print "=================================================="
	
# Periode 2 - july 12th to 14th      - runs 1000463 to 1000622 - 0.6< eta <1.2
if ( RunNumber >= 1000463 and RunNumber <= 1000622 ) :
	# timing offsets still missing :-(
	print "========================================================="
	print " *** RTM OFC timing offsets are missing for this run *** "
	print "   WARNING: LAr OFC reconstruction may not be reliable!  "
	print "========================================================="
	
# Periode 3 - july 26th to aug. 4th  - runs 1000623 to 1000797 - 0.6< eta <1.2
if ( RunNumber >= 1000623 and RunNumber <= 1000797 ) :
        # timing offsets still missing :-(
	print "========================================================="
	print " *** RTM OFC timing offsets are missing for this run *** "
	print "   WARNING: LAr OFC reconstruction may not be reliable!  "
	print "========================================================="

# Periode 4 - aug. 4th  to 6th       - runs 1000813 to 1000918 - eta<0.8
if ( RunNumber >= 1000813 and RunNumber <= 1000918 ) :
        # timing offsets still missing :-(
	print "========================================================="
	print " *** RTM OFC timing offsets are missing for this run *** "
	print "   WARNING: LAr OFC reconstruction may not be reliable!  "
	print "========================================================="
	
# Periode 5 - aug. 6th  to 9th       - runs 1000919 to 1001056 - eta<0.8
if ( RunNumber >= 1000919 and RunNumber <= 1001056 ) : # tested: ok
	FakeLArTimeOffset.GlobalTimeOffset =  5.6
	FakeLArTimeOffset.FEbTimeOffsets  += [-3.9, 3.1, 0.0, 0.0,-4.5, 1.7,-1.3, 0.4, 0.0, 0.0,-0.0, 5.2, 2.2,-5.2, 2.3, 7.6]

# Periode 6 - aug. 23rd to sep. 15th - runs 1002007 to 1004234 & 2100024 to 2100482 - eta<0.8
if ( RunNumber >= 1002007 and RunNumber <= 1004234 ) : # tested: ok
	FakeLArTimeOffset.GlobalTimeOffset =  1.9
	FakeLArTimeOffset.FEbTimeOffsets  += [-1.4, 5.2,-1.0,-0.8,-3.6,-0.8,-0.4, 7.8, 0.0, 0.0,-0.5, 4.9, 0.0, 0.0, 1.2, 1.9]

if ( RunNumber >= 2100024 and RunNumber <= 2100132 ) : # computed with a single run (2100024)
        FakeLArTimeOffset.GlobalTimeOffset = -11.4
	FakeLArTimeOffset.FEbTimeOffsets  += [-3.2, 3.3,10.2, 8.2,-1.9, 5.4, 1.8, 4.8, 0.0, 0.0,-6.9, 0.1,-2.5,-7.2,-2.7,-2.0]

if ( RunNumber >= 2100133 and RunNumber <= 2100482 ) : # computed with a single run (2100133)
	FakeLArTimeOffset.GlobalTimeOffset = 16.5
	FakeLArTimeOffset.FEbTimeOffsets  += [-5.6, 2.2,-3.2, 0.3,-5.0,-1.1, 0.0, 0.0, 0.0, 0.0,-3.6, 0.9, 0.0, 0.0,-1.0, 4.0]

# Periode 7 - sep. 15th to 24th      - runs 2100558 to 2100975 - eta>0.8
if ( RunNumber >= 2100558 and RunNumber <= 2101021 ) : # computed with a single run (2100558)
	FakeLArTimeOffset.GlobalTimeOffset = 12.5
	FakeLArTimeOffset.FEbTimeOffsets  += [ 0.7, 2.4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

# Periode 8 - sep. 24th to nov. 15th - runs 2101022 to 2102992 - eta<0.8
if ( RunNumber >= 2101022 and RunNumber <= 2101570 ) : # to be recomputed!!!
	FakeLArTimeOffset.GlobalTimeOffset = 0.
	FakeLArTimeOffset.FEbTimeOffsets  += [29.7,-25.9,10.9,-11.6, 9.5,119.9, 0.0, 0.0, 0.0, 0.0,11.1,-11.3, 0.0, 0.0, 0.0, 0.0]

if ( RunNumber >= 2101571 and RunNumber <= 2102466 ) : # computed with a single run (2101571)
	FakeLArTimeOffset.GlobalTimeOffset = -4.0
	FakeLArTimeOffset.FEbTimeOffsets  += [-3.0, 2.7, 0.2, 2.8,-4.4, 0.2,-2.0,-0.3,-4.1, 1.9,-2.2, 4.9, 2.2,-4.6, 0.6, 6.2]

if ( RunNumber >= 2102467 and RunNumber <= 2102992 ) : # tested: ok
	FakeLArTimeOffset.GlobalTimeOffset = -1.8
	FakeLArTimeOffset.FEbTimeOffsets  += [-3.0, 4.0,-1.4,-0.3,-4.4,-1.6,-2.3, 0.1,-4.6, 1.9,-1.2, 4.1, 3.1,-3.3, 1.1, 6.6]
