#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# !!! UNTESTED !!!


"""
Update beamspot ntuple for cool for runs that are in the calib loop or have exited in the last 24 hours
"""

print 'Updating COOL ntuple ...'

import cx_Oracle
import time
import os
import sys

from InDetBeamSpotExample import COOLUtils

# Connect to ATLAS_COOLPROD DB for NEMO folders
try:
    #oracle = cx_Oracle.connect(dsn="ATLAS_COOLREADER_U",user="ATLAS_COOL_GLOBAL_W")
    oracle = cx_Oracle.connect(dsn="ATLAS_COOLPROD",user="ATLAS_COOL_READER_U",password="LMXTPRO4RED")
    cur = oracle.cursor()
except Exception,e:
    print e
    sys.exit('ERROR: Unable to connect to ATLAS_COOLPROD database') 


# List of all active runs currently in calib loop (STATE=2)
try:
    d = cur.execute("SELECT RUN FROM ATLAS_COOL_GLOBAL.NEMOP_RUN WHERE ACTIVE=1 AND RUNTYPE LIKE 'Physics' AND STATE=2")
    runsCalib = [r[0] for r in sorted(d)]
except Exception,e:
    print e
    sys.exit('Unable to retrieve runs in calibration loop')
    
# Look back 24 hours (+ half hour safety) from now (last time cron ran).
curtime = int(time.time())
limit = curtime - 24*3600+30*60

# List all active runs exiting calib loop (STATE=4) within this time interval
try:
    cur = oracle.cursor()
    d = cur.execute("SELECT RUN FROM ATLAS_COOL_GLOBAL.NEMOP_RUN WHERE ACTIVE=1 AND RUNTYPE LIKE 'Physics' AND STATE=4 AND UTIME>%s" % limit)
    runsExited = [r[0] for r in sorted(d)]
except Exception,e:
    print e
    sys.exit('Unable to retrieve runs exiting calibration loop')

# Get lowest run in calib loop or having left since last cron
if not bool(runsExited + runsCalib):
    sys.exit('No runs in calib loop or exited recently so nothing to do')

run = min(set(runsExited + runsCalib))
runMax = run - 1 # Since ru is inclusive
status = "59,83"

# Merge previous COOL ntuple into new one excluding runs abve this (which might have been in calib loop last time)
name = '/tmp/b7eamspot.' + str(os.getpid()) + '.root'

stat = os.system("beamspotnt.py -q -f %s --status '' --ru %s -s BeamSpotNt merge ~atlidbs/nt/cool/beamspotnt-COOL-Current.root" % (name, runMax) )
if stat:
    sys.exit("Unable to copy previous COOL ntuple")

# Add in new info from cool tag for runs above this
tag = COOLUtils.resolveCurrentBeamSpotFolder()
# Fit status included in ntuple

stat = os.system("beamspotnt.py -q -f %s --status '%s' --rl %s --fillCOOL -s BeamSpotCOOL  merge %s" % (name, status, run, tag) )
if stat:
    sys.exit("Unable to merge new runs into COOL ntuple")

# Copy ntuple back (backing up previous version)
stat = os.system("mv ~atlidbs/nt/cool/beamspotnt-COOL-Current.root ~atlidbs/nt/cool/beamspotnt-COOL-Current.previous.root")
if stat:
    sys.exit("ERROR: Unable to copy COOL ntuple to final location")

stat = os.system("mv %s ~atlidbs/nt/cool/beamspotnt-COOL-Current.root" % name)
if stat:
    sys.exit("ERROR: Unable to copy COOL ntuple to final location")
    
# Bonus: Can we somehow check if resolved tag has changed since last time and if so regenerate from scratch
# Can we fill type of run into ntuple (e.g. pp vs Hi) - can get project tag from SOR_Params -> projects

# Version for by time
