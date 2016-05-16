#!/usr/bin/env python
import sys


#
#__________________________________________________________________
def dec2hex(n):
    return ("0%X" % n)[-2:]


#
#__________________________________________________________________
def pmt2channel_LB(pmtOrChannel):
    """ 
    This function converts channel to pmt or pmt to channel number.
    Valid for Barrel only!
    """

    pmtOrChannel  = int(pmtOrChannel)
    channelOrPmt = pmtOrChannel;

    # pmt==channel for pmt 0 - 23
    # pmt 24 - 47: reverse in groups of 3
    if pmtOrChannel>23:
	mod = pmtOrChannel%3
	if   mod==0: channelOrPmt = pmtOrChannel+2
	elif mod==2: channelOrPmt = pmtOrChannel-2
	else       : pass
    return channelOrPmt


#
#__________________________________________________________________
modToFrag = {'LBA' : '0x1',
	     'LBC' : '0x2',
	     'EBA' : '0x3',
	     'EBC' : '0x4'}

inFile = sys.argv[1]

tdlas = ""
tclas = ""

lines = open(inFile,"r").readlines()
for line in lines:
    field = line.split()
    if not len(field): continue
    module = field[0][:3]
    modNum = int(field[0][3:]) - 1
    drawerOffset = float(field[1])
    chanOffsets = field[2:]
    hexModule = modToFrag[module] + dec2hex(modNum)

    #=== some sanity checks
    sum=0.
    for chan in xrange(6):
        add  = float(chanOffsets[chan])
        sum += add
    print "%s ---> Found %i channels, sum of first 6 is %f" % ( field[0] , len(chanOffsets) , sum )
        
    
    #====================================================
    #=== fill tdlas (channel is always 0)
    #====================================================
    tdlas = tdlas+ "Tdlas\t%s\t0\t%s\n" % (hexModule,drawerOffset)
        
    #====================================================
    #=== fill tclas
    #====================================================
    for chan in xrange(48):
	offset = chanOffsets[chan]
	tclas = tclas+"Tclas\t%s\t%i\t%s\t%s\n" % (hexModule,chan,offset,offset)

tdlasFile = open("Tile.tdlas","w")
tdlasFile.write(tdlas)
tdlasFile.close()

tclasFile = open("Tile.tclas","w")
tclasFile.write(tclas)
tclasFile.close()


# print "---------------TDLAS--------------------------"
# print tdlas
# print "---------------TDLAS--------------------------"
# print "---------------TCLAS--------------------------"
# print tclas
# print "---------------TCLAS--------------------------"
