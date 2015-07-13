#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#  Usage:
#    python status.py STATUSCODE
#
#  Exmple:
#    python status.py 19456
#  returns:
#     Status 19456:
#     Channel ON, Ramping, Current Limit Error
#
#
# stolen parsing part from Walter Lamp's script
# ~/public/HVTripsDB/CheckHVStatus_daily.py

import sys

def printStatus(stat):

    if stat & 0x400:
        result="Channel ON"
    else:
        result="Channel OFF"
    
    if stat & 1:
        result+= ", Current Trip"
    if stat & 2:
        result+= ", Sum error"
    if stat & 0x200:
        result+= ", Input error"
    if stat & 0x800:
        result+=", Ramping"
    if stat & 0x1000:
        result+=", Emergency stop"
    if stat & 0x2000:
        result+=", Kill enable"
    if stat & 0x4000:
        result+= ", Current Limit Error"
    if stat & 0x8000:
        result+= ", Voltage Limit Error"
    return result

#print sys.argv.length

if len(sys.argv)-1:
    print
    print "   Status",sys.argv[1],":"
    print "    ",printStatus(int(sys.argv[1]))
    print
