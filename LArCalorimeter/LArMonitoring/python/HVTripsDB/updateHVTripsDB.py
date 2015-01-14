#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import CheckHVStatus_daily
import sys
import os
import time

path = sys.argv[1]

date = time.strftime("%Y-%m-%d",time.localtime(time.time()))
date = date+":07:30:00"
##date = "2011-04-12:23:55:55"
##date = "2012-11-12:07:30:00"
print date

os.system( "export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup; source $AtlasSetup/scripts/asetup.sh 16.6.2.4; python %s/CheckHVStatus_daily.py %s" % (path, date) )

os.system( "cp -f %s/trips.dat %s/trips.dat.bak" % (path, path) )

os.system( "cat /afs/cern.ch/user/l/larmon/public/WebTools-scratch/HVTripsDB/dailyFiles/HVTrips_%s.dat >> %s/trips.dat" % (date, path) )

os.system( "cat /afs/cern.ch/user/l/larmon/public/WebTools-scratch/HVTripsDB/dailyFiles/HVTrips_%s.dat > %s/newTrips.dat" % (date, path) )


