#!/usr/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sqlite3
import datetime

conn = sqlite3.connect('trips.db',detect_types=sqlite3.PARSE_DECLTYPES|sqlite3.PARSE_COLNAMES)
#conn = sqlite3.connect('trips.db')

c = conn.cursor()
#c.execute('create table TRIPS (HVLINE INTEGER,TripTimeStamp LONG INTEGER,TripRunNumber INTEGER,TripLB INTEGER,StableZero INTEGER,RampUp INTEGER,RecoveryTimeStamp LONG INTEGER,RecoveryRunNumber INTEGER,RecoveryLB INTEGER, StableBeams TEXT,FillNumber INTEGER,NumCollBunches INTEGER,Solenoid TEXT,Toroid TEXT)')
#cmd = 'insert into TRIPS values (?,?,?,?,?,?,?,?,?,?,?,?,?,?)'

#f = file('trips2.dat','r')

#for l in f:
#  a = l.split()
#  
#  tripString = a[1]
#  recoveryString = a[6]
#  tripDateTime=datetime.datetime.strptime(tripString, '%Y-%m-%d:%H:%M:%S')
#  recoveryDateTime=datetime.datetime.strptime(recoveryString, '%Y-%m-%d:%H:%M:%S')
#  b = (a[0],tripDateTime.strftime('%Y-%m-%d %H:%M:%S'),a[2],a[3],a[4],a[5],recoveryDateTime.strftime('%Y-%m-%d %H:%M:%S'),a[7],a[8],a[9].upper(),a[10],a[11],a[12].upper(),a[13].upper())
#  c.execute( cmd,b )

#  print b
#conn.commit()

c.execute('drop table DETAILS')
c.execute('create table DETAILS (HVLINE INTEGER PRIMARY KEY,Module INTEGER,Channel INTEGER,Det TEXT,Side TEXT,PhiWedge INTEGER,SubPhiWedge INTEGER,EtaSector INTEGER,ElectrodeSide TEXT,Map TEXT, PoweredElectrodes INTEGER, status STRING)')

f2 = file('hvdetails.dat','r')

cmd =  'insert into DETAILS values (?,?,?,?,?,?,?,?,?,?,?,?)'
for l2 in f2:
  a = l2.split()

  if a[0][0] =='#':
    print 'ignoring comment'
  else:
    side = a[4][0]
    phiWedge= a[4][1:]
    sector=0
    if len(a[6]) == 2:
      sector=a[6][1]
  
    b = (a[0],a[1],a[2],a[3],side,phiWedge,a[5],sector,a[7],a[8],a[9],a[10])
    c.execute( cmd,b )

conn.commit()
conn.close()
