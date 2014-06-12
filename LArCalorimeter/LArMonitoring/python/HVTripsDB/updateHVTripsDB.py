#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import CheckHVStatus_daily
import os
import time

date = time.strftime("%Y-%m-%d",time.localtime(time.time()))
date = date+":07:30:00"
##date = "2011-04-12:23:55:55"
##date = "2012-11-12:07:30:00"
print date

os.system("export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup; source $AtlasSetup/scripts/asetup.sh 16.6.2.4; python /afs/cern.ch/user/l/larmon/public/HVTripsDB/CheckHVStatus_daily.py %s" % date)

os.system("cp -f /afs/cern.ch/user/l/larmon/public/HVTripsDB/trips.dat /afs/cern.ch/user/l/larmon/public/HVTripsDB/trips.dat.bak")

os.system("cat /afs/cern.ch/user/l/larmon/public/HVTripsDB/dailyFiles/HVTrips_%s.dat >> /afs/cern.ch/user/l/larmon/public/HVTripsDB/trips.dat" % date)

os.system("cat /afs/cern.ch/user/l/larmon/public/HVTripsDB/dailyFiles/HVTrips_%s.dat > /afs/cern.ch/user/l/larmon/public/HVTripsDB/newTrips.dat" % date)

#os.system("source /afs/cern.ch/user/l/larmon/public/TripVisualizer/updateTripVisualizer.sh")


