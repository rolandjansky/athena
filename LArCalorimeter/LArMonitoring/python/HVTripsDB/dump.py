#!/usr/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sqlite3
import datetime

conn = sqlite3.connect('/afs/cern.ch/user/l/larmon/public/HVTripsDB/trips.db')
#conn = sqlite3.connect('trips.db')

c = conn.cursor()

f = file('resultDump.txt','w')

cmd =  'select trips.TripRunNumber,details.Det,details.Side, trips.TripLB, trips.RecoveryLB from trips INNER JOIN details ON trips.HVLINE = details.HVLINE order by trips.TripRunNumber;'

c.execute(cmd)

r=c.fetchall()

for i in range(len(r)):
	s=''
	for k in r[i]:
		s='%s%s '%(s,k)

	f.write(s+'\n')
	
f.close()

conn.close()
