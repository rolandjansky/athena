#!/usr/bin/env python

import getopt
import sys
import os
import CalibDbDefaults

cool_database_string = CalibDbDefaults.cool_database_string
dbname = CalibDbDefaults.cool_dbname
t0_cool_folder=None
rt_cool_folder=None
cool_tag='CURRENT'


def printhelp(argv0):
	sys.stderr.write("Usage: " + argv0 + " [-h] [-s <cool database string>] [-o <t0 cool folder>] [-r <rt cool folder>] [-t <cool tag>] <run nr>\n")
	sys.exit(1)

optlist, args = getopt.gnu_getopt(sys.argv, 'hs:o:r:t:')

for opt, oa in optlist:
	if opt=='-s':
		if oa[-2:]=='db':
			cool_database_string = 'sqlite://;schema=' + oa + ';dbname=' + dbname
			cool_tag=''
		else:
			cool_database_string = oa
		continue
	if opt=='-o':
		t0_cool_folder=oa
		continue
	if opt=='-r':
		rt_cool_folder=oa
		continue
	if opt=='-t':
		cool_tag=oa
		continue
	if opt=='-h':
		printhelp(sys.argv[0])

if not len(args)==2:
	sys.stderr.write("Need Run Number\n")
	print len(args), args
	printhelp(sys.argv[0])

try:
	run_number=int(args[1])
except ValueError:
	sys.stderr.write("Cannot parse run number " + run_number + "\n")
	printhelp(sys.argv[0])
	
sys.argv=[ sys.argv[0], '-b' ]
from MuonCalibDbOperations.ReadCool import *
from MuonCalibIdentifier.MuonFixedIdUnpack import *
from ROOT import *


def dump_rt(ident, rt):
	up=MuonFixedIdUnpack(ident)
	if up.stationNameString()=="XXX":
		sys.stderr.write("WARNING: Unknown station found!\n")
		return
	f=file("calibration/rts/Rt_" + str(up.stationNameString()) + "_" + str(up.stationPhi()) + "_" + str(up.stationEta()) + ".dat", "w")
	f.write("v1.0 1 Resolution 1 20000 0 0 0 0 dummy 800 ")
	f.write(str(rt.GetN()))
	f.write("\n")
	for i in xrange(rt.GetN()):
		f.write(str(rt.GetX()[i]) + " " + str(rt.GetY()[i]) + " " +  str(rt.GetEY()[i]))
		f.write("\n")
		
	
	
	

if rt_cool_folder:
	graphs=ReadRtCool(cool_database_string, rt_cool_folder, cool_tag, run_number)[0]
	try:
		os.makedirs("calibration/rts")
	except OSError:
		try:
			os.stat("calibration/rts")
		except OSError:
			sys.stderr.write("ERROR: Cannot create calibration directory!\n")
			sys.exit(1)
	for ident, rt in graphs.items():
		dump_rt(ident, rt)
	
	
