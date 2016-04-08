#!/usr/bin/env python

import getopt
import sys
import CalibDbDefaults

cool_database_string = CalibDbDefaults.cool_database_string
cool_folder=CalibDbDefaults.cool_folder['t0']
cool_tag=''

def printhelp(argv0):
	sys.stderr.write("Usage: " + argv0 + " [-h] [-s <cool database string>] [-f <cool folder>] [-t <cool tag>] [<run nr> [<run nr> [..]]]\n")
	sys.exit(1)
	
optlist, args = getopt.gnu_getopt(sys.argv, 'hs:f:t:')
#if not len(args)==0:
#	printhelp(args[0])
	
for opt, oa in optlist:
	if opt=='-s':
		if oa[-2:]=='db':
			cool_database_string = 'sqlite://;schema=' + oa + ';dbname=COMP200'
			cool_tag=''
		else:
			cool_database_string = oa
		continue
	if opt=='-f':
		cool_folder=oa
		continue
	if opt=='-t':
		cool_tag=oa
		continue
	if opt=='-h':
		printhelp(sys.argv[0])
		
	

sys.argv=[ sys.argv[0], '-b' ]
from MuonCalibDbOperations.ReadCool import *

if not cool_tag:
	cool_tag = GetFolderTag(cool_database_string, cool_folder)
	
	
print "Cool database: " + cool_database_string + " " + cool_folder + " " + cool_tag 

if len(args)==1:
	DumpFolderSummary(cool_database_string, cool_folder, cool_tag)
	sys.exit(0)

for run_nr in args[1:]:
	print "IOVs for run ", run_nr
	print (len("IOVs for run ") + len(run_nr) + 1) * "="
	DumpFolderSummary(cool_database_string, cool_folder, cool_tag, int(run_nr))
	print



		
