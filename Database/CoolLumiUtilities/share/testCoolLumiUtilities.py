#!/usr/bin/env python
#
# testCoolLumiUtilities.py
#
# Eric Torrence - 25 June 2014
#
# Simple script to test the python interface to the CoolLumiUtilities objects
#

# Global DB handler object to read COOL
from CoolLumiUtilities.CoolDataReader import CoolDataReader

# Raw objects to handle COOL data
from ROOT import *
##### CoolLumiUtilsTestObj
print 'Testing CoolLumiUtilsTestObj'
from ROOT import CoolLumiUtilsTestObj
import random
from random import randint

# Simple test of reading and writing to object
to = CoolLumiUtilsTestObj()

to.setValue(2.)
print to.getValue(), '-> should be 2.0'

to.append(1.)
to.append(2.)
v = to.getList()
print v[0], '-> should be 1.0'
print v[1], '-> should be 2.0'

##### FillParamsUtil
print 'Testing FillParamsUtil'
from ROOT import FillParamsUtil 

# Find start/end time of run
lblbReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBLB')
lblbReader.setIOVRangeFromRun(215589)
lblbReader.readData()

iovSince = lblbReader.data[0].payload()['StartTime']
iovUntil = lblbReader.data[-1].payload()['EndTime']

#print iovSince, '-> starting time'
print (iovUntil-iovSince)/(10**9*3600), '-> end time'
print "years after 1970", (iovUntil)/(10**9*3600*24*365)

count = 0

fillParamsReader = CoolDataReader("COOLONL_TDAQ/COMP200", "/TDAQ/OLC/LHC/FILLPARAMS")
fillParamsReader.setIOVRange(iovSince, iovUntil)
fillParamsReader.readData()
fpu = FillParamsUtil()
# Iterate over all data objects found
for obj in fillParamsReader.data:
#    print obj
    print (obj.until()-obj.since())/10**9
    fpu.setValue(obj.payload())
    v = fpu.luminousBunches()
    print v
    if (fpu.nBeam1Bunches()>2):
        print "working"
#   print "Start time " 
print iovSince

#    print (obj.until()-obj.since())/(10**9*60)
#    print "HERE ARE LUMINOUS BUNCHES  ", fpu.nLuminousBunches()
#    print "NB1  ", fpu.nBeam1Bunches()
#    print "NB2  ", fpu.nBeam2Bunches()

##### BunchGroupUtil ##########################################################################
print "Testing BunchGroupUtil-------------------"
from ROOT import BunchGroupUtil

#bunchGroupReader = CoolDataReader    print obj.since(), obj.until(),    print obj.since(), obj.until(),    print obj.since(), obj.until(),("COOLONL_TDAQ/COMP200", "/TDAQ/OLC/LHC/FILLPARAMS")
#bunchGroupReader.setIOVRange(iovSince, iovUntil)
#bunchGroupReader.readData()

bunchGroupReader = CoolDataReader("COOLONL_TRIGGER/COMP200", "/TRIGGER/LVL1/BunchGroupContent")
bunchGroupReader.setIOVRangeFromRun(215589)
bunchGroupReader.readData()

dbu = BunchGroupUtil()
# Iterate over all data objects found
for obj in bunchGroupReader.data:
    print obj
#    print obj.since(), obj.until(),
#    dbu.setValue(obj.payload())
#    print 'size of Bunch Group Content'
#    print dbu.nBunchGroup()

###### BunchDescription ######################################################################
print "----------------Testing bunchDescription------------------"
from ROOT import BunchDescription
bunchDescriptionReader = CoolDataReader("COOLONL_TRIGGER/COMP200", "/TRIGGER/LVL1/BunchGroupDescription")
bunchDescriptionReader.setIOVRangeFromRun(215589)
bunchDescriptionReader.readData()

bgd = BunchDescription()
for obj in bunchDescriptionReader.data:
    print obj
#    bgd.setValue(obj.payload())
#    print bgd.nBunchD1()

##### BunchDataUtil ##########################################################################
print "--------------------Testing BunchDataUtil----------------"
from ROOT import BunchDataUtil
bunchDataUtilReader = CoolDataReader("COOLONL_TDAQ/COMP200", "/TDAQ/OLC/LHC/BUNCHDATA")
bunchDataUtilReader.setIOVRange(iovSince, iovUntil)
bunchDataUtilReader.setChannel([0])
bunchDataUtilReader.readData()
count = 0
bdu = BunchDataUtil()
for obj in bunchDataUtilReader.data:
    runlb = obj.payload()['RunLB']
    run = runlb >> 32
    lb = runlb & 0xFFFF
#    print "lb ", lb
    count +=1
    x1=bdu.nx1Storage(obj.payload())
    y1=bdu.ny1Storage(obj.payload())
    x2=bdu.nx2Storage(obj.payload())
    y2=bdu.ny2Storage(obj.payload())

    if (y1==0):
        for objfi in fillParamsReader.data:
            if (obj.since() >= objfi.since()) and (obj.since() <= objfi.until()):
                fpu.setValue(objfi.payload())
                v1 = fpu.beam1Bunches()
                break
        bdu.setCollidingBCID1s(v1)           
    if (y2==0):
        for objfi in fillParamsReader.data:
            if (obj.since() >= objfi.since()) and (obj.since() <= objfi.until()):
                fpu.setValue(objfi.payload())
                v2 = fpu.beam2Bunches()
                break
        bdu.setCollidingBCID2s(v2)

    bdu.setValue(obj.payload())

print count

##### BunchLumisUtil #########################################################################
print "------------ Testing BunchLumisUtil---------------"
from ROOT import BunchLumisUtil
bunchLumisUtilReader = CoolDataReader("COOLONL_TDAQ/COMP200", "/TDAQ/OLC/BUNCHLUMIS")
bunchLumisUtilReader.setIOVRange(iovSince, iovUntil)
bunchLumisUtilReader.setChannel([102])
bunchLumisUtilReader.readData()
count = 0
blu = BunchLumisUtil()
lbu = LumiBlobUtil()

for obj in bunchLumisUtilReader.data:
    runlb = obj.payload()['RunLB']
    run = runlb >> 32
    lb = runlb & 0xFFFF
    count +=1
    x=blu.nxStorage(obj.payload())
    y=blu.nyStorage(obj.payload())
    print "lb ", lb
    if (y==0):
        for objfi in fillParamsReader.data:
            if (obj.since() >= objfi.since()) and (obj.since() <= objfi.until()):
                print "Time interval in FPU:  ", (objfi.until()-objfi.since())/10**9
                fpu.setValue(objfi.payload())
                v1 = fpu.luminousBunches()
                print "v1 value ", v1[0]
                break
        print "v1.size() ", v1.size()
        blu.setCollidingBCIDs(v1)
    blu.setValue(obj.payload())
print "Number of Objects", count
